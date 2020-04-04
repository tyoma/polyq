#pragma once

#include <utility>

namespace polyq
{
	typedef unsigned char byte;
	typedef unsigned short int uint16_t;
	typedef unsigned short int int16_t;

	template <typename T>
	class static_entry
	{
	public:
#define POLYQ_MULTI_CV_DEF(cv)\
		static void create(byte *&at, byte *start, byte *end, T cv value);

		POLYQ_MULTI_CV_DEF(const &);
		POLYQ_MULTI_CV_DEF(&&);

#undef POLYQ_MULTI_CV_DEF

		static void destroy(byte *&at) throw();
		static T &get(byte *&at, byte *start, byte *end) throw();
	};


#pragma pack(push, 1)
	struct poly_entry_descriptor
	{
		uint16_t size;
		int16_t base_offset;
	};
#pragma pack(pop)

	template <typename T>
	class poly_entry
	{
	public:
#define POLYQ_MULTI_CV_DEF(cv)\
		template <typename FinalT>\
		static void create(byte *&at, byte *start, byte *end, FinalT cv value)

		POLYQ_MULTI_CV_DEF(const &);
		POLYQ_MULTI_CV_DEF(&&);

#undef POLYQ_MULTI_CV_DEF

		static void destroy(byte *&at) throw();
		static T &get(byte *&at, byte *start, byte *end) throw();

	private:
		template <typename FinalT>
		static poly_entry_descriptor *prepare_slot(byte *&at, byte *start, byte *end, const FinalT &value);

		template <typename FinalT>
		static void post_construct(byte *&at, poly_entry_descriptor *d, const T *object);
	};



#define POLYQ_MULTI_CV_DEF(cv)\
	template <typename T>\
	inline void static_entry<T>::create(byte *&at, byte *start, byte *end, T cv value)\
	{\
		new(at) T(std::move(value));\
		at = at + 2 * sizeof(T) > end ? start : at + sizeof(T);\
	}

	POLYQ_MULTI_CV_DEF(const &);
	POLYQ_MULTI_CV_DEF(&&);

#undef POLYQ_MULTI_CV_DEF

	template <typename T>
	inline void static_entry<T>::destroy(byte *&at) throw()
	{
		reinterpret_cast<T *>(at)->~T();
		at += sizeof(T);
	}

	template <typename T>
	inline T &static_entry<T>::get(byte *&at, byte *start, byte *end) throw()
	{
		if (end - at < sizeof(T))
			at = start;
		return *reinterpret_cast<T *>(at);
	}


	template <typename T>
	template <typename FinalT>
	inline poly_entry_descriptor *poly_entry<T>::prepare_slot(byte *&at, byte *start, byte *end,
		const FinalT &value)
	{
		struct type_check_t { type_check_t(const T *) {	} } type_check(&value);

		if (end - at < sizeof(poly_entry_descriptor))
		{
			at = start;
		}
		else if (end - at < sizeof(poly_entry_descriptor) + sizeof(FinalT))
		{
			const poly_entry_descriptor zero = {};

			*reinterpret_cast<poly_entry_descriptor *>(at) = zero;
			at = start;
		}

		return reinterpret_cast<poly_entry_descriptor *>(at);
	}

	template <typename T>
	template <typename FinalT>
	inline void poly_entry<T>::post_construct(byte *&at, poly_entry_descriptor *d, const T *object)
	{
		d->base_offset = static_cast<int16_t>(reinterpret_cast<const byte *>(object) - reinterpret_cast<byte *>(d + 1));
		at += d->size = sizeof(poly_entry_descriptor) + sizeof(FinalT);
	}

#define POLYQ_MULTI_CV_DEF(cv)\
	template <typename T>\
	template <typename FinalT>\
	inline void poly_entry<T>::create(byte *&at, byte *start, byte *end, FinalT cv value)\
	{\
		poly_entry_descriptor *d = prepare_slot(at, start, end, value);\
		post_construct<FinalT>(at, d, new(d + 1) FinalT(std::move(value)));\
	}

	POLYQ_MULTI_CV_DEF(const &);
	POLYQ_MULTI_CV_DEF(&&);

#undef POLYQ_MULTI_CV_DEF

	template <typename T>
	inline void poly_entry<T>::destroy(byte *&at) throw()
	{
		poly_entry_descriptor *d = reinterpret_cast<poly_entry_descriptor *>(at);

		reinterpret_cast<T *>(reinterpret_cast<byte *>(d + 1) + d->base_offset)->~T();
		at += d->size;
	}

	template <typename T>
	inline T &poly_entry<T>::get(byte *&at, byte *start, byte *end) throw()
	{
		if (end - at < sizeof(poly_entry_descriptor) || !reinterpret_cast<const poly_entry_descriptor *>(at)->size)
			at = start;

		poly_entry_descriptor *d = reinterpret_cast<poly_entry_descriptor *>(at);
		return *reinterpret_cast<T *>(reinterpret_cast<byte *>(d + 1) + d->base_offset);
	}
}
