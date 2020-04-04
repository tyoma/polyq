#pragma once

#include <utility>

namespace polyq
{
	template <typename T>
	class static_entry
	{
	public:
		enum { element_size = sizeof(T), };
		typedef T *ptr_type;

	public:
#define POLYQ_MULTI_CV_DEF(cv)\
		static void push(ptr_type &at, ptr_type start, ptr_type end, T cv value)\
		{\
			new(at) T(std::move(value));\
			if (++at == end)\
				at = start;\
		}

		POLYQ_MULTI_CV_DEF(const &);
		POLYQ_MULTI_CV_DEF(&&);

#undef POLYQ_MULTI_CV_DEF

		static T &adjust_get(ptr_type &at, ptr_type start, ptr_type end) throw()
		{	return *(at == end ? at = start : at);	}

		static void pop(ptr_type &at_adjusted) throw()
		{	at_adjusted++->~T();	}
	};
}
