//add by Kevin.Wen

#include <cutils/memory.h>

/* size is given in bytes and must be multiple of 2 */
void android_memset16(uint16_t* dst, uint16_t value, size_t size)
{
    uint16_t* temp_dst = dst;
    size_t index = 0;
    size = size>>1;
    for(; index < size; index++)
    	*temp_dst++ = value;
}

/* size is given in bytes and must be multiple of 4 */
void android_memset32(uint32_t* dst, uint32_t value, size_t size)
{
    uint32_t* temp_dst = dst;
    size_t index = 0;
    size = size>>2;
    for(; index < size; index++)
    	*temp_dst++ = value;
}
