// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_MAT4X4_H
#define GDK_MATH_MAT4X4_H

#include <gdk/quaternion.h>
#include <gdk/vector2.h>
#include <gdk/vector3.h>

#include <cmath>
#include <iosfwd>
#include <type_traits>

namespace gdk
{
    /// \brief 4 by 4 matrix of floating point numbers, used to calculate 3D transformations and camera projections.
    template<typename component_type_param = float>
    struct Mat4x4 final {
        using component_type = component_type_param;
        using order_type = char;
        using quaternion_type = Quaternion<component_type_param>;
        using vector2_type = Vector2<component_type_param>;
        using vector3_type = Vector3<component_type_param>;

        static_assert(std::is_floating_point<component_type>::value, "component_type must be a floating point type");
        
        static constexpr order_type order = 4;

        component_type m[order][order] =  {
            {1.,0.,0.,0.},
            {0.,1.,0.,0.},
            {0.,0.,1.,0.},
            {0.,0.,0.,1.},
        };

        //! Sets matrix to an identity matrix
        void setToIdentity() {
            m[0][0] = 1.; m[1][0] = 0.; m[2][0] = 0.; m[3][0] = 0.;
            m[0][1] = 0.; m[1][1] = 1.; m[2][1] = 0.; m[3][1] = 0.;
            m[0][2] = 0.; m[1][2] = 0.; m[2][2] = 1.; m[3][2] = 0.;
            m[0][3] = 0.; m[1][3] = 0.; m[2][3] = 0.; m[3][3] = 1.;    
        }

        //! Sets matrix to an orthographic projection matrix
        void setToOrthographic(
            const vector2_type &aOrthoSize, 
            const component_type aNearClippingPlane, 
            const component_type aFarClippingPlane, 
            const component_type aViewportAspectRatio)
        {
            const component_type x = aOrthoSize.x * 2 / aViewportAspectRatio;
            const component_type y = aOrthoSize.y * 2;
            const component_type n = -(aFarClippingPlane + aNearClippingPlane) / (aFarClippingPlane - aNearClippingPlane);
            const component_type f = -2.0f * aFarClippingPlane * aNearClippingPlane / (aFarClippingPlane - aNearClippingPlane);

            m[0][0] = x ; m[1][0] = 0.; m[2][0] = 0.; m[3][0] = 0.;
            m[0][1] = 0.; m[1][1] = y ; m[2][1] = 0.; m[3][1] = 0.;
            m[0][2] = 0.; m[1][2] = 0.; m[2][2] = n ; m[3][2] = f ;
            m[0][3] = 0.; m[1][3] = 0.; m[2][3] = 0.; m[3][3] = 1.;
        }

        //! Sets matrix to a perspective projection matrix
        void setToPerspective(
            const component_type aFieldOfView, 
            const component_type aNearClippingPlane, 
            const component_type aFarClippingPlane, 
            const component_type aViewportAspectRatio)
        {
            component_type tanHalfFovy = static_cast<component_type>(tan(aFieldOfView * 0.5f));

            m[0][0] = 1.0f / (aViewportAspectRatio * tanHalfFovy);
            m[0][1] = 0.0f;
            m[0][2] = 0.0f;
            m[0][3] = 0.0f;

            m[1][0] = 0.0f;
            m[1][1] = 1.0f / tanHalfFovy;
            m[1][2] = 0.0f;
            m[1][3] = 0.0f;

            m[2][0] = 0.0f;
            m[2][1] = 0.0f;
            m[2][2] =-(aFarClippingPlane + aNearClippingPlane) / (aFarClippingPlane - aNearClippingPlane);
            m[2][3] =-1.0f;

            m[3][0] = 0.0f;
            m[3][1] = 0.0f;
            m[3][2] =-2.0f * aFarClippingPlane * aNearClippingPlane / (aFarClippingPlane - aNearClippingPlane);
            m[3][3] = 0.0f;
        }

        //! apply a translation to the matrix
        void translate(const vector3_type &aTranslation) {
            m[3][0] = m[0][0] * aTranslation.x + m[1][0] * aTranslation.y + m[2][0] * aTranslation.z + m[3][0];
            m[3][1] = m[0][1] * aTranslation.x + m[1][1] * aTranslation.y + m[2][1] * aTranslation.z + m[3][1];
            m[3][2] = m[0][2] * aTranslation.x + m[1][2] * aTranslation.y + m[2][2] * aTranslation.z + m[3][2];
            m[3][3] = m[0][3] * aTranslation.x + m[1][3] * aTranslation.y + m[2][3] * aTranslation.z + m[3][3];
        }

        //! get the translation vector from this matrix
        vector3_type translation() const {
            return vector3_type(m[3][0], m[3][1], m[3][2]);
        }

