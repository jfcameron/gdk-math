// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_IMPL_STD_VECTOR4_INL
#define GDK_MATH_IMPL_STD_VECTOR4_INL

namespace gdk {
    template<typename component_type>
    constexpr vector3<component_type> vector4<component_type>::xyz() const {
        return {x, y, z};
    }

    template<typename component_type>
    constexpr vector3<component_type> vector4<component_type>::to_point() const {
        if (w == static_cast<component_type>(0)) return xyz();

        return {x / w, y / w, z / w};
    }

    template<typename component_type>
    constexpr vector4<component_type> vector4<component_type>::direction(
        const vector3<component_type> &aVector) {
        return {aVector, static_cast<component_type>(0)};
    }

    template<typename component_type>
    constexpr bool vector4<component_type>::operator==(const vector4<component_type> &other) const {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }

    template<typename component_type>
    constexpr bool vector4<component_type>::operator!=(const vector4<component_type> &other) const {
        return x != other.x || y != other.y || z != other.z || w != other.w;
    }

    template<typename component_type>
    constexpr vector4<component_type>::vector4(const component_type &aX, const component_type &aY,
        const component_type &aZ, const component_type &aW)
    : vector4_storage<component_type>{aX, aY, aZ, aW}
    {}

    template<typename component_type>
    constexpr vector4<component_type>::vector4(const vector3<component_type> &avector3,
        const component_type &aW)
    : vector4<component_type>(avector3.x, avector3.y, avector3.z, aW)
    {}

    template<typename component_type>
    constexpr vector4<component_type>::vector4()
    : vector4<component_type>(0., 0., 0., 1.)
    {}

    template<typename T> const vector4<T> vector4<T>::origin = {0., 0., 0., 1.};
}

#endif
