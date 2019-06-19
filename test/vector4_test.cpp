// © 2018 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/vector4.h>

#include <iostream>

using namespace gdk;
using component_type = double long;
using vector_type = Vector4<component_type>;

TEMPLATE_LIST_TEST_CASE("vector4 constructors", "[vector4]", type::floating_point)
{
    SECTION("Default constructor produces a zero vector")
    {
        vector_type vector(0, 0, 0, 0);

        REQUIRE(vector == vector_type::Zero);
    }
}

