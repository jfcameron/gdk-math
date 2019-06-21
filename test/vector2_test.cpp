// © 2018 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/vector2.h>

using namespace gdk;

using test_types = type::signed_arithmetic;

TEMPLATE_LIST_TEST_CASE("vector 2 constructors", "[vector2]", test_types)
{
    using vector_type = Vector2<TestType>;

    SECTION("Default constructor produces a zero vector")
    {
        vector_type vector;

        REQUIRE(vector == vector_type::Zero);
    }

    SECTION("Copy constructor work as expected.")
    {
        vector_type vector(vector_type::One);

        REQUIRE(vector == vector_type::One);
    }

    SECTION("Move constructor work as expected.")
    {
        REQUIRE(vector_type(vector_type()) == vector_type::Zero);
    }

    SECTION("Construct from a single component")
    {
        REQUIRE(vector_type({1}) == vector_type::One);
    }

    SECTION("Construct from two components")
    {
        REQUIRE(vector_type({0}, {1}) == vector_type::Up);
    }
}

TEMPLATE_LIST_TEST_CASE("vector2 mutating operators and methods", "[vector2]", test_types)
{
    using vector_type = Vector2<TestType>;

    SECTION("normalize")
    {
        vector_type a = vector_type::Up * 10;

        REQUIRE(a.normalize() == vector_type::Up);
    }

    SECTION("assignment operator")
    {
        vector_type a = vector_type::Down;

        REQUIRE(a == vector_type::Down);
    }

    SECTION("*= operator")
    {
        vector_type a = vector_type::Right;

        a *= 5;

        REQUIRE(a == vector_type(5, 0));
    }

    SECTION("-= operator")
    {
        vector_type a = vector_type::Right;

        a -= vector_type::Right;

        REQUIRE(a == vector_type::Zero);
    }
}

TEMPLATE_LIST_TEST_CASE("vector2 non-mutating operators and methods", "[vector2]", test_types)
{
    using vector_type = Vector2<TestType>;

    SECTION("length")
    {
        const vector_type a = vector_type::Up;

        REQUIRE(a.y == 1);
    }

    SECTION("getAspectRatio")
    {
        const vector_type a = vector_type(5, 3);

        REQUIRE((5./3.) == a.getAspectRatio(double()));
    }

    SECTION("operator==")
    {
        const vector_type a, b;

        REQUIRE(a == b);
    }
    SECTION("operator!=")
    {
        const vector_type a(0, 1), b(2, 3);

        REQUIRE(a != b);
    }
    SECTION("operator+")
    {
        const vector_type vector = vector_type::Up + vector_type::Right;

        REQUIRE(vector == vector_type::One);
    }
    SECTION("operator-")
    {
        const vector_type vector = vector_type::One - vector_type::Right;

        REQUIRE(vector == vector_type::Up);
    }
    SECTION("operator*")
    {
        const vector_type vector = vector_type::One;

        REQUIRE( vector * 2 == vector_type(2, 2));
    }
}

