#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP

#include <functional>
#include <memory>
#include <map>
#include <optional>
#include "span.hpp"
#include "bf.hpp"

namespace bf
{
struct OptimizationSequence
{
	const std::vector<uint8_t> seq;
	std::function<Program(span<ProgramIt>)> optimize;
};

class ProgramState
{
	mutable std::optional<std::string> cached_output;

public:
	Program program;
	const size_t id;

	ProgramState(const Program& p_program, const size_t p_id) :
		program{p_program},
		id{p_id}
	{}

	const std::string& get_output() const;
};

struct Optimizer
{
	static constexpr size_t stage_count = 2;

	// Parameters
	size_t pass_count = 5;
	bool debug = false;
	bool verbose = false;

	std::vector<ProgramState> debug_states;
	void update_state_debug(Program &program);
	bool analyze_debug_states();

	void optimize(Program &program);

	bool erase_nop(Program &program, ProgramIt begin, ProgramIt end);
	bool peephole_optimize_for(Program& program, ProgramIt begin, ProgramIt end, const std::vector<OptimizationSequence>& optimizers);

	// Stage 1
	bool merge_stackable(Program &program, ProgramIt begin, ProgramIt end);
	bool stage1_peephole_optimize(Program &program, ProgramIt begin, ProgramIt end);
	bool balanced_loop_unrolling(Program &program, ProgramIt begin, ProgramIt end);

	// Stage 2 - involves add-offset and set-offset. it is performed in a separate stage as to simplify stage 1 optimizations.
	bool stage2_peephole_optimize(Program &program, ProgramIt begin, ProgramIt end);
	bool simplify_offset_ops(Program& program, ProgramIt begin, ProgramIt end);
};

struct OptimizerTask
{
	bool (Optimizer::*callback)(Program&, ProgramIt, ProgramIt);
	const std::string_view name;
};
}

#endif // OPTIMIZER_HPP
