#include "bf.hpp"

#include <vector>
#include <functional>
#include <algorithm>

#include "../vecutils.hpp"

namespace bf
{
	void optimize(std::vector<Instruction>& program, const size_t passes)
	{
		typedef std::vector<Instruction> ivec;
		std::vector<OptimizationSequence> optimizers =
		{
			// [-] then set 0
			OptimizationSequence{ {bfLoopBegin, bfDecr, bfLoopEnd}, [](const ivec&) -> ivec { return {{bfSet, 0}}; } },

			// + or - and then set -> set
			OptimizationSequence{ {bfAdd, bfSet}, [](const ivec& v) -> ivec { return {{bfSet, v[1].argument}}; } },
			OptimizationSequence{ {bfIncr, bfSet}, [](const ivec& v) -> ivec { return {{bfSet, v[1].argument}}; } },
			OptimizationSequence{ {bfSub, bfSet}, [](const ivec& v) -> ivec { return {{bfSet, v[1].argument}}; } },
			OptimizationSequence{ {bfDecr, bfSet}, [](const ivec& v) -> ivec { return {{bfSet, v[1].argument}}; } },

			// [->+<] or [-<+>]
			OptimizationSequence{ {bfLoopBegin, bfDecr, bfOnceShiftRight, bfIncr, bfOnceShiftLeft, bfLoopEnd}, [](const ivec&) -> ivec { return {{bfMoveRightAdd, 1}}; } }, // @TODO add bfMoveRightIncr?
			OptimizationSequence{ {bfLoopBegin, bfDecr, bfShiftRight, bfIncr, bfShiftLeft, bfLoopEnd}, [](const ivec& v) -> ivec {
				if (v[2].argument == v[4].argument)
					return {{bfMoveRightAdd, v[2].argument}};
				else
					return v;
			} },

			OptimizationSequence{ {bfLoopBegin, bfDecr, bfOnceShiftLeft, bfIncr, bfOnceShiftRight, bfLoopEnd}, [](const ivec&) -> ivec { return {{bfMoveLeftAdd, 1}}; } }, // @TODO add bfMoveLeftIncr?
			OptimizationSequence{ {bfLoopBegin, bfDecr, bfShiftLeft, bfIncr, bfShiftRight, bfLoopEnd}, [](const ivec& v) -> ivec {
				if (v[2].argument == v[4].argument)
					return {{bfMoveLeftAdd, v[2]. argument}};
				else
					return v;
			} }
		};

		for (size_t p = 0; p < passes; ++p)
		{
			size_t passopt = 0;
			for (auto& optimizer : optimizers)
			{
				for (size_t i = 0; i < program.size(); ++i)
				{
					bool match = true;
					for (size_t j = 0; j < optimizer.seq.size(); ++j)
					{
						if (static_cast<uint8_t>(optimizer.seq[j]) != program[i + j].opcode)
						{
							match = false;
							break;
						}
					}

					if (match)
					{
						++passopt;
						//replace_subvector(program, program.begin() + i, program.begin() + i + optimizer.seq.size(), )
						// @TODO move to a replace_subvector function
						ivec extract(program.begin() + i, program.begin() + i + optimizer.seq.size());
						ivec optimized = optimizer.callback(extract);

						size_t newProgramSize = program.size() - extract.size() + optimized.size();
						if (program.size() < newProgramSize) // Resize first, keep the end of the vector to the end, insert the new subvector
						{
							size_t oldSize = program.size();
							program.resize(newProgramSize); // Resize the vector because we know we won't lose data since we make it bigger
							std::move_backward(program.begin() + i + extract.size(), program.begin() + oldSize, program.end()); // Move the end of the vector to the new boundaries
							std::move(optimized.begin(), optimized.end(), program.begin() + i); // Insert the subvector
						}
						else // Insert the new subvector, stick the end of the vector to the new subvector, resize
						{
							std::move(optimized.begin(), optimized.end(), program.begin() + i); // Insert the subvvector
							ivec lastchunk(program.begin() + i + extract.size(), program.end());
							std::move(lastchunk.begin(), lastchunk.end(), program.begin() + i + optimized.size());
							program.resize(newProgramSize);
						}
					}
				}
			}
			printf("Optimizer pass %ld/%ld > Triggered %ld optimizers", p, passes, passopt);
			if (passopt == 0)
			{
				printf(", stopping\n");
				break;
			}
			putchar('\n');
		}
	}
}