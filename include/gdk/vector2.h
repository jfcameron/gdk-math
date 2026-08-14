// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_VECTOR2_H
#define GDK_MATH_VECTOR2_H

#include <gdk/storage.inl> // varies by implementation

#include <gdk/math_constants.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iosfwd>
#include <stdexcept>
#include <tuple>
#include <type_traits>

namespace gdk {
    /// \brief 2d vector used to represent position, speed, direction, normalized coordinates
    template<typename component_type_param = float>
    class vector2 final : public vector2_storage<component_type_param> {
    public:
        using component_type = component_type_param;

        using vector2_storage<component_type_param>::x;
        using vector2_storage<component_type_param>::y;

        static_assert(std::is_arithmetic<component_type>::value && std::is_signed<component_type>::value,
            "component_type must be a signed arithmetic type");

        //! a new vector made of the largest components of two vectors
        static constexpr vector2 max(const vector2 &a, const vector2 &b);

        //! a new vector made of the smallest components of two vectors
        static constexpr vector2 min(const vector2 &a, const vector2 &b);

        //! calculate the length of the vector
        [[nodiscard]] component_type length() const;

        //! get the squared length of the vector (faster than length, appropriate for certain use cases)
        [[nodiscard]] constexpr component_type length_squared() const;

        //! faster than a component-wise check, but not appropriate if each component must be exactly zero
        [[nodiscard]] constexpr bool is_effectively_zero() const;

        //! calculate the distance between this and another vector
        [[nodiscard]] component_type distance_from(const vector2<component_type> &other) const;

        //! division of the x component by the y component
        [[nodiscard]] constexpr component_type aspect_ratio() const;

        //! returns a new vector containing the element-wise product of this vector and another
        [[nodiscard]] vector2<component_type> element_wise_product(
            const vector2<component_type> &aOther) const;

        //! calculate the dot product of this and another vector
        [[nodiscard]] constexpr component_type dot_product(const vector2<component_type> &other) const;

        //! calculate the 2d cross product: the z component the 3d one would produce
        [[nodiscard]] constexpr component_type cross_product(const vector2<component_type> &other) const;

        //! a unit length copy. Zero for an effectively-zero vector, matching normalize().
        [[nodiscard]] vector2<component_type> normal() const;

        //! normalize the vector
        vector2<component_type> &normalize();

        //! the angle between two directions, in radians, on [0, pi]. Zero for a zero-length operand.
        [[nodiscard]] component_type angle_between(const vector2<component_type> &that) const;

        //! mirror this vector about a line with the given normal. aNormal is assumed unit length.
        [[nodiscard]] constexpr vector2<component_type> reflect(const vector2<component_type> &aNormal) const;

        [[nodiscard]] constexpr bool operator==(const vector2<component_type> &other) const;
        [[nodiscard]] constexpr bool operator!=(const vector2<component_type> &other) const;

        constexpr component_type &operator[](const std::size_t aComponentIndex);
        [[nodiscard]] constexpr component_type operator[](const std::size_t aComponentIndex) const;

        [[nodiscard]] constexpr vector2<component_type> operator+(const vector2<component_type> &other) const;
        [[nodiscard]] constexpr vector2<component_type> operator-(const vector2<component_type> &other) const;
        [[nodiscard]] constexpr vector2<component_type> operator-() const;
        [[nodiscard]] constexpr vector2<component_type> operator*(const component_type aScalar) const;

        //! component-wise division
        [[nodiscard]] constexpr vector2<component_type> operator/(const vector2<component_type> &other) const;
        [[nodiscard]] constexpr vector2<component_type> operator/(const component_type aScalar) const;

        constexpr vector2<component_type> &operator+=(const vector2 &other);
        constexpr vector2<component_type> &operator-=(const vector2 &other);
        constexpr vector2<component_type> &operator*=(const component_type aScalar);
        constexpr vector2<component_type> &operator/=(const vector2 &other);
        constexpr vector2<component_type> &operator/=(const component_type aScalar);

        vector2<component_type> &operator=(const vector2<component_type> &) = default;

        constexpr vector2(const component_type aX, const component_type aY);
        constexpr vector2(const component_type aScalar);

        //! build from a pair, converting the components
        template<class other_component_type = component_type>
        constexpr vector2(const std::pair<other_component_type, other_component_type> aPair);

        vector2() = default;
        vector2(vector2<component_type> &&) = default;
        vector2(const vector2<component_type> &) = default;
        ~vector2() = default;

        static const vector2<component_type> down;
        static const vector2<component_type> left;
        static const vector2<component_type> one;
        static const vector2<component_type> right;
        static const vector2<component_type> up;
        static const vector2<component_type> zero;

    };

    //! linear interpolation
    template<typename component_type>
    [[nodiscard]] constexpr vector2<component_type> lerp(const vector2<component_type> &a,
        const vector2<component_type> &b, const component_type t);
}

#include <gdk/vector2.inl> // varies by implementation

#endif
