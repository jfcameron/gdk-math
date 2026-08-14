// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_IMPL_STD_VECTOR2_INL
#define GDK_MATH_IMPL_STD_VECTOR2_INL

namespace gdk {
    template<typename T> constexpr vector2<T> vector2<T>::min(const vector2<T> &a, const vector2<T> &b) {
        return {std::min(a.x, b.x), std::min(a.y, b.y)};
    }

    template<typename T> constexpr vector2<T> vector2<T>::max(const vector2<T> &a, const vector2<T> &b) {
        return {std::max(a.x, b.x), std::max(a.y, b.y)};
    }

    template<typename component_type>
    component_type vector2<component_type>::length() const {
        return std::sqrt((x * x) + (y * y));
    }

    template<typename component_type>
    constexpr component_type vector2<component_type>::length_squared() const {
        return (x * x) + (y * y);
    }

    template<typename component_type>
    constexpr bool vector2<component_type>::is_effectively_zero() const {
        return length_squared() < numbers::effectively_zero_length_squared_v<component_type>;
    }

    template<typename component_type>
    component_type vector2<component_type>::distance_from(const vector2<component_type> &other) const {
        const auto dx = other.x - x;
        const auto dy = other.y - y;

        return std::sqrt(dx * dx + dy * dy);
    }

    template<typename component_type>
    constexpr component_type vector2<component_type>::aspect_ratio() const {
        return x / y;
    }

    template<typename component_type>
    vector2<component_type> vector2<component_type>::element_wise_product(
        const vector2<component_type> &aOther) const {
        return {x * aOther.x, y * aOther.y};
    }

    template<typename component_type>
    constexpr component_type vector2<component_type>::dot_product(const vector2<component_type> &other) const {
        return x * other.x + y * other.y;
    }

    template<typename component_type>
    constexpr component_type vector2<component_type>::cross_product(const vector2<component_type> &other) const {
        return x * other.y - other.x * y;
    }

    template<typename component_type>
    vector2<component_type> vector2<component_type>::normal() const {
        return vector2(*this).normalize();
    }

    template<typename component_type>
    vector2<component_type> &vector2<component_type>::normalize() {
        if (is_effectively_zero()) {
            *this = vector2::zero;

            return *this;
        }

        const component_type magnitude = length();

        x /= magnitude;
        y /= magnitude;

        return *this;
    }

    template<typename component_type>
    component_type vector2<component_type>::angle_between(
        const vector2<component_type> &that) const {
        const auto lengths = std::sqrt(length_squared() * that.length_squared());

        if (lengths <= static_cast<component_type>(0)) return static_cast<component_type>(0);

        const auto cosine = std::max(static_cast<component_type>(-1),
            std::min(static_cast<component_type>(1), dot_product(that) / lengths));

        return std::acos(cosine);
    }

    template<typename component_type>
    constexpr vector2<component_type> vector2<component_type>::reflect(
        const vector2<component_type> &aNormal) const {
        return *this - aNormal * (static_cast<component_type>(2) * dot_product(aNormal));
    }

    template<typename component_type>
    constexpr bool vector2<component_type>::operator==(const vector2<component_type> &other) const {
        return x == other.x && y == other.y;
    }

    template<typename component_type>
    constexpr bool vector2<component_type>::operator!=(const vector2<component_type> &other) const {
        return x != other.x || y != other.y;
    }

    template<typename component_type>
    constexpr component_type &vector2<component_type>::operator[](const std::size_t aComponentIndex) {
        switch (aComponentIndex) {
            case 0: return x;
            case 1: return y;
        }

        throw std::out_of_range("vector2::operator[] index out of range");
    }

    template<typename component_type>
    constexpr component_type vector2<component_type>::operator[](const std::size_t aComponentIndex) const {
        switch (aComponentIndex) {
            case 0: return x;
            case 1: return y;
        }

        throw std::out_of_range("vector2::operator[] index out of range");
    }

    template<typename component_type>
    constexpr vector2<component_type> vector2<component_type>::operator+(
        const vector2<component_type> &other) const {
        return {static_cast<component_type>(x + other.x), static_cast<component_type>(y + other.y)};
    }

    template<typename component_type>
    constexpr vector2<component_type> vector2<component_type>::operator-(
        const vector2<component_type> &other) const {
        return {static_cast<component_type>(x - other.x), static_cast<component_type>(y - other.y)};
    }

    template<typename component_type>
    constexpr vector2<component_type> vector2<component_type>::operator-() const {
        return {-x, -y};
    }

    template<typename component_type>
    constexpr vector2<component_type> vector2<component_type>::operator*(const component_type aScalar) const {
        return {static_cast<component_type>(x * aScalar), static_cast<component_type>(y * aScalar)};
    }

    template<typename component_type>
    constexpr vector2<component_type> &vector2<component_type>::operator/=(const vector2 &other) {
        if (other.x == 0 || other.y == 0)
            throw std::domain_error("Division by zero in vector2::operator/");

        x /= other.x;
        y /= other.y;

        return *this;
    }

    template<typename component_type>
    constexpr vector2<component_type> &vector2<component_type>::operator/=(const component_type aScalar) {
        if (aScalar == 0) throw std::domain_error("Division by zero in vector2::operator/");

        x /= aScalar;
        y /= aScalar;

        return *this;
    }

    template<typename component_type>
    constexpr vector2<component_type> vector2<component_type>::operator/(
        const vector2<component_type> &other) const {
        auto vec(*this);

        return vec /= other;
    }

    template<typename component_type>
    constexpr vector2<component_type> vector2<component_type>::operator/(const component_type aScalar) const {
        auto vec(*this);

        return vec /= aScalar;
    }

    template<typename component_type>
    constexpr vector2<component_type> &vector2<component_type>::operator+=(const vector2 &other) {
        x += other.x;
        y += other.y;

        return *this;
    }

    template<typename component_type>
    constexpr vector2<component_type> &vector2<component_type>::operator-=(const vector2 &other) {
        x -= other.x;
        y -= other.y;

        return *this;
    }

    template<typename component_type>
    constexpr vector2<component_type> &vector2<component_type>::operator*=(const component_type aScalar) {
        x *= aScalar;
        y *= aScalar;

        return *this;
    }

    template<typename component_type>
    constexpr vector2<component_type>::vector2(const component_type aX, const component_type aY)
    : vector2_storage<component_type>{aX, aY}
    {}

    template<typename component_type>
    constexpr vector2<component_type>::vector2(const component_type aScalar)
    : vector2<component_type>(aScalar, aScalar)
    {}

    template<typename component_type> template<class other_component_type>
    constexpr vector2<component_type>::vector2(
        const std::pair<other_component_type, other_component_type> aPair)
    : vector2<component_type>(static_cast<component_type>(std::get<0>(aPair)),
        static_cast<component_type>(std::get<1>(aPair)))
    {}

    template<typename component_type>
    constexpr vector2<component_type> lerp(const vector2<component_type> &a, const vector2<component_type> &b,
        const component_type t) {
        return a + (b - a) * t;
    }

    template <typename T> const vector2<T> vector2<T>::down  = { 0,-1};
    template <typename T> const vector2<T> vector2<T>::left  = {-1, 0};
    template <typename T> const vector2<T> vector2<T>::one   = { 1, 1};
    template <typename T> const vector2<T> vector2<T>::right = { 1, 0};
    template <typename T> const vector2<T> vector2<T>::up    = { 0, 1};
    template <typename T> const vector2<T> vector2<T>::zero  = { 0, 0};
}

#endif
