// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_QUATERNION_H
#define GDK_MATH_QUATERNION_H

#include <iosfwd>
#include <iostream>
#include <type_traits>

#include <gdk/vector3>

namespace gdk
{
    /// \brief Used to represent 3d rotations in a way insusceptible to gimblal lock (unlike Vector3)
    template<typename component_type = float>
    struct Quaternion final
    {
        static_assert(std::is_floating_point<component_type>::value, "component_type must be a floating point type");

        float x = {0.}, y = {0.}, z = {0.}, w = {1.};
            
        /*void setFromEuler(const Vector3 &aEulers);
        gdk::Vector3 toEuler() const;
            
        Quaternion &operator=(const Quaternion &) = default;
        Quaternion &operator=(Quaternion &&) = default;
            
        Quaternion(const Vector3 &);
        Quaternion(const float &aX, const float &aY, const float &aZ, const float &aW);
        Quaternion();
        Quaternion(const Quaternion &) = default;
        Quaternion(Quaternion &&) = default;
        ~Quaternion() = default;
        
        static const Quaternion Identity;*/
    };
        
    //std::ostream &operator<< (std::ostream &, const gdk::Quaternion &);
}

#endif
