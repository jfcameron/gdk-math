// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_VECTOR3_H
#define GDK_MATH_VECTOR3_H

#include <cmath>
#include <iosfwd>
#include <iostream>
#include <type_traits>

namespace gdk {
    /// \brief 3 component floating point struct.
    ///
    /// Used to represent 3d position, scale, euler angles.
    template<typename component_type_param = float>
    struct Vector3 final {
        using component_type = component_type_param;

        static_assert(std::is_floating_point<component_type>::value, "component_type must be a floating point type");

        component_type x = {0}, y = {0}, z = {0};

        //! Calculates the length of the vector
        component_type length() const {
            return sqrt((x * x) + (y * y) + (z * z));
        }

        //! Sets length of vector to 1
        Vector3<component_type> &normalize() {
            component_type magnitude = Vector3<component_type>::length();

            if (magnitude != 0) {
                x /= magnitude;
                y /= magnitude;
                z /= magnitude;
            }

            return *this;
        }
        
        //! calculate the distance between two vectors
        component_type distance(const Vector3<component_type> &other) const {
            auto a = std::pow(other.x - x, 2);
            auto b = std::pow(other.y - y, 2);
            auto c = std::pow(other.z - z, 2);

            return std::sqrt(a + b + c);
        }

        //! returns a new vector containing the element-wise product of this vector and another
        template<class other_component_type = component_type>
        Vector3<component_type> elementwise_product(const Vector3<other_component_type> &aOther) const {
            return { 
                x * static_cast<component_type>(aOther.x), 
                y * static_cast<component_type>(aOther.y), 
                z * static_cast<component_type>(aOther.z)};
        }

        //! calculate the dot product of this and another vector
        template<class precision_type = component_type>
        precision_type dot_product(const Vector3<component_type> &other) const {
            return { x * other.x + y * other.y + z * other.z };
        }

        //! calculate the cross product of this and another vector
        template<class precision_type = component_type>
        Vector3<precision_type> cross_product(const Vector3<component_type> &other) const {
            return { 
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x
            };
        }
            
        bool operator==(const Vector3<component_type> &other) const {
            return x == other.x && y == other.y && z == other.z;
        }

        bool operator!=(const Vector3<component_type> &other) const {
            return x != other.x || y != other.y || z != other.z;
        }

        Vector3<component_type> operator+(const Vector3<component_type> &other) const {
            return {
                other.x + x,
                other.y + y,
                other.z + z
            };
        }

        Vector3<component_type> operator-(const Vector3<component_type> &other) const {
            return {
                other.x - x,
                other.y - y,
                other.z - z
            };
        }

        Vector3<component_type> operator*(const component_type &aScalar) const {
            return {
                x * aScalar,
                y * aScalar,
                z * aScalar
            };
        }
            
        Vector3<component_type> &operator+=(const Vector3<component_type> &other) {
            x += other.x;
            y += other.y;
            z += other.z;

            return *this;
        }

        Vector3<component_type> &operator-=(const Vector3<component_type> &other) {
            x -= other.x;
            y -= other.y;
            z -= other.z;

            return *this;
        }

        Vector3<component_type> &operator*=(const component_type &aScalar) {
            x *= aScalar;
            y *= aScalar;
            z *= aScalar;

            return *this;
        }

        Vector3<component_type> &operator=(const Vector3<component_type> &) = default;
            
        Vector3<component_type>(const component_type &aX, const component_type &aY, const component_type &aZ)
        : x(aX)
        , y(aY)
        , z(aZ)
        {}

		Vector3<component_type>(const component_type& a)
		: Vector3(a, a, a)
		{}

        Vector3<component_type>() = default;
        Vector3<component_type>(const Vector3<component_type> &other) = default;
        Vector3<component_type>(Vector3<component_type> &&) = default;
        ~Vector3<component_type>() = default;
            
        static const Vector3<component_type> Backward;
        static const Vector3<component_type> Down;
        static const Vector3<component_type> Forward;
        static const Vector3<component_type> Left;
        static const Vector3<component_type> One;
        static const Vector3<component_type> Right;
        static const Vector3<component_type> Up;
        static const Vector3<component_type> Zero;
    };

    template <typename T> const Vector3<T> Vector3<T>::Backward = { 0, 0,-1};
    template <typename T> const Vector3<T> Vector3<T>::Down     = { 0,-1, 0};
    template <typename T> const Vector3<T> Vector3<T>::Forward  = { 0, 0, 1};
    template <typename T> const Vector3<T> Vector3<T>::Left     = {-1, 0, 0};
    template <typename T> const Vector3<T> Vector3<T>::One      = { 1, 1, 1};
    template <typename T> const Vector3<T> Vector3<T>::Right    = { 1, 0, 0};
    template <typename T> const Vector3<T> Vector3<T>::Up       = { 0, 1, 0};
    template <typename T> const Vector3<T> Vector3<T>::Zero     = { 0, 0, 0};
}

#endif

