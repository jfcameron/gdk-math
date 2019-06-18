// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_VECTOR2_H
#define GDK_MATH_VECTOR2_H

#include <iosfwd>
#include <iostream>
#include <type_traits>
#include <cmath>

namespace gdk
{
    /// \brief 2 component floating point struct. 
    ///
    /// useful for storing 2D position, speed, direction, normalized coordinates.
    ///
    /// \note in reference to the static members: 
    /// X is considered the lateral component, pointing right, 
    /// Y the vertical, pointing up
    template<typename component_type = float>
    struct Vector2 final
    {
	static_assert(std::is_floating_point<component_type>::value, "component_type must be a floating point type");

        component_type x = {0.}, y = {0.};
	
        //! squareroot of the sum of the 2nd power of the components
        component_type length() const
	{
	    return std::sqrt( (x * x) + (y * y) );
	}
	
	//! division of the x component by the y component
        component_type getAspectRatio() const 
	{
	    return x / y;
	}

	//! Reduce length of the vector to 1 while maintaning its direction
	void normalize() 
	{
	    const component_type magnitude = Vector2<component_type>::length();
	    
	    if (magnitude == 0.0f) return; // n/0 case
	    
	    x /= magnitude;
	    y /= magnitude;
	}
            
        bool operator==(const Vector2<component_type> &other) const
	{
	    return x == other.x && y == other.y;
	}
        
	bool operator!=(const Vector2<component_type> &other) const
	{
	    return x != other.x || y != other.y;
	}
        
        Vector2<component_type> operator+(const Vector2<component_type> &other) const
	{
	    return {other.x + x, other.y + y};
	}
	
        Vector2<component_type> operator-(const Vector2 &other) const
	{
	    return {other.x - x, other.y - y};
	}
	
	Vector2<component_type> operator*(const component_type aScalar) const
	{
	    return {x * aScalar, y * aScalar};
	}
            
        Vector2<component_type> &operator+=(const Vector2 &other)
	{
	    x += other.x;
	    y += other.y;
	    
	    return *this;
	}

        Vector2<component_type> &operator-=(const Vector2 &other)
	{
	    x -= other.x;
	    y -= other.y;
	    
	    return *this;
	}

        Vector2<component_type> &operator*=(const component_type aScalar)
	{
	    x *= aScalar;
	    y *= aScalar;
	    
	    return *this;
	}

        Vector2<component_type> &operator=(const Vector2<component_type> &) = default;
	
	//! Assign 1 fp value to all components
        Vector2<component_type>(const component_type aScalar)	
	: x(aScalar)
	, y(aScalar)
	{}

	//! Assign each component independently
        Vector2<component_type>(const component_type aX, const component_type aY) 	
	: x(aX)
	, y(aY)
	{}
	
        Vector2<component_type>() = default;
        Vector2<component_type>(const Vector2<component_type> &) = default;
        Vector2<component_type>(Vector2<component_type> &&) = default;
        ~Vector2<component_type>() = default;
            
	static const Vector2<component_type> Up;
	static const Vector2<component_type> Down;
	static const Vector2<component_type> Left;
	static const Vector2<component_type> Right;
	static const Vector2<component_type> Zero;
	static const Vector2<component_type> One;
    };
    
    template <typename T> const Vector2<T> Vector2<T>::Up    = { 0.f, 1.f};
    template <typename T> const Vector2<T> Vector2<T>::Down  = { 0.f,-1.f};
    template <typename T> const Vector2<T> Vector2<T>::Left  = {-1.f, 0.f};
    template <typename T> const Vector2<T> Vector2<T>::Right = { 1.f, 0.f};
    template <typename T> const Vector2<T> Vector2<T>::Zero  = { 0.f, 0.f};
    template <typename T> const Vector2<T> Vector2<T>::One   = { 1.f, 1.f};
    
    template <typename T> std::ostream &operator<<(std::ostream &s, const Vector2<T> &vector)
    {
        return s << "{x: " << vector.x << ", " << "y: " << vector.y << "}";
    }
}

#endif
