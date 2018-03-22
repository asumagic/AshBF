#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP

#include <functional>
#include <memory>

namespace bf
{
struct OptimizationSequence
{
	std::vector<uint8_t> seq;
	std::function<Program(const Program&)> callback;
};

struct CellOperation
{
	VMOp op;
	bool any = false;

	void apply(const VMOp& instruction);
	void simplify();
	void repeat(size_t n);
};

struct PastProgramState
{
	Program program;
	std::string output;
	size_t id = 0;
	bool correct = true;
};

struct Optimizer
{
	// Parameters
	size_t pass_count = 5;
	bool debug = false;
	bool verbose = false;

	static bool is_stackable(const VMOp &ins); // True if the instruction is mergeable/stackable
	static bool is_nop(const VMOp &ins); // True if the instruction has no visible effect

	PastProgramState past_state;
	bool update_state_debug(Program &program);

	void optimize(Program &program);

	bool erase_nop(Program &program, ProgramIt begin, ProgramIt end);
	bool merge_stackable(Program &program, ProgramIt begin, ProgramIt end);
	bool peephole_optimize(Program &program, ProgramIt begin, ProgramIt end);
	bool balanced_loop_unrolling(Program &program, ProgramIt begin, ProgramIt end);
};
}

#endif // OPTIMIZER_HPP
