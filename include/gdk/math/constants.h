// © Joseph Cameron - MIT License

#ifndef GDK_MATH_CONSTANTS_H
#define GDK_MATH_CONSTANTS_H

#include <limits>
#include <type_traits>

namespace gdk::math::numbers {
    namespace detail {
        constexpr long double PI = 3.141592653589793238462643383279502884197L;
        constexpr long double DEGREES_TO_RADIANS = 0.01745329251994329576923690768488612713443L;
        constexpr long double RADIANS_TO_DEGREES = 57.29577951308232087679815481410517033241L;

        template<typename component_type>
        struct pi_value {
            static_assert(std::is_floating_point<component_type>::value,
                "pi requires a floating point type");

            static constexpr component_type value = static_cast<component_type>(PI);
        };

        template<typename component_type>
        struct degrees_to_radians {
            static_assert(std::is_floating_point<component_type>::value,
                "to_radians requires a floating point type");

            static constexpr component_type value = static_cast<component_type>(DEGREES_TO_RADIANS);
        };

        template<typename component_type>
        struct radians_to_degrees {
            static_assert(std::is_floating_point<component_type>::value,
                "to_degrees requires a floating point type");

            static constexpr component_type value = static_cast<component_type>(RADIANS_TO_DEGREES);
        };

        template<typename component_type>
        struct effectively_zero_length_squared {
            static_assert(std::is_floating_point<component_type>::value,
                "is_effectively_zero requires a floating point type");

            static constexpr component_type value = [] {
                auto result = static_cast<component_type>(1e-6L);

                for (int digit = std::numeric_limits<float>::digits;
                    digit < std::numeric_limits<component_type>::digits; ++digit)
                    result *= static_cast<component_type>(0.5);

                return result;
            }();
        };
    }

    //! PI to type appropriate precision
    template<typename component_type>
    inline constexpr component_type pi_v = detail::pi_value<component_type>::value;

    //! PI to double precision
    inline constexpr double pi = pi_v<double>;

    //! PI to float precision
    inline constexpr float pi_f = pi_v<float>;

    //! the squared length below which a vector has no trustworthy direction.
    template<typename component_type>
    inline constexpr component_type effectively_zero_length_squared_v =
        detail::effectively_zero_length_squared<component_type>::value;
}

namespace gdk::math {
    //! convert degrees to radians
    template<typename component_type>
    [[nodiscard]] constexpr component_type to_radians(const component_type aDegrees) {
        return aDegrees * numbers::detail::degrees_to_radians<component_type>::value;
    }

    //! convert radians to degrees
    template<typename component_type>
    [[nodiscard]] constexpr component_type to_degrees(const component_type aRadians) {
        return aRadians * numbers::detail::radians_to_degrees<component_type>::value;
    }
}

#endif
