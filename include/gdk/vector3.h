// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_VECTOR3_H
#define GDK_MATH_VECTOR3_H

#include <gdk/storage.inl> // varies by implementation

#include <gdk/math_constants.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iosfwd>
#include <stdexcept>
#include <type_traits>

namespace gdk {
    /// \brief 3d vector used to represent position, scale, velocity, heading, euler angles, etc.
    /// - **right-handed**: +X right, +Y up, +Z back
    template<typename component_type_param = float>
    class vector3 final : public vector3_storage<component_type_param> {
    public:
        using component_type = component_type_param;

        using vector3_storage<component_type_param>::x;
        using vector3_storage<component_type_param>::y;
        using vector3_storage<component_type_param>::z;

        static_assert(
            std::is_arithmetic<component_type>::value && 
            std::is_signed<component_type>::value, 
            "vector3::component_type must be a signed arithmetic type");

        //! set a single scalar value to all three components. 
        constexpr explicit vector3(const component_type &aBroadcast);
        vector3 &operator=(const vector3 &) = default;
        vector3() = default;
        constexpr vector3(const component_type &aX, const component_type &aY, const component_type &aZ);
        vector3(const vector3 &that) = default;
        vector3(vector3 &&) = default;
        ~vector3() = default;

        //! a new vector made of the largest components of two vectors
        static constexpr vector3 max(const vector3 &a, const vector3 &b);

        //! a new vector made of the smallest components of two vectors
        static constexpr vector3 min(const vector3 &a, const vector3 &b);

        static const vector3 backward;
        static const vector3 down;
        static const vector3 forward;
        static const vector3 left;
        static const vector3 one;
        static const vector3 right;
        static const vector3 up;
        static const vector3 zero;

        //! faster than a component-wise check, but not appropriate if each component must be exactly zero
        [[nodiscard]] constexpr bool is_effectively_zero() const;
        [[nodiscard]] component_type distance_from(const vector3 &that) const;
        [[nodiscard]] constexpr component_type dot_product(const vector3 &that) const;
        [[nodiscard]] component_type length() const;
        //! get the squared length of the vector (faster than length, appropriate for certain use cases)
        [[nodiscard]] constexpr component_type length_squared() const;
        [[nodiscard]] constexpr vector3 cross_product(const vector3 &that) const;
        [[nodiscard]] constexpr vector3 element_wise_product(const vector3 &aOther) const;
        [[nodiscard]] vector3 normal() const;

        vector3 &normalize();

        //! the angle between two directions, in radians, on [0, pi]. Zero for a zero-length operand.
        [[nodiscard]] component_type angle_between(const vector3 &that) const;

        //! mirror this vector about a surface with the given normal. aNormal is assumed unit length.
        [[nodiscard]] constexpr vector3 reflect(const vector3 &aNormal) const;

        [[nodiscard]] constexpr bool operator!=(const vector3 &that) const;
        [[nodiscard]] constexpr bool operator==(const vector3 &that) const;

        constexpr component_type &operator[](const std::size_t aComponentIndex);
        [[nodiscard]] constexpr component_type operator[](const std::size_t aComponentIndex) const;
 
        [[nodiscard]] constexpr vector3 operator*(const component_type aScalar) const;
        [[nodiscard]] constexpr vector3 operator/(const component_type aScalar) const;
        [[nodiscard]] constexpr vector3 operator+(const vector3 &that) const;
        [[nodiscard]] constexpr vector3 operator-() const;
        [[nodiscard]] constexpr vector3 operator-(const vector3 &that) const;
        [[nodiscard]] constexpr vector3 operator/(const vector3 &that) const;

        constexpr vector3 &operator*=(const component_type &aScalar);
        constexpr vector3 &operator/=(const component_type &aScalar);
        constexpr vector3 &operator+=(const vector3 &that);
        constexpr vector3 &operator-=(const vector3 &that);
        constexpr vector3 &operator/=(const vector3 &that);

    };

    //! linear interpolation, 
    template<typename component_type>
    [[nodiscard]] constexpr vector3<component_type> lerp(const vector3<component_type> &a,
        const vector3<component_type> &b, const component_type t);
}

#include <gdk/vector3.inl> // varies by implementation

#endif
