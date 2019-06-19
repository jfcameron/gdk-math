// © 2018 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/mat4x4.h>

#include <iostream>

using namespace gdk;
using component_type = double long;
using mat_type = Mat4x4<component_type>;

TEMPLATE_LIST_TEST_CASE("mat4x4 constructors", "[mat4x4]", type::floating_point)
{
    SECTION("Default constructor produces an identity matrix")
    {
        mat_type mat;

        REQUIRE(mat == mat_type::Identity);
    }
}

TEMPLATE_LIST_TEST_CASE("mat4x4 operators", "[mat4x4]", type::floating_point)
{
    SECTION("equality operator")
    {
        mat_type a, b;

        REQUIRE(a == b);
    }
}

