#include "disasm.hpp"
#include "optimizer.hpp"
#include "il.hpp"
#include "logger.hpp"
#include "vecutils.hpp"
#include "span.hpp"

#include <vector>
#include <map>
#include <array>
#include <functional>
#include <algorithm>
#include <sstream>

namespace bf
{
void CellOperation::apply(const VMOp& ins)
{
	switch (ins.opcode)
	{
	case bfAdd:
		if (any && (op.opcode == bfAdd || op.opcode == bfSet))
		{
			op.args[0] += ins.args[0];
		}
		else
		{
			op = ins;
			any = true;
		}
	break;

	case bfSet:
		op = ins;
		any = true;

	default: break;
	}
}

void CellOperation::simplify()
{
	if (any && instructions[op.opcode].stackable && op.args[0] == 0)
	{
		any = false;
	}
}

void CellOperation::repeat(size_t n)
{
	if (any && instructions[op.opcode].stackable)
	{
		op.args[0] *= n;
	}
}

bool Optimizer::is_stackable(const VMOp& ins)
{
	return instructions[ins.opcode].stackable;
}

bool Optimizer::is_nop(const VMOp& ins)
{
	return ins.opcode == bfNop ||
			(is_stackable(ins) && ins.args[0] == 0);
}

bool Optimizer::update_state_debug(Program &program)
{
	if (debug)
	{
		std::stringstream ss;

		Brainfuck bf;
		bf.program = program;
		bf.pipeout = &ss;
		bf.link();
		bf.interpret(30000);

		std::string program_output = ss.str();

		Program &old_program = past_state.program;

		if (past_state.program.empty())
		{
			infoout(optimizeinfo) << "Saving reference program output for regression testing.\n";
		}
		else if (program_output != past_state.output)
		{
			errout(optimizeinfo) << "Optimization #" << past_state.id << " has regressed!\n";
			past_state.correct = false;

			auto [a_mismatch_begin, b_mismatch_begin] = std::mismatch(old_program.begin(), old_program.end(), program.begin(), program.end());
			auto [a_mismatch_end, b_mismatch_end] = std::mismatch(old_program.rbegin(), old_program.rend(), program.rbegin(), program.rend());

			warnout(optimizeinfo) << "Latest correct assembly:\n";
			disasm.print_range(a_mismatch_begin, a_mismatch_end.base());

			warnout(optimizeinfo) << "Broken assembly:\n";
			disasm.print_range(b_mismatch_begin, b_mismatch_end.base());
		}
		else if (past_state.correct)
		{
			infoout(optimizeinfo) << "Optimization marker #" << past_state.id << " correct.\n";
		}
		else
		{
			infoout(optimizeinfo) << "Optimization marker #" << past_state.id << " consistent with previous invalid results.\n";
		}

		past_state.program = program;
		++past_state.id;
		past_state.output = program_output;
	}

	return past_state.correct;
}

bool Optimizer::erase_nop(Program &program, ProgramIt begin, ProgramIt end)
{
	size_t old_size = program.size();
	program.erase(std::remove_if(begin, end, is_nop), end);
	return program.size() != old_size;
}

bool Optimizer::merge_stackable(Program &program, ProgramIt begin, ProgramIt end)
{
	for (auto i = begin; i != end; ++i)
	{
		if (!is_stackable(*i))
			continue;

		for (auto j = i + 1; (j != end) && (j->opcode == i->opcode); ++j)
		{
			i->args[0] += j->args[0];
			j->opcode = bfNop; // Mark for deletion
		}
	}

	return erase_nop(program, begin, end);
}

bool Optimizer::peephole_optimize(Program &program, ProgramIt begin, ProgramIt end)
{
	static std::array<OptimizationSequence, 5> peephole_optimizers
	{{
		// [+] to bfSet 0
		{{bfLoopBegin, bfAdd, bfLoopEnd}, [](auto) -> Program {
			return {{bfSet, 0}};
		}},

		// Merge bfSet then bfAdd to a single set.
		{{bfSet, bfAdd}, [](auto v) -> Program {
			return {{bfSet, v[0].args[0] + v[1].args[0]}};
		}},

		// Optimize adding then setting, because adding will not be effective.
		{{bfAdd, bfSet}, [](auto v) -> Program {
			return {{bfSet, v[1].args[0]}};
		}},

		// Optimize 2 sets in a row.
		{{bfSet, bfSet}, [](auto v) -> Program {
			return {{bfSet, v[1].args[0]}};
		}},

		// [>]
		{{bfLoopBegin, bfShift, bfLoopEnd}, [](auto v) -> Program {
			return {{bfShiftUntilZero, v[1].args[0]}};
		}}
	}};

	bool effective = false;

	for (auto &optimizer : peephole_optimizers)
	{
		for (auto i = begin; i != end; ++i)
		{
			span candidate{i, i + optimizer.seq.size()};

			if (candidate == span{optimizer.seq})
			{
				move_range(program, candidate.begin(), candidate.end(), optimizer.optimize(candidate));
				effective = true;
			}
		}
	}

	return effective;
}

bool Optimizer::balanced_loop_unrolling(Program& program, ProgramIt begin, ProgramIt end)
{
	auto loop_begin = begin;
	bool expandable = true, effective = false;
	size_t shift_count = 0;
	std::map<int, CellOperation> operations;

	// Reset counters when we enter a potentially expandable loop
	auto mark_expandable = [&](ProgramIt current) {
		loop_begin = current;
		expandable = true;
		shift_count = 0;
		operations.clear();
	};

	for (auto i = begin; i != end; ++i)
	{
		// If we encounter a [ then the next ] we find ends it
		if (i->opcode == bfLoopBegin)
			mark_expandable(i);

		if (i->opcode == bfShiftUntilZero ||
			i->opcode == bfMAC ||
			i->opcode == bfCharIn ||
			i->opcode == bfCharOut) // TODO could be expanded
			expandable = false;

		// All we handle below assumes we are in an expandable loop
		if (!expandable)
			continue;

		if (i->opcode == bfShift)
			shift_count += i->args[0];

		// We found the loop boundaries
		if (i->opcode == bfLoopEnd)
		{
			// Reset expandable in advance
			expandable = false;

			// Unbalanced loop
			if (shift_count != 0)
				continue;

			// Not great.. maybe instruction metadata could be nice at some point
			shift_count = 0;
			for (auto j = loop_begin; j != i; ++j)
			{
				if (j->opcode == bfShift)
					shift_count += j->args[0];
				else
					operations[shift_count].apply(*j);
			}

			for (auto &p : operations)
				p.second.simplify();

			auto loopit = operations.find(0);
			if (loopit == operations.end() || !loopit->second.any)
			{
				warnout(optimizeinfo) << "Infinite loop: Iterator is never modified\n";
				continue;
			}

			CellOperation loopit_op = loopit->second;

			// We handle the offset 0 case manually.
			operations.erase(loopit);

			if (loopit_op.op.opcode == bfSet && loopit_op.op.args[0] != 0)
			{
				warnout(optimizeinfo) << "Infinite loop: Iterator is always `" << loopit_op.op.args[0] << "`\n";
				continue;
			}

			if (loopit_op.op.args[0] != -1)
				continue;

			Program unrolled;
			if (loop_begin != begin && (loop_begin - 1)->opcode == bfSet)
			{
				// We know how many times the loop runs.
				VMOp &prec = *(loop_begin - 1);
				if (prec.args[0] == 0)
				{
					warnout(optimizeinfo) << "Loop never runs, iterator is initialized to 0\n";
					// TODO erase
					continue;
				}

				if (prec.args[0] == 1)
				{
					warnout(optimizeinfo) << "Loop runs exactly once\n";
				}

				shift_count = 0;
				for (auto &p : operations)
				{
					p.second.repeat(prec.args[0]);
					unrolled.emplace_back(bfShift, p.first - shift_count);
					unrolled.push_back(p.second.op);
					shift_count = p.first;
				}

				// Shift back to the iterator cell and set it to 0
				unrolled.emplace_back(bfShift, -shift_count);
				unrolled.emplace_back(bfSet, 0);

				move_range(program, loop_begin - 1, i + 1, unrolled);
			}
			else
			{
				// We don't know how many times the loop runs, but we assumed that the loop is decremented
				// by one every time.

				bool legal_when_zero = true;
				shift_count = 0;
				for (auto &p : operations)
				{
					unrolled.emplace_back(bfShift, p.first - shift_count);
					shift_count = p.first;

					if (p.second.op.opcode == bfAdd)
					{
						unrolled.emplace_back(bfMAC, p.second.op.args[0], -shift_count);
					}
					else if (p.second.op.opcode == bfSet)
					{
						legal_when_zero = false;
						break;
					}
					else
					{
						unrolled.push_back(p.second.op);
					}
				}

				// TODO: conditionals to allow some optimization anyway
				if (!legal_when_zero)
				{
					update_state_debug(program);
					continue;
				}

				// Shift back to the iterator cell and set it to 0
				unrolled.emplace_back(bfShift, -shift_count);
				unrolled.emplace_back(bfSet, 0);

				move_range(program, loop_begin, i + 1, unrolled);

				update_state_debug(program);
			}

			effective = true;
			//i = begin; // HACK
		}
	}

	return effective;
}

void Optimizer::optimize(Program& program)
{
	for (size_t p = 0;; ++p)
	{
		if (p >= pass_count)
		{
			warnout(optimizeinfo) << "Maximal optimization pass reached. Consider increasing -optimizepasses.\n";
			break;
		}

		bool pass_effective = false;

		if (verbose)
			infoout(optimizeinfo) << "Performing pass #" << p + 1 << '\n';

		struct OptimizerTask
		{
			bool (Optimizer::*callback)(Program&, ProgramIt, ProgramIt);
			const char *name;
		};

		std::array<OptimizerTask, 3> tasks
		{{
			{&Optimizer::merge_stackable,         "Merge stackable instructions"},
			{&Optimizer::peephole_optimize,       "Peephole"},
			{&Optimizer::balanced_loop_unrolling, "Balanced loop unrolling"}
		}};

		for (auto &task : tasks)
		{
			bool effective = (this->*(task.callback))(program, program.begin(), program.end());

			if (verbose)
				infoout(optimizeinfo) << "Performed optimization task '" << task.name << "' and " << (effective ? "was effective\n" : "had no effect\n");

			if (effective)
				pass_effective = true;

			update_state_debug(program);
		}

		if (!pass_effective)
		{
			if (verbose)
				infoout(optimizeinfo) << "Pass was not effective, optimizations performed\n";
			break;
		}
	}

	program.shrink_to_fit();
}
}
