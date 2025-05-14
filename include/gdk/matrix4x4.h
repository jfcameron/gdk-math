// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_MAT4X4_H
#define GDK_MATH_MAT4X4_H

#include <gdk/quaternion.h>
#include <gdk/vector2.h>
#include <gdk/vector3.h>
#include <gdk/vector4.h>

#include <cmath>
#include <iosfwd>
#include <type_traits>

namespace gdk {
    /// \brief 4 by 4 matrix used to calculate 3D transformations and camera projections.
    /// - the matrix assumes opengl style "right handedness": +X is right, +Y is up, -Z is forward. 
    /// - the matrix is row-major
    template<typename component_type_param>
    struct matrix4x4 final {
        static_assert(std::is_floating_point<component_type_param>::value, 
            "component_type must be a floating point type");

        using component_type = component_type_param;
        using order_type = char;
        using quaternion_type = quaternion<component_type_param>;
        using vector2_type = Vector2<component_type_param>;
        using vector3_type = vector3<component_type_param>;

        static constexpr order_type order{4};

        static const matrix4x4<component_type> identity; 

        component_type m[order][order] = {
            {1.,0.,0.,0.},
            {0.,1.,0.,0.},
            {0.,0.,1.,0.},
            {0.,0.,0.,1.},
        };

        //! Sets matrix to an identity matrix
        void set_to_identity() {
            *this = matrix4x4<component_type>::identity;
        }

        //! Sets matrix to an orthographic projection matrix
        void set_to_orthographic(
            const vector2_type &aOrthoSize, 
            const component_type aNearClippingPlane, 
            const component_type aFarClippingPlane, 
            const component_type aViewportAspectRatio) {
            const auto x = 2.0f / (aOrthoSize.x * aViewportAspectRatio);
            const auto y = 2.0f / aOrthoSize.y;
            const auto n = - (aFarClippingPlane + aNearClippingPlane) / (aFarClippingPlane - aNearClippingPlane);
            const auto f = -2.0f / (aFarClippingPlane - aNearClippingPlane);

            m[0][0] = x ; m[1][0] = 0.; m[2][0] = 0.; m[3][0] = 0.;
            m[0][1] = 0.; m[1][1] = y ; m[2][1] = 0.; m[3][1] = 0.;
            m[0][2] = 0.; m[1][2] = 0.; m[2][2] = f ; m[3][2] = n ;
            m[0][3] = 0.; m[1][3] = 0.; m[2][3] = 0.; m[3][3] = 1.;
        }

        //! Sets matrix to a perspective projection matrix
        void set_to_perspective(
            const component_type aFieldOfView, 
            const component_type aNearClippingPlane, 
            const component_type aFarClippingPlane, 
            const component_type aViewportAspectRatio) {
            const auto tanHalfFovy = static_cast<component_type>(tan(aFieldOfView * 0.5));

            m[0][0] = 1.0 / (aViewportAspectRatio * tanHalfFovy);
            m[0][1] = 0.0;
            m[0][2] = 0.0;
            m[0][3] = 0.0;

            m[1][0] = 0.0;
            m[1][1] = 1.0 / tanHalfFovy;
            m[1][2] = 0.0;
            m[1][3] = 0.0;

            m[2][0] = 0.0;
            m[2][1] = 0.0;
            m[2][2] =-(aFarClippingPlane + aNearClippingPlane) / (aFarClippingPlane - aNearClippingPlane);
            m[2][3] =-1.0;

            m[3][0] = 0.0;
            m[3][1] = 0.0;
            m[3][2] =-2.0 * aFarClippingPlane * aNearClippingPlane / (aFarClippingPlane - aNearClippingPlane);
            m[3][3] = 0.0;
        }

        //! set the translation components directly
        void set_translation(const vector3_type &aTranslation) {
            // Only modify the translation components in column 3
            m[3][0] = aTranslation.x;
            m[3][1] = aTranslation.y;
            m[3][2] = aTranslation.z;
        }

        //! get the translation vector from this matrix
        vector3_type translation() const {
            return vector3_type(
                m[3][0], 
                m[3][1], 
                m[3][2]);
        }

