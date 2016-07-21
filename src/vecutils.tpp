#include "vecutils.hpp"

#include <iostream>
#include <iomanip>
template<typename Container, typename Iterator>
void replace_subvector(Container& vec, Iterator sub_begin, Iterator sub_end, Container new_sub)
{
	const size_t ext_size = sub_end - sub_begin,
				 new_size = vec.size() - ext_size + new_sub.size();

	if (vec.size() < new_size) // Resize first, keep the end of the vector to the end, insert the new subvector
	{
		const size_t old_size = vec.size();
		vec.resize(new_size); // Resize the vector because we know we won't lose data since we make it bigger
		std::move_backward(sub_begin + ext_size, vec.begin() + old_size, vec.end()); // Move the end of the vector to the new boundaries
		std::move(new_sub.begin(), new_sub.end(), sub_begin); // Insert the subvector
	}
	else // Insert the new subvector, stick the end of the vector to the new subvector, resize
	{
		std::move(begin(new_sub), end(new_sub), sub_begin);
		std::move_backward(sub_end, end(vec), end(vec) - ext_size + new_sub.size());
		vec.resize(new_size);
	}
}
