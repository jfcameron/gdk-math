// © 2018 Joseph Cameron - All Rights Reserved

#include <string>

#include <catch.hpp>

#include <gdk/vector2.h>

using namespace gdk;
using component_type = long double;
using vector_type = Vector2<component_type>;

TEST_CASE("vector 2 constructors", "[vector2]" )
{

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

TEST_CASE("vector2 operators", "[vector2]" )
{

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
}

