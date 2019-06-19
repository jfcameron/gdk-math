// © 2018 Joseph Cameron - All Rights Reserved

#include <iostream>
#include <sstream>
#include <string>
#include <tuple>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/vector3.h>

using namespace gdk;

TEMPLATE_LIST_TEST_CASE("vector3 constructors", "[vector3]", type::floating_point)
{
    using vector_type = Vector3<TestType>;

    SECTION("Default constructor produces a zero vector")
    {
        vector_type vector;

        REQUIRE(vector == vector_type::Zero);
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

        REQUIRE(a == vector_type(4, 5, 6));
    }
}

TEMPLATE_LIST_TEST_CASE("vector3 operators", "[vector3]", type::floating_point)
{
    using vector_type = Vector3<TestType>;

    SECTION("operator<< serializes to JSON")
    {
        std::stringstream ss;

        ss << vector_type::Zero;

        REQUIRE(ss.str() == "{x: 0, y: 0, z: 0}");
    }
}

