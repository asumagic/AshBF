#ifndef VECUTILS_HPP
#define VECUTILS_HPP

#include <algorithm>

template<typename SrcC, typename DstC, typename Iterator>
void move_range(SrcC& target, Iterator begin, Iterator end, DstC&& replacement)
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

#endif
