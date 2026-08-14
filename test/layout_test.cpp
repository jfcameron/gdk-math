// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/math.h>

#include <cstddef>
#include <type_traits>

using namespace gdk;

TEMPLATE_LIST_TEST_CASE("gdk-math std backend: layout is unchanged by the storage split", "[layout]", type::floating_point)
{
    SECTION("no padding is introduced")
    {
        REQUIRE(sizeof(vector2<TestType>) == 2 * sizeof(TestType));
        REQUIRE(sizeof(vector3<TestType>) == 3 * sizeof(TestType));
        REQUIRE(sizeof(vector4<TestType>) == 4 * sizeof(TestType));
        REQUIRE(sizeof(quaternion<TestType>) == 4 * sizeof(TestType));
        REQUIRE(sizeof(matrix4x4<TestType>) == 16 * sizeof(TestType));
    }

    SECTION("the types stay standard layout and trivially copyable")
    {
        REQUIRE(std::is_standard_layout<vector2<TestType>>::value);
        REQUIRE(std::is_standard_layout<vector3<TestType>>::value);
        REQUIRE(std::is_standard_layout<vector4<TestType>>::value);
        REQUIRE(std::is_standard_layout<quaternion<TestType>>::value);
        REQUIRE(std::is_standard_layout<matrix4x4<TestType>>::value);

        REQUIRE(std::is_trivially_copyable<vector3<TestType>>::value);
        REQUIRE(std::is_trivially_copyable<quaternion<TestType>>::value);
        REQUIRE(std::is_trivially_copyable<matrix4x4<TestType>>::value);
    }

    SECTION("the default values survive the move into the storage base")
    {
        REQUIRE(vector3<TestType>() == vector3<TestType>(0, 0, 0));

        const vector4<TestType> v;
        REQUIRE(v.w == Approx(1.0));

        REQUIRE(quaternion<TestType>() == quaternion<TestType>::identity);
    }
}

TEMPLATE_LIST_TEST_CASE("gdk-math: matrix4x4 storage is contiguous and column major", "[layout]", type::floating_point)
{
    matrix4x4<TestType> m;
    m.set_to_identity();

    for (std::size_t column = 0; column < matrix4x4<TestType>::order; ++column)
        for (std::size_t row = 0; row < matrix4x4<TestType>::order; ++row)
            m.set(column, row, static_cast<TestType>(column * matrix4x4<TestType>::order + row));

    const TestType *const raw = &m.front();

    for (std::size_t k = 0; k < 16; ++k) {
        REQUIRE(raw[k] == Approx(static_cast<TestType>(k)));
        REQUIRE(raw[k] == Approx(m.get(k / matrix4x4<TestType>::order, k % matrix4x4<TestType>::order)));
    }
}

TEMPLATE_LIST_TEST_CASE("gdk-math: order remains part of the public interface", "[layout]", type::floating_point)
{
    static_assert(matrix4x4<TestType>::order == 4, "matrix4x4 is 4x4");

    REQUIRE(matrix4x4<TestType>::order == 4);
}
