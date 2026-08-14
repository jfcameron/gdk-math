// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_IMPL_STD_MATRIX3X3_INL
#define GDK_MATH_IMPL_STD_MATRIX3X3_INL

namespace gdk {
    template<typename component_type>
    constexpr std::size_t matrix3x3<component_type>::index(order_type aX, order_type aY) const {
        return aX * order + aY;
    }

    template<typename component_type>
    constexpr void matrix3x3<component_type>::set(const order_type aX, const order_type aY,
        const component_type aValue) {
        m[index(aX, aY)] = aValue;
    }

    template<typename component_type>
    constexpr component_type &matrix3x3<component_type>::get(const order_type aX, const order_type aY) {
        return m[index(aX, aY)];
    }

    template<typename component_type>
    constexpr const component_type &matrix3x3<component_type>::get(const order_type aX,
        const order_type aY) const {
        return m[index(aX, aY)];
    }

    template<typename component_type>
    constexpr const component_type &matrix3x3<component_type>::front() const {
        return m.front();
    }

    template<typename component_type>
    constexpr void matrix3x3<component_type>::set_to_identity() {
        *this = matrix3x3<component_type>();
    }

    template<typename component_type>
    constexpr matrix3x3<component_type> &matrix3x3<component_type>::set(
        const component_type m00, const component_type m01, const component_type m02,
        const component_type m10, const component_type m11, const component_type m12,
        const component_type m20, const component_type m21, const component_type m22) {
        set(0, 0, m00); set(0, 1, m01); set(0, 2, m02);
        set(1, 0, m10); set(1, 1, m11); set(1, 2, m12);
        set(2, 0, m20); set(2, 1, m21); set(2, 2, m22);

        return *this;
    }

    template<typename component_type>
    constexpr matrix3x3<component_type> &matrix3x3<component_type>::multiply(const matrix3x3 &right) {
        return set(
            get(0, 0) * right.get(0, 0) + get(1, 0) * right.get(0, 1) + get(2, 0) * right.get(0, 2),
            get(0, 1) * right.get(0, 0) + get(1, 1) * right.get(0, 1) + get(2, 1) * right.get(0, 2),
            get(0, 2) * right.get(0, 0) + get(1, 2) * right.get(0, 1) + get(2, 2) * right.get(0, 2),

            get(0, 0) * right.get(1, 0) + get(1, 0) * right.get(1, 1) + get(2, 0) * right.get(1, 2),
            get(0, 1) * right.get(1, 0) + get(1, 1) * right.get(1, 1) + get(2, 1) * right.get(1, 2),
            get(0, 2) * right.get(1, 0) + get(1, 2) * right.get(1, 1) + get(2, 2) * right.get(1, 2),

            get(0, 0) * right.get(2, 0) + get(1, 0) * right.get(2, 1) + get(2, 0) * right.get(2, 2),
            get(0, 1) * right.get(2, 0) + get(1, 1) * right.get(2, 1) + get(2, 1) * right.get(2, 2),
            get(0, 2) * right.get(2, 0) + get(1, 2) * right.get(2, 1) + get(2, 2) * right.get(2, 2));
    }

    template<typename component_type>
    constexpr void matrix3x3<component_type>::transpose() {
        for (order_type i{0}; i < order; ++i)
            for (order_type j{static_cast<order_type>(i + 1)}; j < order; ++j)
                {
                    const auto held = get(i, j);
                    get(i, j) = get(j, i);
                    get(j, i) = held;
                }
    }

    template<typename component_type>
    constexpr matrix3x3<component_type> matrix3x3<component_type>::transposed() const {
        matrix3x3<component_type> a = *this;

        a.transpose();

        return a;
    }

    template<typename component_type>
    constexpr component_type matrix3x3<component_type>::determinant() const {
        const auto r = [this](const order_type aRow, const order_type aColumn) {
            return get(aColumn, aRow);
        };

        return r(0, 0) * (r(1, 1) * r(2, 2) - r(1, 2) * r(2, 1))
             - r(0, 1) * (r(1, 0) * r(2, 2) - r(1, 2) * r(2, 0))
             + r(0, 2) * (r(1, 0) * r(2, 1) - r(1, 1) * r(2, 0));
    }

    template<typename component_type>
    constexpr void matrix3x3<component_type>::inverse() {
        const auto det = determinant();

        if (det == 0) throw std::domain_error("cannot invert a singular matrix3x3");

        const auto invdet = component_type(1) / det;

        const auto r = [this](const order_type aRow, const order_type aColumn) {
            return get(aColumn, aRow);
        };

        const component_type b[3][3] = {
            {(r(1, 1) * r(2, 2) - r(1, 2) * r(2, 1)) * invdet,
             (r(0, 2) * r(2, 1) - r(0, 1) * r(2, 2)) * invdet,
             (r(0, 1) * r(1, 2) - r(0, 2) * r(1, 1)) * invdet},
            {(r(1, 2) * r(2, 0) - r(1, 0) * r(2, 2)) * invdet,
             (r(0, 0) * r(2, 2) - r(0, 2) * r(2, 0)) * invdet,
             (r(0, 2) * r(1, 0) - r(0, 0) * r(1, 2)) * invdet},
            {(r(1, 0) * r(2, 1) - r(1, 1) * r(2, 0)) * invdet,
             (r(0, 1) * r(2, 0) - r(0, 0) * r(2, 1)) * invdet,
             (r(0, 0) * r(1, 1) - r(0, 1) * r(1, 0)) * invdet}};

        for (order_type row = 0; row < order; ++row)
            for (order_type column = 0; column < order; ++column)
                set(column, row, b[row][column]);
    }

    template<typename component_type>
    constexpr matrix3x3<component_type> matrix3x3<component_type>::inversed() const {
        matrix3x3<component_type> a = *this;

        a.inverse();

        return a;
    }

    template<typename component_type>
    constexpr matrix3x3<component_type> matrix3x3<component_type>::operator*(const matrix3x3 &other) const {
        matrix3x3 copy(*this);
        copy *= other;

        return copy;
    }

    template<typename component_type>
    constexpr matrix3x3<component_type> &matrix3x3<component_type>::operator*=(const matrix3x3 &other) {
        multiply(other);

        return *this;
    }

    template<typename component_type>
    constexpr bool matrix3x3<component_type>::operator==(const matrix3x3<component_type> &other) const {
        for (order_type i = 0; i < order; ++i) for (order_type j{0}; j < order; ++j)
            if (get(i, j) != other.get(i, j))
                return false;

        return true;
    }

    template<typename component_type>
    constexpr bool matrix3x3<component_type>::operator!=(const matrix3x3<component_type> &other) const {
        return !(*this == other);
    }

    template<typename component_type>
    constexpr matrix3x3<component_type>::matrix3x3(
        const component_type a00, const component_type a01, const component_type a02,
        const component_type a10, const component_type a11, const component_type a12,
        const component_type a20, const component_type a21, const component_type a22) {
        set(a00, a01, a02,
            a10, a11, a12,
            a20, a21, a22);
    }

    template<typename component_type>
    const matrix3x3<component_type> matrix3x3<component_type>::identity = matrix3x3<component_type>();
}

#endif
