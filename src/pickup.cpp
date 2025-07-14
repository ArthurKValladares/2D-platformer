#include "pickup.h"

Renderable Pickup::draw() {
    Renderable renderable;
    renderable.push_child(colored_quad(
        location,
        texture_id(texture),
        color
    ));
    return renderable;
}