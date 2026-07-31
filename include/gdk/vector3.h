// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_VECTOR3_H
#define GDK_MATH_VECTOR3_H

#include <cmath>
#include <iosfwd>
#include <stdexcept>
#include <type_traits>

namespace gdk {
    /// \brief 3d vector class used to represent position, scale, velocity, heading, euler angles, etc.
    ///
    /// \tparam component_type_param numeric type used by the vector's components
    ///
    template<typename component_type_param = float>
    struct vector3 final {
        using component_type = component_type_param;

        static_assert(
            std::is_arithmetic<component_type>::value && 
            std::is_signed<component_type>::value, 
            "vector3::component_type must be a signed arithmetic type");

		vector3(const component_type& a);
        vector3 &operator=(const vector3 &) = default;
        vector3() = default;
        vector3(const component_type &aX, const component_type &aY, const component_type &aZ);
        vector3(const vector3 &that) = default;
        vector3(vector3 &&) = default;
        ~vector3() = default;

        /// \brief creates a new vector made of the largest components of two vectors
        static vector3 max(const vector3 &a, const vector3 &b);
        /// \brief creates a new vector made of the smallest components of two vectors
        static vector3 min(const vector3 &a, const vector3 &b);

        static const vector3 backward;
        static const vector3 down;
        static const vector3 forward;
        static const vector3 left;
        static const vector3 one;
        static const vector3 right;
        static const vector3 up;
        static const vector3 zero;

        //! faster than a component-wise check but not approprate if each component must be precisely zero
        [[nodiscard]] bool is_effectively_zero() const;
        [[nodiscard]] component_type distance_from(const vector3 &that) const;
        [[nodiscard]] component_type dot_product(const vector3 &that) const;
        [[nodiscard]] component_type length() const;
        //! get the squared length of the vector (faster than length, appropriate for certain use cases)
        [[nodiscard]] component_type length_squared() const;
        [[nodiscard]] vector3 cross_product(const vector3 &that) const;
        [[nodiscard]] vector3 element_wise_product(const vector3 &aOther) const;
        [[nodiscard]] vector3 normal() const;

        vector3 &normalize();

        bool operator!=(const vector3 &that) const;
        bool operator==(const vector3 &that) const;

        component_type &operator[](const size_t aComponentIndex);
        component_type operator[](const size_t aComponentIndex) const;
 
        [[nodiscard]] vector3 operator*(const component_type aScalar) const;
        [[nodiscard]] vector3 operator+(const vector3 &that) const;
        [[nodiscard]] vector3 operator-() const;
        [[nodiscard]] vector3 operator-(const vector3 &that) const;
        [[nodiscard]] vector3 operator/(const vector3 &that) const;

        vector3 &operator*=(const component_type &aScalar);
        vector3 &operator+=(const vector3 &that);
        vector3 &operator-=(const vector3 &that);
        vector3 &operator/=(const vector3 &that);

        component_type x = {0}, y = {0}, z = {0};

    private:
        constexpr static component_type IS_EFFECTIVELY_ZERO_THRESHOLD = static_cast<component_type>(1e-6);
    };

