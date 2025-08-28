#pragma once
#include <cstdint>

namespace QtDocxTemplate { namespace util {

// EMU conversion utilities (English Metric Units) 1 inch = 914400 EMU
// Default DPI per charter: 96
inline std::uint64_t pixelsToEmu(int px, double dpi = 96.0) {
    // EMU per pixel = 914400 / dpi
    return static_cast<std::uint64_t>((914400.0 / dpi) * px + 0.5);
}

}} // namespace QtDocxTemplate::util
