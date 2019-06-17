// © 2018 Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_MAT4X4_H
#define GDK_MATH_MAT4X4_H

#include <iosfwd>
#include <type_traits>

#include <gdk/vector2.h>
#include <gdk/vector3.h>
#include <gdk/quaternion.h>

namespace gdk
{
    /// \brief 4 by 4 matrix of floating point numbers, used to calculate 3D transformations and camera projections.
    template<typename T = float>
    struct Mat4x4 final
    {
        static_assert(std::is_floating_point<T>::value, "T must be a floating point type");

        using size_type = char;

        static constexpr size_type RowOrColumnCount = 4;

        //! the 16 Ts of data, arranged in 2d grid
        T m[RowOrColumnCount][RowOrColumnCount] =  
        {
            {1.,0.,0.,0.},
            {0.,1.,0.,0.},
            {0.,0.,1.,0.},
            {0.,0.,0.,1.},
        };

        /*//! Sets matrix to an identity matrix
        void setToIdentity(); //TODO: merge this with set, so set will have a set(const Mat4x4 &) overload. so a.setToIdentity() becomes a.set(mat4x4::Identity)? Or call it Reset? Not sure.

        //! Sets matrix to an orthographic projection matrix, typically used to render a 2D scene or to render maps (lighting, depth) of a 3D scene
        void setToOrthographic(const gdk::Vector2 &aOrthoSize, const T aNearClippingPlane, const T aFarClippingPlane, const T aViewportAspectRatio); //TODO: Remove this?

        //! Sets matrix to a perspective projection matrix, typically used to render a 3D scene
        void setToPerspective(const T aFieldOfView, const T aNearClippingPlane, const T aFarClippingPlane, const T aViewportAspectRatio); //TODO: Remove this?*/

        //! apply a translation to the matrix
        void translate(const Vector3<T> &aTranslation)
        {
            m[3][0] = m[0][0] * aTranslation.x + m[1][0] * aTranslation.y + m[2][0] * aTranslation.z + m[3][0];
            m[3][1] = m[0][1] * aTranslation.x + m[1][1] * aTranslation.y + m[2][1] * aTranslation.z + m[3][1];
            m[3][2] = m[0][2] * aTranslation.x + m[1][2] * aTranslation.y + m[2][2] * aTranslation.z + m[3][2];
            m[3][3] = m[0][3] * aTranslation.x + m[1][3] * aTranslation.y + m[2][3] * aTranslation.z + m[3][3];
        }

        //! apply a rotation to the matrix
        template<typename high_precision_buffer_type = long double>
        void rotate(const Quaternion<T> &aRotation)
        {
            const Quaternion<T> &q = aRotation;
            
            high_precision_buffer_type sqw = q.w * q.w;
            high_precision_buffer_type sqx = q.x * q.x;
            high_precision_buffer_type sqy = q.y * q.y;
            high_precision_buffer_type sqz = q.z * q.z;
            
            // invs (inverse square length) is only required if quaternion is not already normalised
            high_precision_buffer_type invs = 1 / (sqx + sqy + sqz + sqw);
            
            m[0][0] = ( sqx - sqy - sqz + sqw) * invs ; // since sqw + sqx + sqy + sqz =1/invs*invs
            m[1][1] = (-sqx + sqy - sqz + sqw) * invs ;
            m[2][2] = (-sqx - sqy + sqz + sqw) * invs ;
            
            high_precision_buffer_type tmp1 = q.x * q.y;
            high_precision_buffer_type tmp2 = q.z * q.w;
            
            m[1][0] = 2.0 * static_cast<T>(tmp1 + tmp2) * invs;
            m[0][1] = 2.0 * static_cast<T>(tmp1 - tmp2) * invs;
            
            tmp1 = q.x * q.z;
            tmp2 = q.y * q.w;
            
            m[2][0] = 2.0 * static_cast<T>(tmp1 - tmp2) * invs;
            m[0][2] = 2.0 * static_cast<T>(tmp1 + tmp2) * invs;
            
            tmp1 = q.y * q.z;
            tmp2 = q.x * q.w;
            
            m[2][1] = 2.0 * static_cast<T>(tmp1 + tmp2) * invs;
            m[1][2] = 2.0 * static_cast<T>(tmp1 - tmp2) * invs;
        }

        //! apply a scale to the matrix
        void scale(const Vector3<T> &aScale)
        {
            m[0][0] = m[0][0] * aScale.x;
            m[0][1] = m[0][1] * aScale.x;
            m[0][2] = m[0][2] * aScale.x;
            m[0][3] = m[0][3] * aScale.x;
            m[1][0] = m[1][0] * aScale.y;
            m[1][1] = m[1][1] * aScale.y;
            m[1][2] = m[1][2] * aScale.y;
            m[1][3] = m[1][3] * aScale.y;
            m[2][0] = m[2][0] * aScale.z;
            m[2][1] = m[2][1] * aScale.z;
            m[2][2] = m[2][2] * aScale.z;
            m[2][3] = m[2][3] * aScale.z;
            m[3][0] = m[3][0];
            m[3][1] = m[3][1];
            m[3][2] = m[3][2];
            m[3][3] = m[3][3];
        }

