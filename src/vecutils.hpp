#ifndef VECUTILS_HPP
#define VECUTILS_HPP

template<typename Container, typename Iterator>
void replace_subvector_smaller(Container& vec, Iterator sub_begin, Iterator sub_end, Container new_sub);

#include "vecutils.tpp"

#endif
