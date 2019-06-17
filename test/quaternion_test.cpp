// © 2018 Joseph Cameron - All Rights Reserved

#include <string>

#include <catch.hpp>

#include <gdk/quaternion.h>

#include <iostream>

using namespace gdk;
using component_type = double long;
using quaternion_type = Quaternion<component_type>;

TEST_CASE("quaternion constructors", "[quaternion]" )
{
    SECTION("Default constructor produces an identity quaternion")
    {
        quaternion_type quaternion;

        REQUIRE(quaternion == quaternion_type::Identity);
    }
}

