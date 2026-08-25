// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_MATH_CONCEPTS_H
#define GDK_MATH_MATH_CONCEPTS_H

#include <type_traits>

/// \file what the types in this library may be made of.
namespace gdk {
    template<typename T>
    concept arithmetic_component = std::is_arithmetic_v<T> && std::is_signed_v<T>;

    template<typename T>
    concept floating_point_component = std::is_floating_point_v<T>;
}

#endif
