// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_MAT4X4_H
#define GDK_MATH_MAT4X4_H

#include <gdk/quaternion.h>
#include <gdk/vector2.h>
#include <gdk/vector3.h>
#include <gdk/vector4.h>

#include <array>
#include <cmath>
#include <iosfwd>
#include <type_traits>

namespace gdk {
    /// \brief 4 by 4 matrix used to calculate 3D transformations and camera projections.
    /// - the matrix assumes opengl style "right handedness": +X is right, +Y is up, -Z is forward. 
    /// - the matrix is row-major
    template<typename component_type_param>
    class matrix4x4 final {
    public:
        static_assert(std::is_floating_point<component_type_param>::value, 
            "component_type must be a floating point type");

        using component_type = component_type_param;
        using order_type = std::size_t;
        using quaternion_type = quaternion<component_type_param>;
        using vector2_type = vector2<component_type_param>;
        using vector3_type = vector3<component_type_param>;

        static constexpr order_type order{4};

        static const matrix4x4<component_type> identity; 

    private: 
        std::array<component_type, order * order> m = {
            1.,0.,0.,0.,
            0.,1.,0.,0.,
            0.,0.,1.,0.,
            0.,0.,0.,1.,
        };

        constexpr std::size_t index(order_type aX, order_type aY) const {
            return aX * order + aY;
        }

    public:
        void set(const order_type aX, const order_type aY, const component_type aValue) {
            m[index(aX, aY)] = aValue;
        }

        component_type &get(const order_type aX, const order_type aY) {
            return m[index(aX, aY)];
        }

        const component_type &get(const order_type aX, const order_type aY) const {
            return m[index(aX, aY)];
        }

        inline const component_type &front() const {
            return m.front();
        }

        //! Sets matrix to an identity matrix
        void set_to_identity() {
            *this = matrix4x4<component_type>::identity;
        }

        //! set the translation components directly
        void set_translation(const vector3_type &aTranslation) {
            // Only modify the translation components in column 3
            set(3, 0, aTranslation.x);
            set(3, 1, aTranslation.y);
            set(3, 2, aTranslation.z);
        }

        //! get the translation vector from this matrix
        vector3_type translation() const {
            return vector3_type(get(3, 0), get(3, 1), get(3, 2));
        }

