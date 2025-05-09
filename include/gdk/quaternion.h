// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_QUATERNION_H
#define GDK_MATH_QUATERNION_H

#include <gdk/vector3.h>

#include <iosfwd>
#include <iostream>
#include <type_traits>

namespace gdk {
    /// \brief Used to represent 3d rotations 
    template<typename component_type_param = float>
    struct Quaternion final {
        using component_type = component_type_param;

        static_assert(std::is_floating_point<component_type>::value, "component_type must be a floating point type");

        component_type x = {0.}, y = {0.}, z = {0.}, w = {1.};
            
        void setFromEuler(const Vector3<component_type> &aEulerAngles) {
            static const component_type HALF(0.5);

            const auto yaw(aEulerAngles.z);
            const auto pitch(aEulerAngles.y);
            const auto roll(aEulerAngles.x);

            using namespace std;

            const auto cy = cos(yaw * HALF);
            const auto sy = sin(yaw * HALF);
            const auto cp = cos(pitch * HALF);
            const auto sp = sin(pitch * HALF);
            const auto cr = cos(roll * HALF);
            const auto sr = sin(roll * HALF);

            w = cr * cp * cy + sr * sp * sy;
            x = sr * cp * cy - cr * sp * sy;
            y = cr * sp * cy + sr * cp * sy;
            z = cr * cp * sy - sr * sp * cy;
        }

        Vector3<component_type> toEuler() const {
            using namespace std;

            const component_type sinr_cosp = 2 * (w * x + y * z);
            const component_type cosr_cosp = 1 - 2 * (x * x + y * y);
            const component_type roll = atan2(sinr_cosp, cosr_cosp);

            const component_type sinp = 2 * (w * y - z * x);
            const component_type pitch = asin(sinp);

            const component_type siny_cosp = 2 * (w * z + x * y);
            const component_type cosy_cosp = 1 - 2 * (y * y + z * z);
            const component_type yaw = atan2(siny_cosp, cosy_cosp);

            return {roll, pitch, yaw};
        }
            
        Quaternion<component_type> &operator=(const Quaternion<component_type> &) = default;
        Quaternion<component_type> &operator=(Quaternion<component_type> &&) = default;
        
        bool operator==(const Quaternion<component_type> &other) const {
            return x == other.x && y == other.y && z == other.z && w == other.w;
        }

        Quaternion<component_type> &operator*=(const component_type aScalar) {
            x *= aScalar;
            y *= aScalar;
            z *= aScalar;
            w *= aScalar;

            return *this;
        }
            
        Quaternion<component_type>(const Vector3<component_type> &aEulerAngles) {
            setFromEuler(aEulerAngles);
        }

        Quaternion<component_type>(const component_type &aX, const component_type &aY, const component_type &aZ, const component_type &aW)
        : x(aX)
        , y(aY)
        , z(aZ)
        , w(aW)
        {}

        Quaternion<component_type>() = default;
        Quaternion<component_type>(const Quaternion<component_type> &) = default;
        Quaternion<component_type>(Quaternion<component_type> &&) = default;
        ~Quaternion<component_type>() = default;
        
        static const Quaternion<component_type> Identity;
    };
        
    template <typename T> const Quaternion<T> Quaternion<T>::Identity = Quaternion();
}

#endif

