#ifndef SPAN_HPP
#define SPAN_HPP

#include <algorithm>
#include <cstddef>

template<class It>
class span
{
	It _begin, _end;

public:
	span() = default;

	span(It begin, It end) :
	    _begin(begin),
	    _end(end)
	{}

	template<class T>
	T as_container() const
	{
		return {_begin, _end};
	}

	auto& operator[](size_t off)
	{
		return _begin[off];
	}

	const auto& operator[](size_t off) const
	{
		return _begin[off];
	}

	It begin() const
	{
		return _begin;
	}

	It end() const
	{
		return _end;
	}

	size_t size() const
	{
		return static_cast<size_t>(std::distance(_begin, _end));
	}

	template<class T>
	bool operator==(const T& other) const
	{
		return std::equal(begin(), end(), other.begin(), other.end());
	}

	template<class T>
	bool operator!=(const T& other) const
	{
		return !operator==(other);
	}
};

#endif // SPAN_HPP