        //! get a rotation quaternion from this matrix
        quaternion_type rotation() const {
            component_type t{};
            quaternion_type q{};

            if (m[2][2] < 0) {
                if (m[0][0] > m[1][1]) {
                    t = 1 + m[0][0] - m[1][1] - m[2][2];
                    q = quaternion_type(t, m[0][1] + m[1][0], m[2][0] + m[0][2], m[1][2] - m[2][1]);
                }
                else {
                    t = 1 - m[0][0] + m[1][1] - m[2][2];
                    q = quaternion_type(m[0][1] + m[1][0], t, m[1][2] + m[2][1], m[2][0] - m[0][2]);
                }
            }
            else {
                if (m[0][0] < - m[1][1]) {
                    t = 1 - m[0][0] - m[1][1] + m[2][2];
                    q = quaternion_type(m[2][0] + m[0][2], m[1][2] + m[2][1], t, m[0][1] - m[1][0]);
                }
                else {
                    t = 1 + m[0][0] + m[1][1] + m[2][2];
                    q = quaternion_type(m[1][2] - m[2][1], m[2][0] - m[0][2], m[0][1] - m[1][0], t);
                }
            }

            q *= 0.5 / sqrt(t);

            q = {q.toEuler() * - 1};

            return q;
        }

        //! apply a rotation to the matrix
        template<typename high_precision_buffer_type = long double>
        void rotate(const quaternion_type &aRotation) {
            static_assert(std::is_floating_point<high_precision_buffer_type>::value, "high_precision_buffer_type must be a floating point type");

            using namespace std;

            const Quaternion<component_type> &q = aRotation;
            
            high_precision_buffer_type sqw = q.w * q.w;
            high_precision_buffer_type sqx = q.x * q.x;
            high_precision_buffer_type sqy = q.y * q.y;
            high_precision_buffer_type sqz = q.z * q.z;
            
            high_precision_buffer_type invs = 1 / (sqx + sqy + sqz + sqw);
            
            m[0][0] = ( sqx - sqy - sqz + sqw) * invs ; 
            m[1][1] = (-sqx + sqy - sqz + sqw) * invs ;
            m[2][2] = (-sqx - sqy + sqz + sqw) * invs ;
            
            high_precision_buffer_type tmp1 = q.x * q.y;
            high_precision_buffer_type tmp2 = q.z * q.w;
            
            m[1][0] = 2.0 * static_cast<component_type>(tmp1 + tmp2) * invs;
            m[0][1] = 2.0 * static_cast<component_type>(tmp1 - tmp2) * invs;
            
            tmp1 = q.x * q.z;
            tmp2 = q.y * q.w;
            
            m[2][0] = 2.0 * static_cast<component_type>(tmp1 - tmp2) * invs;
            m[0][2] = 2.0 * static_cast<component_type>(tmp1 + tmp2) * invs;
            
            tmp1 = q.y * q.z;
            tmp2 = q.x * q.w;
            
            m[2][1] = 2.0 * static_cast<component_type>(tmp1 + tmp2) * invs;
            m[1][2] = 2.0 * static_cast<component_type>(tmp1 - tmp2) * invs;
        }

