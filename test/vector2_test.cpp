// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/vector2.h>
#include <gdk/vector3.h>
#include <gdk/vector4.h>

#include <cmath>

using namespace gdk;

TEMPLATE_LIST_TEST_CASE("vector2 constructors and constants", "[vector2]", type::floating_point)
{
    using vec = vector2<TestType>;

    SECTION("the default is zero")
    {
        REQUIRE(vec() == vec::zero);
    }

    SECTION("the named directions are the axes they claim")
    {
        REQUIRE(vec::right == vec{1, 0});
        REQUIRE(vec::left == vec{-1, 0});
        REQUIRE(vec::up == vec{0, 1});
        REQUIRE(vec::down == vec{0, -1});
        REQUIRE(vec::one == vec{1, 1});
        REQUIRE(vec::zero == vec{0, 0});
    }

    SECTION("the constants are named the same way across vector2, vector3 and vector4")
    {
        REQUIRE(vector2<float>::zero == vector2<float>{0, 0});
        REQUIRE(vector3<float>::zero == vector3<float>(0, 0, 0));
        REQUIRE(vector4<float>::origin == vector4<float>());

        REQUIRE(vector2<float>::up.y == Approx(vector3<float>::up.y));
        REQUIRE(vector2<float>::down.y == Approx(vector3<float>::down.y));
        REQUIRE(vector2<float>::left.x == Approx(vector3<float>::left.x));
        REQUIRE(vector2<float>::right.x == Approx(vector3<float>::right.x));
        REQUIRE(vector2<float>::one.x == Approx(vector3<float>::one.x));
    }
}

TEMPLATE_LIST_TEST_CASE("vector2 magnitude", "[vector2]", type::floating_point)
{
    using vec = vector2<TestType>;

    SECTION("length is the euclidean length")
    {
        REQUIRE(vec{3, 4}.length() == Approx(5.0f));
        REQUIRE(vec::zero.length() == Approx(0.0f));
    }

    SECTION("distance_from is the length of the difference, and is symmetric")
    {
        const vec a{1, 2};
        const vec b{4, 6};

        REQUIRE(a.distance_from(b) == Approx(5.0f));
        REQUIRE(b.distance_from(a) == Approx(5.0f));
        REQUIRE(a.distance_from(a) == Approx(0.0f));
    }

    SECTION("aspect_ratio is x over y")
    {
        REQUIRE(vec{16, 9}.aspect_ratio() == Approx(16.0f / 9.0f));
        REQUIRE(vec{1, 1}.aspect_ratio() == Approx(1.0f));
    }

    SECTION("normalize gives unit length and returns itself")
    {
        vec v{3, 4};
        const auto &returned = v.normalize();

        REQUIRE(v.length() == Approx(1.0f));
        REQUIRE(v.x == Approx(0.6f));
        REQUIRE(v.y == Approx(0.8f));
        REQUIRE(&returned == &v);
    }

    SECTION("normalizing zero leaves it zero rather than dividing by zero")
    {
        vec zero = vec::zero;
        zero.normalize();

        REQUIRE(std::isfinite(zero.x));
        REQUIRE(zero == vec::zero);
    }
}

TEMPLATE_LIST_TEST_CASE("vector2 products", "[vector2]", type::floating_point)
{
    using vec = vector2<TestType>;

    SECTION("dot_product is commutative, and zero for perpendicular vectors")
    {
        const vec a{1, 2};
        const vec b{3, 4};

        REQUIRE(a.dot_product(b) == Approx(11.0f));   // 1*3 + 2*4
        REQUIRE(b.dot_product(a) == Approx(11.0f));
        REQUIRE(vec::right.dot_product(vec::up) == Approx(0.0f));
        REQUIRE(vec::right.dot_product(vec::left) == Approx(-1.0f));
    }
}

TEMPLATE_LIST_TEST_CASE("vector2 operators", "[vector2]", type::floating_point)
{
    using vec = vector2<TestType>;

    SECTION("arithmetic returns new values and leaves the operands alone")
    {
        const vec a{1, 2};
        const vec b{3, 5};

        REQUIRE(a + b == vec{4, 7});
        REQUIRE(b - a == vec{2, 3});
        REQUIRE(a * 3.0f == vec{3, 6});
        REQUIRE(a == vec{1, 2});   // unchanged
    }

    SECTION("compound assignment")
    {
        vec v{1, 2};

        v += vec{1, 1};
        REQUIRE(v == vec{2, 3});

        v -= vec{1, 1};
        REQUIRE(v == vec{1, 2});

        v *= 4.0f;
        REQUIRE(v == vec{4, 8});
    }

    SECTION("equality is exact, component-wise")
    {
        REQUIRE(vec{1, 2} == vec{1, 2});
        REQUIRE(vec{1, 2} != vec{1, 2.0001f});
    }
}
