// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/math.h>

#include <type_traits>

namespace gdk {
    template class vector2<float>;
    template class vector2<double>;
    template class vector2<long double>;

    template class vector3<float>;
    template class vector3<double>;
    template class vector3<long double>;

    template class vector4<float>;
    template class vector4<double>;
    template class vector4<long double>;

    template class quaternion<float>;
    template class quaternion<double>;
    template class quaternion<long double>;

    template class matrix3x3<float>;
    template class matrix3x3<double>;
    template class matrix3x3<long double>;

    template class matrix4x4<float>;
    template class matrix4x4<double>;
    template class matrix4x4<long double>;
}

using namespace gdk;

namespace {
    template<typename T>
    void use_every_free_function() {
        const vector2<T> a2(1, 2), b2(3, 4);
        const vector3<T> a3(1, 2, 3), b3(4, 5, 6);
        const quaternion<T> q(0, 0, 0, 1);

        matrix4x4<T> m;
        m.set_to_identity();
        m.set_translation({1, 2, 3});

        matrix3x3<T> m3;
        m3.set_to_identity();

        (void)lerp(a2, b2, static_cast<T>(0.5));
        (void)lerp(a3, b3, static_cast<T>(0.5));
        (void)slerp(q, q, static_cast<T>(0.5));
        (void)nlerp(q, q, static_cast<T>(0.5));
        (void)(q * q);
        (void)(q * a3);
        (void)(m * a3);
        (void)(m * vector4<T>(a3));
        (void)(m3 * a3);
        (void)upper_left(m);
        (void)normal_matrix(m);
        (void)to_radians(static_cast<T>(90));
        (void)to_degrees(static_cast<T>(1));
    }
}

TEST_CASE("gdk-math instantiates for every supported component type", "[instantiation]")
{
    SECTION("the free functions compile for all three")
    {
        use_every_free_function<float>();
        use_every_free_function<double>();
        use_every_free_function<long double>();

        SUCCEED("compiled");
    }

    SECTION("double really is double, not a float in disguise")
    {
        static_assert(std::is_same<vector3<double>::component_type, double>::value, "");
        static_assert(std::is_same<matrix4x4<long double>::component_type, long double>::value, "");

        // and a double carries precision a float cannot
        const vector3<double> precise(0.1, 0.2, 0.3);
        const vector3<float> coarse(0.1f, 0.2f, 0.3f);

        REQUIRE(precise.length_squared() != static_cast<double>(coarse.length_squared()));
    }

    SECTION("the arithmetic agrees across types")
    {
        const vector3<double> d(1, 2, 3), e(4, 5, 6);
        const vector3<long double> ld(1, 2, 3), le(4, 5, 6);

        REQUIRE(d.dot_product(e) == Approx(32.0));
        REQUIRE(static_cast<double>(ld.dot_product(le)) == Approx(32.0));

        REQUIRE(d.cross_product(e).x == Approx(-3.0));
        REQUIRE(static_cast<double>(ld.cross_product(le).x) == Approx(-3.0));
    }
}
