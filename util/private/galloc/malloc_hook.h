#pragma once

#include <util/memory/profile.h>

class MallocHook {
	public:
		typedef void (*NewHook)(void* ptr, size_t size);
		typedef void (*DeleteHook)(void* ptr);

		inline static NewHook GetNewHook() throw () {
			return TAllocatorHooks::NewHook();
		}

		inline static NewHook SetNewHook(NewHook hook) throw () {
			NewHook result = GetNewHook();
            TAllocatorHooks::SetNewHook(hook);

			return result;
		}

		inline static void InvokeNewHook(void* p, size_t s) {
            NewHook hook = GetNewHook();

			if (hook) {
				(*hook)(p, s);
			}
		}

		inline static DeleteHook GetDeleteHook() throw () {
			return TAllocatorHooks::DelHook();
		}

		inline static DeleteHook SetDeleteHook(DeleteHook hook) throw () {
			DeleteHook result = GetDeleteHook();
            TAllocatorHooks::SetDelHook(hook);

			return result;
		}

		inline static void InvokeDeleteHook(void* p) {
            DeleteHook hook = GetDeleteHook();

			if (hook) {
				(*hook)(p);
			}
		}
};
