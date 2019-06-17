// © 2018 Joseph Cameron - All Rights Reserved

#include <string>

#include <catch.hpp>

#include <gdk/vector3.h>

#include <iostream>

using namespace gdk;
using component_type = double long;
using vector_type = Vector3<component_type>;

TEST_CASE("vector3 constructors", "[vector3]" )
{
    SECTION("Default constructor produces a zero vector")
    {
        vector_type vector;

        std::cout << vector << std::endl;

        REQUIRE(vector == vector_type::Zero);
    }
}

