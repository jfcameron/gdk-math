// © 2018 Joseph Cameron - All Rights Reserved

#include <string>

#include <catch.hpp>

#include <gdk/vector4.h>

#include <iostream>

using namespace gdk;
using component_type = double long;
using vector_type = Vector4<component_type>;

TEST_CASE("vector4 constructors", "[vector4]" )
{
    SECTION("Default constructor produces a zero vector")
    {
        vector_type vector(0, 0, 0, 0);

        REQUIRE(vector == vector_type::Zero);
    }
}

