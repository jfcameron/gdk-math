// © Joseph Cameron - MIT License

#include <jfc/catch.hpp>

#include "string_makers.h"
#include <jfc/types.h>

#include <gdk/math.h>

#include <stdexcept>
#include <type_traits>
#include <utility>

namespace gdk::math {
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

using namespace gdk::math;

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

TEMPLATE_TEST_CASE("integer vectors do integer arithmetic", "[instantiation]", int, short, signed char)
{
    using vec2 = vector2<TestType>;
    using vec3 = vector3<TestType>;

    SECTION("the operators stay in the component type")
    {
        const vec3 a{1, 2, 3}, b{4, 5, 6};

        STATIC_REQUIRE(std::is_same_v<decltype(a + b), vec3>);

        REQUIRE(a + b == vec3{5, 7, 9});
        REQUIRE(b - a == vec3{3, 3, 3});
        REQUIRE(-a == vec3{-1, -2, -3});
        REQUIRE(a * static_cast<TestType>(2) == vec3{2, 4, 6});
        REQUIRE(a.element_wise_product(b) == vec3{4, 10, 18});
        REQUIRE(a.cross_product(b) == vec3{-3, 6, -3});
        REQUIRE(a.dot_product(b) == 32);
        REQUIRE(b / static_cast<TestType>(2) == vec3{2, 2, 3});

        const vec2 c{3, 4}, d{1, 2};

        REQUIRE(c + d == vec2{4, 6});
        REQUIRE(c - d == vec2{2, 2});
        REQUIRE(-c == vec2{-3, -4});
        REQUIRE(c * static_cast<TestType>(2) == vec2{6, 8});
        REQUIRE(c / static_cast<TestType>(2) == vec2{1, 2});
        REQUIRE(c / d == vec2{3, 2});
        REQUIRE(c.element_wise_product(d) == vec2{3, 8});
        REQUIRE(c.cross_product(d) == 2);
        REQUIRE(c.dot_product(d) == 11);
        REQUIRE(c.length_squared() == 25);
    }

    SECTION("every member an integer vector offers compiles and does what it says")
    {
        vec3 a{3, 4, 0};
        const vec3 b{1, 2, 3};

        REQUIRE(a.length() == 5);
        REQUIRE(a.distance_from(vec3{0, 0, 0}) == 5);
        REQUIRE(a[0] == 3);
        REQUIRE_THROWS_AS(a[3], std::out_of_range);
        REQUIRE(vec3::min(a, b) == vec3{1, 2, 0});
        REQUIRE(vec3::max(a, b) == vec3{3, 4, 3});
        REQUIRE(lerp(vec3{0, 0, 0}, vec3{4, 8, 2}, static_cast<TestType>(1)) == vec3{4, 8, 2});
        REQUIRE(vec3{1, -1, 0}.reflect(vec3{0, 1, 0}) == vec3{1, 1, 0});
        REQUIRE(a / vec3{3, 2, 1} == vec3{1, 2, 0});
        REQUIRE_THROWS_AS((a / vec3{1, 0, 1}), std::domain_error);

        a += b;
        a -= b;
        a *= static_cast<TestType>(2);
        a /= static_cast<TestType>(2);
        a /= vec3{1, 1, 1};
        REQUIRE(a == vec3{3, 4, 0});
        REQUIRE(vec3(static_cast<TestType>(7)) == vec3{7, 7, 7});

        vec2 c{6, 8};
        const vec2 d{2, 4};

        REQUIRE(c.length() == 10);
        REQUIRE(c.distance_from(vec2{0, 0}) == 10);
        REQUIRE(c.aspect_ratio() == 0);
        REQUIRE(c[1] == 8);
        REQUIRE(vec2::min(c, d) == vec2{2, 4});
        REQUIRE(vec2::max(c, d) == vec2{6, 8});
        REQUIRE(lerp(vec2{0, 0}, vec2{4, 8}, static_cast<TestType>(1)) == vec2{4, 8});
        REQUIRE(vec2{1, -1}.reflect(vec2{0, 1}) == vec2{1, 1});
        REQUIRE(vec2(std::pair<int, int>{2, 3}) == vec2{2, 3});
        REQUIRE(vec2(static_cast<TestType>(5)) == vec2{5, 5});

        c += d;
        c -= d;
        c *= static_cast<TestType>(2);
        c /= static_cast<TestType>(2);
        c /= d;
        REQUIRE(c == vec2{3, 2});

        const vector4<TestType> point{8, 4, -4, 4};

        REQUIRE(point.xyz() == vec3{8, 4, -4});
        REQUIRE(point.to_point() == vec3{2, 1, -1});
        REQUIRE(vector4<TestType>::direction(b).w == 0);
        REQUIRE(vector4<TestType>{} == vector4<TestType>::origin);
    }

    SECTION("the members that need a floating point type are not offered")
    {
        STATIC_REQUIRE_FALSE(requires(vec3 v) { v.normalize(); });
        STATIC_REQUIRE_FALSE(requires(const vec3 v) { v.normal(); });
        STATIC_REQUIRE_FALSE(requires(const vec3 v) { v.is_effectively_zero(); });
        STATIC_REQUIRE_FALSE(requires(const vec2 v) { v.angle_between(v); });

        STATIC_REQUIRE(requires(vector3<float> v) { v.normalize(); });
    }
}
