// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/math.h>

#include <cmath>
#include <stdexcept>

using namespace gdk;

namespace {
    constexpr float QUARTER_TURN = to_radians(90.0f);

    template<typename T>
    void require_near(const vector3<T> &a, const vector3<T> &b, const double aMargin = 1e-5) {
        REQUIRE(a.x == Approx(b.x).margin(aMargin));
        REQUIRE(a.y == Approx(b.y).margin(aMargin));
        REQUIRE(a.z == Approx(b.z).margin(aMargin));
    }

    template<typename T>
    void require_near(const matrix3x3<T> &a, const matrix3x3<T> &b, const double aMargin = 1e-4) {
        for (std::size_t c = 0; c < matrix3x3<T>::order; ++c)
            for (std::size_t r = 0; r < matrix3x3<T>::order; ++r)
                REQUIRE(a.get(c, r) == Approx(b.get(c, r)).margin(aMargin));
    }
}

TEMPLATE_LIST_TEST_CASE("gdk::matrix3x3 storage and construction", "[mat3x3]", type::floating_point)
{
    using mat3 = matrix3x3<TestType>;
    using mat4 = matrix4x4<TestType>;
    using vec = vector3<TestType>;
    using quat = quaternion<TestType>;

    SECTION("the default is the identity")
    {
        REQUIRE(mat3() == mat3::identity);
        REQUIRE(mat3::order == 3);
    }

    SECTION("set and get take the column first, the row second")
    {
        mat3 m;
        m.set_to_identity();
        m.set(2, 1, 7.0f);

        REQUIRE(m.get(2, 1) == Approx(7.0f));
        REQUIRE(m.get(1, 2) == Approx(0.0f));
    }

    SECTION("the raw array is nine contiguous components in the order gl reads")
    {
        mat3 m;

        for (std::size_t column = 0; column < mat3::order; ++column)
            for (std::size_t row = 0; row < mat3::order; ++row)
                m.set(column, row, static_cast<TestType>(column * mat3::order + row));

        const TestType *const raw = &m.front();

        for (std::size_t k = 0; k < 9; ++k) {
            REQUIRE(raw[k] == Approx(static_cast<TestType>(k)));
            REQUIRE(raw[k] == Approx(m.get(k / mat3::order, k % mat3::order)));
        }
    }

    SECTION("the nine component constructor fills it in storage order")
    {
        const mat3 a(0, 1, 2,
                     3, 4, 5,
                     6, 7, 8);

        for (std::size_t k = 0; k < 9; ++k)
            REQUIRE(a.get(k / mat3::order, k % mat3::order) == Approx(static_cast<TestType>(k)));
    }
}

TEMPLATE_LIST_TEST_CASE("gdk::matrix3x3 arithmetic", "[mat3x3]", type::floating_point)
{
    using mat3 = matrix3x3<TestType>;
    using mat4 = matrix4x4<TestType>;
    using vec = vector3<TestType>;
    using quat = quaternion<TestType>;

    SECTION("multiplying by the identity changes nothing")
    {
        const mat3 a(1, 2, 3, 4, 5, 6, 7, 8, 10);

        REQUIRE(a * mat3::identity == a);
        REQUIRE(mat3::identity * a == a);
    }

    SECTION("composition applies the rightmost factor first")
    {
        const auto scaleThenNothing = mat3(2, 0, 0, 0, 3, 0, 0, 0, 4);

        mat4 rotation4;
        rotation4.set_rotation(quat::from_angle_axis(QUARTER_TURN, vec::up));
        const auto rotation = upper_left(rotation4);

        const vec v(1, 1, 1);

        require_near((rotation * scaleThenNothing) * v, rotation * (scaleThenNothing * v));
    }

    SECTION("transpose swaps rows and columns and is its own inverse")
    {
        const mat3 a(1, 2, 3, 4, 5, 6, 7, 8, 10);

        REQUIRE(a.transposed().get(0, 1) == Approx(a.get(1, 0)));
        REQUIRE(a.transposed().transposed() == a);
    }

    SECTION("the determinant matches a hand computation")
    {
        // | 1 2 3 |
        // | 4 5 6 |  = 1(5*10 - 6*8) - 2(4*10 - 6*7) + 3(4*8 - 5*7) = 2 + 4 - 9 = -3
        // | 7 8 10|
        const mat3 rowMajor(1, 4, 7,   // column 0
                            2, 5, 8,   // column 1
                            3, 6, 10); // column 2

        REQUIRE(rowMajor.determinant() == Approx(-3.0f));
        REQUIRE(mat3::identity.determinant() == Approx(1.0f));
    }

    SECTION("a matrix times its inverse is the identity")
    {
        mat3 a(2, 0, 0,
               0, 3, 0,
               1, 2, 4);

        require_near(a * a.inversed(), mat3::identity);
        require_near(a.inversed() * a, mat3::identity);
    }

    SECTION("inverting a singular matrix throws rather than producing NaN")
    {
        mat3 singular(1, 2, 3,
                      2, 4, 6,   
                      1, 1, 1);

        REQUIRE(singular.determinant() == Approx(0.0f));
        REQUIRE_THROWS_AS(singular.inverse(), std::domain_error);
        REQUIRE_THROWS_AS(mat3(0, 0, 0, 0, 0, 0, 0, 0, 0).inversed(), std::domain_error);
    }
}