        //! get the rotation as a quaternion
        quaternion_type rotation() const {
            vector3_type scale = this->scale();
            std::array<component_type *, 3> scaleAsArray { &scale.x, &scale.y, &scale.z };

            matrix4x4<component_type> normalizedMatrix = *this;
            for (order_type i{0}; i < 3; ++i) {
                for (order_type j{0}; j < 3; ++j) {
                    normalizedMatrix.m[i][j] /= *scaleAsArray[i];
                }
            }

            component_type trace = normalizedMatrix.m[0][0] + normalizedMatrix.m[1][1] + normalizedMatrix.m[2][2];
            quaternion_type q;

            if (trace > 0) {
                component_type s = 0.5 / std::sqrt(trace + 1.0);
                q.w = 0.25 / s;
                q.x = (normalizedMatrix.m[2][1] - normalizedMatrix.m[1][2]) * s;
                q.y = (normalizedMatrix.m[0][2] - normalizedMatrix.m[2][0]) * s;
                q.z = (normalizedMatrix.m[1][0] - normalizedMatrix.m[0][1]) * s;
            } 
            else {
                if (normalizedMatrix.m[0][0] > normalizedMatrix.m[1][1] && normalizedMatrix.m[0][0] > normalizedMatrix.m[2][2]) {
                    component_type s = 2.0 * std::sqrt(1.0 + normalizedMatrix.m[0][0] - normalizedMatrix.m[1][1] - normalizedMatrix.m[2][2]);
                    q.w = (normalizedMatrix.m[2][1] - normalizedMatrix.m[1][2]) / s;
                    q.x = 0.25 * s;
                    q.y = (normalizedMatrix.m[0][1] + normalizedMatrix.m[1][0]) / s;
                    q.z = (normalizedMatrix.m[0][2] + normalizedMatrix.m[2][0]) / s;
                } 
                else if (normalizedMatrix.m[1][1] > normalizedMatrix.m[2][2]) {
                    component_type s = 2.0 * std::sqrt(1.0 + normalizedMatrix.m[1][1] - normalizedMatrix.m[0][0] - normalizedMatrix.m[2][2]);
                    q.w = (normalizedMatrix.m[0][2] - normalizedMatrix.m[2][0]) / s;
                    q.x = (normalizedMatrix.m[0][1] + normalizedMatrix.m[1][0]) / s;
                    q.y = 0.25 * s;
                    q.z = (normalizedMatrix.m[1][2] + normalizedMatrix.m[2][1]) / s;
                } 
                else {
                    component_type s = 2.0 * std::sqrt(1.0 + normalizedMatrix.m[2][2] - normalizedMatrix.m[0][0] - normalizedMatrix.m[1][1]);
                    q.w = (normalizedMatrix.m[1][0] - normalizedMatrix.m[0][1]) / s;
                    q.x = (normalizedMatrix.m[0][2] + normalizedMatrix.m[2][0]) / s;
                    q.y = (normalizedMatrix.m[1][2] + normalizedMatrix.m[2][1]) / s;
                    q.z = 0.25 * s;
                }
            }

            q.z = -q.z; // opengl right-handed convention 

            return q;
        }

        //! set the rotation component directly
        void set_rotation(const quaternion_type &aRotation, const vector3_type &aScale = {1}) {
            using namespace std;

            const quaternion_type &q = aRotation;
            
            const auto sqw = q.w * q.w;
            const auto sqx = q.x * q.x;
            const auto sqy = q.y * q.y;
            const auto sqz = q.z * q.z;
            
            const auto invs = 1 / (sqx + sqy + sqz + sqw);
            
            m[0][0] = ( sqx - sqy - sqz + sqw) * invs ; 
            m[1][1] = (-sqx + sqy - sqz + sqw) * invs ;
            m[2][2] = (-sqx - sqy + sqz + sqw) * invs ;
            
            auto tmp1 = q.x * q.y;
            auto tmp2 = q.z * q.w;
            
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

            m[0][0] *= aScale.x;
            m[0][1] *= aScale.x;
            m[0][2] *= aScale.x;

            m[1][0] *= aScale.y;
            m[1][1] *= aScale.y;
            m[1][2] *= aScale.y;

            m[2][0] *= aScale.z;
            m[2][1] *= aScale.z;
            m[2][2] *= aScale.z;
        }

        //! transpose the matrix in place
        void transpose() {
            component_type temp[4][4];

            for (order_type i{0}; i < 4; ++i) 
                for (order_type j{0}; j < 4; ++j) 
                    temp[j][i] = m[i][j];

            set(
                temp[0][0], temp[0][1], temp[0][2], temp[0][3],
                temp[1][0], temp[1][1], temp[1][2], temp[1][3],
                temp[2][0], temp[2][1], temp[2][2], temp[2][3],
                temp[3][0], temp[3][1], temp[3][2], temp[3][3]
            );
        }

