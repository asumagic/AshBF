#include "vecutils.hpp"

template<typename Container, typename Iterator>
void replace_subvector(Container& vec, Iterator sub_begin, Iterator sub_end, Container new_sub)
{
	const size_t ext_size = sub_end - sub_begin,
				 new_size = vec.size() - ext_size + new_sub.size();

	if (vec.size() < new_size)
	{
		const size_t old_size = vec.size();
		vec.resize(new_size); // Resize the vector because we know we won't lose data since we make it bigger
		std::move_backward(sub_begin + ext_size, vec.begin() + old_size, vec.end()); // Move the end of the vector to the new boundaries
		std::move(new_sub.begin(), new_sub.end(), sub_begin); // Replace the subvector
	}
	else
	{
		std::move(begin(new_sub), end(new_sub), sub_begin); // Replace the subvector
		std::move_backward(sub_end, end(vec), end(vec) - ext_size + new_sub.size()); // Move the end of the vector to fill the gap
		vec.resize(new_size); // Shorten up the vector
	}
}
