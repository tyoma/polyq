#pragma once

#include "entry.h"

#include <atomic>
#include <memory>

namespace polyq
{
	template <typename T, typename EntryT = static_entry<T>>
	class circular_buffer
	{
	public:
		typedef EntryT entry_type;

	public:
		circular_buffer(unsigned size = 1000);
		~circular_buffer();

#define POLYQ_MULTI_CV_DEF(cv)\
		template <typename FinalT, typename PostProduceFn>\
		void produce(FinalT cv value, const PostProduceFn &postproduce)

		POLYQ_MULTI_CV_DEF(const &);
		POLYQ_MULTI_CV_DEF(&&);

#undef POLYQ_MULTI_CV_DEF

		template <typename ConsumerFn, typename PreConsumeFn>
		bool consume(const ConsumerFn &consumer, const PreConsumeFn &preconsume);

	private:
		std::atomic<int> _count;
		byte *_start, *_end, *_write, *_read;
	};



	template <typename T, typename EntryT>
	inline circular_buffer<T, EntryT>::circular_buffer(unsigned size)
		: _count(0), _start(new byte[size]), _end(_start + size), _write(_start), _read(_start)
	{	}

	template <typename T, typename EntryT>
	inline circular_buffer<T, EntryT>::~circular_buffer()
	{
		while (_count.fetch_add(-1) > 0)
		{
			entry_type::get(_read, _start, _end);
			entry_type::destroy(_read);
		}
		delete[] _start;
	}

#define POLYQ_MULTI_CV_DEF(cv)\
	template <typename T, typename EntryT>\
	template <typename FinalT, typename PostProduceFn>\
	inline void circular_buffer<T, EntryT>::produce(FinalT cv value, const PostProduceFn &postproduce)\
	{\
		entry_type::create(_write, _start, _end, std::move(value));\
		postproduce(_count.fetch_add(1, std::memory_order_release) + 1);\
	}

	POLYQ_MULTI_CV_DEF(const &);
	POLYQ_MULTI_CV_DEF(&&);

#undef POLYQ_MULTI_CV_DEF

	template <typename T, typename EntryT>
	template <typename ConsumerFn, typename PreConsumeFn>
	inline bool circular_buffer<T, EntryT>::consume(const ConsumerFn &consumer, const PreConsumeFn &preconsume)
	{
		if (!preconsume(_count.fetch_add(-1, std::memory_order_acquire)))
			return _count.fetch_add(1, std::memory_order_relaxed), false;
		consumer(entry_type::get(_read, _start, _end));
		entry_type::destroy(_read);
		return true;
	}
}
