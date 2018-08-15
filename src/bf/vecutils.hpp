#ifndef VECUTILS_HPP
#define VECUTILS_HPP

#include <algorithm>

template<typename SrcC, typename DstC, typename Iterator>
void move_range(DstC& target, Iterator begin, Iterator end, SrcC&& replacement)
{
	auto range_diff = long(replacement.size()) - std::distance(begin, end);

	if (range_diff > 0)
	{
		target.resize(target.size() + range_diff);
		std::move_backward(end, target.end(), end + range_diff);
	}
	else if (range_diff < 0)
	{
		std::move(end, target.end(), end + range_diff);
		target.resize(target.size() + range_diff);
	}

	std::move(replacement.begin(), replacement.end(), begin);
}

//! Moves 'replacement' to 'target' in range {begin, end} but if the replacement is smaller than the source range, the target is not
//! shrinked, but default-initialized values are left in the "gap" instead.
template<typename SrcC, typename DstC, typename Iterator>
void move_range_no_shrink(DstC& target, Iterator begin, Iterator end, SrcC&& replacement)
{
	auto range_diff = long(replacement.size()) - std::distance(begin, end);

	if (range_diff >= 0)
	{
		move_range(target, begin, end, replacement);
		return;
	}

	std::move(replacement.begin(), replacement.end(), begin);
	std::fill(begin + replacement.size(), end, typename DstC::value_type{});
}

#endif
