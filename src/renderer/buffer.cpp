#include "buffer.h"

void Buffer::destroy(VmaAllocator allocator) {
    vmaDestroyBuffer(allocator, raw, allocation);
}