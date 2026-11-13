/**
 * @file Utils.hpp
 * @brief Generic programming utilities for PasswordGuard.
 */
#pragma once

#include <vector>
#include <array>
#include <algorithm>
#include <stdexcept>

namespace tk {

/**
 * @brief Computes the minimum and maximum elements of a non-empty vector.
 * @tparam T Comparable element type.
 * @param vec Input vector of elements.
 * @return std::array<T, 2> containing {min_element, max_element}.
 * @throws std::invalid_argument if the input vector is empty.
 */
template<typename T>
std::array<T, 2> extents(const std::vector<T>& vec) {
    if (vec.empty()) {
        throw std::invalid_argument("Cannot compute extents of an empty vector.");
    }
    auto x = std::minmax_element(std::begin(vec), std::end(vec));
    return {{ *x.first, *x.second }};
}

} // namespace tk
