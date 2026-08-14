// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_IMPL_STD_MATRIX4X4_INL
#define GDK_MATH_IMPL_STD_MATRIX4X4_INL

namespace gdk {
    template<typename component_type>
    constexpr std::size_t matrix4x4<component_type>::index(order_type aX, order_type aY) const {
        return aX * order + aY;
    }

    template<typename component_type>
    constexpr component_type matrix4x4<component_type>::basis_determinant() const {
        return get(0, 0) * (get(1, 1) * get(2, 2) - get(2, 1) * get(1, 2))
             - get(1, 0) * (get(0, 1) * get(2, 2) - get(2, 1) * get(0, 2))
             + get(2, 0) * (get(0, 1) * get(1, 2) - get(1, 1) * get(0, 2));
    }

    template<typename component_type>
    constexpr void matrix4x4<component_type>::set(const order_type aX, const order_type aY,
        const component_type aValue) {
        m[index(aX, aY)] = aValue;
    }

    template<typename component_type>
    constexpr component_type &matrix4x4<component_type>::get(const order_type aX, const order_type aY) {
        return m[index(aX, aY)];
    }

    template<typename component_type>
    constexpr const component_type &matrix4x4<component_type>::get(const order_type aX,
        const order_type aY) const {
        return m[index(aX, aY)];
    }

    template<typename component_type>
    constexpr const component_type &matrix4x4<component_type>::front() const {
        return m.front();
    }

    template<typename component_type>
    constexpr void matrix4x4<component_type>::set_to_identity() {
        *this = matrix4x4<component_type>();
    }

    template<typename component_type>
    constexpr void matrix4x4<component_type>::set_translation(const vector3_type &aTranslation) {
        set(3, 0, aTranslation.x);
        set(3, 1, aTranslation.y);
        set(3, 2, aTranslation.z);
    }

    template<typename component_type>
    constexpr typename matrix4x4<component_type>::vector3_type
    matrix4x4<component_type>::translation() const {
        return vector3_type(get(3, 0), get(3, 1), get(3, 2));
    }

    template<typename component_type>
    typename matrix4x4<component_type>::quaternion_type
    matrix4x4<component_type>::rotation() const {
        if (basis_determinant() < 0) throw std::domain_error(
            "matrix4x4::rotation() on a mirrored transform: no rotation describes a negative "
            "determinant, and scale() cannot carry the sign to put it back");

        vector3_type scale = this->scale();
        std::array<component_type *, 3> scaleAsArray { &scale.x, &scale.y, &scale.z };

        matrix4x4<component_type> normalizedMatrix = *this;
        for (order_type i{0}; i < 3; ++i) {
            for (order_type j{0}; j < 3; ++j) {
                normalizedMatrix.get(i, j) /= *scaleAsArray[i];
            }
        }

        const auto r = [&normalizedMatrix](const order_type aRow, const order_type aColumn) {
            return normalizedMatrix.get(aColumn, aRow);
        };

        component_type trace = r(0, 0) + r(1, 1) + r(2, 2);
        quaternion_type q;

        if (trace > 0) {
            component_type s = 0.5 / std::sqrt(trace + 1.0);
            q.w = 0.25 / s;
            q.x = (r(2, 1) - r(1, 2)) * s;
            q.y = (r(0, 2) - r(2, 0)) * s;
            q.z = (r(1, 0) - r(0, 1)) * s;
        }
        else {
            if (r(0, 0) > r(1, 1) && r(0, 0) > r(2, 2)) {
                component_type s = 2.0 * std::sqrt(1.0 + r(0, 0) - r(1, 1) - r(2, 2));
                q.w = (r(2, 1) - r(1, 2)) / s;
                q.x = 0.25 * s;
                q.y = (r(0, 1) + r(1, 0)) / s;
                q.z = (r(0, 2) + r(2, 0)) / s;
            }
            else if (r(1, 1) > r(2, 2)) {
                component_type s = 2.0 * std::sqrt(1.0 + r(1, 1) - r(0, 0) - r(2, 2));
                q.w = (r(0, 2) - r(2, 0)) / s;
                q.x = (r(0, 1) + r(1, 0)) / s;
                q.y = 0.25 * s;
                q.z = (r(1, 2) + r(2, 1)) / s;
            }
            else {
                component_type s = 2.0 * std::sqrt(1.0 + r(2, 2) - r(0, 0) - r(1, 1));
                q.w = (r(1, 0) - r(0, 1)) / s;
                q.x = (r(0, 2) + r(2, 0)) / s;
                q.y = (r(1, 2) + r(2, 1)) / s;
                q.z = 0.25 * s;
            }
        }

        return q;
    }

