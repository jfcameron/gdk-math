// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_VECTOR4_H
#define GDK_MATH_VECTOR4_H

#include <gdk/vector3.h>

#include <iosfwd>

namespace gdk {
    /// \brief Like Vector3<component_type> but allows w to != 1. Used in Vector vs Mat4x4 operations
    template<typename component_type_param = float>
    struct Vector4 final {
        using component_type = component_type_param;

        float x = {0.}, y = {0.}, z = {0.}, w = {1.};
            
        bool operator==(const Vector4<component_type> &other) const {
            return x == other.x && y == other.y && z == other.z && w == other.w;
        }

        bool operator!=(const Vector4<component_type> &other) const {
            return x != other.x || y != other.y || z != other.z || w != other.w;
        } 

        Vector4<component_type> &operator+=(const Vector4<component_type> &other) {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;

            return *this;
        }

        Vector4<component_type> &operator*=(const float &aScalar) {
            x *= aScalar;
            y *= aScalar;
            z *= aScalar;
            w *= aScalar;

            return *this;
        }

        Vector4<component_type> &operator=(const Vector4<component_type> &) = default;
            
        Vector4<component_type>(const Vector3<component_type> &aVector3, const float &aW = 1.)
        : x(aVector3.x)
        , y(aVector3.y)
        , z(aVector3.z)
        , w(aW)
        {}

        Vector4<component_type>(const float &aX, const float &aY, const float &aZ, const float &aW)
        : x(aX)
        , y(aY)
        , z(aZ)
        , w(aW)
        {}

        Vector4<component_type>()
        : x(0.)
        , y(0.)
        , z(0.)
        , w(1.)
        {}

        Vector4<component_type>(const Vector4<component_type> &) = default;
        Vector4<component_type>(Vector4<component_type> &&) = default;
        ~Vector4<component_type>() = default;
            
        static const Vector4<component_type> Zero;
    };

    template<typename T> const Vector4<T> Vector4<T>::Zero = {0., 0., 0., 0.};
    
    template<typename T> std::ostream &operator<< (std::ostream &s, const Vector4<T> &vector) {
        return s << "{x: " << vector.x << ", y: " << vector.y << ", z: " << vector.z << ", w: " << vector.w << "}";
    }
}

#endif

