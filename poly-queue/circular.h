#pragma once

#include "entry.h"

#include <atomic>
#include <memory>

namespace pq
{
	template <typename T, typename EntryT = static_entry<T>>
	class circular_buffer
	{
	public:
		typedef EntryT entry_type;

	public:
		circular_buffer(unsigned size = 1000);
		~circular_buffer();

		template <typename FinalT, typename PostProduceFn>
		void produce(const FinalT &value, const PostProduceFn &postproduce);

		template <typename ConsumerFn, typename PreConsumeFn>
		bool consume(const ConsumerFn &consumer, const PreConsumeFn &preconsume);

	private:
		std::atomic<int> _count;
		uint8_t *_start, *_end, *_write, *_read;
	};



	template <typename T, typename EntryT>
	inline circular_buffer<T, EntryT>::circular_buffer(unsigned size)
		: _count(0), _start(new uint8_t[size]), _end(_start + size), _write(_start), _read(_start)
	{	}

	template <typename T, typename EntryT>
	inline circular_buffer<T, EntryT>::~circular_buffer()
	{
		while (_count--)
			entry_type::destroy(_read, _start, _end);
		delete[] _start;
	}

	template <typename T, typename EntryT>
	template <typename FinalT, typename PostProduceFn>
	inline void circular_buffer<T, EntryT>::produce(const FinalT &value, const PostProduceFn &postproduce)
	{
		entry_type::create(_write, _start, _end, value);
		postproduce(_count.fetch_add(1, std::memory_order_release));
	}

	template <typename T, typename EntryT>
	template <typename ConsumerFn, typename PreConsumeFn>
	inline bool circular_buffer<T, EntryT>::consume(const ConsumerFn &consumer, const PreConsumeFn &preconsume)
	{
		if (!preconsume(_count.fetch_sub(1, std::memory_order_acquire)))
			return _count.fetch_add(1, std::memory_order_release), false;
		consumer(entry_type::get(_read));
		entry_type::destroy(_read, _start, _end);
		return true;
	}
}