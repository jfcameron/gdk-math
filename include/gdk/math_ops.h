// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_MATH_OPS_H
#define GDK_MATH_MATH_OPS_H

#include <gdk/matrix3x3.h>
#include <gdk/matrix4x4.h>
#include <gdk/quaternion.h>
#include <gdk/vector3.h>
#include <gdk/vector4.h>

/// \file Every operation that combines two *different* math types
namespace gdk {
    //! apply a transform to a 4d vector, as `M * v` 
    template<typename component_type>
    [[nodiscard]] constexpr vector4<component_type> operator*(const matrix4x4<component_type> &aMatrix,
        const vector4<component_type> &aVector) {
        return {
            aMatrix.get(0, 0) * aVector.x + aMatrix.get(1, 0) * aVector.y
                + aMatrix.get(2, 0) * aVector.z + aMatrix.get(3, 0) * aVector.w,
            aMatrix.get(0, 1) * aVector.x + aMatrix.get(1, 1) * aVector.y
                + aMatrix.get(2, 1) * aVector.z + aMatrix.get(3, 1) * aVector.w,
            aMatrix.get(0, 2) * aVector.x + aMatrix.get(1, 2) * aVector.y
                + aMatrix.get(2, 2) * aVector.z + aMatrix.get(3, 2) * aVector.w,
            aMatrix.get(0, 3) * aVector.x + aMatrix.get(1, 3) * aVector.y
                + aMatrix.get(2, 3) * aVector.z + aMatrix.get(3, 3) * aVector.w};
    }

    //! apply a transform to a 3d point, as `M * v`. Extended with w = 1, so the translation
    //! applies, then divided through by w.
    template<typename component_type>
    [[nodiscard]] constexpr vector3<component_type> operator*(const matrix4x4<component_type> &aMatrix,
        const vector3<component_type> &aVector) {
        return (aMatrix * vector4<component_type>(aVector)).to_point();
    }

    //! apply a 3x3 transform to a 3d vector, as `M * v`. No translation, so correct for a direction.
    template<typename component_type>
    [[nodiscard]] constexpr vector3<component_type> operator*(const matrix3x3<component_type> &aMatrix,
        const vector3<component_type> &aVector) {
        return {
            aMatrix.get(0, 0) * aVector.x + aMatrix.get(1, 0) * aVector.y
                + aMatrix.get(2, 0) * aVector.z,
            aMatrix.get(0, 1) * aVector.x + aMatrix.get(1, 1) * aVector.y
                + aMatrix.get(2, 1) * aVector.z,
            aMatrix.get(0, 2) * aVector.x + aMatrix.get(1, 2) * aVector.y
                + aMatrix.get(2, 2) * aVector.z};
    }

    //! the upper-left 3x3 of a transform: its rotation and scale, without the translation
    template<typename component_type>
    [[nodiscard]] constexpr matrix3x3<component_type> upper_left(const matrix4x4<component_type> &aMatrix) {
        matrix3x3<component_type> result;

        for (std::size_t column = 0; column < matrix3x3<component_type>::order; ++column)
            for (std::size_t row = 0; row < matrix3x3<component_type>::order; ++row)
                result.set(column, row, aMatrix.get(column, row));

        return result;
    }

    //! the matrix that transforms normals: the inverse transpose of the upper-left 3x3, **not**
    //! the model matrix, which does not preserve perpendicularity under non-uniform scale.
    template<typename component_type>
    [[nodiscard]] constexpr matrix3x3<component_type> normal_matrix(const matrix4x4<component_type> &aMatrix) {
        return upper_left(aMatrix).inversed().transposed();
    }

    //! rotate a 3d vector by a **unit** quaternion. No translation, so correct for a direction.
    template<typename component_type>
    [[nodiscard]] constexpr vector3<component_type> operator*(const quaternion<component_type> &aRotation,
        const vector3<component_type> &aVector) {
        const vector3<component_type> axis{aRotation.x, aRotation.y, aRotation.z};

        const auto t = axis.cross_product(aVector) * component_type(2);

        return aVector + t * aRotation.w + axis.cross_product(t);
    }
}

#endif
