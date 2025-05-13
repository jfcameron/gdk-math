// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_VECTOR4_H
#define GDK_MATH_VECTOR4_H

#include <gdk/vector3.h>

#include <iosfwd>

namespace gdk {
    /// \brief Like vector3<component_type> but allows w to != 1. Used in Vector vs matrix4x4 operations
    template<typename component_type_param = float>
    struct vector4 final {
        using component_type = component_type_param;

        component_type x = {0.}, y = {0.}, z = {0.}, w = {1.};
            
        bool operator==(const vector4<component_type> &other) const {
            return x == other.x && y == other.y && z == other.z && w == other.w;
        }

        bool operator!=(const vector4<component_type> &other) const {
            return x != other.x || y != other.y || z != other.z || w != other.w;
        } 

        vector4<component_type> &operator+=(const vector4<component_type> &other) {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;

            return *this;
        }

        vector4<component_type> &operator*=(const component_type &aScalar) {
            x *= aScalar;
            y *= aScalar;
            z *= aScalar;
            w *= aScalar;

            return *this;
        }

        vector4<component_type> &operator=(const vector4<component_type> &) = default;

        vector4<component_type>(const component_type &aX, const component_type &aY, const component_type &aZ, const component_type &aW = 1.)
        : x(aX)
        , y(aY)
        , z(aZ)
        , w(aW)
        {}
            
        vector4<component_type>(const vector3<component_type> &avector3, const component_type &aW = 1.)
        : vector4<component_type>(avector3.x, avector3.y, avector3.z, aW) {}

        vector4<component_type>() 
        : vector4<component_type>(0., 0., 0., 1.) {}

        vector4<component_type>(const vector4<component_type> &) = default;
        vector4<component_type>(vector4<component_type> &&) = default;
        ~vector4<component_type>() = default;
            
        static const vector4<component_type> Zero;
    };

    template<typename T> const vector4<T> vector4<T>::Zero = {0., 0., 0., 0.};
}

#endif

