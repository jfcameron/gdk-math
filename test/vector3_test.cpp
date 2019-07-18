// © 2018 Joseph Cameron - All Rights Reserved

#include <iostream>
#include <sstream>
#include <string>
#include <tuple>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/vector3.h>

using namespace gdk;

using test_types = type::floating_point; //signed_arithmetic;

TEMPLATE_LIST_TEST_CASE("vector3 constructors", "[vector3]", test_types)
{
    using vector_type = Vector3<TestType>;

    SECTION("Default constructor produces a zero vector")
    {
        vector_type vector;

        REQUIRE(vector == vector_type::Zero);
    }

    SECTION("three component ctor produces a vector with expected component values")
    {
        vector_type vector(10, 10, 10);

        REQUIRE(vector == vector_type::One * 10);
    }

    SECTION("Copy constructor produces a copy")
    {
        vector_type vector(vector_type::One);

        REQUIRE(vector == vector_type::One);
    }
    
    SECTION("Move constructor accepts rvalue reference & explicit move semantics")
    {
        vector_type vector(vector_type(1, 2, 3));

        REQUIRE(vector == vector_type(1, 2, 3));

        vector_type a(4, 5, 6);
        vector_type b(std::move(a));

        REQUIRE(b == vector_type(4, 5, 6));
    }
}

TEMPLATE_LIST_TEST_CASE("vector3 mutating operators and methods", "[vector3]", test_types)
{
    using vector_type = Vector3<TestType>;
    
    SECTION("normalize")
    {
        vector_type vector(vector_type::Backward * 25);

        REQUIRE(vector.normalize() == vector_type::Backward);
    }

    SECTION("operator+=")
    {
        vector_type vector(vector_type::Up);

        REQUIRE((vector += vector_type::Right) == (vector_type::One + vector_type::Backward));
    }

    SECTION("operator-="){}
    SECTION("operator*="){}
    SECTION("operator="){}
}

TEMPLATE_LIST_TEST_CASE("vector3 non-mutating operators and methods", "[vector3]", test_types)
{
    using vector_type = Vector3<TestType>;

    SECTION("operator<< serializes to JSON")
    {
        std::stringstream ss;

        ss << vector_type::Zero;

        REQUIRE(ss.str() == "{x: 0, y: 0, z: 0}");
    }
    
    SECTION("length")
    {
        vector_type vector = vector_type::Forward;

        REQUIRE(vector.length() == 1);
    }

    SECTION("operator==")
    {
        vector_type vector = vector_type::Backward;

        REQUIRE(vector == vector_type::Backward);
    }

    SECTION("operator!=")
    {
        vector_type vector(vector_type::Right);

        REQUIRE(vector != vector_type::Left);
    }

    SECTION("operator+")
    {
        vector_type a(5, 0, 0), b(3, 0, 0);

        REQUIRE(a + b == vector_type::Right * 8);
    }

    SECTION("operator-")
    {
        REQUIRE(vector_type::Up - vector_type::Up == vector_type::Zero);
    }

    SECTION("operator*")
    {
        REQUIRE(vector_type::Down * 10 == vector_type(0, -10, 0));
    }
}

