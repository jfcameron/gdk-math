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
    struct quaternion final {
        using component_type = component_type_param;

        static_assert(std::is_floating_point<component_type>::value, "component_type must be a floating point type");

        component_type x = {0.}, y = {0.}, z = {0.}, w = {1.};

        quaternion<component_type> normalized() const {
            component_type magnitude = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2) + pow(w, 2));
            if (magnitude == 0.0) {
                //throw std::runtime_error("Cannot normalize a zero-length quaternion.");
                return {};
            }
            float invMagnitude = 1.0f / magnitude;
            return { x * invMagnitude, y * invMagnitude, z * invMagnitude, w * invMagnitude };
        }
            
        void setFromEuler(const vector3<component_type> &aEulerAngles) {
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

        vector3<component_type> toEuler() const {
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

        // Full inverse (use for non-unit quaternions)
        quaternion<component_type> inverse() const {
            component_type normSquared = x*x + y*y + z*z + w*w;
            if (normSquared == 0.0f) {
                throw std::runtime_error("Cannot invert a zero quaternion.");
            }
            component_type invNorm = 1.0f / normSquared;
            return { -x * invNorm, -y * invNorm, -z * invNorm, w * invNorm };
        }
        // Faster inverse but quanternion must be unit length
        void inverseUnit() {
            x = -x;
            y = -y;
            z = -z;
            w = w;
        }
        // Faster inverse but quanternion must be unit length
        quaternion<component_type> inverseUnit() const {
            return { -x, -y, -z, w };
        }
            
        quaternion<component_type> &operator=(const quaternion<component_type> &) = default;
        quaternion<component_type> &operator=(quaternion<component_type> &&) = default;
        
        bool operator==(const quaternion<component_type> &other) const {
            return x == other.x && y == other.y && z == other.z && w == other.w;
        }

        quaternion<component_type> &operator*=(const component_type aScalar) {
            x *= aScalar;
            y *= aScalar;
            z *= aScalar;
            w *= aScalar;

            return *this;
        }
            
        quaternion<component_type>(const vector3<component_type> &aEulerAngles) {
            setFromEuler(aEulerAngles);
        }

        quaternion<component_type>(const component_type &aX, const component_type &aY, const component_type &aZ, const component_type &aW)
        : x(aX)
        , y(aY)
        , z(aZ)
        , w(aW)
        {}

        quaternion<component_type>() = default;
        quaternion<component_type>(const quaternion<component_type> &) = default;
        quaternion<component_type>(quaternion<component_type> &&) = default;
        ~quaternion<component_type>() = default;
        
        static const quaternion<component_type> identity;

        static quaternion<component_type> fromEuler(const vector3<component_type> &aVector) {
            return quaternion(aVector);
        }
    };

    //! quaternion multiplication
    template <typename component_type>
    quaternion<component_type> operator*(const quaternion<component_type> a, const quaternion<component_type> b) {
        return {
            a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,  
            a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x, 
            a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w, 
            a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z 
        };
    }
        
    template <typename component_type> const quaternion<component_type> quaternion<component_type>::identity = quaternion();
}

#endif

