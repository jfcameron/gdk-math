// © Joseph Cameron - MIT License

#ifndef GDK_MATH_IMPL_STD_VECTOR4_INL
#define GDK_MATH_IMPL_STD_VECTOR4_INL

namespace gdk::math {
    template<arithmetic_component component_type>
    constexpr vector3<component_type> vector4<component_type>::xyz() const {
        return {x, y, z};
    }

    template<arithmetic_component component_type>
    constexpr vector3<component_type> vector4<component_type>::to_point() const {
        if (w == static_cast<component_type>(0)) return xyz();

        return {static_cast<component_type>(x / w), static_cast<component_type>(y / w),
            static_cast<component_type>(z / w)};
    }

    template<arithmetic_component component_type>
    constexpr vector4<component_type> vector4<component_type>::direction(
        const vector3<component_type> &aVector) {
        return {aVector, static_cast<component_type>(0)};
    }

    template<arithmetic_component component_type>
    constexpr bool vector4<component_type>::operator==(const vector4<component_type> &other) const {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }

    template<arithmetic_component component_type>
    constexpr vector4<component_type>::vector4(const component_type &aX, const component_type &aY,
        const component_type &aZ, const component_type &aW)
    : vector4_storage<component_type>{aX, aY, aZ, aW}
    {}

    template<arithmetic_component component_type>
    constexpr vector4<component_type>::vector4(const vector3<component_type> &avector3,
        const component_type &aW)
    : vector4<component_type>(avector3.x, avector3.y, avector3.z, aW)
    {}

    template<arithmetic_component component_type>
    constexpr vector4<component_type>::vector4()
    : vector4<component_type>(component_type(0), component_type(0), component_type(0), component_type(1))
    {}

    template<arithmetic_component T> const vector4<T> vector4<T>::origin = {T(0), T(0), T(0), T(1)};
}

#endif