        //! transpose the matrix
        void transpose()
        {
            T t00 = m[0][0]; T t10 = m[0][1]; T t20 = m[0][2]; T t30 = m[0][3];
            T t01 = m[1][0]; T t11 = m[1][1]; T t21 = m[1][2]; T t31 = m[1][3];
            T t02 = m[2][0]; T t12 = m[2][1]; T t22 = m[2][2]; T t32 = m[2][3];
            T t03 = m[3][0]; T t13 = m[3][1]; T t23 = m[3][2]; T t33 = m[3][3];
            
            set(
                t00, t10, t20, t30,
                t01, t11, t21, t31,
                t02, t12, t22, t32,
                t03, t13, t23, t33
            );
        }

        //! assign values to all 16 elements of the matrix
        Mat4x4 &set(
            const T m00, const T m01, const T m02, const T m03, 
            const T m10, const T m11, const T m12, const T m13,
            const T m20, const T m21, const T m22, const T m23, 
            const T m30, const T m31, const T m32, const T m33
        )
        {
            m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
            m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
            m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
            m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
            
            return *this;
        }

        //! multiply the matrix against another
        Mat4x4<T> &multiply(const Mat4x4 &right)
        {
            set(
                m[0][0] * right.m[0][0] + m[1][0] * right.m[0][1] + m[2][0] * right.m[0][2] + m[3][0] * right.m[0][3],
                m[0][1] * right.m[0][0] + m[1][1] * right.m[0][1] + m[2][1] * right.m[0][2] + m[3][1] * right.m[0][3],
                m[0][2] * right.m[0][0] + m[1][2] * right.m[0][1] + m[2][2] * right.m[0][2] + m[3][2] * right.m[0][3],
                m[0][3] * right.m[0][0] + m[1][3] * right.m[0][1] + m[2][3] * right.m[0][2] + m[3][3] * right.m[0][3],
                m[0][0] * right.m[1][0] + m[1][0] * right.m[1][1] + m[2][0] * right.m[1][2] + m[3][0] * right.m[1][3],
                m[0][1] * right.m[1][0] + m[1][1] * right.m[1][1] + m[2][1] * right.m[1][2] + m[3][1] * right.m[1][3],
                m[0][2] * right.m[1][0] + m[1][2] * right.m[1][1] + m[2][2] * right.m[1][2] + m[3][2] * right.m[1][3],
                m[0][3] * right.m[1][0] + m[1][3] * right.m[1][1] + m[2][3] * right.m[1][2] + m[3][3] * right.m[1][3],
                m[0][0] * right.m[2][0] + m[1][0] * right.m[2][1] + m[2][0] * right.m[2][2] + m[3][0] * right.m[2][3],
                m[0][1] * right.m[2][0] + m[1][1] * right.m[2][1] + m[2][1] * right.m[2][2] + m[3][1] * right.m[2][3],
                m[0][2] * right.m[2][0] + m[1][2] * right.m[2][1] + m[2][2] * right.m[2][2] + m[3][2] * right.m[2][3],
                m[0][3] * right.m[2][0] + m[1][3] * right.m[2][1] + m[2][3] * right.m[2][2] + m[3][3] * right.m[2][3],
                m[0][0] * right.m[3][0] + m[1][0] * right.m[3][1] + m[2][0] * right.m[3][2] + m[3][0] * right.m[3][3],
                m[0][1] * right.m[3][0] + m[1][1] * right.m[3][1] + m[2][1] * right.m[3][2] + m[3][1] * right.m[3][3],
                m[0][2] * right.m[3][0] + m[1][2] * right.m[3][1] + m[2][2] * right.m[3][2] + m[3][2] * right.m[3][3],
                m[0][3] * right.m[3][0] + m[1][3] * right.m[3][1] + m[2][3] * right.m[3][2] + m[3][3] * right.m[3][3]
            );
            
            return *this;
        }

        //! returns a new matrix which is the result of a multiplication, without mutating this
        Mat4x4 operator*(const Mat4x4 &other) const
        {
            Mat4x4 copy(*this);

            copy *= other;

            return copy;
        }

        //! alias for multiply
        Mat4x4& operator*=(const Mat4x4 &other)
        {
            multiply(other);
            
            return *this;
        }
        
        Mat4x4& operator=(const Mat4x4&) = default; 

        bool operator==(const Mat4x4<T> &other) const
        {
            for(size_type i = 0; i < RowOrColumnCount; ++i) for (size_type j = 0; j < RowOrColumnCount; ++j)
                if (m[i][j] != other.m[i][j])
                    return false;

            return true;
        }
            
        Mat4x4(
            const T a00, const T a01, const T a02, const T a03, 
            const T a10, const T a11, const T a12, const T a13,
            const T a20, const T a21, const T a22, const T a23, 
            const T a30, const T a31, const T a32, const T a33
        )
        : m ( 
            a00, a01, a02, a03, 
            a10, a11, a12, a13, 
            a20, a21, a22, a23, 
            a30, a31, a32, a33
        ) 
        {}

        Mat4x4<T>() = default;
        Mat4x4<T>(const Mat4x4<T>&) = default;
        Mat4x4<T>(Mat4x4<T>&&) = default;
        ~Mat4x4<T>() = default;

        //! multiplicative identity matrix
        static const Mat4x4<T> Identity; 
    };

    template<typename T> const Mat4x4<T> Mat4x4<T>::Identity = Mat4x4<T>();
        
    //std::ostream& operator<< (std::ostream&, const gdk::Mat4x4&);
}

#endif
