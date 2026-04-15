#pragma once

#include <blend2d/blend2d.h>

namespace bl {

struct blStyle {
    BLRgba32 backgroundColor = BLRgba32(0x4A, 0x90, 0xE2, 0xFF);  // Default blue
    BLRgba32 textColor = BLRgba32(0xFF, 0xFF, 0xFF, 0xFF);        // White
    float padding = 16.0f;
};

}  // namespace bl