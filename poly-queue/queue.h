#pragma once

#include "fifo.h"

//#include <atomic>

namespace agge
{
	template <typename BaseT>
	class queue
	{
	public:
		template <typename T, typename PostProduceFn>
		void produce(T &value, const PostProduceFn &postproduce);

		template <typename ConsumerFn, typename PreConsumeFn>
		void consume(const ConsumerFn &consumer, const PreConsumeFn &preconsume);

   private:
	};



	template <typename BaseT>
	template <typename ConsumerFn, typename PreConsumeFn>
	inline void queue<BaseT>::consume(const ConsumerFn &/*consumer*/, const PreConsumeFn &preconsume)
	{
		preconsume(-1);
	}
}
