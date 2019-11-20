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
	static_assert(std::is_arithmetic<component_type>::value && std::is_signed<component_type>::value, "component_type must be a signed arithmetic type");

        component_type x = {0}, y = {0};
	
        //! squareroot of the sum of the 2nd power of the components
        template<class precision_type = component_type>
        precision_type &length(precision_type &&instance = precision_type()) const
        {
            static_assert(std::is_arithmetic<precision_type>::value, "precision_type must be an arithmetic type");
            
            instance = std::sqrt( 
                (static_cast<precision_type>(x) * static_cast<precision_type>(x)) + 
                (static_cast<precision_type>(y) * static_cast<precision_type>(y)));

            return instance;
        }

        //! division of the x component by the y component
        template<class precision_type = component_type>
        precision_type &getAspectRatio(precision_type &&instance = precision_type()) const
	{
            static_assert(std::is_arithmetic<precision_type>::value, "precision_type must be an arithmetic type");

            instance = static_cast<precision_type>(x) / static_cast<precision_type>(y);

	    return instance;
	}

	//! Reduce length of the vector to 1 while maintaning its direction
	Vector2<component_type> &normalize() 
	{
	    const component_type magnitude = Vector2<component_type>::length();
	    
	    if (magnitude != 0) // avoid divide by 0 case
            {
                x /= magnitude;
                y /= magnitude;
            }

            return *this;
	}
            
        bool operator==(const Vector2<component_type> &other) const
	{
	    return 
                x == other.x && 
                y == other.y;
	}
        
	bool operator!=(const Vector2<component_type> &other) const
	{
	    return x != other.x || y != other.y;
	}
        
        Vector2<component_type> operator+(const Vector2<component_type> &other) const
	{
	    return {static_cast<component_type>(x + other.x), static_cast<component_type>(y + other.y)};
	}
	
        Vector2<component_type> operator-(const Vector2 &other) const
	{
	    return {static_cast<component_type>(x - other.x), static_cast<component_type>(y - other.y)};
	}
	
	Vector2<component_type> operator*(const component_type aScalar) const
	{
	    return {static_cast<component_type>(x * aScalar), static_cast<component_type>(y * aScalar)};
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

        //typedef typename std::make_signed<component_type>::type signed_type; //thinking about support for unsigned integrals

        static const Vector2<component_type> Up;
	static const Vector2<component_type> Down;
	//static const Vector2<signed_type> Left;
	static const Vector2<component_type> Left;
	static const Vector2<component_type> Right;
	static const Vector2<component_type> Zero;
	static const Vector2<component_type> One;
    };

///home/joe/Workspace/gdk-math/include/gdk/vector2.h:137:36: note: previous declaration as ‘const gdk::Vector2<typename std::make_signed<component_type>::type> gdk::Vector2 <component_type>::Left’

    template <typename T> const Vector2<T> Vector2<T>::Up    = { 0, 1};
    template <typename T> const Vector2<T> Vector2<T>::Down  = { 0,-1};

    //template <typename component_type> const auto  gdk::Vector2 <component_type>::Left = {-1, 0};
    //template <typename T> const Vector2<typename std::make_signed<T>::type> Vector2<T>::Left  = {-1, 0};
    template <typename T> const Vector2<T> Vector2<T>::Left  = {-1, 0};

    template <typename T> const Vector2<T> Vector2<T>::Right = { 1, 0};
    template <typename T> const Vector2<T> Vector2<T>::Zero  = { 0, 0};
    template <typename T> const Vector2<T> Vector2<T>::One   = { 1, 1};
    
    template <typename T> std::ostream &operator<<(std::ostream &s, const Vector2<T> &vector)
    {
        return s << "{x: " << vector.x << ", " << "y: " << vector.y << "}";
    }
}

#endif
