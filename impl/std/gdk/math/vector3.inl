// © Joseph Cameron - MIT License

#ifndef GDK_MATH_IMPL_STD_VECTOR3_INL
#define GDK_MATH_IMPL_STD_VECTOR3_INL

namespace gdk::math {

    template<arithmetic_component T> constexpr vector3<T> vector3<T>::min(const vector3<T> &a, const vector3<T> &b) {
        return { std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z) };
    }
    template<arithmetic_component T> constexpr vector3<T> vector3<T>::max(const vector3<T> &a, const vector3<T> &b) {
        return { std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z) };
    }

    template<arithmetic_component component_type> 
    inline const vector3<component_type> vector3<component_type>::backward = {0, 0, 1};
    template<arithmetic_component component_type> 
    inline const vector3<component_type> vector3<component_type>::down = {0, -1, 0};
    template<arithmetic_component component_type> 
    inline const vector3<component_type> vector3<component_type>::forward = {0, 0, -1};
    template<arithmetic_component component_type> 
    inline const vector3<component_type> vector3<component_type>::left = {-1, 0, 0};
    template<arithmetic_component component_type> 
    inline const vector3<component_type> vector3<component_type>::one = {1, 1, 1};
    template<arithmetic_component component_type> 
    inline const vector3<component_type> vector3<component_type>::right = {1, 0, 0};
    template<arithmetic_component component_type> 
    inline const vector3<component_type> vector3<component_type>::up = {0, 1, 0};
    template<arithmetic_component component_type> 
    inline const vector3<component_type> vector3<component_type>::zero = {0, 0, 0};

    template<arithmetic_component component_type> 
    vector3<component_type> &vector3<component_type>::normalize() requires std::floating_point<component_type> {
        if (is_effectively_zero()) {
            *this = vector3::zero;

            return *this;
        }

        const component_type length = this->length();

        x /= length;
        y /= length;
        z /= length;

        return *this;
    }

    template<arithmetic_component component_type> 
    component_type vector3<component_type>::length() const {
        return static_cast<component_type>(std::sqrt((x * x) + (y * y) + (z * z)));
    }

    template<arithmetic_component component_type> 
    constexpr component_type vector3<component_type>::length_squared() const {
        return static_cast<component_type>((x * x) + (y * y) + (z * z));
    }

    template<arithmetic_component component_type> 
    constexpr bool vector3<component_type>::is_effectively_zero() const requires std::floating_point<component_type> {
        return length_squared() < numbers::effectively_zero_length_squared_v<component_type>;
    }

    template<arithmetic_component component_type> vector3<component_type>
    vector3<component_type>::normal() const requires std::floating_point<component_type> {
        return vector3(*this).normalize();
    }

    template<arithmetic_component component_type> 
    component_type vector3<component_type>::distance_from(const vector3<component_type> &that) const {
        const auto dx = that.x - x;
        const auto dy = that.y - y;
        const auto dz = that.z - z;
        return static_cast<component_type>(std::sqrt(dx * dx + dy * dy + dz * dz));
    }

    template<arithmetic_component component_type>
    constexpr bool vector3<component_type>::operator==(const vector3<component_type> &that) const {
        return x == that.x && y == that.y && z == that.z;
    }

    template<arithmetic_component component_type>
    constexpr vector3<component_type> &vector3<component_type>::operator/=(const vector3 &that) {
        if (that.x == 0 || that.y == 0 || that.z == 0) 
            throw std::domain_error("Division by zero in vector3::operator/");
        
        x /= that.x;
        y /= that.y;
        z /= that.z;

        return *this;
    }

    template<arithmetic_component component_type>
    constexpr vector3<component_type> vector3<component_type>::operator/(const vector3<component_type> &that) const {
        auto vec(*this);
        return vec /= that;
    }

    template<arithmetic_component component_type>
    constexpr vector3<component_type> vector3<component_type>::operator+(const vector3<component_type> &that) const {
        return {static_cast<component_type>(x + that.x), static_cast<component_type>(y + that.y), static_cast<component_type>(z + that.z)};
    }

    template<arithmetic_component component_type>
    constexpr vector3<component_type> vector3<component_type>::operator-(const vector3<component_type> &that) const {
        return {static_cast<component_type>(x - that.x), static_cast<component_type>(y - that.y), static_cast<component_type>(z - that.z)};
    }

    template<arithmetic_component component_type>
    constexpr vector3<component_type> vector3<component_type>::operator-() const {
        return {static_cast<component_type>(-x), static_cast<component_type>(-y), static_cast<component_type>(-z)};
    }

    template<arithmetic_component component_type>
    constexpr vector3<component_type> vector3<component_type>::operator*(const component_type aScalar) const {
        return {static_cast<component_type>(x * aScalar), static_cast<component_type>(y * aScalar), static_cast<component_type>(z * aScalar)};
    }

    template<arithmetic_component component_type>
    constexpr vector3<component_type> &vector3<component_type>::operator/=(const component_type &aScalar) {
        if (aScalar == 0) throw std::domain_error("Division by zero in vector3::operator/");

        x /= aScalar;
        y /= aScalar;
        z /= aScalar;

        return *this;
    }

    template<arithmetic_component component_type>
    constexpr vector3<component_type> vector3<component_type>::operator/(const component_type aScalar) const {
        auto vec(*this);

        return vec /= aScalar;
    }

    template<arithmetic_component component_type>
    constexpr component_type &vector3<component_type>::operator[](const std::size_t aComponentIndex) {
        switch(aComponentIndex) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
        }
        throw std::out_of_range("vector3::operator[] index out of range");
    }

    template<arithmetic_component component_type>
    constexpr component_type vector3<component_type>::operator[](const std::size_t aComponentIndex) const {
        switch(aComponentIndex) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
        }
        throw std::out_of_range("vector3::operator[] index out of range");
    }

    template<arithmetic_component component_type>
    constexpr vector3<component_type> &vector3<component_type>::operator+=(const vector3<component_type> &that) {
        x += that.x;
        y += that.y;
        z += that.z;
        return *this;
    }

    template<arithmetic_component component_type>
    constexpr vector3<component_type> &vector3<component_type>::operator-=(const vector3<component_type> &that) {
        x -= that.x;
        y -= that.y;
        z -= that.z;
        return *this;
    }

    template<arithmetic_component component_type>
    constexpr vector3<component_type> &vector3<component_type>::operator*=(const component_type &aScalar) {
        x *= aScalar;
        y *= aScalar;
        z *= aScalar;
        return *this;
    }

    template<arithmetic_component component_type>
    constexpr vector3<component_type>::vector3(const component_type &aX, const component_type &aY,
        const component_type &aZ)
    : vector3_storage<component_type>{aX, aY, aZ}
    {}

    template<arithmetic_component component_type>
    constexpr vector3<component_type>::vector3(const component_type &aBroadcast)
    : vector3(aBroadcast, aBroadcast, aBroadcast)
    {}

    template<arithmetic_component component_type>
    component_type vector3<component_type>::angle_between(const vector3<component_type> &that) const requires std::floating_point<component_type> {
        const auto lengths = std::sqrt(length_squared() * that.length_squared());

        if (lengths <= static_cast<component_type>(0)) return static_cast<component_type>(0);

        const auto cosine = std::max(static_cast<component_type>(-1),
            std::min(static_cast<component_type>(1), dot_product(that) / lengths));

        return std::acos(cosine);
    }

    template<arithmetic_component component_type>
    constexpr vector3<component_type> vector3<component_type>::reflect(
        const vector3<component_type> &aNormal) const {
        return *this - aNormal * (static_cast<component_type>(2) * dot_product(aNormal));
    }

    template<arithmetic_component component_type>
    constexpr vector3<component_type> lerp(const vector3<component_type> &a, const vector3<component_type> &b,
        const component_type t) {
        return a + (b - a) * t;
    }

    template<arithmetic_component component_type>
    constexpr component_type vector3<component_type>::dot_product(const vector3<component_type> &that) const {
        return static_cast<component_type>(x * that.x + y * that.y + z * that.z);
    }

    template<arithmetic_component component_type>
    constexpr vector3<component_type> vector3<component_type>::cross_product(const vector3<component_type> &that) const {
        return {static_cast<component_type>(y * that.z - z * that.y), static_cast<component_type>(z * that.x - x * that.z),
            static_cast<component_type>(x * that.y - y * that.x)};
    }

    template<arithmetic_component component_type>
    constexpr vector3<component_type> vector3<component_type>::element_wise_product(const vector3<component_type> &aOther) const {
        return {static_cast<component_type>(x * aOther.x), static_cast<component_type>(y * aOther.y), static_cast<component_type>(z * aOther.z)};
    }
}

#endif
