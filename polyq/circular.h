#pragma once

#include <atomic>
#include <memory>
#include <utility>

namespace polyq
{
	typedef unsigned char byte;

	template <typename T, typename EntryT>
	class circular_buffer
	{
	public:
		typedef EntryT entry_type;

	public:
		circular_buffer(size_t size = 1000);
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
		circular_buffer(const circular_buffer &other);
		const circular_buffer &operator =(const circular_buffer &rhs);

	private:
		std::atomic<int> _count;
		typename entry_type::ptr_type _start, _end;
		typename entry_type::ptr_type _write, _read;
	};



	template <typename T, typename EntryT>
	inline circular_buffer<T, EntryT>::circular_buffer(size_t size)
		: _count(0), _start(reinterpret_cast<typename entry_type::ptr_type>(new byte[size * entry_type::element_size])),
			_end(_start + size), _write(_start), _read(_start)
	{	}

	template <typename T, typename EntryT>
	inline circular_buffer<T, EntryT>::~circular_buffer()
	{
		while (_count.fetch_add(-1) > 0)
		{
			entry_type::adjust_get(_read, _start, _end);
			entry_type::pop(_read);
		}
		delete[] reinterpret_cast<byte *>(_start);
	}

#define POLYQ_MULTI_CV_DEF(cv)\
	template <typename T, typename EntryT>\
	template <typename FinalT, typename PostProduceFn>\
	inline void circular_buffer<T, EntryT>::produce(FinalT cv value, const PostProduceFn &postproduce)\
	{\
		entry_type::push(_write, _start, _end, std::move(value));\
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
		consumer(entry_type::adjust_get(_read, _start, _end));
		entry_type::pop(_read);
		return true;
	}
}
