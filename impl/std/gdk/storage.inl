// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_IMPL_STD_STORAGE_INL
#define GDK_MATH_IMPL_STD_STORAGE_INL

#include <array>
#include <cstddef>

namespace gdk {
    template<typename component_type>
    class vector2_storage {
    public:
        component_type x = {0}, y = {0};
    };

    template<typename component_type>
    class vector3_storage {
    public:
        component_type x = {0}, y = {0}, z = {0};
    };

    template<typename component_type>
    class vector4_storage {
    public:
        component_type x = {0.}, y = {0.}, z = {0.}, w = {1.};
    };

    template<typename component_type>
    class quaternion_storage {
    public:
        component_type x = {0.}, y = {0.}, z = {0.}, w = {1.};
    };

    template<typename component_type>
    class matrix3x3_storage {
    public:
        static constexpr std::size_t order{3};

    protected:
        std::array<component_type, order * order> m = {
            1.,0.,0.,
            0.,1.,0.,
            0.,0.,1.,
        };

        matrix3x3_storage() = default;
        matrix3x3_storage(const matrix3x3_storage &) = default;
        matrix3x3_storage(matrix3x3_storage &&) = default;
        matrix3x3_storage &operator=(const matrix3x3_storage &) = default;
        matrix3x3_storage &operator=(matrix3x3_storage &&) = default;
        ~matrix3x3_storage() = default;
    };

    template<typename component_type>
    class matrix4x4_storage {
    public:
        static constexpr std::size_t order{4};

    protected:
        std::array<component_type, order * order> m = {
            1.,0.,0.,0.,
            0.,1.,0.,0.,
            0.,0.,1.,0.,
            0.,0.,0.,1.,
        };

        matrix4x4_storage() = default;
        matrix4x4_storage(const matrix4x4_storage &) = default;
        matrix4x4_storage(matrix4x4_storage &&) = default;
        matrix4x4_storage &operator=(const matrix4x4_storage &) = default;
        matrix4x4_storage &operator=(matrix4x4_storage &&) = default;
        ~matrix4x4_storage() = default;
    };
}

#endif