    template <typename T> vector3<T> vector3<T>::min(const vector3<T> &a, const vector3<T> &b) {
        return { std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z) };
    }
    template <typename T> vector3<T> vector3<T>::max(const vector3<T> &a, const vector3<T> &b) {
        return { std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z) };
    }

    template <typename component_type> 
    inline const vector3<component_type> vector3<component_type>::backward = {0, 0, -1};
    template <typename component_type> 
    inline const vector3<component_type> vector3<component_type>::down = {0, -1, 0};
    template <typename component_type> 
    inline const vector3<component_type> vector3<component_type>::forward = {0, 0, 1};
    template <typename component_type> 
    inline const vector3<component_type> vector3<component_type>::left = {-1, 0, 0};
    template <typename component_type> 
    inline const vector3<component_type> vector3<component_type>::one = {1, 1, 1};
    template <typename component_type> 
    inline const vector3<component_type> vector3<component_type>::right = {1, 0, 0};
    template <typename component_type> 
    inline const vector3<component_type> vector3<component_type>::up = {0, 1, 0};
    template <typename component_type> 
    inline const vector3<component_type> vector3<component_type>::zero = {0, 0, 0};

    template<typename component_type> 
    vector3<component_type> &vector3<component_type>::normalize() {
        component_type length = this->length();
        if (length != 0) {
            x /= length;
            y /= length;
            z /= length;
        }
        return *this;
    }

    template<typename component_type> 
    component_type vector3<component_type>::length() const {
        return sqrt((x * x) + (y * y) + (z * z));
    }

    template<typename component_type> 
    component_type vector3<component_type>::length_squared() const {
        return (x * x) + (y * y) + (z * z);
    }

    template<typename component_type> 
    bool vector3<component_type>::is_effectively_zero() const {
        return length_squared() < IS_EFFECTIVELY_ZERO_THRESHOLD;
    }

    template<typename component_type> vector3<component_type> 
    vector3<component_type>::normal() const {
        if (is_effectively_zero()) return vector3::zero;
        return vector3(*this).normalize();
    }

    template<typename component_type> 
    component_type vector3<component_type>::distance_from(const vector3<component_type> &that) const {
        const auto dx = that.x - x;
        const auto dy = that.y - y;
        const auto dz = that.z - z;
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }

    template<typename component_type>
    bool vector3<component_type>::operator==(const vector3<component_type> &that) const {
        return x == that.x && y == that.y && z == that.z;
    }

    template<typename component_type>
    bool vector3<component_type>::operator!=(const vector3<component_type> &that) const {
        return x != that.x || y != that.y || z != that.z;
    }

    template<typename component_type>
    vector3<component_type> &vector3<component_type>::operator/=(const vector3 &that) {
        if (that.x == 0 || that.y == 0 || that.z == 0) 
            throw std::domain_error("Division by zero in vector3::operator/");
        
        x /= that.x;
        y /= that.y;
        z /= that.z;

        return *this;
    }

    template<typename component_type>
    vector3<component_type> vector3<component_type>::operator/(const vector3<component_type> &that) const {
        auto vec(*this);
        return vec /= that;
    }

    template<typename component_type>
    vector3<component_type> vector3<component_type>::operator+(const vector3<component_type> &that) const {
        return {
            x + that.x,
            y + that.y,
            z + that.z
        };
    }

    template<typename component_type>
    vector3<component_type> vector3<component_type>::operator-(const vector3<component_type> &that) const {
        return {
            x - that.x,
            y - that.y,
            z - that.z
        };
    }

    template<typename component_type>
    vector3<component_type> vector3<component_type>::operator-() const {
        return {
            x * -1,
            y * -1,
            z * -1
        };
    }

    template<typename component_type>
    vector3<component_type> vector3<component_type>::operator*(const component_type aScalar) const {
        return {
            x * aScalar,
            y * aScalar,
            z * aScalar
        };
    }

    template<typename component_type>
    component_type &vector3<component_type>::operator[](const size_t aComponentIndex) {
        switch(aComponentIndex) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
        }
        throw std::out_of_range("vector3::operator[] index out of range");
    }

    template<typename component_type>
    component_type vector3<component_type>::operator[](const size_t aComponentIndex) const {
        switch(aComponentIndex) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
        }
        throw std::out_of_range("vector3::operator[] index out of range");
    }

    template<typename component_type>
    vector3<component_type> &vector3<component_type>::operator+=(const vector3<component_type> &that) {
        x += that.x;
        y += that.y;
        z += that.z;
        return *this;
    }

    template<typename component_type>
    vector3<component_type> &vector3<component_type>::operator-=(const vector3<component_type> &that) {
        x -= that.x;
        y -= that.y;
        z -= that.z;
        return *this;
    }

    template<typename component_type>
    vector3<component_type> &vector3<component_type>::operator*=(const component_type &aScalar) {
        x *= aScalar;
        y *= aScalar;
        z *= aScalar;
        return *this;
    }

    template<typename component_type>
    vector3<component_type>::vector3(const component_type &aX, const component_type &aY, const component_type &aZ)
    : x(aX)
    , y(aY)
    , z(aZ)
    {}

    template<typename component_type>
    vector3<component_type>::vector3(const component_type& a)
    : vector3(a, a, a)
    {}

    template<typename component_type>
    component_type vector3<component_type>::dot_product(const vector3<component_type> &that) const {
        return { x * that.x + y * that.y + z * that.z };
    }

    template<typename component_type>
    vector3<component_type> vector3<component_type>::cross_product(const vector3<component_type> &that) const {
        return { 
            y * that.z - z * that.y,
            z * that.x - x * that.z,
            x * that.y - y * that.x
        };
    }

    template<typename component_type>
    vector3<component_type> vector3<component_type>::element_wise_product(const vector3<component_type> &aOther) const {
        return { 
            x * aOther.x, 
            y * aOther.y, 
            z * aOther.z
        };
    }
}

#endif

