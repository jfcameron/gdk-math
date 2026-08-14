// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_IMPL_STD_QUATERNION_INL
#define GDK_MATH_IMPL_STD_QUATERNION_INL

namespace gdk {
    template<typename component_type>
    quaternion<component_type> quaternion<component_type>::normalized() const {
        const component_type magnitude = std::sqrt(x * x + y * y + z * z + w * w);

        if (magnitude == 0.0) return {};

        const component_type invMagnitude = static_cast<component_type>(1.0) / magnitude;

        return {x * invMagnitude, y * invMagnitude, z * invMagnitude, w * invMagnitude};
    }

    template<typename component_type>
    void quaternion<component_type>::set_from_euler(const vector3<component_type> &aEulerAngles) {
        static const component_type HALF(0.5);

        const auto heading(aEulerAngles.y);
        const auto pitch(aEulerAngles.x);
        const auto roll(aEulerAngles.z);

        const auto ch = std::cos(heading * HALF);
        const auto sh = std::sin(heading * HALF);
        const auto cp = std::cos(pitch * HALF);
        const auto sp = std::sin(pitch * HALF);
        const auto cr = std::cos(roll * HALF);
        const auto sr = std::sin(roll * HALF);

        x = ch * sp * cr + sh * cp * sr;
        y = sh * cp * cr - ch * sp * sr;
        z = ch * cp * sr - sh * sp * cr;
        w = ch * cp * cr + sh * sp * sr;
    }

    template<typename component_type>
    vector3<component_type> quaternion<component_type>::to_euler() const {
        const component_type sinPitch = std::max(static_cast<component_type>(-1),
            std::min(static_cast<component_type>(1), 2 * (w * x - y * z)));

        const component_type pitch = std::asin(sinPitch);

        const component_type heading = std::atan2(2 * (x * z + y * w),
            1 - 2 * (x * x + y * y));

        const component_type roll = std::atan2(2 * (x * y + z * w),
            1 - 2 * (x * x + z * z));

        return {pitch, heading, roll};
    }

    template<typename component_type>
    constexpr component_type quaternion<component_type>::dot_product(
        const quaternion<component_type> &other) const {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    template<typename component_type>
    component_type quaternion<component_type>::angle() const {
        return static_cast<component_type>(2) * std::atan2(std::sqrt(x * x + y * y + z * z), w);
    }

    template<typename component_type>
    vector3<component_type> quaternion<component_type>::axis() const {
        const auto lengthSquared = x * x + y * y + z * z;

        if (lengthSquared <= static_cast<component_type>(0))
            return vector3<component_type>::right;

        const auto invLength = static_cast<component_type>(1) / std::sqrt(lengthSquared);

        return {x * invLength, y * invLength, z * invLength};
    }

    template<typename component_type>
    constexpr quaternion<component_type> quaternion<component_type>::operator*(
        const component_type aScalar) const {
        return {x * aScalar, y * aScalar, z * aScalar, w * aScalar};
    }

    template<typename component_type>
    constexpr quaternion<component_type> quaternion<component_type>::operator+(
        const quaternion<component_type> &other) const {
        return {x + other.x, y + other.y, z + other.z, w + other.w};
    }

    template<typename component_type>
    constexpr quaternion<component_type> quaternion<component_type>::operator-() const {
        return {-x, -y, -z, -w};
    }

    template<typename component_type>
    constexpr bool quaternion<component_type>::operator!=(const quaternion<component_type> &other) const {
        return !(*this == other);
    }

    template<typename component_type>
    quaternion<component_type> quaternion<component_type>::from_angle_axis(
        const component_type aAngle, const vector3<component_type> &aAxis) {
        const auto lengthSquared = aAxis.length_squared();

        if (lengthSquared <= static_cast<component_type>(0)) return {};

        const auto invLength = static_cast<component_type>(1) / std::sqrt(lengthSquared);
        const auto half = aAngle * static_cast<component_type>(0.5);
        const auto s = std::sin(half);

        return {aAxis.x * invLength * s,
            aAxis.y * invLength * s,
            aAxis.z * invLength * s,
            std::cos(half)};
    }

    template<typename component_type>
    constexpr quaternion<component_type> quaternion<component_type>::inverse() const {
        const component_type normSquared = x * x + y * y + z * z + w * w;

        if (normSquared == 0.0f) throw std::runtime_error("Cannot invert a zero quaternion.");

        const component_type invNorm = static_cast<component_type>(1.0) / normSquared;

        return {-x * invNorm, -y * invNorm, -z * invNorm, w * invNorm};
    }

    template<typename component_type>
    constexpr quaternion<component_type> quaternion<component_type>::inverse_unit() const {
        return {-x, -y, -z, w};
    }

    template<typename component_type>
    constexpr bool quaternion<component_type>::operator==(const quaternion<component_type> &other) const {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }

    template<typename component_type>
    constexpr quaternion<component_type> &quaternion<component_type>::operator*=(
        const component_type aScalar) {
        x *= aScalar;
        y *= aScalar;
        z *= aScalar;
        w *= aScalar;

        return *this;
    }

    template<typename component_type>
    constexpr quaternion<component_type>::quaternion(const vector3<component_type> &aEulerAngles) {
        set_from_euler(aEulerAngles);
    }

    template<typename component_type>
    constexpr quaternion<component_type>::quaternion(const component_type &aX, const component_type &aY,
        const component_type &aZ, const component_type &aW)
    : quaternion_storage<component_type>{aX, aY, aZ, aW}
    {}

    template<typename component_type>
    quaternion<component_type> quaternion<component_type>::from_euler(
        const vector3<component_type> &aVector) {
        return quaternion(aVector);
    }

    template <typename component_type>
    quaternion<component_type> nlerp(const quaternion<component_type> &a,
        const quaternion<component_type> &b, const component_type t) {
        const auto adjusted = a.dot_product(b) < static_cast<component_type>(0) ? -b : b;

        return (a * (static_cast<component_type>(1) - t) + adjusted * t).normalized();
    }

    template <typename component_type>
    quaternion<component_type> slerp(const quaternion<component_type> &a,
        const quaternion<component_type> &b, const component_type t) {
        constexpr auto LINEAR_THRESHOLD = static_cast<component_type>(0.9995);

        auto cosTheta = a.dot_product(b);

        auto adjusted = b;
        if (cosTheta < static_cast<component_type>(0)) {
            adjusted = -b;
            cosTheta = -cosTheta;
        }

        if (cosTheta > LINEAR_THRESHOLD)
            return nlerp(a, adjusted, t);

        const auto theta = std::acos(cosTheta);
        const auto sinTheta = std::sin(theta);

        const auto scaleA = std::sin((static_cast<component_type>(1) - t) * theta) / sinTheta;
        const auto scaleB = std::sin(t * theta) / sinTheta;

        return a * scaleA + adjusted * scaleB;
    }

    template <typename component_type>
    constexpr quaternion<component_type> operator*(const quaternion<component_type> &a,
        const quaternion<component_type> &b) {
        return {
            a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,  
            a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x, 
            a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w, 
            a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z 
        };
    }

    template <typename component_type>
    const quaternion<component_type> quaternion<component_type>::identity = quaternion();
}

#endif
