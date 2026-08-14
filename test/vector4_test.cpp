// © 2018 Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/math.h>

#include <cmath>
#include <type_traits>

using namespace gdk;

TEMPLATE_LIST_TEST_CASE("vector4 constructors", "[vector4]", type::floating_point)
{
    using vec4 = vector4<TestType>;
    using vec3 = vector3<TestType>;
    using mat4 = matrix4x4<TestType>;

    SECTION("origin agrees with the default constructor")
    {
        REQUIRE(vec4() == vec4::origin);
        REQUIRE(vec4::origin.w == Approx(1.0f));

        REQUIRE(vec4(0, 0, 0, 0).w == Approx(0.0f));
        REQUIRE(vec4(0, 0, 0, 0) != vec4::origin);
    }

    SECTION("the default is the origin as a point, with w = 1")
    {
        const vec4 v;

        REQUIRE(v.x == Approx(0.0f));
        REQUIRE(v.y == Approx(0.0f));
        REQUIRE(v.z == Approx(0.0f));
        REQUIRE(v.w == Approx(1.0f));
    }

    SECTION("w defaults to 1 when only xyz are given")
    {
        const vec4 v(1, 2, 3);

        REQUIRE(v.z == Approx(3.0f));
        REQUIRE(v.w == Approx(1.0f));
    }

    SECTION("w can be given explicitly, including 0 for a direction")
    {
        const vec4 direction(1, 2, 3, 0);

        REQUIRE(direction.w == Approx(0.0f));
    }

    SECTION("constructing from a vector3 carries xyz and defaults w to 1")
    {
        const vec4 fromThree(vec3(4, 5, 6));

        REQUIRE(fromThree.x == Approx(4.0f));
        REQUIRE(fromThree.y == Approx(5.0f));
        REQUIRE(fromThree.z == Approx(6.0f));
        REQUIRE(fromThree.w == Approx(1.0f));

        const vec4 asDirection(vec3(4, 5, 6), 0.0f);
        REQUIRE(asDirection.w == Approx(0.0f));
    }
}

TEMPLATE_LIST_TEST_CASE("vector4 operators", "[vector4]", type::floating_point)
{
    using vec4 = vector4<TestType>;
    using vec3 = vector3<TestType>;
    using mat4 = matrix4x4<TestType>;

    SECTION("it has no arithmetic, deliberately")
    {
        const auto lifted = vec4(vec3(1, 2, 3) + vec3(1, 1, 1));

        REQUIRE(lifted.x == Approx(2.0f));
        REQUIRE(lifted.w == Approx(1.0f));  
    }

    SECTION("equality is exact, component-wise, and includes w")
    {
        REQUIRE(vec4(1, 2, 3, 1) == vec4(1, 2, 3, 1));

        REQUIRE(vec4(1, 2, 3, 1) != vec4(1, 2, 3, 0));
    }
}

TEMPLATE_LIST_TEST_CASE("vector4 homogeneous operations", "[vector4]", type::floating_point)
{
    using vec4 = vector4<TestType>;
    using vec3 = vector3<TestType>;
    using mat4 = matrix4x4<TestType>;

    SECTION("xyz extracts the three components without dividing")
    {
        REQUIRE(vec4(1, 2, 3, 4).xyz() == vec3(1, 2, 3));
        REQUIRE(vec4(1, 2, 3, 0).xyz() == vec3(1, 2, 3));
    }

    SECTION("to_point divides through by w")
    {
        REQUIRE(vec4(8, 4, -4, 4).to_point() == vec3(2, 1, -1));
        REQUIRE(vec4(1, 2, 3, 1).to_point() == vec3(1, 2, 3));
    }

    SECTION("**a point with w = 1 is unchanged, so the two agree there**")
    {
        const vec4 affineResult(5, -6, 7, 1);

        REQUIRE(affineResult.xyz() == affineResult.to_point());
    }

    SECTION("**a projected point is where the two differ, and by the whole projection**")
    {
        const vec4 clipSpace(8, 4, -4, 4);

        REQUIRE(clipSpace.xyz() != clipSpace.to_point());
        REQUIRE(clipSpace.xyz() == vec3(8, 4, -4));
    }

    SECTION("w = 0 returns xyz undivided rather than dividing by zero")
    {
        const auto d = vec4::direction(vec3(1, 2, 3));

        REQUIRE(d.to_point() == vec3(1, 2, 3));
        REQUIRE(std::isfinite(d.to_point().x));
    }

    SECTION("direction gives w = 0, the promoting constructor gives w = 1")
    {
        const vec3 v(1, 2, 3);

        REQUIRE(vec4::direction(v).w == Approx(0.0f));
        REQUIRE(vec4(v).w == Approx(1.0f));

        REQUIRE(vec4::direction(v).xyz() == v);
        REQUIRE(vec4(v).xyz() == v);

        REQUIRE(vec4::direction(v) != vec4(v));
    }

    SECTION("a direction is not translated by a transform, a point is")
    {
        mat4 m;
        m.set_to_identity();
        m.set_translation({10, 20, 30});

        const vec3 v(1, 2, 3);

        REQUIRE((m * vec4(v)).to_point() == vec3(11, 22, 33));
        REQUIRE((m * vec4::direction(v)).to_point() == v);
    }
}
