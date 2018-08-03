#pragma once

namespace pq
{
	typedef unsigned char uint8_t;
	typedef unsigned char byte;
	typedef unsigned short int uint16_t;

	template <typename T>
	class static_entry
	{
	public:
		static void create(uint8_t *&at, uint8_t *start, uint8_t *end, const T &value);
		static void destroy(uint8_t *&at, uint8_t *start, uint8_t *end) throw();
		static T &get(uint8_t *at) throw();
	};


	struct poly_entry_descriptor
	{
		uint16_t size;
	};

	template <typename T>
	class poly_entry
	{
	public:
		template <typename FinalT>
		static void create(uint8_t *&at, uint8_t *start, uint8_t *end, const FinalT &value);
		static void destroy(uint8_t *&at, uint8_t *start, uint8_t *end) throw();
		static T &get(uint8_t *at) throw();
	};



	template <typename T>
	inline void static_entry<T>::create(uint8_t *&at, uint8_t *start, uint8_t *end, const T &value)
	{
		new(at) T(value);
		if (at + 2 * sizeof(T) > end)
			at = start;
		else
			at += sizeof(T);
	}

	template <typename T>
	inline void static_entry<T>::destroy(uint8_t *&at, uint8_t *start, uint8_t *end) throw()
	{
		get(at).~T();
		if (at + 2 * sizeof(T) > end)
			at = start;
		else
			at += sizeof(T);
	}

	template <typename T>
	inline T &static_entry<T>::get(uint8_t *at) throw()
	{	return *reinterpret_cast<T *>(at);	}


	template <typename T>
	template <typename FinalT>
	inline void poly_entry<T>::create(uint8_t *&at, uint8_t *start, uint8_t *end, const FinalT &value)
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

		poly_entry_descriptor *d = reinterpret_cast<poly_entry_descriptor *>(at);

		new(d + 1) FinalT(value);
		at += d->size = sizeof(poly_entry_descriptor) + sizeof(FinalT);
	}

	template <typename T>
	inline void poly_entry<T>::destroy(uint8_t *&at, uint8_t *start, uint8_t * end) throw()
	{
		if (end - at < sizeof(poly_entry_descriptor) || !reinterpret_cast<const poly_entry_descriptor *>(at)->size)
			at = start;

		const poly_entry_descriptor *d = reinterpret_cast<const poly_entry_descriptor *>(at);

		get(at).~T();
		at += d->size;
	}

	template <typename T>
	inline T &poly_entry<T>::get(uint8_t *at) throw()
	{
		poly_entry_descriptor *d = reinterpret_cast<poly_entry_descriptor *>(at);
		return *reinterpret_cast<T *>(d + 1);
	}
}