TEMPLATE_LIST_TEST_CASE("gdk::upper_left", "[mat3x3][math_ops]", type::floating_point)
{
    using mat3 = matrix3x3<TestType>;
    using mat4 = matrix4x4<TestType>;
    using vec = vector3<TestType>;
    using quat = quaternion<TestType>;

    SECTION("it takes the linear part and drops the translation")
    {
        mat4 m;
        m.set_to_identity();
        m.set_translation({10, 20, 30});
        m.set_rotation(quat::from_angle_axis(0.7f, vec(1, 2, 3).normal()));

        const auto linear = upper_left(m);

        for (std::size_t c = 0; c < mat3::order; ++c)
            for (std::size_t r = 0; r < mat3::order; ++r)
                REQUIRE(linear.get(c, r) == Approx(m.get(c, r)));
    }

    SECTION("a direction transformed by it is the 4x4's rotation without the offset")
    {
        mat4 m;
        m.set_to_identity();
        m.set_translation({10, 20, 30});
        m.set_rotation(quat::from_angle_axis(QUARTER_TURN, vec::up));

        const vec direction(0, 0, -1);

        require_near(upper_left(m) * direction, m * direction - vec(10, 20, 30));
    }
}

TEMPLATE_LIST_TEST_CASE("gdk::normal_matrix", "[mat3x3][math_ops]", type::floating_point)
{
    using mat3 = matrix3x3<TestType>;
    using mat4 = matrix4x4<TestType>;
    using vec = vector3<TestType>;
    using quat = quaternion<TestType>;

    SECTION("for a pure rotation it is the rotation itself")
    {
        mat4 m;
        m.set_rotation(quat::from_angle_axis(0.7f, vec(1, 2, 3).normal()));

        require_near(normal_matrix(m), upper_left(m));
    }

    SECTION("**under non-uniform scale it is not the model matrix, and the difference is the point**")
    {
        mat4 m;
        m.set_to_identity();
        m.set(1, 1, 0.25f); 

        const auto surfaceNormal = vec(1, 1, 0).normal();

        const auto wrong = (upper_left(m) * surfaceNormal).normal();
        const auto right = (normal_matrix(m) * surfaceNormal).normal();

        REQUIRE(std::abs(wrong.y - right.y) > 0.4f);

        const auto alongSurface = vec(1, -1, 0);            // perpendicular to (1,1,0)
        const auto transformedSurface = upper_left(m) * alongSurface;

        REQUIRE(right.dot_product(transformedSurface.normal()) == Approx(0.0f).margin(1e-5f));
        REQUIRE(std::abs(wrong.dot_product(transformedSurface.normal())) > 0.4f);
    }

    SECTION("it throws on a singular transform")
    {
        mat4 flattened;
        flattened.set_to_identity();
        flattened.set(2, 2, 0.0f);  

        REQUIRE_THROWS_AS(normal_matrix(flattened), std::domain_error);
    }
}

TEMPLATE_LIST_TEST_CASE("gdk::matrix3x3 storage layout", "[mat3x3][layout]", type::floating_point)
{
    using mat3 = matrix3x3<TestType>;
    using mat4 = matrix4x4<TestType>;
    using vec = vector3<TestType>;
    using quat = quaternion<TestType>;

    REQUIRE(sizeof(mat3) == 9 * sizeof(TestType));
    REQUIRE(std::is_standard_layout<mat3>::value);
    REQUIRE(std::is_trivially_copyable<mat3>::value);
}
