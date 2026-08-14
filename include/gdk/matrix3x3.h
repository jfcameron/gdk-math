// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_MAT3X3_H
#define GDK_MATH_MAT3X3_H

#include <gdk/storage.inl> // varies by implementation 

#include <array>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace gdk {
    /// \brief 3 by 3 matrix: the linear part of a transform, without translation
    /// - **column-major**: data layout is column first
    /// - **Storage order is part of the interface, it does not vary with implementations.**
    template<typename component_type_param = float>
    class matrix3x3 final : public matrix3x3_storage<component_type_param> {
    public:
        static_assert(std::is_floating_point<component_type_param>::value,
            "component_type must be a floating point type");

        using component_type = component_type_param;
        using order_type = std::size_t;

        using matrix3x3_storage<component_type_param>::order;

        static const matrix3x3<component_type> identity;

    private:
        using matrix3x3_storage<component_type_param>::m;

        constexpr std::size_t index(order_type aX, order_type aY) const;

    public:
        //! write one element. **Column first, row second.**
        constexpr void set(const order_type aX, const order_type aY, const component_type aValue);

        constexpr component_type &get(const order_type aX, const order_type aY);
        [[nodiscard]] constexpr const component_type &get(const order_type aX, const order_type aY) const;

        //! first of the nine contiguous components, column-major
        [[nodiscard]] constexpr const component_type &front() const;

        //! Sets the matrix to an identity matrix
        constexpr void set_to_identity();

        //! assign values to all 9 elements of the matrix
        constexpr matrix3x3<component_type> &set(
            const component_type m00, const component_type m01, const component_type m02,
            const component_type m10, const component_type m11, const component_type m12,
            const component_type m20, const component_type m21, const component_type m22);

        //! multiply the matrix against another
        constexpr matrix3x3<component_type> &multiply(const matrix3x3 &right);

        //! transpose the matrix in place
        constexpr void transpose();

        //! get the transpose matrix, leaving this one alone
        [[nodiscard]] constexpr matrix3x3<component_type> transposed() const;

        //! the determinant. Zero means the matrix is singular and cannot be inverted.
        [[nodiscard]] constexpr component_type determinant() const;

        //! convert this matrix to its inverse
        constexpr void inverse();

        //! get the inverse, leaving this one alone
        [[nodiscard]] constexpr matrix3x3<component_type> inversed() const;

        [[nodiscard]] constexpr matrix3x3 operator*(const matrix3x3 &other) const;
        constexpr matrix3x3 &operator*=(const matrix3x3 &other);

        matrix3x3 &operator=(const matrix3x3 &) = default;

        //! component-wise equivalence
        [[nodiscard]] constexpr bool operator==(const matrix3x3<component_type> &other) const;

        //! the negation of operator==
        [[nodiscard]] constexpr bool operator!=(const matrix3x3<component_type> &other) const;

        constexpr matrix3x3(
            const component_type a00, const component_type a01, const component_type a02,
            const component_type a10, const component_type a11, const component_type a12,
            const component_type a20, const component_type a21, const component_type a22);

        matrix3x3() = default;
        matrix3x3(const matrix3x3<component_type> &) = default;
        matrix3x3(matrix3x3<component_type> &&) = default;
        ~matrix3x3() = default;
    };
}

#include <gdk/matrix3x3.inl> // varies by implementation 

#endif
