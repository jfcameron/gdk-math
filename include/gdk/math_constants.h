// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_CONSTANTS_H
#define GDK_MATH_CONSTANTS_H

#include <limits>
#include <type_traits>

namespace gdk::numbers {
    namespace detail {
        constexpr long double PI = 3.14159265358979323846L;

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

            static constexpr component_type value = static_cast<component_type>(PI / 180.0L);
        };

        template<typename component_type>
        struct radians_to_degrees {
            static_assert(std::is_floating_point<component_type>::value,
                "to_degrees requires a floating point type");

            static constexpr component_type value = static_cast<component_type>(180.0L / PI);
        };

        template<typename component_type>
        struct effectively_zero_length_squared {
            static_assert(std::is_floating_point<component_type>::value,
                "is_effectively_zero requires a floating point type");

            static constexpr component_type value = static_cast<component_type>(
                1e-6L * (static_cast<long double>(std::numeric_limits<component_type>::epsilon())
                    / static_cast<long double>(std::numeric_limits<float>::epsilon())));
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

namespace gdk {
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
