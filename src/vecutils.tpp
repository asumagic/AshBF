#include "vecutils.hpp"

#include <iostream>

template<typename Container, typename Iterator>
void replace_subvector_smaller(Container& vec, Iterator sub_begin, Iterator sub_end, Container new_sub)
{
	const size_t ext_size = sub_end - sub_begin,
				 new_size = vec.size() - ext_size + new_sub.size();

	std::move(begin(new_sub), end(new_sub), sub_begin); // Replace the subvector
	std::move(sub_end, end(vec), sub_end - ext_size + new_sub.size());
	vec.resize(new_size); // Shorten up the vector
}
