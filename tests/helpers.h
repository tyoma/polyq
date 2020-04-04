#pragma once

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
	}
}
