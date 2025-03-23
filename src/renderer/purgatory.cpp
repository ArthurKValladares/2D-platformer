#include "purgatory.h"
#include "renderer.h"

void Purgatory::destroy(Renderer* renderer) {
    for (std::vector<Buffer>& frame_buffers : buffers) {
        for (Buffer& buffer : frame_buffers) {
            buffer.destroy(renderer->get_allocator());
        }
    }
}