        //! get the transpose matrix 
        matrix4x4<component_type> transpose() const {
            matrix4x4<component_type> a = *this;
            a.transpose();
            return a;
        };

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

            b[0][0] = ( m[1][1] * c5 - m[1][2] * c4 + m[1][3] * c3) * invdet;
            b[0][1] = (-m[0][1] * c5 + m[0][2] * c4 - m[0][3] * c3) * invdet;
            b[0][2] = ( m[3][1] * s5 - m[3][2] * s4 + m[3][3] * s3) * invdet;
            b[0][3] = (-m[2][1] * s5 + m[2][2] * s4 - m[2][3] * s3) * invdet;

            b[1][0] = (-m[1][0] * c5 + m[1][2] * c2 - m[1][3] * c1) * invdet;
            b[1][1] = ( m[0][0] * c5 - m[0][2] * c2 + m[0][3] * c1) * invdet;
            b[1][2] = (-m[3][0] * s5 + m[3][2] * s2 - m[3][3] * s1) * invdet;
            b[1][3] = ( m[2][0] * s5 - m[2][2] * s2 + m[2][3] * s1) * invdet;

            b[2][0] = ( m[1][0] * c4 - m[1][1] * c2 + m[1][3] * c0) * invdet;
            b[2][1] = (-m[0][0] * c4 + m[0][1] * c2 - m[0][3] * c0) * invdet;
            b[2][2] = ( m[3][0] * s4 - m[3][1] * s2 + m[3][3] * s0) * invdet;
            b[2][3] = (-m[2][0] * s4 + m[2][1] * s2 - m[2][3] * s0) * invdet;

            b[3][0] = (-m[1][0] * c3 + m[1][1] * c1 - m[1][2] * c0) * invdet;
            b[3][1] = ( m[0][0] * c3 - m[0][1] * c1 + m[0][2] * c0) * invdet;
            b[3][2] = (-m[3][0] * s3 + m[3][1] * s1 - m[3][2] * s0) * invdet;
            b[3][3] = ( m[2][0] * s3 - m[2][1] * s1 + m[2][2] * s0) * invdet;

            set(
                b[0][0], b[0][1], b[0][2], b[0][3],
                b[1][0], b[1][1], b[1][2], b[1][3],
                b[2][0], b[2][1], b[2][2], b[2][3],
                b[3][0], b[3][1], b[3][2], b[3][3]
            );
        }

        //! set this matrix to its inverse via a faster means but requires that the matrix is affine
        /// affine means the matrix must only contain translations, rotations and scales.
        void inverse_affine() { 
            component_type rot[3][3] = {
                { m[0][0], m[0][1], m[0][2] },
                { m[1][0], m[1][1], m[1][2] },
                { m[2][0], m[2][1], m[2][2] }
            };

            component_type trans[3] = { m[3][0], m[3][1], m[3][2] };

            component_type rotInv[3][3] = {
                { rot[0][0], rot[1][0], rot[2][0] },
                { rot[0][1], rot[1][1], rot[2][1] },
                { rot[0][2], rot[1][2], rot[2][2] }
            };

            component_type transInv[3] = {
                -(rotInv[0][0] * trans[0] + rotInv[0][1] * trans[1] + rotInv[0][2] * trans[2]),
                -(rotInv[1][0] * trans[0] + rotInv[1][1] * trans[1] + rotInv[1][2] * trans[2]),
                -(rotInv[2][0] * trans[0] + rotInv[2][1] * trans[1] + rotInv[2][2] * trans[2])
            };

            set(
                rotInv[0][0], rotInv[0][1], rotInv[0][2], 0.0,
                rotInv[1][0], rotInv[1][1], rotInv[1][2], 0.0,
                rotInv[2][0], rotInv[2][1], rotInv[2][2], 0.0,
                transInv[0], transInv[1], transInv[2], 1.0
            );
        }

