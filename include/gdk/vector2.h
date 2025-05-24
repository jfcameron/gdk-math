// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_VECTOR2_H
#define GDK_MATH_VECTOR2_H

#include <cmath>
#include <iosfwd>
#include <iostream>
#include <tuple>
#include <type_traits>

namespace gdk {
    /// \brief 2 component floating point struct. 
    ///
    /// useful for storing 2D position, speed, direction, normalized coordinates.
    ///
    template<typename component_type_param = float>
    struct vector2 final {
        using component_type = component_type_param;

        static_assert(std::is_arithmetic<component_type>::value && std::is_signed<component_type>::value, 
            "component_type must be a signed arithmetic type");

        component_type x = {0}, y = {0};

        //! calculate the length of the vector	
        template<class precision_type = component_type>
        precision_type &length() const {
            static_assert(std::is_arithmetic<precision_type>::value, "precision_type must be an arithmetic type");
            auto a = std::pow(static_cast<precision_type>(x), 2);
            auto b = std::pow(static_cast<precision_type>(y), 2);

            return std::sqrt(a + b);
        }

        //! calculate the distance between this and another vector
        template<class precision_type = component_type>
        precision_type distance(const vector2<component_type> &other) const {
            static_assert(std::is_arithmetic<precision_type>::value, "precision_type must be an arithmetic type");
            auto a = std::pow(other.x - x, 2);
            auto b = std::pow(other.y - y, 2);

            return std::sqrt(a + b);
        }

        //! division of the x component by the y component
        template<class precision_type = component_type>
        precision_type &getAspectRatio(precision_type &&instance = precision_type()) const {
            static_assert(std::is_arithmetic<precision_type>::value, "precision_type must be an arithmetic type");

            return static_cast<precision_type>(x) / static_cast<precision_type>(y);
        }

        //! normalize the vector
        vector2<component_type> &normalize() {
            const component_type magnitude = vector2<component_type>::length();
            
            if (magnitude != 0) {
                x /= magnitude;
                y /= magnitude;
            }
            return *this;
        }

        //! returns a new vector containing the element-wise product of this vector and another
        template<class other_component_type = component_type>
        vector2<component_type> elementwise_product(const vector2<other_component_type> &aOther) const {
            return { x * static_cast<component_type>(aOther.x), y * static_cast<component_type>(aOther.y)};
        }

        //! calculate the dot product of this and another vector
        template<class precision_type = component_type>
        precision_type dot_product(const vector2<component_type> &other) const {
            return { x * other.x + y * other.y };
        }

        //! calculate the cross product of this and another vector
        template<class precision_type = component_type>
        precision_type cross_product(const vector2<component_type> &other) const {
            return { x * other.y - other.x * y };
        }
                
        bool operator==(const vector2<component_type> &other) const { return x == other.x && y == other.y; }
            
        bool operator!=(const vector2<component_type> &other) const { return x != other.x || y != other.y; }
            
        vector2<component_type> operator+(const vector2<component_type> &other) const {
            return {static_cast<component_type>(x + other.x), static_cast<component_type>(y + other.y)};
        }
        
        vector2<component_type> operator-(const vector2<component_type> &other) const {
            return {static_cast<component_type>(x - other.x), static_cast<component_type>(y - other.y)};
        }

        vector2<component_type> operator*(const component_type aScalar) const {
            return {static_cast<component_type>(x * aScalar), static_cast<component_type>(y * aScalar)};
        }
                
        vector2<component_type> &operator+=(const vector2 &other) {
            x += other.x;
            y += other.y;
            return *this;
        }

        vector2<component_type> &operator-=(const vector2 &other) {
            x -= other.x;
            y -= other.y;
            return *this;
        }

        vector2<component_type> &operator*=(const component_type aScalar) {
            x *= aScalar;
            y *= aScalar;
            return *this;
        }

        vector2<component_type> &operator=(const vector2<component_type> &) = default;

        vector2<component_type>(const component_type aX, const component_type aY) 	
        : x(aX)
        , y(aY)
        {}
        
        vector2<component_type>(const component_type aScalar)	
        : vector2<component_type>(aScalar, aScalar) {}

        template<class other_component_type = component_type>
        vector2<component_type>(const std::pair<other_component_type, other_component_type> aPair)
        : vector2<component_type>(static_cast<component_type>(std::get<0>(aPair)), static_cast<component_type>(std::get<1>(aPair))) {}
        
        vector2<component_type>() = default;
        vector2<component_type>(vector2<component_type> &&) = default;
        vector2<component_type>(const vector2<component_type> &) = default;
        ~vector2<component_type>() = default;

        static const vector2<component_type> Down;
        static const vector2<component_type> Left;
        static const vector2<component_type> One;
        static const vector2<component_type> Right;
        static const vector2<component_type> Up;
        static const vector2<component_type> Zero;
    };

    template <typename T> const vector2<T> vector2<T>::Down  = { 0,-1};
    template <typename T> const vector2<T> vector2<T>::Left  = {-1, 0};
    template <typename T> const vector2<T> vector2<T>::One   = { 1, 1};
    template <typename T> const vector2<T> vector2<T>::Right = { 1, 0};
    template <typename T> const vector2<T> vector2<T>::Up    = { 0, 1};
    template <typename T> const vector2<T> vector2<T>::Zero  = { 0, 0};
}

#endif

