// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/math.h>

#include <cmath>

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
    [[nodiscard]] quaternion<T> a_rotation() {
        return quaternion<T>::from_euler({0.3f, 0.6f, 0.4f});
    }

    template<typename T>
    [[nodiscard]] T component(const vector4<T> &aVector, const std::size_t aIndex) {
        switch (aIndex) {
            case 0: return aVector.x;
            case 1: return aVector.y;
            case 2: return aVector.z;
            default: return aVector.w;
        }
    }
}

TEMPLATE_LIST_TEST_CASE("gdk::math_ops matrix4x4 * vector4", "[math_ops]", type::floating_point)
{
    using vec3 = vector3<TestType>;
    using vec4 = vector4<TestType>;
    using mat = matrix4x4<TestType>;
    using quat = quaternion<TestType>;

    SECTION("it is the product a shader computes, M * v")
    {
        mat m;
        m.set_to_identity();

        for (std::size_t column = 0; column < mat::order; ++column)
            for (std::size_t row = 0; row < mat::order; ++row)
                m.set(column, row, static_cast<float>(column * mat::order + row));

        const vec4 v(1, 2, 3, 4);
        const auto result = m * v;

        for (std::size_t row = 0; row < mat::order; ++row) {
            float expected = 0;
            for (std::size_t column = 0; column < mat::order; ++column)
                expected += m.get(column, row) * component(v, column);

            REQUIRE(component(result, row) == Approx(expected));
        }
    }

    SECTION("the identity leaves a vector alone")
    {
        const vec4 v(1, -2, 3, 1);
        const auto result = mat::identity * v;

        REQUIRE(result.x == Approx(1.0f));
        REQUIRE(result.y == Approx(-2.0f));
        REQUIRE(result.z == Approx(3.0f));
        REQUIRE(result.w == Approx(1.0f));
    }
}

TEMPLATE_LIST_TEST_CASE("gdk::math_ops matrix4x4 * vector3", "[math_ops]", type::floating_point)
{
    using vec3 = vector3<TestType>;
    using vec4 = vector4<TestType>;
    using mat = matrix4x4<TestType>;
    using quat = quaternion<TestType>;

    SECTION("it agrees with the vector4 product extended by w = 1")
    {
        mat m;
        m.set_translation({4, 5, 6});
        m.set_rotation(a_rotation<TestType>());

        const vec3 point(1, -2, 3);

        const auto viaVector3 = m * point;
        const auto viaVector4 = m * vec4(point.x, point.y, point.z, 1);

        require_near(viaVector3, {viaVector4.x, viaVector4.y, viaVector4.z});
    }

    SECTION("a point is translated")
    {
        mat m;
        m.set_to_identity();
        m.set_translation({10, 20, 30});

        require_near(m * vec3(1, 2, 3), {11, 22, 33});
    }

    SECTION("a projection divides through by w")
    {
        mat m;
        m.set_to_identity();

        m.set(2, 3, -1.0f);   
        m.set(3, 3, 0.0f);   

        require_near(m * vec3(8, 4, -4), {2, 1, -1});
    }

    SECTION("the identity leaves a point alone")
    {
        require_near(mat::identity * vec3(1, -2, 3), {1, -2, 3});
    }
}

TEMPLATE_LIST_TEST_CASE("gdk::math_ops quaternion * vector3", "[math_ops]", type::floating_point)
{
    using vec3 = vector3<TestType>;
    using vec4 = vector4<TestType>;
    using mat = matrix4x4<TestType>;
    using quat = quaternion<TestType>;

    SECTION("a quarter turn about +Z sends +X to +Y")
    {
        const auto q = quat::from_euler({0, 0, QUARTER_TURN});

        require_near(q * vec3::right, vec3::up);
        require_near(q * vec3::up, vec3::right * -1.0f);
    }

    SECTION("it agrees with rotating by the equivalent matrix")
    {
        const auto q = a_rotation<TestType>();

        mat m;
        m.set_rotation(q);

        for (const auto &v : {vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1), vec3(1, -2, 3)})
            require_near(q * v, m * v);
    }

    SECTION("the identity rotation leaves a vector alone")
    {
        require_near(quat::identity * vec3(1, -2, 3), {1, -2, 3});
    }

    SECTION("it preserves length")
    {
        const auto q = a_rotation<TestType>();
        const vec3 v(1, -2, 3);

        REQUIRE((q * v).length() == Approx(v.length()));
    }

    SECTION("it carries no translation, unlike the matrix product")
    {
        const auto q = a_rotation<TestType>();

        mat m;
        m.set_translation({4, 5, 6});
        m.set_rotation(q);

        const vec3 direction(0, 0, -1);

        require_near(q * direction, m * direction - vec3(4, 5, 6));
    }

    SECTION("composing rotations composes their action on a vector")
    {
        const auto a = quat::from_euler({0, 0, QUARTER_TURN});
        const auto b = quat::from_euler({QUARTER_TURN, 0, 0});

        const vec3 v(1, -2, 3);

        require_near((a * b) * v, a * (b * v));

        REQUIRE_FALSE((a * b) * v == (b * a) * v);
    }

    SECTION("the inverse rotation undoes it")
    {
        const auto q = a_rotation<TestType>();
        const vec3 v(1, -2, 3);

        require_near(q.inverse_unit() * (q * v), v);
    }
}
