#pragma once

#define __TOKENPASTE(x, y) x ## y
#define TOKENPASTE(x, y) __TOKENPASTE(x, y)
#define AUTO_DESTROY(entry_type, p, begin, end) polyq::tests::destroyer<entry_type> TOKENPASTE(__d, __LINE__)(p, begin, end)

namespace polyq
{
	namespace tests
	{
		class nonassignable
		{
			const nonassignable &operator =(const nonassignable &rhs);
		};

		class instance_counter : nonassignable
		{
		public:
			instance_counter(int &n)
				: _n(n), _inactive(false)
			{	++_n;	}

			instance_counter(const instance_counter &other)
				: _n(other._n), _inactive(false)
			{	++_n;	}

			instance_counter(instance_counter &&other)
				: _n(other._n), _inactive(false)
			{	other._inactive = true;	}

			~instance_counter()
			{	_inactive ? 0 : --_n;	}

		private:
			int &_n;
			bool _inactive;
		};


		template <typename EntryT>
		class destroyer
		{
		public:
			destroyer(typename EntryT::ptr_type p, typename EntryT::ptr_type begin, typename EntryT::ptr_type end)
				: _p(p), _begin(begin), _end(end)
			{	}

			~destroyer()
			{
				EntryT::adjust_get(_p, _begin, _end);
				EntryT::pop(_p);
			}

		private:
			typename EntryT::ptr_type _p, _begin, _end;
		};
	}
}
