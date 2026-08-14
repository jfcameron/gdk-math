// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/math.h>

#include <cmath>
#include <stdexcept>
#include <utility>

using namespace gdk;

namespace {
    template<typename T>
    struct pair2d final {
        vector2<T> two;
        vector3<T> three;

        pair2d(const T aX, const T aY) : two(aX, aY), three(aX, aY, 0) {}
    };
}

TEMPLATE_LIST_TEST_CASE("gdk-math: vector2 and vector3 agree on shared measurements", "[parity]", type::floating_point)
{
    using vec2 = vector2<TestType>;
    using vec3 = vector3<TestType>;

    const pair2d<TestType> a(3, 4);
    const pair2d<TestType> b(-1.5f, 2.25f);

    SECTION("length and length_squared")
    {
        REQUIRE(a.two.length() == Approx(a.three.length()));
        REQUIRE(a.two.length_squared() == Approx(a.three.length_squared()));
        REQUIRE(a.two.length() == Approx(5.0f));
    }

    SECTION("distance_from")
    {
        REQUIRE(a.two.distance_from(b.two) == Approx(a.three.distance_from(b.three)));
    }

    SECTION("dot_product")
    {
        REQUIRE(a.two.dot_product(b.two) == Approx(a.three.dot_product(b.three)));
    }

    SECTION("cross_product")
    {
        REQUIRE(a.two.cross_product(b.two) == Approx(a.three.cross_product(b.three).z));
    }

    SECTION("angle_between")
    {
        REQUIRE(a.two.angle_between(b.two) == Approx(a.three.angle_between(b.three)));
    }

    SECTION("is_effectively_zero uses the same threshold")
    {
        for (const auto scale : {0.0f, 1e-9f, 1e-4f, 1.0f}) {
            const pair2d<TestType> v(scale, 0);

            REQUIRE(v.two.is_effectively_zero() == v.three.is_effectively_zero());
        }
    }
}

TEMPLATE_LIST_TEST_CASE("gdk-math: vector2 and vector3 agree on shared transformations", "[parity]", type::floating_point)
{
    using vec2 = vector2<TestType>;
    using vec3 = vector3<TestType>;

    SECTION("normal and normalize")
    {
        const pair2d<TestType> v(3, 4);

        REQUIRE(v.two.normal().x == Approx(v.three.normal().x));
        REQUIRE(v.two.normal().y == Approx(v.three.normal().y));

        auto two = v.two;
        auto three = v.three;

        REQUIRE(two.normalize() == v.two.normal());
        REQUIRE(three.normalize() == v.three.normal());
    }

    SECTION("**a tiny but nonzero vector normalizes to zero in both**")
    {
        const auto cutoff = std::sqrt(numbers::effectively_zero_length_squared_v<TestType>);
        const pair2d<TestType> tiny(static_cast<TestType>(cutoff / 100), 0);

        REQUIRE(tiny.two.is_effectively_zero());
        REQUIRE(tiny.three.is_effectively_zero());

        REQUIRE(tiny.two.normal() == vec2::zero);
        REQUIRE(tiny.three.normal() == vec3::zero);

        auto two = tiny.two;
        auto three = tiny.three;
        REQUIRE(two.normalize() == vec2::zero);
        REQUIRE(three.normalize() == vec3::zero);
    }

    SECTION("a zero vector normalizes to zero rather than to NaN, in both")
    {
        auto two = vec2::zero;
        auto three = vec3::zero;

        REQUIRE(two.normalize() == vec2::zero);
        REQUIRE(three.normalize() == vec3::zero);
        REQUIRE(vec2::zero.normal() == vec2::zero);
        REQUIRE(vec3::zero.normal() == vec3::zero);
    }

    SECTION("element_wise_product")
    {
        const pair2d<TestType> a(2, 3);
        const pair2d<TestType> b(5, 7);

        REQUIRE(a.two.element_wise_product(b.two) == vec2(10, 21));
        REQUIRE(a.three.element_wise_product(b.three) == vec3(10, 21, 0));
    }

    SECTION("reflect")
    {
        const pair2d<TestType> v(1, -1);

        REQUIRE(v.two.reflect(vec2::up) == vec2(1, 1));
        REQUIRE(v.three.reflect(vec3::up) == vec3(1, 1, 0));
    }

    SECTION("lerp")
    {
        const auto two = lerp(vec2(0, 0), vec2(4, 8), static_cast<TestType>(0.25));
        const auto three = lerp(vec3(0, 0, 0), vec3(4, 8, 0), static_cast<TestType>(0.25));

        REQUIRE(two.x == Approx(three.x));
        REQUIRE(two.y == Approx(three.y));
    }

    SECTION("min and max")
    {
        REQUIRE(vec2::min(vec2(1, 8), vec2(4, 2)) == vec2(1, 2));
        REQUIRE(vec2::max(vec2(1, 8), vec2(4, 2)) == vec2(4, 8));

        REQUIRE(vec3::min(vec3(1, 8, 0), vec3(4, 2, 0)) == vec3(1, 2, 0));
        REQUIRE(vec3::max(vec3(1, 8, 0), vec3(4, 2, 0)) == vec3(4, 8, 0));
    }
}