        //! assign values to all 16 elements of the matrix
        matrix4x4<component_type> &set(
            const component_type m00, const component_type m01, const component_type m02, const component_type m03, 
            const component_type m10, const component_type m11, const component_type m12, const component_type m13,
            const component_type m20, const component_type m21, const component_type m22, const component_type m23, 
            const component_type m30, const component_type m31, const component_type m32, const component_type m33) {
            m[0][0] = m00; 
            m[0][1] = m01; 
            m[0][2] = m02; 
            m[0][3] = m03;

            m[1][0] = m10; 
            m[1][1] = m11; 
            m[1][2] = m12; 
            m[1][3] = m13;

            m[2][0] = m20; 
            m[2][1] = m21; 
            m[2][2] = m22; 
            m[2][3] = m23;

            m[3][0] = m30; 
            m[3][1] = m31; 
            m[3][2] = m32;
            m[3][3] = m33;
            
            return *this;
        }

        //! multiply the matrix against another
        matrix4x4<component_type> &multiply(const matrix4x4 &right) {
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

        //! gets the scaling factor from the internal rotation matrix
        vector3_type scale() const {
            return {
                std::sqrt(m[0][0] * m[0][0] + m[0][1] * m[0][1] + m[0][2] * m[0][2]),
                std::sqrt(m[1][0] * m[1][0] + m[1][1] * m[1][1] + m[1][2] * m[1][2]),
                std::sqrt(m[2][0] * m[2][0] + m[2][1] * m[2][1] + m[2][2] * m[2][2])
            };
        }

        //! returns a new matrix which is the result of a multiplication, without mutating this
        matrix4x4 operator*(const matrix4x4 &other) const {
            matrix4x4 copy(*this);
            copy *= other;
            return copy;
        }

        //! alias for multiply
        matrix4x4& operator*=(const matrix4x4 &other) {
            multiply(other);
            return *this;
        }
        
        matrix4x4& operator=(const matrix4x4&) = default; 

        bool operator==(const matrix4x4<component_type> &other) const {
            for(order_type i = 0; i < order; ++i) for (order_type j{0}; j < order; ++j)
                if (m[i][j] != other.m[i][j])
                    return false;

            return true;
        }
            
        matrix4x4<component_type>(
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

        matrix4x4<component_type>(
            const vector3_type &aTranslationComponent, 
            const quaternion_type &aRotationComponent,
            const vector3_type &aScale = {1}) {
            set_to_identity();
            set_rotation(aRotationComponent, aScale);
            set_translation(aTranslationComponent);
        }

        matrix4x4<component_type>() = default;
        matrix4x4<component_type>(const matrix4x4<component_type>&) = default;
        matrix4x4<component_type>(matrix4x4<component_type>&&) = default;
        ~matrix4x4<component_type>() = default;

    };

    template<typename component_type> 
    const matrix4x4<component_type> matrix4x4<component_type>::identity = matrix4x4<component_type>();

    //! multiply a 3d vector by a matrix
    template<typename component_type> 
    vector3<component_type> &operator*=(vector3<component_type> &aVector, matrix4x4<component_type> aMatrix) {
        vector4<component_type> vec(aVector.x, aVector.y, aVector.z, 1.0);

        aMatrix.m[0][1] = -aMatrix.m[0][1];
        aMatrix.m[0][2] = -aMatrix.m[0][2];
        aMatrix.m[1][0] = -aMatrix.m[1][0];
        aMatrix.m[1][2] = -aMatrix.m[1][2];
        aMatrix.m[2][0] = -aMatrix.m[2][0];
        aMatrix.m[2][1] = -aMatrix.m[2][1];

        const auto newX = aMatrix.m[0][0] * vec.x + aMatrix.m[1][0] * vec.y + aMatrix.m[2][0] * vec.z + aMatrix.m[3][0]; 
        const auto newY = aMatrix.m[0][1] * vec.x + aMatrix.m[1][1] * vec.y + aMatrix.m[2][1] * vec.z + aMatrix.m[3][1];
        const auto newZ = aMatrix.m[0][2] * vec.x + aMatrix.m[1][2] * vec.y + aMatrix.m[2][2] * vec.z + aMatrix.m[3][2];
        const auto newW = aMatrix.m[0][3] * vec.x + aMatrix.m[1][3] * vec.y + aMatrix.m[2][3] * vec.z + aMatrix.m[3][3];

        if (std::abs(newW - 1.0) > 1e-6) {
            aVector.x = newX / newW;
            aVector.y = newY / newW;
            aVector.z = newZ / newW;
        } else {
            aVector.x = newX;
            aVector.y = newY;
            aVector.z = newZ;
        }

        return aVector;
    }
}

#endif

