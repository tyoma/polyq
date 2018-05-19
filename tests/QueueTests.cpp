#include <poly-queue/queue.h>

#include <ut/assert.h>
#include <ut/test.h>

using namespace std;

namespace agge
{
	namespace tests
	{
		namespace
		{
			class Foo
			{
			};
		}

		begin_test_suite( QueueTests )
			static void dummy_consume_foo(Foo &) {	}

			static void dummy_consume_int(int &) {	}

			struct pre_consume
			{
				void operator ()(int records) const
				{	records_log.push_back(records);	}

				mutable vector<int> records_log;
			};

			test( PreConsumeCallbackIsCalledWithMinusOneForEmptySequence )
			{
				// INIT
				queue<Foo> q1;
				queue<int> q2;
				pre_consume pc;

				// ACT
				q1.consume(&dummy_consume_foo, pc);

				// ASSERT
				int reference1[] = { -1, };

				assert_equal(reference1, pc.records_log);

				// ACT
				q2.consume(&dummy_consume_int, pc);

				// ASSERT
				int reference2[] = { -1, -1, };

				assert_equal(reference2, pc.records_log);
			}
		end_test_suite
	}
}