        //! get the rotation as a quaternion
        quaternion_type rotation() const {
            vector3_type scale = this->scale();
            std::array<component_type *, 3> scaleAsArray { &scale.x, &scale.y, &scale.z };

            matrix4x4<component_type> normalizedMatrix = *this;
            for (order_type i{0}; i < 3; ++i) {
                for (order_type j{0}; j < 3; ++j) {
                    normalizedMatrix.get(i, j) /= *scaleAsArray[i];
                }
            }

            component_type trace = normalizedMatrix.get(0, 0) + normalizedMatrix.get(1, 1) + normalizedMatrix.get(2, 2);
            quaternion_type q;

            if (trace > 0) {
                component_type s = 0.5 / std::sqrt(trace + 1.0);
                q.w = 0.25 / s;
                q.x = (normalizedMatrix.get(2, 1) - normalizedMatrix.get(1, 2)) * s;
                q.y = (normalizedMatrix.get(0, 2) - normalizedMatrix.get(2, 0)) * s;
                q.z = (normalizedMatrix.get(1, 0) - normalizedMatrix.get(0, 1)) * s;
            } 
            else {
                if (normalizedMatrix.get(0, 0) > normalizedMatrix.get(1, 1) && normalizedMatrix.get(0, 0) > normalizedMatrix.get(2, 2)) {
                    component_type s = 2.0 * std::sqrt(1.0 + normalizedMatrix.get(0, 0) - normalizedMatrix.get(1, 1) - normalizedMatrix.get(2, 2));
                    q.w = (normalizedMatrix.get(2, 1) - normalizedMatrix.get(1, 2)) / s;
                    q.x = 0.25 * s;
                    q.y = (normalizedMatrix.get(0, 1) + normalizedMatrix.get(1, 0)) / s;
                    q.z = (normalizedMatrix.get(0, 2) + normalizedMatrix.get(2, 0)) / s;
                } 
                else if (normalizedMatrix.get(1, 1) > normalizedMatrix.get(2, 2)) {
                    component_type s = 2.0 * std::sqrt(1.0 + normalizedMatrix.get(1, 1) - normalizedMatrix.get(0, 0) - normalizedMatrix.get(2, 2));
                    q.w = (normalizedMatrix.get(0, 2) - normalizedMatrix.get(2, 0)) / s;
                    q.x = (normalizedMatrix.get(0, 1) + normalizedMatrix.get(1, 0)) / s;
                    q.y = 0.25 * s;
                    q.z = (normalizedMatrix.get(1, 2) + normalizedMatrix.get(2, 1)) / s;
                } 
                else {
                    component_type s = 2.0 * std::sqrt(1.0 + normalizedMatrix.get(2, 2) - normalizedMatrix.get(0, 0) - normalizedMatrix.get(1, 1));
                    q.w = (normalizedMatrix.get(1, 0) - normalizedMatrix.get(0, 1)) / s;
                    q.x = (normalizedMatrix.get(0, 2) + normalizedMatrix.get(2, 0)) / s;
                    q.y = (normalizedMatrix.get(1, 2) + normalizedMatrix.get(2, 1)) / s;
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
            
            set(0, 0, ( sqx - sqy - sqz + sqw) * invs); 
            set(1, 1, (-sqx + sqy - sqz + sqw) * invs);
            set(2, 2, (-sqx - sqy + sqz + sqw) * invs);
            
            auto tmp1 = q.x * q.y;
            auto tmp2 = q.z * q.w;
            
            set(1, 0, 2.0 * static_cast<component_type>(tmp1 + tmp2) * invs);
            set(0, 1, 2.0 * static_cast<component_type>(tmp1 - tmp2) * invs);
            
            tmp1 = q.x * q.z;
            tmp2 = q.y * q.w;
            
            set(2, 0, 2.0 * static_cast<component_type>(tmp1 - tmp2) * invs);
            set(0, 2, 2.0 * static_cast<component_type>(tmp1 + tmp2) * invs);
            
            tmp1 = q.y * q.z;
            tmp2 = q.x * q.w;
            
            set(2, 1, 2.0 * static_cast<component_type>(tmp1 + tmp2) * invs);
            set(1, 2, 2.0 * static_cast<component_type>(tmp1 - tmp2) * invs);

            get(0, 0) *= aScale.x;
            get(0, 1) *= aScale.x;
            get(0, 2) *= aScale.x;

            get(1, 0) *= aScale.y;
            get(1, 1) *= aScale.y;
            get(1, 2) *= aScale.y;

            get(2, 0) *= aScale.z;
            get(2, 1) *= aScale.z;
            get(2, 2) *= aScale.z;
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
            component_type s0 = get(0, 0) * get(1, 1) - get(1, 0) * get(0, 1);
            component_type s1 = get(0, 0) * get(1, 2) - get(1, 0) * get(0, 2);
            component_type s2 = get(0, 0) * get(1, 3) - get(1, 0) * get(0, 3);
            component_type s3 = get(0, 1) * get(1, 2) - get(1, 1) * get(0, 2);
            component_type s4 = get(0, 1) * get(1, 3) - get(1, 1) * get(0, 3);
            component_type s5 = get(0, 2) * get(1, 3) - get(1, 2) * get(0, 3);

            component_type c5 = get(2, 2) * get(3, 3) - get(3, 2) * get(2, 3);
            component_type c4 = get(2, 1) * get(3, 3) - get(3, 1) * get(2, 3);
            component_type c3 = get(2, 1) * get(3, 2) - get(3, 1) * get(2, 2);
            component_type c2 = get(2, 0) * get(3, 3) - get(3, 0) * get(2, 3);
            component_type c1 = get(2, 0) * get(3, 2) - get(3, 0) * get(2, 2);
            component_type c0 = get(2, 0) * get(3, 1) - get(3, 0) * get(2, 1);

            component_type invdet = component_type(1) / (s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0);

            component_type b[order][order];

            b[0][0] = ( get(1, 1) * c5 - get(1, 2) * c4 + get(1, 3) * c3) * invdet;
            b[0][1] = (-get(0, 1) * c5 + get(0, 2) * c4 - get(0, 3) * c3) * invdet;
            b[0][2] = ( get(3, 1) * s5 - get(3, 2) * s4 + get(3, 3) * s3) * invdet;
            b[0][3] = (-get(2, 1) * s5 + get(2, 2) * s4 - get(2, 3) * s3) * invdet;

            b[1][0] = (-get(1, 0) * c5 + get(1, 2) * c2 - get(1, 3) * c1) * invdet;
            b[1][1] = ( get(0, 0) * c5 - get(0, 2) * c2 + get(0, 3) * c1) * invdet;
            b[1][2] = (-get(3, 0) * s5 + get(3, 2) * s2 - get(3, 3) * s1) * invdet;
            b[1][3] = ( get(2, 0) * s5 - get(2, 2) * s2 + get(2, 3) * s1) * invdet;

            b[2][0] = ( get(1, 0) * c4 - get(1, 1) * c2 + get(1, 3) * c0) * invdet;
            b[2][1] = (-get(0, 0) * c4 + get(0, 1) * c2 - get(0, 3) * c0) * invdet;
            b[2][2] = ( get(3, 0) * s4 - get(3, 1) * s2 + get(3, 3) * s0) * invdet;
            b[2][3] = (-get(2, 0) * s4 + get(2, 1) * s2 - get(2, 3) * s0) * invdet;

            b[3][0] = (-get(1, 0) * c3 + get(1, 1) * c1 - get(1, 2) * c0) * invdet;
            b[3][1] = ( get(0, 0) * c3 - get(0, 1) * c1 + get(0, 2) * c0) * invdet;
            b[3][2] = (-get(3, 0) * s3 + get(3, 1) * s1 - get(3, 2) * s0) * invdet;
            b[3][3] = ( get(2, 0) * s3 - get(2, 1) * s1 + get(2, 2) * s0) * invdet;

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
                { get(0, 0), get(0, 1), get(0, 2) },
                { get(1, 0), get(1, 1), get(1, 2) },
                { get(2, 0), get(2, 1), get(2, 2) }
            };

            component_type trans[3] = { get(3, 0), get(3, 1), get(3, 2) };

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
                 transInv[0],  transInv[1],  transInv[2], 1.0
            );
        }

