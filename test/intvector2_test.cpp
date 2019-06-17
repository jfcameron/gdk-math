// © 2018 Joseph Cameron - All Rights Reserved

#include <string>

#include <catch.hpp>

#include <gdk/intvector2.h>

TEST_CASE("intvector2 constructors", "[intvector2]" )
{
    using namespace gdk;
    using component_type = int;
    using vector_type = IntVector2<component_type>;

    SECTION("Default constructor produces a zero vector")
    {
        vector_type vector;

        REQUIRE(vector == vector_type::Zero);
    }
}

