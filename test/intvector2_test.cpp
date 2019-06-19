// © 2018 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/intvector2.h>

using namespace gdk;

TEMPLATE_LIST_TEST_CASE("intvector2 constructors", "[intvector2]", type::integral)
{
    using vector_type = IntVector2<TestType>;

    SECTION("Default constructor produces a zero vector")
    {
        vector_type vector;

        REQUIRE(vector == vector_type::Zero);
    }
}

