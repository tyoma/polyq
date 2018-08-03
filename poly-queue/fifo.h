#pragma once

#include "entry.h"

#pragma warning(push)
#pragma warning(disable: 4702)

namespace pq
{
	typedef unsigned int count_t;

	template < typename BaseT, typename EntryT = poly_entry<BaseT> >
	class fifo
	{
	public:
		typedef EntryT entry_type;

	public:
		fifo(count_t byte_size);
		~fifo();

		bool empty() const throw();

		template <typename T>
		void push_back(const T &value);

		void pop_front() throw();

		BaseT &front() throw();

	private:
		fifo(const fifo &other);
		const fifo &operator =(const fifo &rhs);

		void grow_by(count_t sz);
		count_t capacity() const throw();
		void destroy() throw();

	private:
		entry_type *_write, *_read;
		void *_start, *_limit;
	};



	template <typename BaseT, typename EntryT>
	inline fifo<BaseT, EntryT>::fifo(count_t byte_size)
		: _write(0), _read(0), _start(0), _limit(0)
	{	grow_by(byte_size);	}

	template <typename BaseT, typename EntryT>
	inline fifo<BaseT, EntryT>::~fifo()
	{	destroy();	}

	template <typename BaseT, typename EntryT>
	inline bool fifo<BaseT, EntryT>::empty() const throw()
	{	return _read == _write;	}

	template <typename BaseT, typename EntryT>
	template <typename T>
	inline void fifo<BaseT, EntryT>::push_back(const T &value)
	{
		if (static_cast<uint8_t *>(_limit) - reinterpret_cast<uint8_t *>(_write) < entry_type::size<T>())
			grow_by(entry_type::size<T>());
		entry_type::create(_write, value);
	}

	template <typename BaseT, typename EntryT>
	inline void fifo<BaseT, EntryT>::pop_front() throw()
	{
		entry_type *current = _read, *next = current->next();

		_read = next == _write ? _write = static_cast<entry_type *>(_start) : next;
		current->~entry_type();
	}

	template <typename BaseT, typename EntryT>
	inline BaseT &fifo<BaseT, EntryT>::front() throw()
	{	return _read->get();	}

	template <typename BaseT, typename EntryT>
	inline void fifo<BaseT, EntryT>::grow_by(count_t sz)
	{
		count_t new_size = capacity() + sz;
		void *start = new uint8_t[new_size];
		entry_type *write = static_cast<entry_type *>(start);

		for (entry_type *i = _read; i != _write; i = i->next())
			i->clone(write);
		destroy();
		_start = start;
		_write = write;
		_read = static_cast<entry_type *>(start);
		_limit = static_cast<uint8_t *>(_start) + new_size;
	}

	template <typename BaseT, typename EntryT>
	inline count_t fifo<BaseT, EntryT>::capacity() const throw()
	{	return static_cast<count_t>(static_cast<uint8_t *>(_limit) - static_cast<uint8_t *>(_start));	}

	template <typename BaseT, typename EntryT>
	inline void fifo<BaseT, EntryT>::destroy() throw()
	{
		while (!empty())
			pop_front();
		delete []static_cast<uint8_t *>(_start);
	}
}

#pragma warning(pop)
