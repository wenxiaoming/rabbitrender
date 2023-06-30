//add by Kevin.Wen

#include "atomic.h"

int32_t android_atomic_inc(volatile int32_t* addr)
{
	*addr++;
	return *addr;
}

int32_t android_atomic_dec(volatile int32_t* addr)
{
	int temp = *addr;
	*addr--;
	return temp;
}

