// © 2018 Joseph Cameron - All Rights Reserved

#include <string>

#include <catch.hpp>

#include <gdk/mat4x4.h>

#include <iostream>

using namespace gdk;
using component_type = double long;
using mat_type = Mat4x4<component_type>;

TEST_CASE("mat4x4 constructors", "[mat4x4]" )
{
    SECTION("Default constructor produces an identity matrix")
    {
        mat_type mat;

        REQUIRE(mat == mat_type::Identity);
    }
}

TEST_CASE("mat4x4 operators", "[mat4x4]")
{
    SECTION("equality operator")
    {
        mat_type a, b;

        REQUIRE(a == b);
    }
}