        //! assign values to all 16 elements of the matrix
        matrix4x4<component_type> &set(
            const component_type m00, const component_type m01, const component_type m02, const component_type m03, 
            const component_type m10, const component_type m11, const component_type m12, const component_type m13,
            const component_type m20, const component_type m21, const component_type m22, const component_type m23, 
            const component_type m30, const component_type m31, const component_type m32, const component_type m33) {
            set(0, 0, m00); 
            set(0, 1, m01); 
            set(0, 2, m02); 
            set(0, 3, m03);

            set(1, 0, m10); 
            set(1, 1, m11); 
            set(1, 2, m12); 
            set(1, 3, m13);

            set(2, 0, m20); 
            set(2, 1, m21); 
            set(2, 2, m22); 
            set(2, 3, m23);

            set(3, 0, m30); 
            set(3, 1, m31); 
            set(3, 2, m32);
            set(3, 3, m33);
            
            return *this;
        }

        //! multiply the matrix against another
        matrix4x4<component_type> &multiply(const matrix4x4 &right) {
            set(
                get(0, 0) * right.get(0, 0) + get(1, 0) * right.get(0, 1) + get(2, 0) * right.get(0, 2) + get(3, 0) * right.get(0, 3),
                get(0, 1) * right.get(0, 0) + get(1, 1) * right.get(0, 1) + get(2, 1) * right.get(0, 2) + get(3, 1) * right.get(0, 3),
                get(0, 2) * right.get(0, 0) + get(1, 2) * right.get(0, 1) + get(2, 2) * right.get(0, 2) + get(3, 2) * right.get(0, 3),
                get(0, 3) * right.get(0, 0) + get(1, 3) * right.get(0, 1) + get(2, 3) * right.get(0, 2) + get(3, 3) * right.get(0, 3),
                get(0, 0) * right.get(1, 0) + get(1, 0) * right.get(1, 1) + get(2, 0) * right.get(1, 2) + get(3, 0) * right.get(1, 3),
                get(0, 1) * right.get(1, 0) + get(1, 1) * right.get(1, 1) + get(2, 1) * right.get(1, 2) + get(3, 1) * right.get(1, 3),
                get(0, 2) * right.get(1, 0) + get(1, 2) * right.get(1, 1) + get(2, 2) * right.get(1, 2) + get(3, 2) * right.get(1, 3),
                get(0, 3) * right.get(1, 0) + get(1, 3) * right.get(1, 1) + get(2, 3) * right.get(1, 2) + get(3, 3) * right.get(1, 3),
                get(0, 0) * right.get(2, 0) + get(1, 0) * right.get(2, 1) + get(2, 0) * right.get(2, 2) + get(3, 0) * right.get(2, 3),
                get(0, 1) * right.get(2, 0) + get(1, 1) * right.get(2, 1) + get(2, 1) * right.get(2, 2) + get(3, 1) * right.get(2, 3),
                get(0, 2) * right.get(2, 0) + get(1, 2) * right.get(2, 1) + get(2, 2) * right.get(2, 2) + get(3, 2) * right.get(2, 3),
                get(0, 3) * right.get(2, 0) + get(1, 3) * right.get(2, 1) + get(2, 3) * right.get(2, 2) + get(3, 3) * right.get(2, 3),
                get(0, 0) * right.get(3, 0) + get(1, 0) * right.get(3, 1) + get(2, 0) * right.get(3, 2) + get(3, 0) * right.get(3, 3),
                get(0, 1) * right.get(3, 0) + get(1, 1) * right.get(3, 1) + get(2, 1) * right.get(3, 2) + get(3, 1) * right.get(3, 3),
                get(0, 2) * right.get(3, 0) + get(1, 2) * right.get(3, 1) + get(2, 2) * right.get(3, 2) + get(3, 2) * right.get(3, 3),
                get(0, 3) * right.get(3, 0) + get(1, 3) * right.get(3, 1) + get(2, 3) * right.get(3, 2) + get(3, 3) * right.get(3, 3));
            
            return *this;
        }

