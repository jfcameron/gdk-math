// © 2018 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/quaternion.h>

#include <iostream>

using namespace gdk;
using component_type = double long;
using quaternion_type = Quaternion<component_type>;

TEMPLATE_LIST_TEST_CASE("quaternion constructors", "[quaternion]", type::floating_point)
{
    SECTION("Default constructor produces an identity quaternion")
    {
        quaternion_type quaternion;

        REQUIRE(quaternion == quaternion_type::Identity);
    }
}

