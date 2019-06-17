// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_VECTOR3_H
#define GDK_MATH_VECTOR3_H

#include <iosfwd>
#include <iostream>
#include <type_traits>

namespace gdk
{
    /// \brief 3 component floating point struct.
    ///
    /// Used to represent 3d position, scale, euler angles.
    template<typename component_type = float>
    struct Vector3 final
    {
        static_assert(std::is_floating_point<component_type>::value, "component_type must be a floating point type");

        float x = {0.}, y = {0.}, z = {0.};

        //! Calculates the length of the vector
        float length() const
        {
            return sqrt( (x*x) + (y*y) + (z*z) );
        }

        //! Sets length of vector to 1
        void normalize()
        {
            float magnitude = Vector3<component_type>::length();

            if (magnitude == 0.0f) return; // n/0 case

            x /= magnitude;
            y /= magnitude;
            z /= magnitude;
        }
            
        bool operator==(const Vector3<component_type> &other) const
        {
            return (x == other.x && y == other.y && z == other.z);
        }

        bool operator!=(const Vector3<component_type> &other) const
        {
            return (x != other.x || y != other.y || z != other.z);
        }

        Vector3<component_type> operator+(const Vector3<component_type> &other) const
        {
            return {
                other.x + x,
                other.y + y,
                other.z + z
            };
        }

        Vector3<component_type> operator-(const Vector3<component_type> &other) const
        {
            return {
                other.x - x,
                other.y - y,
                other.z - z
            };
        }

        Vector3<component_type> operator*(const float &aScalar) const
        {
            return {
                x * aScalar,
                y * aScalar,
                z * aScalar
            };
        }
            
        Vector3<component_type> &operator+=(const Vector3<component_type> &other)
        {
            x += other.x;
            y += other.y;
            z += other.z;

            return *this;
        }

        Vector3<component_type> &operator-=(const Vector3<component_type> &other)
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;

            return *this;
        }

        Vector3<component_type> &operator*=(const float &aScalar)
        {
            x *= aScalar;
            y *= aScalar;
            z *= aScalar;

            return *this;
        }

        Vector3<component_type> &operator=(const Vector3<component_type> &) = default;
            
        Vector3<component_type>(const float &aX, const float &aY, const float &aZ)
        : x(aX)
        , y(aY)
        , z(aZ)
        {}

        Vector3<component_type>() = default;
        Vector3<component_type>(const Vector3<component_type> &other) = default;
        Vector3<component_type>(Vector3<component_type> &&) = default;
        ~Vector3<component_type>() = default;
            
        static const Vector3<component_type> Up;
        static const Vector3<component_type> Down;
        static const Vector3<component_type> Left;
        static const Vector3<component_type> Right;
        static const Vector3<component_type> Forward;
        static const Vector3<component_type> Backward;
        static const Vector3<component_type> Zero;
        static const Vector3<component_type> One;
    };

    template <typename T> const Vector3<T> Vector3<T>::Up       = { 0.f, 1.f, 0.f};
    template <typename T> const Vector3<T> Vector3<T>::Down     = { 0.f,-1.f, 0.f};
    template <typename T> const Vector3<T> Vector3<T>::Left     = {-1.f, 0.f, 0.f};
    template <typename T> const Vector3<T> Vector3<T>::Right    = { 1.f, 0.f, 0.f};
    template <typename T> const Vector3<T> Vector3<T>::Forward  = { 0.f, 0.f, 1.f};
    template <typename T> const Vector3<T> Vector3<T>::Backward = { 0.f, 0.f,-1.f};
    template <typename T> const Vector3<T> Vector3<T>::Zero     = { 0.f, 0.f, 0.f};
    template <typename T> const Vector3<T> Vector3<T>::One      = { 1.f, 1.f, 1.f};

    template <typename T> std::ostream &operator<< (std::ostream &s, const gdk::Vector3<T> &vector)
    {
        return s << "{x: " << vector.x << ", " << "y: " << vector.y << ", " << "z: " << vector.z << "}";
    }
}

#endif
