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
			OptimizationSequence{ {bfAdd, bfSet},  [](const ivec& v) -> ivec { return {{bfSet, v[1].argument}}; } },
			OptimizationSequence{ {bfIncr, bfSet}, [](const ivec& v) -> ivec { return {{bfSet, v[1].argument}}; } },
			OptimizationSequence{ {bfSub, bfSet},  [](const ivec& v) -> ivec { return {{bfSet, v[1].argument}}; } },
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
					if (std::equal(begin(program) + i, begin(program) + i + optimizer.seq.size(), begin(optimizer.seq), end(optimizer.seq)))
					{
						++passopt;

						ivec extract(program.begin() + i, program.begin() + i + optimizer.seq.size());
						replace_subvector(program, begin(program) + i, begin(program) + i + extract.size(), optimizer.callback(extract));
					}
				}
			}
			/*printf("Optimizer pass %ld/%ld > Triggered %ld optimizers", p, passes, passopt);
			if (passopt == 0)
			{
				printf(", stopping\n");
				break;
			}
			putchar('\n');*/
		}
	}
}
