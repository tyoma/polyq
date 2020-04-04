#pragma once

#include <utility>

namespace polyq
{
	typedef unsigned char byte;

#pragma pack(push, 1)
	struct poly_entry_descriptor
	{
		typedef unsigned short int size_t;
		typedef short int offset_t;

		static poly_entry_descriptor &from(void *ptr) throw();
		static poly_entry_descriptor &from(byte *&ptr, byte *start, byte *end) throw();

		template <typename T>
		T &get_object() throw();

		size_t size;
		offset_t base_offset;
	};
#pragma pack(pop)

	template <typename T>
	class poly_entry
	{
	public:
		enum { element_size = 1, };
		typedef byte *ptr_type;

	public:
#define POLYQ_MULTI_CV_DEF(cv)\
		template <typename FinalT>\
		static void push(ptr_type &at, ptr_type start, ptr_type end, FinalT cv value)\
		{\
			poly_entry_descriptor *d = prepare_slot(at, start, end, value);\
			post_construct<FinalT>(at, d, new(d + 1) FinalT(std::move(value)));\
		}

		POLYQ_MULTI_CV_DEF(const &);
		POLYQ_MULTI_CV_DEF(&&);

#undef POLYQ_MULTI_CV_DEF

		static T &adjust_get(ptr_type &at, ptr_type start, ptr_type end) throw()
		{	return poly_entry_descriptor::from(at, start, end).get_object<T>();	}

		static void pop(ptr_type &at_adjusted) throw()
		{
			poly_entry_descriptor &d = poly_entry_descriptor::from(at_adjusted);

			d.get_object<T>().~T();
			at_adjusted += d.size;
		}

	private:
		template <typename FinalT>
		static poly_entry_descriptor *prepare_slot(ptr_type &at, ptr_type start, ptr_type end, const FinalT &value) throw()
		{
			struct type_check_t { type_check_t(const T *) {	} } type_check(&value);

			if (end - at < sizeof(poly_entry_descriptor))
				at = start;
			else if (end - at < sizeof(poly_entry_descriptor) + sizeof(FinalT))
				poly_entry_descriptor::from(at).size = 0, at = start;
			return &poly_entry_descriptor::from(at);
		}

		template <typename FinalT>
		static void post_construct(ptr_type &at, poly_entry_descriptor *d, const T *object) throw()
		{
			d->base_offset = static_cast<poly_entry_descriptor::offset_t>(reinterpret_cast<const byte *>(object)
				- reinterpret_cast<const byte *>(d + 1));
			at += d->size = sizeof(poly_entry_descriptor) + sizeof(FinalT);
		}
	};




	inline poly_entry_descriptor &poly_entry_descriptor::from(void *ptr) throw()
	{	return *static_cast<poly_entry_descriptor *>(ptr);	}

	inline poly_entry_descriptor &poly_entry_descriptor::from(byte *&at, byte *start, byte *end) throw()
	{
		if (end - at < sizeof(poly_entry_descriptor))
			return at = start, from(start);
		poly_entry_descriptor &d = from(at);
		return !d.size ? from(at = start) : d;
	}

	template <typename T>
	inline T &poly_entry_descriptor::get_object() throw()
	{	return *reinterpret_cast<T *>(reinterpret_cast<byte *>(this + 1) + base_offset);	}
}
