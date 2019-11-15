// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_QUATERNION_H
#define GDK_MATH_QUATERNION_H

#include <iosfwd>
#include <iostream>
#include <type_traits>

#include <glm/gtc/quaternion.hpp> //TODO: REMOVE GLM DEPENDENCY

#include <gdk/vector3.h>

namespace gdk
{
    /// \brief Used to represent 3d rotations in a way insusceptible to gimblal lock (unlike Vector3)
    template<typename component_type_param = float>
    struct Quaternion final
    {
        using component_type = component_type_param;

        static_assert(std::is_floating_point<component_type>::value, "component_type must be a floating point type");

        component_type x = {0.}, y = {0.}, z = {0.}, w = {1.};
            
        void setFromEuler(const Vector3<component_type> &aEulerAngles) //TODO: REMOVE GLM DEPENDENCY
        {
            glm::vec3 buff = {aEulerAngles.x, aEulerAngles.y, aEulerAngles.z};

            glm::quat quat(buff);

            x = quat.x;
            y = quat.y;
            z = quat.z;
            w = quat.w;
        }

        Vector3<component_type> toEuler() const //TODO: REMOVE GLM DEPENDENCY
        {
            glm::quat quaternion(w, x, y, z);

            //convert from radian to euler
            glm::vec3 buff = glm::eulerAngles(quaternion);

            return {buff.x, buff.y, buff.z};
        }
            
        Quaternion<component_type> &operator=(const Quaternion<component_type> &) = default;
        Quaternion<component_type> &operator=(Quaternion<component_type> &&) = default;
        
        bool operator==(const Quaternion<component_type> &other) const
        {
            return x == other.x && y == other.y && z == other.z && w == other.w;
        }
            
        Quaternion<component_type>(const Vector3<component_type> &aEulerAngles)
        {
            setFromEuler(aEulerAngles);
        }

        Quaternion<component_type>(const component_type &aX, const component_type &aY, const component_type &aZ, const component_type &aW)
        : x(aX)
        , y(aY)
        , z(aZ)
        , w(aW)
        {}

        Quaternion<component_type>() = default;

        Quaternion<component_type>(const Quaternion<component_type> &) = default;
        Quaternion<component_type>(Quaternion<component_type> &&) = default;
        ~Quaternion<component_type>() = default;
        
        static const Quaternion<component_type> Identity;

    private:
    };
        
    template <typename T> const Quaternion<T> Quaternion<T>::Identity = Quaternion();

    //std::ostream &operator<< (std::ostream &, const gdk::Quaternion<component_type> &);
    
}

#endif