TEMPLATE_LIST_TEST_CASE("gdk-math: vector2 and vector3 fail the same way", "[parity]", type::floating_point)
{
    using vec2 = vector2<TestType>;
    using vec3 = vector3<TestType>;

    SECTION("dividing by zero throws domain_error in both")
    {
        REQUIRE_THROWS_AS(vec2(1, 2) / 0.0f, std::domain_error);
        REQUIRE_THROWS_AS(vec3(1, 2, 3) / 0.0f, std::domain_error);

        REQUIRE_THROWS_AS(vec2(1, 2) / vec2(1, 0), std::domain_error);
        REQUIRE_THROWS_AS(vec3(1, 2, 3) / vec3(1, 0, 1), std::domain_error);
    }

    SECTION("indexing past the end throws out_of_range in both")
    {
        REQUIRE_THROWS_AS(vec2(1, 2)[2], std::out_of_range);
        REQUIRE_THROWS_AS(vec3(1, 2, 3)[3], std::out_of_range);
    }

    SECTION("valid indices work in both")
    {
        const vec2 two(7, 8);
        REQUIRE(two[0] == Approx(7.0f));
        REQUIRE(two[1] == Approx(8.0f));

        const vec3 three(7, 8, 9);
        REQUIRE(three[0] == Approx(7.0f));
        REQUIRE(three[2] == Approx(9.0f));

        vec2 mutableTwo(0, 0);
        mutableTwo[1] = 5;
        REQUIRE(mutableTwo.y == Approx(5.0f));
    }
}

TEMPLATE_LIST_TEST_CASE("gdk::vector2 operations added for parity with vector3", "[vector2]", type::floating_point)
{
    using vec2 = vector2<TestType>;
    using vec3 = vector3<TestType>;

    SECTION("unary minus negates both components")
    {
        REQUIRE(-vec2(3, -4) == vec2(-3, 4));
    }

    SECTION("division by a scalar and by a vector")
    {
        REQUIRE(vec2(10, 20) / 2.0f == vec2(5, 10));
        REQUIRE(vec2(10, 20) / vec2(2, 5) == vec2(5, 4));

        auto v = vec2(10, 20);
        v /= 2.0f;
        REQUIRE(v == vec2(5, 10));

        v /= vec2(5, 2);
        REQUIRE(v == vec2(1, 5));
    }

    SECTION("aspect_ratio is x over y")
    {
        REQUIRE(vec2(16, 9).aspect_ratio() == Approx(16.0f / 9.0f));
        REQUIRE(vec2(1, 1).aspect_ratio() == Approx(1.0f));
    }

    SECTION("the pair constructor converts its components")
    {
        const vec2 fromFloats(std::make_pair(1.5f, 2.5f));
        REQUIRE(fromFloats == vec2(1.5f, 2.5f));

        const vec2 fromInts(std::make_pair(3, 4));
        REQUIRE(fromInts == vec2(3, 4));

        const vec2 fromDoubles(std::make_pair(0.25, 0.75));
        REQUIRE(fromDoubles == vec2(0.25f, 0.75f));
    }
}

TEMPLATE_LIST_TEST_CASE("gdk::vector2 has no precision_type parameter", "[vector2]", type::floating_point)
{
    using vec2 = vector2<TestType>;
    using vec3 = vector3<TestType>;

    const vec2 v(3, 4);

    static_assert(std::is_same<decltype(v.length()), TestType>::value,
        "length returns the component type");
    static_assert(std::is_same<decltype(v.distance_from(v)), TestType>::value,
        "distance_from returns the component type");
    static_assert(std::is_same<decltype(v.aspect_ratio()), TestType>::value,
        "aspect_ratio returns the component type");
    static_assert(std::is_same<decltype(v.dot_product(v)), TestType>::value,
        "dot_product returns the component type");
    static_assert(std::is_same<decltype(v.cross_product(v)), TestType>::value,
        "cross_product returns the component type");

    REQUIRE(v.length() == Approx(5.0f));
}
