#include "bf.hpp"

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
	bool CellOperation::apply(Instruction ins)
	{
		switch (ins.opcode)
		{
		case bfAdd:
			if (any && (op.opcode == bfAdd || op.opcode == bfSet))
			{
				op.argument += ins.argument;
			}
			else
			{
				op = ins;
				any = true;
			}
			return true;

		case bfSet:
			op = ins;
			any = true;
		return true;

		default:
			return false;
		}
	}

	void CellOperation::simplify()
	{
		if (any && instructions[op.opcode].stackable && op.argument == 0)
		{
			any = false;
		}
	}

	void CellOperation::repeat(size_t n)
	{
		// Do not multiply set!
		if (any && instructions[op.opcode].stackable)
		{
			op.argument *= n;
		}
	}

	void Brainfuck::optimize(size_t passes)
	{		
		using ivec = std::vector<Instruction>;
		static std::array<OptimizationSequence, 5> peephole_optimizers
		{{
			// [+] to bfSet 0
			{{bfLoopBegin, bfAdd, bfLoopEnd}, [](const ivec&) {
				return ivec{{bfSet, 0}};
			}},

			// Merge bfSet then bfAdd to a single set.
			{{bfSet, bfAdd}, [](const ivec &v) {
				return ivec{{bfSet, v[0].argument + v[1].argument}};
			}},

			// Optimize adding then setting, because adding will not be effective.
			{{bfAdd, bfSet}, [](const ivec& v) {
				return ivec{{bfSet, v[1].argument}};
			}},

			// Optimize 2 sets in a row.
			{{bfSet, bfSet}, [](const ivec& v) {
				return ivec{{bfSet, v[1].argument}};
			}},

			// [>]
			{{bfLoopBegin, bfShift, bfLoopEnd}, [](const ivec& v) {
				return ivec{{bfShiftUntilZero, v[1].argument}};
			}}
		}};

		for (size_t p = 0;; ++p)
		{
			if (p >= passes)
			{
				warnout(optimizeinfo) << "Maximal optimization pass reached. Consider increasing -optimizepasses.\n";
				return;
			}
			
			bool useful_pass = false;
			
			// Merge stackable ops
			for (size_t i = 0; i < program.size(); ++i)
			{
				if (!instructions[program[i].opcode].stackable)
					continue;
				
				size_t j = i;
				while (++j < program.size() && program[i].opcode == program[j].opcode)
					program[i].argument += program[j].argument;
				
				program.erase(begin(program) + i + 1, begin(program) + j);
			}

			// Eliminate stacked instructions with argument = 0
			program.erase(std::remove_if(program.begin(), program.end(), [](Instruction& ins) {
				return instructions[ins.opcode].stackable && ins.argument == 0; }), program.end());
			
			// Peephole optimization
			for (auto& optimizer : peephole_optimizers)
			{
				for (size_t i = 0; i < program.size(); ++i)
				{
					if (std::equal(begin(program) + i, begin(program) + i + optimizer.seq.size(), begin(optimizer.seq), end(optimizer.seq)))
					{
						ivec extract(program.begin() + i, program.begin() + i + optimizer.seq.size());
						ivec optimized = optimizer.callback(extract);
						
						if (!std::equal(begin(extract), end(extract), begin(optimized), end(optimized)))
						{
							useful_pass = true;
							replace_subvector_smaller(program, begin(program) + i, begin(program) + i + extract.size(), optimized);
						}	
					}
				}
			}

			// Detect balanced innermost loops: We can perform loop unrolling to some extent if we have enough
			// context. For example, a set followed by an inner loop can safely be completely unrolled.
			// See OPTIMIZATIONS.md
			{
				bool is_innermost = true;
				for (size_t i = 0; i < program.size(); ++i)
				{
					if (program[i].opcode == bfLoopBegin)
					{
						// No matter what, the next ] we encounter will be the loop end
						is_innermost = true;
					}
					else if (program[i].opcode == bfLoopEnd && is_innermost)
					{
						// Walk back to determine whether the loop is balanced
						int shift_sum = 0;

						size_t j = i;
						while (--j != 0)
						{
							if (program[j].opcode == bfLoopBegin)
							{
								break;
							}
							else if (program[j].opcode == bfShiftUntilZero)
							{
								break;
							}
							else if (program[j].opcode == bfShift)
							{
								shift_sum += program[j].argument;
							}
						}

						if (j != 0 &&
							program[j].opcode == bfLoopBegin && // Make sure the opcode wasn't bfShiftUntilZero
							shift_sum == 0)
						{
							bool is_recognized = true;

							std::map<int, CellOperation> operations;
							size_t offset = 0, k = j + 1;
							for (; k < i; ++k)
							{
								if (program[k].opcode == bfShift)
								{
									offset += program[k].argument;
									continue;
								}

								CellOperation &op = operations[offset];
								if (!op.apply(program[k]))
								{
									is_recognized = false;
									break;
								}
							}

							if (!is_recognized)
							{
								/*warnout(optimizeinfo) << "Unexpected instruction `" << disassemble(program[k]) << "` in memory pattern optimizer, within:\n";
								print_assembly(j, i + 1);*/
								is_recognized = false;
								continue;
							}

							// Simplify expressions (required for debugging hints below)
							for (auto &p : operations)
							{
								p.second.simplify();
							}

							std::map<int, CellOperation>::iterator loop_iterator = operations.find(0);

							bool bad_iterator = false;
							if (loop_iterator == operations.end())
							{
								warnout(optimizeinfo) << "Possible infinite loop; iterator cell never modified, within:\n";
								bad_iterator = true;
							}

							CellOperation co = loop_iterator->second;
							operations.erase(loop_iterator);
							if (!co.any)
							{
								is_innermost = false;
								continue;
							}

							if (!bad_iterator && co.op.opcode != bfAdd)
							{
								if (co.op.opcode == bfSet)
								{
									if (co.op.argument != 0)
									{
										warnout(optimizeinfo) << "Infinite loop, because its iterator is non-null, within:\n";
									}
									else
									{
										// TODO: if this is reached, then the loop is run exactly once. Thus we should be able to remove
										// the bfLoopEnd. However the linker relies on it for now...
										is_innermost = false;
										continue;
									}
								}
								else
								{
									warnout(optimizeinfo) << "Unrecognized loop iterator `" << disassemble(co.op) << "`, within:\n";
								}

								bad_iterator = true;
							}

							if (bad_iterator)
							{
								print_assembly(j, i + 1);
								is_innermost = false;
								continue;
							}

							if (Instruction &prev = program[j - 1]; prev.opcode == bfSet)
							{
								if (prev.argument == 0)
								{
									// TODO optimize away the loop
									warnout(optimizeinfo) << "Unused loop, because its iterator is initialized to 0:\n";
									print_assembly(j - 1, i + 1);
									program.erase(program.begin() + j, program.begin() + i + 1);
									continue;
								}

								if (co.op.argument > -1)
								{
									warnout(optimizeinfo) << "Loop iterator `" << disassemble(co.op) << "` relies on cell overflow:\n";
									print_assembly(j - 1, i + 1);
									is_innermost = false;
									continue;
								}
								else if (co.op.argument < -1)
								{
									is_innermost = false;
									continue;
								}

								ivec unrolled;
								unrolled.emplace_back(bfSet, 0); // The iterator will necessarily be 0 after the loop

								size_t offset = 0;
								for (auto &p : operations)
								{
									CellOperation &current = p.second;
									p.second.repeat(prev.argument);

									int diff = p.first - offset;
									offset += diff;

									unrolled.emplace_back(bfShift, diff);
									unrolled.push_back(current.op);
								}

								unrolled.emplace_back(bfShift, -offset);

								replace_subvector_smaller(program, program.begin() + j - 1, program.begin() + i + 1, unrolled);

								is_innermost = true;
								continue;
							}
							else
							{
								std::cout << "Found expandable loop, pass " << p + 1 << '\n';
								print_assembly(j, i + 1);

								if (co.op.argument != -1)
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
			}

			if (!useful_pass)
				break;
		}

		program.shrink_to_fit();
	}
}