        //! gets the scaling factor from the internal rotation matrix
        vector3_type scale() const {
            return {
                std::sqrt(get(0, 0) * get(0, 0) + get(0, 1) * get(0, 1) + get(0, 2) * get(0, 2)),
                std::sqrt(get(1, 0) * get(1, 0) + get(1, 1) * get(1, 1) + get(1, 2) * get(1, 2)),
                std::sqrt(get(2, 0) * get(2, 0) + get(2, 1) * get(2, 1) + get(2, 2) * get(2, 2))
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
                if (get(i, j) != other.get(i, j))
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

        //! makes an orthographic projection matrix
        static matrix4x4<component_type> make_orthographic_projection_matrix(
            const vector2<component_type> &aOrthoSize, 
            const component_type aNearClippingPlane, 
            const component_type aFarClippingPlane, 
            const component_type aViewportAspectRatio) {
            const auto x = 2.0f / (aOrthoSize.x * aViewportAspectRatio);
            const auto y = 2.0f / aOrthoSize.y;
            const auto n = - (aFarClippingPlane + aNearClippingPlane) / (aFarClippingPlane - aNearClippingPlane);
            const auto f = -2.0f / (aFarClippingPlane - aNearClippingPlane);

            matrix4x4<component_type> matrix;
            matrix.set(0, 0, x ); matrix.set(1, 0, 0.); matrix.set(2, 0, 0.); matrix.set(3, 0, 0.);
            matrix.set(0, 1, 0.); matrix.set(1, 1, y ); matrix.set(2, 1, 0.); matrix.set(3, 1, 0.);
            matrix.set(0, 2, 0.); matrix.set(1, 2, 0.); matrix.set(2, 2, f ); matrix.set(3, 2, n );
            matrix.set(0, 3, 0.); matrix.set(1, 3, 0.); matrix.set(2, 3, 0.); matrix.set(3, 3, 1.);
            return matrix;
        }

        //! makes a perspective projection matrix
        static matrix4x4<component_type> make_perspective_projection_matrix(
            const component_type aFieldOfView, 
            const component_type aNearClippingPlane, 
            const component_type aFarClippingPlane, 
            const component_type aViewportAspectRatio) {
            const auto tanHalfFovy = static_cast<component_type>(tan(aFieldOfView * 0.5));

            matrix4x4<component_type> matrix;

            matrix.set(0, 0, 1.0 / (aViewportAspectRatio * tanHalfFovy));
            matrix.set(0, 1, 0.0);
            matrix.set(0, 2, 0.0);
            matrix.set(0, 3, 0.0);

            matrix.set(1, 0, 0.0);
            matrix.set(1, 1, 1.0 / tanHalfFovy);
            matrix.set(1, 2, 0.0);
            matrix.set(1, 3, 0.0);

            matrix.set(2, 0, 0.0);
            matrix.set(2, 1, 0.0);
            matrix.set(2, 2,-(aFarClippingPlane + aNearClippingPlane) / (aFarClippingPlane - aNearClippingPlane));
            matrix.set(2, 3,-1.0);

            matrix.set(3, 0, 0.0);
            matrix.set(3, 1, 0.0);
            matrix.set(3, 2,-2.0 * aFarClippingPlane * aNearClippingPlane / (aFarClippingPlane - aNearClippingPlane));
            matrix.set(3, 3, 0.0);

            return matrix;
        }
    };

    template<typename component_type> 
    const matrix4x4<component_type> matrix4x4<component_type>::identity = matrix4x4<component_type>();

    //! multiply a 3d vector by a matrix
    template<typename component_type> 
    vector3<component_type> &operator*=(vector3<component_type> &aVector, matrix4x4<component_type> aMatrix) {
        vector4<component_type> vec(aVector.x, aVector.y, aVector.z, 1.0);

        aMatrix.set(0, 1, -aMatrix.get(0, 1));
        aMatrix.set(0, 2, -aMatrix.get(0, 2));
        aMatrix.set(1, 0, -aMatrix.get(1, 0));
        aMatrix.set(1, 2, -aMatrix.get(1, 2));
        aMatrix.set(2, 0, -aMatrix.get(2, 0));
        aMatrix.set(2, 1, -aMatrix.get(2, 1));

        const auto newX = aMatrix.get(0, 0) * vec.x + aMatrix.get(1, 0) * vec.y + aMatrix.get(2, 0) * vec.z + aMatrix.get(3, 0); 
        const auto newY = aMatrix.get(0, 1) * vec.x + aMatrix.get(1, 1) * vec.y + aMatrix.get(2, 1) * vec.z + aMatrix.get(3, 1);
        const auto newZ = aMatrix.get(0, 2) * vec.x + aMatrix.get(1, 2) * vec.y + aMatrix.get(2, 2) * vec.z + aMatrix.get(3, 2);
        const auto newW = aMatrix.get(0, 3) * vec.x + aMatrix.get(1, 3) * vec.y + aMatrix.get(2, 3) * vec.z + aMatrix.get(3, 3);

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

