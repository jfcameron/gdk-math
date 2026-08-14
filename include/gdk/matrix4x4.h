// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_MAT4X4_H
#define GDK_MATH_MAT4X4_H

#include <gdk/storage.inl> // varies by implementation 

#include <gdk/quaternion.h>
#include <gdk/vector2.h>
#include <gdk/vector3.h>
#include <gdk/vector4.h>

#include <array>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <cmath>
#include <iosfwd>
#include <type_traits>

namespace gdk {
    /// \brief 4 by 4 matrix used to calculate 3D transformations and camera projections.
    /// - **right-handed**: +X right, +Y up, +Z back
    /// - **column-major**: data layout is column first 
    /// - **Storage order is part of the interface, it does not vary with implementations.** 
    template<typename component_type_param = float>
    class matrix4x4 final : public matrix4x4_storage<component_type_param> {
    public:
        static_assert(std::is_floating_point<component_type_param>::value, 
            "component_type must be a floating point type");

        using component_type = component_type_param;
        using order_type = std::size_t;
        using quaternion_type = quaternion<component_type_param>;
        using vector2_type = vector2<component_type_param>;
        using vector3_type = vector3<component_type_param>;

        using matrix4x4_storage<component_type_param>::order;

        static const matrix4x4<component_type> identity; 

    private: 
        using matrix4x4_storage<component_type_param>::m;

        constexpr std::size_t index(order_type aX, order_type aY) const;

        //! determinant of the upper-left 3x3. Negative means the basis is mirrored.
        [[nodiscard]] constexpr component_type basis_determinant() const;

    public:
        //! write one element. **Column first, row second.**
        constexpr void set(const order_type aX, const order_type aY, const component_type aValue);

        constexpr component_type &get(const order_type aX, const order_type aY);
        [[nodiscard]] constexpr const component_type &get(const order_type aX, const order_type aY) const;

        //! first of the sixteen contiguous components, column-major
        [[nodiscard]] constexpr const component_type &front() const;

        //! Sets the matrix to an identity matrix
        constexpr void set_to_identity();

        //! set the translation components directly
        constexpr void set_translation(const vector3_type &aTranslation);

        //! get the translation vector from this matrix
        [[nodiscard]] constexpr vector3_type translation() const;

        //! get the rotation as a quaternion. 
        [[nodiscard]] quaternion_type rotation() const;

        //! set the rotation component directly (rotation and scale use the same components of a 4x4 matrix)
        constexpr void set_rotation_and_scale(const quaternion_type &aRotation, const vector3_type &aScale);

        //! sets the rotation while preserving scaling
        void set_rotation(const quaternion_type &aRotation);

        //! sets the scale while preserving rotation
        void set_scale(const vector3_type &aScale);

        //! transpose the matrix in place
        constexpr void transpose();

        //! get the transpose matrix, leaving this one alone
        [[nodiscard]] constexpr matrix4x4<component_type> transposed() const;

        //! the determinant. Zero means the matrix is singular and cannot be inverted.
        [[nodiscard]] constexpr component_type determinant() const;

        //! convert this matrix to its inverse
        constexpr void inverse();

        //! get the inverse, leaving this one alone
        [[nodiscard]] constexpr matrix4x4<component_type> inversed() const;

        //! set this matrix to its inverse via a faster means but requires that the matrix is affine,
        /// meaning the matrix must only contain translations, rotations and scales.
        constexpr void inverse_affine();

        //! assign values to all 16 elements of the matrix
        constexpr matrix4x4<component_type> &set(
            const component_type m00, const component_type m01, const component_type m02, const component_type m03, 
            const component_type m10, const component_type m11, const component_type m12, const component_type m13,
            const component_type m20, const component_type m21, const component_type m22, const component_type m23, 
            const component_type m30, const component_type m31, const component_type m32, const component_type m33);

        //! multiply the matrix against another
        constexpr matrix4x4<component_type> &multiply(const matrix4x4 &right);

        //! gets the scaling factor from the internal rotation matrix
        [[nodiscard]] vector3_type scale() const;

        //! returns a new matrix which is the result of a multiplication
        [[nodiscard]] constexpr matrix4x4 operator*(const matrix4x4 &other) const;

        //! alias for multiply
        constexpr matrix4x4 &operator*=(const matrix4x4 &other);

        matrix4x4 &operator=(const matrix4x4&) = default; 

        //! the negation of operator==
        [[nodiscard]] constexpr bool operator!=(const matrix4x4<component_type> &other) const;

        //! component-wise equivalance
        [[nodiscard]] constexpr bool operator==(const matrix4x4<component_type> &other) const;

        constexpr matrix4x4(
            const component_type a00, const component_type a01, const component_type a02, const component_type a03, 
            const component_type a10, const component_type a11, const component_type a12, const component_type a13,
            const component_type a20, const component_type a21, const component_type a22, const component_type a23, 
            const component_type a30, const component_type a31, const component_type a32, const component_type a33);

        constexpr matrix4x4(
            const vector3_type &aTranslationComponent, 
            const quaternion_type &aRotationComponent,
            const vector3_type &aScale = vector3_type(1));

        matrix4x4() = default;
        matrix4x4(const matrix4x4<component_type>&) = default;
        matrix4x4(matrix4x4<component_type>&&) = default;
        ~matrix4x4() = default;
    };
}

#include <gdk/matrix4x4.inl> // varies by implementation 

#endif
