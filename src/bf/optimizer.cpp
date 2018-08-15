#include "optimizer.hpp"

#include "disasm.hpp"
#include "il.hpp"
#include "logger.hpp"
#include "span.hpp"
#include "vecutils.hpp"

#include <algorithm>
#include <array>
#include <functional>
#include <sstream>
#include <vector>

namespace bf
{

bool Optimizer::update_state_debug(Program &program)
{
	if (debug)
	{
		if (past_state.program.empty())
		{
			infoout(optimizeinfo) << "Running reference unoptimized program for optimization regressiong testing.\n";
		}

		std::stringstream ss;

		// TODO: make this parameterizable to some extent
		Brainfuck bf;
		bf.program = program;
		bf.pipeout = &ss;
		bf.link();
		bf.interpret(30000);

		std::string program_output = ss.str();

		Program &old_program = past_state.program;

		if (!past_state.program.empty())
		{
			if (program_output != past_state.output)
			{
				errout(optimizeinfo) << "Optimization #" << past_state.id << " has regressed!\n";
				past_state.correct = false;

				auto [a_mismatch_begin, b_mismatch_begin] = std::mismatch(old_program.begin(), old_program.end(), program.begin(), program.end());
						auto [a_mismatch_end, b_mismatch_end] = std::mismatch(old_program.rbegin(), old_program.rend(), program.rbegin(), program.rend());

						warnout(optimizeinfo) << "Latest correct assembly:\n";
						disasm.print_range({a_mismatch_begin, a_mismatch_end.base()});

				warnout(optimizeinfo) << "Broken assembly:\n";
				disasm.print_range({b_mismatch_begin, b_mismatch_end.base()});
			}
			else if (past_state.correct)
			{
				infoout(optimizeinfo) << "Optimization marker #" << past_state.id << " correct.\n";
			}
			else
			{
				infoout(optimizeinfo) << "Optimization marker #" << past_state.id << " consistent with previous invalid results.\n";
			}
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
	program.erase(std::remove_if(begin, end, [](auto op) { return op.is_nop_like(); }), end);
	return program.size() != old_size;
}

bool Optimizer::peephole_optimize_for(Program& program, ProgramIt begin, ProgramIt end, const std::vector<OptimizationSequence>& optimizers)
{
	bool effective = false;

	for (auto &optimizer : optimizers)
	{
		for (auto i = begin; i != end; ++i)
		{
			span candidate{i, optimizer.seq.size()};

			if (candidate == span{optimizer.seq})
			{
				move_range(program, candidate.begin(), candidate.end(), optimizer.optimize(candidate));
				effective = true;
			}
		}
	}

	return effective;
}

bool Optimizer::merge_stackable(Program &program, ProgramIt begin, ProgramIt end)
{
	for (auto i = begin; i != end; ++i)
	{
		if (!i->is_stackable())
		{
			continue;
		}

		for (auto j = i + 1; (j != end) && (j->opcode == i->opcode); ++j)
		{
			i->args[0] += j->args[0];
			j->opcode = bfNop; // Mark for deletion
		}
	}

	return erase_nop(program, begin, end);
}

bool Optimizer::stage1_peephole_optimize(Program &program, ProgramIt begin, ProgramIt end)
{
	const std::vector<OptimizationSequence> peephole_optimizers
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

	return peephole_optimize_for(program, begin, end, peephole_optimizers);
}

bool Optimizer::balanced_loop_unrolling(Program& program, ProgramIt begin, ProgramIt end)
{
	auto loop_begin = begin;
	bool expandable = true, effective = false;
	int shift_count = 0;
	std::map<int, VMOp> operations;

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
		{
			mark_expandable(i);
		}

		if (i->opcode == bfShiftUntilZero ||
			i->opcode == bfMAC ||
			i->opcode == bfCharIn ||
			i->opcode == bfCharOut) // TODO could be expanded
		{
			expandable = false;
		}

		// All we handle below assumes we are in an expandable loop
		if (!expandable)
		{
			continue;
		}

		if (i->opcode == bfShift)
		{
			shift_count += i->args[0];
		}

		// We found the loop boundaries
		if (i->opcode == bfLoopEnd)
		{
			// Reset expandable in advance
			expandable = false;

			// Unbalanced loop
			if (shift_count != 0)
			{
				continue;
			}

			// Not great.. maybe instruction metadata could be nice at some point
			shift_count = 0;
			for (auto j = loop_begin; j != i; ++j)
			{
				if (j->opcode == bfShift)
				{
					shift_count += j->args[0];
				}
				else
				{
					operations[shift_count].try_merge_with(*j);
				}
			}

			for (auto &p : operations)
			{
				p.second.simplify();
			}

			auto loopit = operations.find(0);
			if (loopit == operations.end() || !loopit->second)
			{
				warnout(optimizeinfo) << "Infinite loop: Iterator is never modified\n";
				continue;
			}

			VMOp loopit_op = loopit->second;

			// We handle the offset 0 case manually.
			operations.erase(loopit);

			if (loopit_op.opcode == bfSet && loopit_op.args[0] != 0)
			{
				warnout(optimizeinfo) << "Infinite loop: Iterator is always `" << loopit_op.args[0] << "`\n";
				continue;
			}

			if (loopit_op.args[0] != -1)
			{
				continue;
			}

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
					unrolled.push_back(p.second);
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

					if (p.second.opcode == bfAdd)
					{
						unrolled.emplace_back(bfMAC, p.second.args[0], -shift_count);
					}
					else if (p.second.opcode == bfSet)
					{
						legal_when_zero = false;
						break;
					}
					else
					{
						unrolled.push_back(p.second);
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
		}
	}

	return effective;
}

bool Optimizer::stage2_peephole_optimize(Program& program, ProgramIt begin, ProgramIt end)
{
	const std::vector<OptimizationSequence> peephole_optimizers
	{{
		// Shifted adds
		{{bfShift, bfAdd, bfShift}, [](auto v) -> Program {
			return {
				{bfAddOffset, v[1].args[0], v[0].args[0]},
				{bfShift, v[0].args[0] + v[2].args[0]}
			};
		}},

		// Shifted sets
		{{bfShift, bfSet, bfShift}, [](auto v) -> Program {
			return {
				{bfSetOffset, v[1].args[0], v[0].args[0]},
				{bfShift, v[0].args[0] + v[2].args[0]}
			};
		}},
	}};

	return peephole_optimize_for(program, begin, end, peephole_optimizers);
}

void Optimizer::optimize(Program& program)
{
	update_state_debug(program);

	const std::array<std::vector<OptimizerTask>, stage_count> tasks
	{{
		{
			{&Optimizer::merge_stackable,          "Merge stackable instructions"},
			{&Optimizer::stage1_peephole_optimize, "Peephole"},
			{&Optimizer::balanced_loop_unrolling,  "Balanced loop unrolling"}
		},

		{
			{&Optimizer::merge_stackable,          "Merge stackable instructions"},
			{&Optimizer::stage2_peephole_optimize, "Optimize offset memory sets through specialized instructions"}
		}
	}};

	for (size_t s = 0; s < stage_count; ++s)
	for (size_t p = 0;; ++p)
	{
		if (p >= pass_count)
		{
			warnout(optimizeinfo) << "Maximal optimization pass reached for stage " << s << ". Consider increasing -optimizepasses.\n";
			break;
		}

		bool pass_effective = false;

		if (verbose)
		{
			infoout(optimizeinfo) << "Performing pass #" << p + 1 << '\n';
		}

		for (auto& task : tasks[s])
		{
			bool effective = (this->*(task.callback))(program, program.begin(), program.end());

			if (verbose)
			{
				infoout(optimizeinfo) << "Performed optimization task '" << task.name << "' and " << (effective ? "was effective\n" : "had no effect\n");
			}

			if (effective)
			{
				pass_effective = true;
			}

			update_state_debug(program);
		}

		if (!pass_effective)
		{
			if (verbose)
			{
				infoout(optimizeinfo) << "Pass was not effective, optimizations performed\n";
			}

			break;
		}
	}

	program.shrink_to_fit();
}

}
