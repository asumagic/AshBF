#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP

#include "bf.hpp"

namespace bf
{
struct Optimizer
{
	// Parameters
	size_t pass_count = 5;
	bool debug = false;
	bool verbose = false;

	static bool is_stackable(const VMOp &ins); // True if the instruction is mergeable/stackable
	static bool is_nop(const VMOp &ins); // True if the instruction has no visible effect

	void optimize(Program &program);

	bool erase_nop(Program &program, ProgramIt begin, ProgramIt end);
	bool merge_stackable(Program &program, ProgramIt begin, ProgramIt end);
	bool peephole_optimize(Program &program, ProgramIt begin, ProgramIt end);
	bool balanced_loop_unrolling(Program &program, ProgramIt begin, ProgramIt end);
};
}

#endif // OPTIMIZER_HPP
