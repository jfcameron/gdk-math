// © 2018 - 2019 Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_INTVECTOR2_H
#define GDK_MATH_INTVECTOR2_H

#include <iosfwd>
#include <iostream>
#include <type_traits>
#include <utility>

#include <gdk/vector2.h>

namespace gdk
{
    /// \brief Behaves like a Vector2 but uses integral types for storage rather than floating point types.
    ///
    /// Useful for describing such things as non-normalized texel position,
    /// tilegrid position, window size, etc.
    ///
    /// \todo consider merging this into vector2. Now that this lib is templatized, the distinction has blurred.
    template<typename component_type = int>
    struct IntVector2 final
    {
        static_assert(std::is_integral<component_type>::value, "component_type must be an integral type");
        //static_assert(std::is_signed<component_type>::value, "component_type must be a signed type");

        using std_pair_type = std::pair<component_type, component_type>;

        component_type x = {0}, y = {0};

        /// create a Vector2 (related floating-point struct) from an IntVector2 instance
        template<typename T = float> Vector2<T> toVector2() const     
        {
            return Vector2<T>(static_cast<T>(x), static_cast<T>(y));
        }

        bool operator==(const IntVector2<component_type> &other) const
        {
            return x == other.x && y == other.y;
        }

        bool operator!=(const IntVector2<component_type> &other) const
        {
            return x != other.x || y != other.y;
        }
        
        IntVector2<component_type> operator+(const IntVector2<component_type> &other) const
        {
            return IntVector2<component_type>(x + other.x, y + other.y);
        }

        IntVector2<component_type> operator-(const IntVector2<component_type> &other) const
        {
            return IntVector2<component_type>(x - other.x, y - other.y);
        }

        IntVector2<component_type> operator*(const float aScalar) const //TODO: float?
        {
            return IntVector2<component_type>(x * aScalar, y * aScalar); //TODO: explicit casts?
        }

        template<class floating_point_component_type>
        IntVector2<component_type> operator*(const Vector2<floating_point_component_type> &other) const
        {
            return IntVector2<component_type>(x * other.x, y * other.y);
        }

        IntVector2<component_type> &operator=(const IntVector2<component_type> &) = default;
        IntVector2<component_type> &operator=(IntVector2<component_type> &&) = default;
        
        IntVector2<component_type> &operator+=(const IntVector2<component_type> &other)
        {
            x += other.x;
            y += other.y;

            return *this;
        }

        IntVector2<component_type> &operator-=(const IntVector2<component_type> &other)
        {
            x -= other.x;
            y -= other.y;

            return *this;
        }

        IntVector2<component_type> &operator*=(const component_type aScalar)
        {
            x *= aScalar;
            y *= aScalar;

            return *this;
        }

        explicit operator std_pair_type() const
        {            
            return std::pair<component_type, component_type>(x, y);
        }

        /// alias for toVector2() using explicit c++ cast semantics
        template<typename T = float> explicit operator Vector2<T>() const
        {
            return this->toVector2();
        }

        IntVector2<component_type>(const int aX, const int aY)
        : x(aX)
        , y(aY)
        {}
        
        IntVector2<component_type>(const std_pair_type &aPair)
        : IntVector2<component_type>::IntVector2<component_type>(aPair.first, aPair.second)
        {}

        IntVector2<component_type>() = default;
        
        IntVector2<component_type>(const IntVector2<component_type> &) = default;
        
        IntVector2<component_type>(IntVector2<component_type> &&) = default;
        
        ~IntVector2<component_type>() = default;
            
        static const IntVector2<component_type> Up;
        static const IntVector2<component_type> Down;
        static const IntVector2<component_type> Left;
        static const IntVector2<component_type> Right;
        static const IntVector2<component_type> Zero;
    };

    template <typename T> std::ostream &operator<< (std::ostream &s, const gdk::IntVector2<T> &vector)
    {
        return s << "{x: " << vector.x << ", " << "y: " << vector.y << "}";
    }

    template <typename T> const IntVector2<T> IntVector2<T>::Up =    { 0, 1};
    template <typename T> const IntVector2<T> IntVector2<T>::Down =  { 0,-1};
    template <typename T> const IntVector2<T> IntVector2<T>::Left =  {-1, 0};
    template <typename T> const IntVector2<T> IntVector2<T>::Right = { 1, 0};
    template <typename T> const IntVector2<T> IntVector2<T>::Zero =  { 0, 0};
}

#endif