    template<typename component_type>
    constexpr void matrix4x4<component_type>::set_rotation_and_scale(const quaternion_type &aRotation,
        const vector3_type &aScale) {
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

        set(1, 0, 2.0 * static_cast<component_type>(tmp1 - tmp2) * invs);
        set(0, 1, 2.0 * static_cast<component_type>(tmp1 + tmp2) * invs);

        tmp1 = q.x * q.z;
        tmp2 = q.y * q.w;

        set(2, 0, 2.0 * static_cast<component_type>(tmp1 + tmp2) * invs);
        set(0, 2, 2.0 * static_cast<component_type>(tmp1 - tmp2) * invs);

        tmp1 = q.y * q.z;
        tmp2 = q.x * q.w;

        set(2, 1, 2.0 * static_cast<component_type>(tmp1 - tmp2) * invs);
        set(1, 2, 2.0 * static_cast<component_type>(tmp1 + tmp2) * invs);

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

    template<typename component_type>
    void matrix4x4<component_type>::set_rotation(const quaternion_type &aRotation) {
        if (basis_determinant() < 0) throw std::domain_error(
            "matrix4x4::set_rotation() on a mirrored transform: scale() cannot carry the sign, so "
            "the mirror would be silently lost");

        return set_rotation_and_scale(aRotation, scale());
    }

    template<typename component_type>
    void matrix4x4<component_type>::set_scale(const vector3_type &aScale) {
        return set_rotation_and_scale(rotation(), aScale);
    }

    template<typename component_type>
    constexpr void matrix4x4<component_type>::transpose() {
        for (order_type i{0}; i < order; ++i)
            for (order_type j{static_cast<order_type>(i + 1)}; j < order; ++j)
                {
                    const auto held = get(i, j);
                    get(i, j) = get(j, i);
                    get(j, i) = held;
                }
    }

    template<typename component_type>
    constexpr matrix4x4<component_type> matrix4x4<component_type>::transposed() const {
        matrix4x4<component_type> a = *this;

        a.transpose();

        return a;
    }

    template<typename component_type>
    constexpr component_type matrix4x4<component_type>::determinant() const {
        const component_type s0 = get(0, 0) * get(1, 1) - get(1, 0) * get(0, 1);
        const component_type s1 = get(0, 0) * get(1, 2) - get(1, 0) * get(0, 2);
        const component_type s2 = get(0, 0) * get(1, 3) - get(1, 0) * get(0, 3);
        const component_type s3 = get(0, 1) * get(1, 2) - get(1, 1) * get(0, 2);
        const component_type s4 = get(0, 1) * get(1, 3) - get(1, 1) * get(0, 3);
        const component_type s5 = get(0, 2) * get(1, 3) - get(1, 2) * get(0, 3);

        const component_type c5 = get(2, 2) * get(3, 3) - get(3, 2) * get(2, 3);
        const component_type c4 = get(2, 1) * get(3, 3) - get(3, 1) * get(2, 3);
        const component_type c3 = get(2, 1) * get(3, 2) - get(3, 1) * get(2, 2);
        const component_type c2 = get(2, 0) * get(3, 3) - get(3, 0) * get(2, 3);
        const component_type c1 = get(2, 0) * get(3, 2) - get(3, 0) * get(2, 2);
        const component_type c0 = get(2, 0) * get(3, 1) - get(3, 0) * get(2, 1);

        return s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;
    }

    template<typename component_type>
    constexpr matrix4x4<component_type> matrix4x4<component_type>::inversed() const {
        matrix4x4<component_type> a = *this;

        a.inverse();

        return a;
    }

    template<typename component_type>
    constexpr void matrix4x4<component_type>::inverse() {
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

        const component_type det = determinant();

        if (det == 0) throw std::domain_error("cannot invert a singular matrix4x4");

        const component_type invdet = component_type(1) / det;

        component_type b[order][order] = {};

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

    template<typename component_type>
    constexpr void matrix4x4<component_type>::inverse_affine() { 
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

    template<typename component_type>
    constexpr matrix4x4<component_type> &matrix4x4<component_type>::set(
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

    template<typename component_type>
    constexpr matrix4x4<component_type> &matrix4x4<component_type>::multiply(const matrix4x4 &right) {
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

    template<typename component_type>
    typename matrix4x4<component_type>::vector3_type matrix4x4<component_type>::scale() const {
        return {
            std::sqrt(get(0, 0) * get(0, 0) + get(0, 1) * get(0, 1) + get(0, 2) * get(0, 2)),
            std::sqrt(get(1, 0) * get(1, 0) + get(1, 1) * get(1, 1) + get(1, 2) * get(1, 2)),
            std::sqrt(get(2, 0) * get(2, 0) + get(2, 1) * get(2, 1) + get(2, 2) * get(2, 2))
        };
    }

    template<typename component_type>
    constexpr matrix4x4<component_type> matrix4x4<component_type>::operator*(const matrix4x4 &other) const {
        matrix4x4 copy(*this);
        copy *= other;

        return copy;
    }

    template<typename component_type>
    constexpr matrix4x4<component_type> &matrix4x4<component_type>::operator*=(const matrix4x4 &other) {
        multiply(other);

        return *this;
    }

    template<typename component_type>
    constexpr bool matrix4x4<component_type>::operator!=(const matrix4x4<component_type> &other) const {
        return !(*this == other);
    }

    template<typename component_type>
    constexpr bool matrix4x4<component_type>::operator==(const matrix4x4<component_type> &other) const {
        for(order_type i = 0; i < order; ++i) for (order_type j{0}; j < order; ++j)
            if (get(i, j) != other.get(i, j))
                return false;

        return true;
    }

    template<typename component_type>
    constexpr matrix4x4<component_type>::matrix4x4(
        const component_type a00, const component_type a01, const component_type a02, const component_type a03, 
        const component_type a10, const component_type a11, const component_type a12, const component_type a13,
        const component_type a20, const component_type a21, const component_type a22, const component_type a23, 
        const component_type a30, const component_type a31, const component_type a32, const component_type a33) {
        set(
            a00, a01, a02, a03, 
            a10, a11, a12, a13, 
            a20, a21, a22, a23, 
            a30, a31, a32, a33);
    }

    template<typename component_type>
    constexpr matrix4x4<component_type>::matrix4x4(
        const vector3_type &aTranslationComponent, 
        const quaternion_type &aRotationComponent,
        const vector3_type &aScale) {
        set_to_identity();
        set_rotation_and_scale(aRotationComponent, aScale);
        set_translation(aTranslationComponent);
    }

    template<typename component_type> 
    const matrix4x4<component_type> matrix4x4<component_type>::identity = matrix4x4<component_type>();
}

#endif