        //! apply a scale to the matrix
        void scale(const vector3_type &aScale) {
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

        //! transpose the matrix in place
        void transpose() {
            component_type t00 = m[0][0]; 
            component_type t10 = m[0][1]; 
            component_type t20 = m[0][2]; 
            component_type t30 = m[0][3];

            component_type t01 = m[1][0]; 
            component_type t11 = m[1][1]; 
            component_type t21 = m[1][2]; 
            component_type t31 = m[1][3];

            component_type t02 = m[2][0]; 
            component_type t12 = m[2][1]; 
            component_type t22 = m[2][2]; 
            component_type t32 = m[2][3];

            component_type t03 = m[3][0]; 
            component_type t13 = m[3][1]; 
            component_type t23 = m[3][2]; 
            component_type t33 = m[3][3];
            
            set(
                t00, t10, t20, t30,
                t01, t11, t21, t31,
                t02, t12, t22, t32,
                t03, t13, t23, t33
            );
        }

        //! convert this matrix to its inverse
        void inverse() {
            component_type s0 = m[0][0] * m[1][1] - m[1][0] * m[0][1];
            component_type s1 = m[0][0] * m[1][2] - m[1][0] * m[0][2];
            component_type s2 = m[0][0] * m[1][3] - m[1][0] * m[0][3];
            component_type s3 = m[0][1] * m[1][2] - m[1][1] * m[0][2];
            component_type s4 = m[0][1] * m[1][3] - m[1][1] * m[0][3];
            component_type s5 = m[0][2] * m[1][3] - m[1][2] * m[0][3];

            component_type c5 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
            component_type c4 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
            component_type c3 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
            component_type c2 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
            component_type c1 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
            component_type c0 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

            component_type invdet = component_type(1) / (s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0);

            component_type b[order][order];

            b[0][0] = (m[1][1] * c5 - m[1][2] * c4 + m[1][3] * c3) * invdet;
            b[0][1] = (-m[0][1] * c5 + m[0][2] * c4 - m[0][3] * c3) * invdet;
            b[0][2] = (m[3][1] * s5 - m[3][2] * s4 + m[3][3] * s3) * invdet;
            b[0][3] = (-m[2][1] * s5 + m[2][2] * s4 - m[2][3] * s3) * invdet;

            b[1][0] = (-m[1][0] * c5 + m[1][2] * c2 - m[1][3] * c1) * invdet;
            b[1][1] = (m[0][0] * c5 - m[0][2] * c2 + m[0][3] * c1) * invdet;
            b[1][2] = (-m[3][0] * s5 + m[3][2] * s2 - m[3][3] * s1) * invdet;
            b[1][3] = (m[2][0] * s5 - m[2][2] * s2 + m[2][3] * s1) * invdet;

            b[2][0] = (m[1][0] * c4 - m[1][1] * c2 + m[1][3] * c0) * invdet;
            b[2][1] = (-m[0][0] * c4 + m[0][1] * c2 - m[0][3] * c0) * invdet;
            b[2][2] = (m[3][0] * s4 - m[3][1] * s2 + m[3][3] * s0) * invdet;
            b[2][3] = (-m[2][0] * s4 + m[2][1] * s2 - m[2][3] * s0) * invdet;

            b[3][0] = (-m[1][0] * c3 + m[1][1] * c1 - m[1][2] * c0) * invdet;
            b[3][1] = (m[0][0] * c3 - m[0][1] * c1 + m[0][2] * c0) * invdet;
            b[3][2] = (-m[3][0] * s3 + m[3][1] * s1 - m[3][2] * s0) * invdet;
            b[3][3] = (m[2][0] * s3 - m[2][1] * s1 + m[2][2] * s0) * invdet;

            set(
                b[0][0], b[0][1], b[0][2], b[0][3],
                b[1][0], b[1][1], b[1][2], b[1][3],
                b[2][0], b[2][1], b[2][2], b[2][3],
                b[3][0], b[3][1], b[3][2], b[3][3]
            );
        }

        //! assign values to all 16 elements of the matrix
        Mat4x4 &set(
            const component_type m00, const component_type m01, const component_type m02, const component_type m03, 
            const component_type m10, const component_type m11, const component_type m12, const component_type m13,
            const component_type m20, const component_type m21, const component_type m22, const component_type m23, 
            const component_type m30, const component_type m31, const component_type m32, const component_type m33)
        {
            m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
            m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
            m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
            m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
            
            return *this;
        }

        //! multiply the matrix against another
        Mat4x4<component_type> &multiply(const Mat4x4 &right)
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
                m[0][3] * right.m[3][0] + m[1][3] * right.m[3][1] + m[2][3] * right.m[3][2] + m[3][3] * right.m[3][3]);
            
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

        bool operator==(const Mat4x4<component_type> &other) const
        {
            for(order_type i = 0; i < order; ++i) for (order_type j = 0; j < order; ++j)
                if (m[i][j] != other.m[i][j])
                    return false;

            return true;
        }
            
        Mat4x4<component_type>(
            const component_type a00, const component_type a01, const component_type a02, const component_type a03, 
            const component_type a10, const component_type a11, const component_type a12, const component_type a13,
            const component_type a20, const component_type a21, const component_type a22, const component_type a23, 
            const component_type a30, const component_type a31, const component_type a32, const component_type a33)
        : m ( 
            a00, a01, a02, a03, 
            a10, a11, a12, a13, 
            a20, a21, a22, a23, 
            a30, a31, a32, a33
        ) 
        {}

        Mat4x4<component_type>(
            const vector3_type &aWorldPos, 
            const Quaternion<component_type> &aRotation, 
            const vector3_type &aScale)
        {
            setToIdentity();
            translate(aWorldPos);
            rotate(aRotation);
            scale(aScale);
        }

        Mat4x4<component_type>() = default;
        Mat4x4<component_type>(const Mat4x4<component_type>&) = default;
        Mat4x4<component_type>(Mat4x4<component_type>&&) = default;
        ~Mat4x4<component_type>() = default;

        static const Mat4x4<component_type> Identity; 
    };

    template<typename component_type> const Mat4x4<component_type> Mat4x4<component_type>::Identity = Mat4x4<component_type>();
}

#endif
