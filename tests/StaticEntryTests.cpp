#include <polyq/static_entry.h>

#include "helpers.h"

#include <ut/assert.h>
#include <ut/test.h>

using namespace std;

namespace polyq
{
	namespace tests
	{
		begin_test_suite( StaticEntryTests )
			enum {
				int_buffer_size = 3,
				object_buffer_size = 4,
			};

			int *int_buffer, *int_buffer_end;
			instance_counter *object_buffer, *object_buffer_end;

			init( InitBuffers )
			{
				int_buffer = static_cast<int *>(malloc(int_buffer_size * sizeof(int)));
				int_buffer_end = int_buffer + int_buffer_size;

				object_buffer = static_cast<instance_counter *>(malloc(object_buffer_size * sizeof(instance_counter)));
				object_buffer_end = object_buffer + object_buffer_size;
			}

			teardown( FreeBuffers )
			{
				free(int_buffer);
				free(object_buffer);
			}


			test( TrivialEntryIsConstructedByPush )
			{
				// INIT
				int *p = int_buffer;

				// ACT
				int *v1 = p;
				static_entry<int>::push(p, int_buffer, int_buffer_end, 3171223);
				AUTO_DESTROY(static_entry<int>, v1, int_buffer, int_buffer_end);

				// ASSERT
				assert_equal(v1 + 1, p);
				assert_equal(3171223, *v1);

				// ACT
				int *v2 = p;
				static_entry<int>::push(p, int_buffer, int_buffer_end, 17171923);
				AUTO_DESTROY(static_entry<int>, v2, int_buffer, int_buffer_end);

				// ASSERT
				assert_equal(v2 + 1, p);
				assert_equal(17171923, *v2);
			}


			test( WritePointerIsWrappedOnReachingEndOfRegion )
			{
				// INIT
				int *p = int_buffer;

				int *v1 = p;
				static_entry<int>::push(p, int_buffer, int_buffer_end, 1);
				AUTO_DESTROY(static_entry<int>, v1, int_buffer, int_buffer_end);
				int *v2 = p;
				static_entry<int>::push(p, int_buffer, int_buffer_end, 1);
				AUTO_DESTROY(static_entry<int>, v2, int_buffer, int_buffer_end);

				// ACT
				int *v3 = p;
				static_entry<int>::push(p, int_buffer, int_buffer_end, 1);
				AUTO_DESTROY(static_entry<int>, v3, int_buffer, int_buffer_end);

				// ASSERT
				assert_equal(int_buffer, p);
			}


			test( GetReturnsTheObjectAtReaderPointer )
			{
				// INIT
				int *p = int_buffer;

				int *v1 = p;
				static_entry<int>::push(p, int_buffer, int_buffer_end, 1);
				AUTO_DESTROY(static_entry<int>, v1, int_buffer, int_buffer_end);
				int *v2 = p;
				static_entry<int>::push(p, int_buffer, int_buffer_end, 1);
				AUTO_DESTROY(static_entry<int>, v2, int_buffer, int_buffer_end);
				int *v3 = p;
				static_entry<int>::push(p, int_buffer, int_buffer_end, 1);
				AUTO_DESTROY(static_entry<int>, v3, int_buffer, int_buffer_end);

				// ACT / ASSERT
				assert_equal(v1, &static_entry<int>::adjust_get(v1, int_buffer, int_buffer_end));
				assert_equal(v2, &static_entry<int>::adjust_get(v2, int_buffer, int_buffer_end));
				assert_equal(v3, &static_entry<int>::adjust_get(v3, int_buffer, int_buffer_end));
			}


			test( DestroyAdvancesTheReadPointer )
			{
				// INIT
				int *p = int_buffer;

				int *v1 = p;
				static_entry<int>::push(p, int_buffer, int_buffer_end, 1);
				int *v2 = p;
				static_entry<int>::push(p, int_buffer, int_buffer_end, 1);

				// ACT
				p = v1;
				static_entry<int>::pop(p);

				// ASSERT
				assert_equal(v1 + 1, p);

				// ACT
				p = v2;
				static_entry<int>::pop(p);

				// ASSERT
				assert_equal(v2 + 1, p);
			}


			test( ReadPointerIsWrappedOnReachingEndOfRegionOnAttemptedRead )
			{
				// INIT
				int *p = int_buffer + 1, *v = p;

				static_entry<int>::push(p, int_buffer, int_buffer_end, 1);
				static_entry<int>::push(p, int_buffer, int_buffer_end, 1);
				static_entry<int>::push(p, int_buffer, int_buffer_end, 17112);

				static_entry<int>::adjust_get(v, int_buffer, int_buffer_end);
				static_entry<int>::pop(v);
				static_entry<int>::adjust_get(v, int_buffer, int_buffer_end);
				static_entry<int>::pop(v);

				// ACT / ASSERT
				assert_equal(17112, static_entry<int>::adjust_get(v, int_buffer, int_buffer_end));
				assert_equal(int_buffer, v);
				static_entry<int>::pop(v);
			}


			test( ObjectIsCopyConstructedOnPush )
			{
				// INIT
				int n = 0;
				instance_counter ref(n);
				instance_counter *p = object_buffer;

				// ACT
				instance_counter *v1 = p;
				static_entry<instance_counter>::push(p, object_buffer, object_buffer_end, ref);
				AUTO_DESTROY(static_entry<instance_counter>, v1, object_buffer, object_buffer_end);

				// ASSERT
				assert_equal(2, n);

				// ACT
				instance_counter *v2 = p;
				static_entry<instance_counter>::push(p, object_buffer, object_buffer_end, ref);
				AUTO_DESTROY(static_entry<instance_counter>, v2, object_buffer, object_buffer_end);

				// ASSERT
				assert_equal(3, n);
			}


			test( ObjectIsMoveConstructedOnPush )
			{
				// INIT
				int n = 0;
				instance_counter ref(n);
				instance_counter *p = object_buffer;

				// ACT
				instance_counter *v1 = p;
				static_entry<instance_counter>::push(p, object_buffer, object_buffer_end, move(ref));
				AUTO_DESTROY(static_entry<instance_counter>, v1, object_buffer, object_buffer_end);

				// ASSERT
				assert_equal(1, n);
			}


			test( ObjectIsDestroyedOnDestroy )
			{
				// INIT
				int n = 0;
				instance_counter *p = object_buffer;

				instance_counter *v1 = p;
				static_entry<instance_counter>::push(p, object_buffer, object_buffer_end, instance_counter(n));
				instance_counter *v2 = p;
				static_entry<instance_counter>::push(p, object_buffer, object_buffer_end, instance_counter(n));

				// ACT
				static_entry<instance_counter>::pop(v1);

				// ASSERT
				assert_equal(1, n);

				// ACT
				static_entry<instance_counter>::pop(v2);

				// ASSERT
				assert_equal(0, n);
			}
		end_test_suite
	}
}
