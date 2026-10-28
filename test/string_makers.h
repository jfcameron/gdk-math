// © Joseph Cameron - MIT License

#ifndef GDK_MATH_TEST_STRING_MAKERS_H
#define GDK_MATH_TEST_STRING_MAKERS_H

#include <jfc/catch.hpp>

#include <gdk/math.h>

#include <cstddef>
#include <initializer_list>
#include <sstream>
#include <string>

namespace gdk_math_test {
    template<typename component_type>
    std::string join(std::initializer_list<component_type> aComponents) {
        std::ostringstream stream;
        stream.precision(9);

        stream << '{';

        const char *separator = "";

        for (const auto component : aComponents) {
            stream << separator << +component;
            separator = ", ";
        }

        stream << '}';

        return stream.str();
    }

    template<typename matrix_type>
    std::string columns(const matrix_type &aMatrix) {
        std::string result = "[";

        for (std::size_t column = 0; column < matrix_type::order; ++column) {
            std::ostringstream stream;
            stream.precision(9);

            stream << '{';

            for (std::size_t row = 0; row < matrix_type::order; ++row)
                stream << (row ? ", " : "") << aMatrix.get(column, row);

            stream << '}';

            result += (column ? " " : "") + stream.str();
        }

        return result + "]";
    }
}

namespace Catch {
    template<typename component_type>
    struct StringMaker<gdk::math::vector2<component_type>> {
        static std::string convert(const gdk::math::vector2<component_type> &a) {
            return gdk_math_test::join({a.x, a.y});
        }
    };

    template<typename component_type>
    struct StringMaker<gdk::math::vector3<component_type>> {
        static std::string convert(const gdk::math::vector3<component_type> &a) {
            return gdk_math_test::join({a.x, a.y, a.z});
        }
    };

    template<typename component_type>
    struct StringMaker<gdk::math::vector4<component_type>> {
        static std::string convert(const gdk::math::vector4<component_type> &a) {
            return gdk_math_test::join({a.x, a.y, a.z, a.w});
        }
    };

    template<typename component_type>
    struct StringMaker<gdk::math::quaternion<component_type>> {
        static std::string convert(const gdk::math::quaternion<component_type> &a) {
            return gdk_math_test::join({a.x, a.y, a.z, a.w});
        }
    };

    template<typename component_type>
    struct StringMaker<gdk::math::matrix3x3<component_type>> {
        static std::string convert(const gdk::math::matrix3x3<component_type> &a) {
            return gdk_math_test::columns(a);
        }
    };

    template<typename component_type>
    struct StringMaker<gdk::math::matrix4x4<component_type>> {
        static std::string convert(const gdk::math::matrix4x4<component_type> &a) {
            return gdk_math_test::columns(a);
        }
    };
}

#endif
