#pragma once

#include <atomic>
#include <memory>
#include <utility>

namespace polyq
{
	typedef unsigned char byte;

	template < typename T, typename EntryT, typename CounterT = std::atomic<int> >
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

		template <typename T2, typename MemOrderT>
		static T2 fetch_add(T2 &lhs, T2 rhs, MemOrderT order);

		template <typename T2, typename MemOrderT>
		static T2 fetch_add(std::atomic<T2> &lhs, T2 rhs, MemOrderT order);

	private:
		CounterT _count;
		typename entry_type::ptr_type _start, _end;
		typename entry_type::ptr_type _write, _read;
	};



	template <typename T, typename EntryT, typename CounterT>
	inline circular_buffer<T, EntryT, CounterT>::circular_buffer(size_t size)
		: _count(0), _start(reinterpret_cast<typename entry_type::ptr_type>(new byte[size * entry_type::element_size])),
			_end(_start + size), _write(_start), _read(_start)
	{	}

	template <typename T, typename EntryT, typename CounterT>
	inline circular_buffer<T, EntryT, CounterT>::~circular_buffer()
	{
		while (fetch_add(_count, -1, std::memory_order_relaxed) > 0)
		{
			entry_type::adjust_get(_read, _start, _end);
			entry_type::pop(_read);
		}
		delete[] reinterpret_cast<byte *>(_start);
	}

#define POLYQ_MULTI_CV_DEF(cv)\
	template <typename T, typename EntryT, typename CounterT>\
	template <typename FinalT, typename PostProduceFn>\
	inline void circular_buffer<T, EntryT, CounterT>::produce(FinalT cv value, const PostProduceFn &postproduce)\
	{\
		entry_type::push(_write, _start, _end, std::move(value));\
		postproduce(fetch_add(_count, 1, std::memory_order_release) + 1);\
	}

	POLYQ_MULTI_CV_DEF(const &);
	POLYQ_MULTI_CV_DEF(&&);

#undef POLYQ_MULTI_CV_DEF

	template <typename T, typename EntryT, typename CounterT>
	template <typename ConsumerFn, typename PreConsumeFn>
	inline bool circular_buffer<T, EntryT, CounterT>::consume(const ConsumerFn &consumer, const PreConsumeFn &preconsume)
	{
		if (!preconsume(fetch_add(_count, -1, std::memory_order_acquire)))
			return fetch_add(_count, 1, std::memory_order_relaxed), false;
		consumer(entry_type::adjust_get(_read, _start, _end));
		entry_type::pop(_read);
		return true;
	}

	template <typename T, typename EntryT, typename CounterT>
	template <typename T2, typename MemOrderT>
	inline T2 circular_buffer<T, EntryT, CounterT>::fetch_add(T2 &lhs, T2 rhs, MemOrderT /*order*/)
	{
		T2 previous = lhs;
		return lhs += rhs, previous;
	}

	template <typename T, typename EntryT, typename CounterT>
	template <typename T2, typename MemOrderT>
	inline T2 circular_buffer<T, EntryT, CounterT>::fetch_add(std::atomic<T2> &lhs, T2 rhs, MemOrderT order)
	{	return lhs.fetch_add(rhs, order);	}
}
