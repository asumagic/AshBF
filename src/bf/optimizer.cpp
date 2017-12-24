#include "bf.hpp"

#include <vector>
#include <array>
#include <functional>
#include <algorithm>
#include "../vecutils.hpp"

// @TODO add code analysis for optimizations, improve the sequence-based optimizer or remove it
// @TODO have a way to profile the execution of the brainfuck program and make it possible to analyze it (with an external tool eventually)
namespace bf
{
	void Brainfuck::optimize(size_t passes)
	{		
		using ivec = std::vector<Instruction>;
		static std::array<OptimizationSequence, 4> peephole_optimizers
		{{
			// [+] to bfSet 0
			{{bfLoopBegin, bfAdd, bfLoopEnd}, [](const ivec&) {
				return ivec{{bfSet, 0}};
			}},

			// Merge bfSet then bfAdd
			{{bfSet, bfAdd}, [](const ivec &v) {
				return ivec{{bfSet, v[0].argument + v[1].argument}};
			}},

			// + and then set -> set
			{{bfAdd, bfSet}, [](const ivec& v) {
				return ivec{{bfSet, v[1].argument}};
			}},

			// [>] and [<]
			{{bfLoopBegin, bfShift, bfLoopEnd}, [](const ivec& v) {
				return ivec{{bfShiftUntilZero, v[1].argument}};
			}},
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

			if (!useful_pass)
				break;
		}

		program.shrink_to_fit();
	}
}
