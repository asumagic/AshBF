#include "bf.hpp"
#include "optimizer.hpp"
#include "il.hpp"

#include <vector>
#include <map>
#include <array>
#include <functional>
#include <algorithm>
#include "../vecutils.hpp"

// TODO split this into a few files
// TODO unindent namespaces everywhere

namespace bf
{
void CellOperation::apply(const Instruction& ins)
{
	switch (ins.opcode)
	{
	case bfAdd:
		if (any && (op.opcode == bfAdd || op.opcode == bfSet))
		{
			op.argument() += ins.argument();
		}
		else
		{
			op = ins;
			any = true;
		}

	case bfSet:
		op = ins;
		any = true;

	default: break;
	}
}

void CellOperation::simplify()
{
	if (any && instructions[op.opcode].stackable && op.argument() == 0)
	{
		any = false;
	}
}

void CellOperation::repeat(size_t n)
{
	// Do not multiply set!
	if (any && instructions[op.opcode].stackable)
	{
		op.argument() *= n;
	}
}

bool Optimizer::is_stackable(const Instruction& ins)
{
	return instructions[ins.opcode].stackable;
}

bool Optimizer::is_nop(const Instruction& ins)
{
	return ins.opcode == bfNop ||
		(is_stackable(ins) && ins.argument() == 0);
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
			i->argument() += j->argument();
			j->opcode = bfNop; // Mark for deletion
		}
	}

	return erase_nop(program, begin, end);
}

bool Optimizer::peephole_optimize(Program &program, ProgramIt begin, ProgramIt end)
{
	bool effective = false;

	static std::array<OptimizationSequence, 5> peephole_optimizers
	{{
		// [+] to bfSet 0
		{{bfLoopBegin, bfAdd, bfLoopEnd}, [](const Program&) {
			return Program{{bfSet, 0}};
		}},

		// Merge bfSet then bfAdd to a single set.
		{{bfSet, bfAdd}, [](const Program &v) {
			return Program{{bfSet, v[0].argument() + v[1].argument()}};
		}},

		// Optimize adding then setting, because adding will not be effective.
		{{bfAdd, bfSet}, [](const Program& v) {
			return Program{{bfSet, v[1].argument()}};
		}},

		// Optimize 2 sets in a row.
		{{bfSet, bfSet}, [](const Program& v) {
			return Program{{bfSet, v[1].argument()}};
		}},

		// [>]
		{{bfLoopBegin, bfShift, bfLoopEnd}, [](const Program& v) {
			return Program{{bfShiftUntilZero, v[1].argument()}};
		}}
	}};

	for (auto &optimizer : peephole_optimizers)
	{
		for (auto i = begin; i != end; ++i)
		{
			if (!std::equal(i, i + optimizer.seq.size(), optimizer.seq.begin(), optimizer.seq.end()))
				continue;

			Program extract(i, i + optimizer.seq.size()); // TODO pass iterators or some sort of view instead
			Program optimized = optimizer.callback(extract);

			if (std::equal(extract.begin(), extract.end(), optimized.begin(), optimized.end()))
				continue;

			replace_subvector_smaller(program, i, i + extract.size(), optimized);
			effective = true;
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
			shift_count += i->argument();

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
					shift_count += j->argument();
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

			if (loopit_op.op.opcode == bfSet && loopit_op.op.argument() != 0)
			{
				warnout(optimizeinfo) << "Infinite loop: Iterator is always `" << loopit_op.op.argument() << "`\n";
				continue;
			}

			if (loopit_op.op.argument() != -1)
				continue;

			Program unrolled;
			if (loop_begin != begin && (loop_begin - 1)->opcode == bfSet)
			{
				// We know how many times the loop runs.
				Instruction &prec = *(loop_begin - 1);
				if (prec.argument() == 0)
				{
					warnout(optimizeinfo) << "Loop never runs, iterator is initialized to 0\n";
					// TODO erase
					continue;
				}

				if (prec.argument() == 1)
				{
					warnout(optimizeinfo) << "Loop runs exactly once\n";
				}

				shift_count = 0;
				for (auto &p : operations)
				{
					p.second.repeat(prec.argument());
					unrolled.emplace_back(bfShift, p.first - shift_count);
					unrolled.push_back(p.second.op);
					shift_count = p.first;
				}

				// Shift back to the iterator cell and set it to 0
				unrolled.emplace_back(bfShift, -shift_count);
				unrolled.emplace_back(bfSet, 0);

				replace_subvector_smaller(program, loop_begin - 1, i + 1, unrolled);
			}
#ifdef BROKEN_OPTIMIZATIONS
			else
			{
				// We don't know how many times the loop runs, but we assumed that the loop is decremented
				// by one every time.

				shift_count = 0;
				for (auto &p : operations)
				{
					unrolled.emplace_back(bfShift, p.first - shift_count);
					shift_count = p.first;

					if (p.second.op.opcode == bfAdd)
						unrolled.emplace_back(bfMAC, p.second.op.argument(), -shift_count);
					else
						unrolled.push_back(p.second.op);
				}

				// Shift back to the iterator cell and set it to 0
				unrolled.emplace_back(bfShift, -shift_count);
				unrolled.emplace_back(bfSet, 0);

				infoout() << "Optimized:\n";
				disasm.print_range(loop_begin, i + 1);
				infoout() << "into:\n";
				disasm.print_range(unrolled.begin(), unrolled.end());

				replace_subvector_smaller(program, loop_begin, i + 1, unrolled);
			}
#else
			else { effective = false; }
#endif

			effective = true;
			//i = begin; // HACK
			return false;
		}
	}

	return effective;
}
/*
							{
								std::cout << "Found expandable loop, pass " << p + 1 << '\n';
								print_assembly(j, i + 1);

								if (co.op.argument() != -1)
								{
									warnout(optimizeinfo) << "Loop iterator `" << disassemble(co.op) << "` may rely on cell overflow:\n";
									print_assembly(j - 1, i + 1);
									is_innermost = false;
									continue;
								}

								ivec unrolled;

								size_t offset = 0;
								for (auto &p : operations)
								{
									CellOperation &current = p.second;

									int diff = p.first - offset;
									offset += diff;

									unrolled.emplace_back(bfShift, diff);
									if (current.op.opcode == bfSet)
									{
										unrolled.push_back(current.op);
									}
									else
									{
										// wrong
										unrolled.emplace_back(bfMul, -offset);
									}
								}

								if (!is_innermost) continue;

								unrolled.emplace_back(bfShift, -offset);

								// We do this now because because bfMul uses this above
								unrolled.emplace_back(bfSet, 0);

								replace_subvector_smaller(program, program.begin() + j, program.begin() + i + 1, unrolled);

								is_innermost = true;
								continue;
							}
						}

						is_innermost = false;
					}
				}

		return true;
	}*/

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
				infoout(optimizeinfo) << "Performed optimization task '" << task.name << "' and " << (effective ? "was effective\n" : "had no effect\n");

				if (effective)
					pass_effective = true;
			}

			if (!pass_effective)
			{
				infoout(optimizeinfo) << "Pass was not effective, optimizations performed\n";
				break;
			}
		}

		program.shrink_to_fit();
	}
}
