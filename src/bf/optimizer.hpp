#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP

#include <functional>
#include <memory>
#include <map>
#include "span.hpp"
#include "bf.hpp"

namespace bf
{
struct OptimizationSequence
{
	const std::vector<uint8_t> seq;
	std::function<Program(span<ProgramIt>)> optimize;
};

struct CellOpSingle
{
	VMOp op;
	bool any = false;

	bool apply(const VMOp& instruction);
	void simplify();
	void repeat(int n);
};

struct CellOp
{
	std::vector<CellOpSingle> ops;

	bool apply(const VMOp& instruction);
	void simplify();
	void repeat(size_t n);
};

struct CellOpMap
{
	std::map<int, CellOp> operation_map;
	int offset_from_start = 0;
	bool correct = false;
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

	// Generates a map binding offsets from the current cell to cell operations.
	CellOpMap make_operation_map(span<ProgramIt> range);

	PastProgramState past_state;
	bool update_state_debug(Program &program);

	void optimize(Program &program);

	bool erase_nop(Program &program, ProgramIt begin, ProgramIt end);
	bool merge_stackable(Program &program, ProgramIt begin, ProgramIt end);
	bool peephole_optimize(Program &program, ProgramIt begin, ProgramIt end);
	bool balanced_loop_unrolling(Program &program, ProgramIt begin, ProgramIt end);
	bool balanced_loop_unrolling_v2(Program &program, ProgramIt begin, ProgramIt end);
};
}

#endif // OPTIMIZER_HPP
