// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_QUATERNION_H
#define GDK_MATH_QUATERNION_H

#include <gdk/storage.inl> // varies by implementation

#include <gdk/math_constants.h>
#include <gdk/vector3.h>

#include <algorithm>
#include <cmath>
#include <iosfwd>
#include <stdexcept>
#include <type_traits>

namespace gdk {
    /// \brief Used to represent 3d rotations 
    template<typename component_type_param = float>
    class quaternion final : public quaternion_storage<component_type_param> {
    public:
        using component_type = component_type_param;

        using quaternion_storage<component_type_param>::x;
        using quaternion_storage<component_type_param>::y;
        using quaternion_storage<component_type_param>::z;
        using quaternion_storage<component_type_param>::w;

        static_assert(std::is_floating_point<component_type>::value, 
            "component_type must be a floating point type");

        //! unit length copy. 
        [[nodiscard]] quaternion<component_type> normalized() const;

        //! assign from euler angles in radians. **YXZ**
        void set_from_euler(const vector3<component_type> &aEulerAngles);

        //! read back as euler angles, in radians. The inverse of set_from_euler, YXZ as above.
        [[nodiscard]] vector3<component_type> to_euler() const;

        //! calculate the dot product
        [[nodiscard]] constexpr component_type dot_product(const quaternion<component_type> &other) const;

        //! the angle of this rotation, in radians, on [0, 2pi]
        [[nodiscard]] component_type angle() const;

        //! the unit axis this rotation turns about
        [[nodiscard]] vector3<component_type> axis() const;

        //! full inverse; use for non-unit quaternions
        [[nodiscard]] constexpr quaternion<component_type> inverse() const;

        //! faster inverse, but the quaternion must be unit length
        [[nodiscard]] constexpr quaternion<component_type> inverse_unit() const;

        quaternion<component_type> &operator=(const quaternion<component_type> &) = default;
        quaternion<component_type> &operator=(quaternion<component_type> &&) = default;

        [[nodiscard]] constexpr bool operator==(const quaternion<component_type> &other) const;

        constexpr quaternion<component_type> &operator*=(const component_type aScalar);

        //! scale all four components
        [[nodiscard]] constexpr quaternion<component_type> operator*(const component_type aScalar) const;

        //! component-wise sum
        [[nodiscard]] constexpr quaternion<component_type> operator+(const quaternion<component_type> &other) const;

        //! negation of all four components
        [[nodiscard]] constexpr quaternion<component_type> operator-() const;

        //! the negation of operator==
        [[nodiscard]] constexpr bool operator!=(const quaternion<component_type> &other) const;

        //! construct from euler angles, in radians. YXZ 
        constexpr explicit quaternion(const vector3<component_type> &aEulerAngles);

        constexpr quaternion(const component_type &aX, const component_type &aY,
            const component_type &aZ, const component_type &aW);

        quaternion() = default;
        quaternion(const quaternion<component_type> &) = default;
        quaternion(quaternion<component_type> &&) = default;
        ~quaternion() = default;

        static const quaternion<component_type> identity;

        //! alias for the from-euler-angles constructor
        [[nodiscard]] static quaternion<component_type> from_euler(
            const vector3<component_type> &aVector);

        //! build a rotation of aAngle radians about aAxis, right-handed
        [[nodiscard]] static quaternion<component_type> from_angle_axis(const component_type aAngle,
            const vector3<component_type> &aAxis);
    };

    //! spherical linear interpolation: constant angular velocity along the shorter arc
    template <typename component_type>
    [[nodiscard]] quaternion<component_type> slerp(const quaternion<component_type> &a,
        const quaternion<component_type> &b, const component_type t);

    //! normalized linear interpolation: cheaper than slerp, and not constant velocity
    template <typename component_type>
    [[nodiscard]] quaternion<component_type> nlerp(const quaternion<component_type> &a,
        const quaternion<component_type> &b, const component_type t);

    //! quaternion multiplication: compose two rotations, rightmost applied first
    template <typename component_type>
    [[nodiscard]] constexpr quaternion<component_type> operator*(const quaternion<component_type> &a,
        const quaternion<component_type> &b);
}

#include <gdk/quaternion.inl> // varies by implementation

#endif
