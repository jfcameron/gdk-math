// © 2018 Joseph Cameron - All Rights Reserved

#include <string>

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/math.h>

#include <array>
#include <cmath>
#include <stdexcept>

using namespace gdk;

namespace {
    template<typename T>
    vector3<T> turn_by(const quaternion<T> &aQ, const vector3<T> &aV) {
        using vec_type = vector3<T>;
        const vec_type axis{aQ.x, aQ.y, aQ.z};
        const vec_type t{
            2 * (axis.y * aV.z - axis.z * aV.y),
            2 * (axis.z * aV.x - axis.x * aV.z),
            2 * (axis.x * aV.y - axis.y * aV.x)};

        return vec_type{
            aV.x + aQ.w * t.x + (axis.y * t.z - axis.z * t.y),
            aV.y + aQ.w * t.y + (axis.z * t.x - axis.x * t.z),
            aV.z + aQ.w * t.z + (axis.x * t.y - axis.y * t.x)};
    }

    template<typename T>
    vector3<T> as_a_shader_would(const matrix4x4<T> &aM, const vector3<T> &aV) {
        using vec_type = vector3<T>;
        const auto e = [&aM](const std::size_t aColumn, const std::size_t aRow) {
            return aM.get(aColumn, aRow);
        };

        return vec_type{
            e(0, 0) * aV.x + e(1, 0) * aV.y + e(2, 0) * aV.z + e(3, 0),
            e(0, 1) * aV.x + e(1, 1) * aV.y + e(2, 1) * aV.z + e(3, 1),
            e(0, 2) * aV.x + e(1, 2) * aV.y + e(2, 2) * aV.z + e(3, 2)};
    }

    template<typename T>
    T rotation_difference(const quaternion<T> &a, const quaternion<T> &b) {
        const auto plus = std::sqrt((a.w - b.w) * (a.w - b.w) + (a.x - b.x) * (a.x - b.x)
            + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
        const auto minus = std::sqrt((a.w + b.w) * (a.w + b.w) + (a.x + b.x) * (a.x + b.x)
            + (a.y + b.y) * (a.y + b.y) + (a.z + b.z) * (a.z + b.z));

        return std::min(plus, minus);
    }

    template<typename T>
    const std::array<vector3<T>, 5> EULERS = {vector3<T>
        {0.3f, 0.6f, 0.4f}, {0.05f, 0, 0}, {1.1f, 0.7f, 0.3f}, {0.9f, 1.3f, 2.1f}, {-0.4f, 2.2f, -1.1f}};

    template<typename T>
    const std::array<vector3<T>, 4> POINTS = {vector3<T>
        {0.9f, 0.3f, 0.5f}, {-0.2f, 1.7f, 0.4f}, {1, 0, 0}, {0, 0, 1}};
}

TEMPLATE_LIST_TEST_CASE("mat4x4 constructors", "[mat4x4]", type::floating_point)
{
    using component_type = TestType;
    using mat_type = matrix4x4<component_type>;
    using quat_type = quaternion<component_type>;
    using vec_type = vector3<component_type>;

    SECTION("Default constructor produces an identity matrix")
    {
        mat_type mat;

        REQUIRE(mat == mat_type::identity);
    }
}

TEMPLATE_LIST_TEST_CASE("mat4x4 operators", "[mat4x4]", type::floating_point)
{
    using component_type = TestType;
    using mat_type = matrix4x4<component_type>;
    using quat_type = quaternion<component_type>;
    using vec_type = vector3<component_type>;

    SECTION("equality operator")
    {
        mat_type a, b;

        REQUIRE(a == b);
    }
}

TEMPLATE_LIST_TEST_CASE("mat4x4 rotation", "[mat4x4]", type::floating_point)
{
    using component_type = TestType;
    using mat_type = matrix4x4<component_type>;
    using quat_type = quaternion<component_type>;
    using vec_type = vector3<component_type>;

    SECTION("set_rotation encodes the rotation a shader will apply")
    {
        for (const auto &euler : EULERS<component_type>) {
            quat_type q;
            q.set_from_euler(euler);

            mat_type m;
            m.set_rotation(q);

            for (const auto &p : POINTS<component_type>) {
                const auto expected = turn_by(q, p);
                const auto actual = as_a_shader_would(m, p);

                REQUIRE(actual.x == Approx(expected.x).margin(1e-5));
                REQUIRE(actual.y == Approx(expected.y).margin(1e-5));
                REQUIRE(actual.z == Approx(expected.z).margin(1e-5));
            }
        }
    }

    SECTION("rotation() is the inverse of set_rotation()")
    {
        for (const auto &euler : EULERS<component_type>) {
            quat_type q;
            q.set_from_euler(euler);

            mat_type m;
            m.set_rotation(q);

            REQUIRE(rotation_difference(m.rotation(), q) < 1e-5);
        }
    }

    SECTION("**decomposition refuses a mirrored transform rather than silently unmirroring it**")
    {
        mat_type mirrored;
        mirrored.set_rotation_and_scale(quat_type::identity, {-1, 1, 1});

        REQUIRE(mirrored.determinant() < 0);

        REQUIRE(mirrored.scale().x == Approx(1.0));

        REQUIRE_THROWS_AS(mirrored.rotation(), std::domain_error);
        REQUIRE_THROWS_AS(mirrored.set_scale({2, 2, 2}), std::domain_error);
        REQUIRE_THROWS_AS(mirrored.set_rotation(quat_type::identity), std::domain_error);

        mat_type ordinary;
        ordinary.set_rotation_and_scale(quat_type::identity, {2, 3, 4});

        REQUIRE(ordinary.determinant() > 0);
        REQUIRE_NOTHROW(ordinary.rotation());
        REQUIRE_NOTHROW(ordinary.set_rotation(quat_type::identity));

        mat_type twice;
        twice.set_rotation_and_scale(quat_type::identity, {-1, -1, 1});

        REQUIRE(twice.determinant() > 0);
        REQUIRE_NOTHROW(twice.rotation());
    }

    SECTION("set_scale preserves the rotation")
    {
        for (const auto &euler : EULERS<component_type>) {
            quat_type q;
            q.set_from_euler(euler);

            mat_type m;
            m.set_rotation(q);
            m.set_scale({2.0f, 3.0f, 4.0f});

            const auto scale = m.scale();
            REQUIRE(scale.x == Approx(2.0f).margin(1e-4));
            REQUIRE(scale.y == Approx(3.0f).margin(1e-4));
            REQUIRE(scale.z == Approx(4.0f).margin(1e-4));

            REQUIRE(rotation_difference(m.rotation(), q) < 1e-4);
        }
    }

    SECTION("the vector multiply operators agree with the matrix")
    {
        for (const auto &euler : EULERS<component_type>) {
            quat_type q;
            q.set_from_euler(euler);

            mat_type m;
            m.set_rotation(q);

            for (const auto &p : POINTS<component_type>) {
                const auto expected = turn_by(q, p);

                const auto viaVector3 = m * p;
                const auto viaVector4 = m * vector4<component_type>(p.x, p.y, p.z, 1);
                const auto viaQuaternion = q * p;

                REQUIRE(viaVector3.x == Approx(expected.x).margin(1e-5));
                REQUIRE(viaVector3.y == Approx(expected.y).margin(1e-5));
                REQUIRE(viaVector3.z == Approx(expected.z).margin(1e-5));

                REQUIRE(viaVector4.x == Approx(expected.x).margin(1e-5));
                REQUIRE(viaVector4.y == Approx(expected.y).margin(1e-5));
                REQUIRE(viaVector4.z == Approx(expected.z).margin(1e-5));

                REQUIRE(viaQuaternion.x == Approx(expected.x).margin(1e-5));
                REQUIRE(viaQuaternion.y == Approx(expected.y).margin(1e-5));
                REQUIRE(viaQuaternion.z == Approx(expected.z).margin(1e-5));
            }
        }
    }

    SECTION("translation survives a rotation and comes back out of column 3")
    {
        quat_type q;
        q.set_from_euler({0.3f, 0.6f, 0.4f});

        mat_type m;
        m.set_translation({-7, 2, 3});
        m.set_rotation(q);

        const auto t = m.translation();
        REQUIRE(t.x == Approx(-7.0f).margin(1e-5));
        REQUIRE(t.y == Approx(2.0f).margin(1e-5));
        REQUIRE(t.z == Approx(3.0f).margin(1e-5));

        const vec_type p{0.9f, 0.3f, 0.5f};
        const auto expected = turn_by(q, p) + t;
        const auto actual = as_a_shader_would(m, p);

        REQUIRE(actual.x == Approx(expected.x).margin(1e-5));
        REQUIRE(actual.y == Approx(expected.y).margin(1e-5));
        REQUIRE(actual.z == Approx(expected.z).margin(1e-5));
    }
}

TEMPLATE_LIST_TEST_CASE("mat4x4 transpose", "[mat4x4]", type::floating_point)
{
    using component_type = TestType;
    using mat_type = matrix4x4<component_type>;
    using quat_type = quaternion<component_type>;
    using vec_type = vector3<component_type>;

    SECTION("swapping twice returns the original")
    {
        mat_type m;
        m.set_translation({1, 2, 3});
        m.set_rotation(quat_type::identity);

        const mat_type original = m;

        m.transpose();
        m.transpose();

        REQUIRE(m == original);
    }

    SECTION("it exchanges the element at (a,b) with the one at (b,a)")
    {
        mat_type m;
        for (std::size_t column = 0; column < mat_type::order; ++column)
            for (std::size_t row = 0; row < mat_type::order; ++row)
                m.set(column, row, static_cast<component_type>(column * 4 + row));

        const mat_type original = m;
        m.transpose();

        for (std::size_t a = 0; a < mat_type::order; ++a)
            for (std::size_t b = 0; b < mat_type::order; ++b)
                REQUIRE(m.get(a, b) == Approx(original.get(b, a)));
    }

    SECTION("identity is its own transpose")
    {
        mat_type m = mat_type::identity;
        m.transpose();

        REQUIRE(m == mat_type::identity);
    }

    SECTION("transposed() returns a copy and leaves the receiver alone")
    {
        mat_type m;
        m.set_translation({5, 6, 7});

        const mat_type original = m;
        const auto transposed = m.transposed();

        REQUIRE(m == original);
        REQUIRE(transposed.get(0, 3) == Approx(original.get(3, 0)));
    }
}

TEMPLATE_LIST_TEST_CASE("mat4x4 inverse", "[mat4x4]", type::floating_point)
{
    using component_type = TestType;
    using mat_type = matrix4x4<component_type>;
    using quat_type = quaternion<component_type>;
    using vec_type = vector3<component_type>;

    SECTION("a transform composed with its inverse is identity")
    {
        quat_type q;
        q.set_from_euler({0.3f, 0.6f, 0.4f});

        mat_type m;
        m.set_translation({2, -3, 4});
        m.set_rotation(q);

        mat_type inverted = m;
        inverted.inverse();

        const auto product = m * inverted;

        for (std::size_t a = 0; a < mat_type::order; ++a)
            for (std::size_t b = 0; b < mat_type::order; ++b)
                REQUIRE(product.get(a, b) == Approx(mat_type::identity.get(a, b)).margin(1e-4));
    }

    SECTION("inverting a singular matrix throws rather than producing NaN")
    {
        mat_type singular;
        singular.set_rotation_and_scale(quat_type::identity, {1, 0, 1});

        REQUIRE_THROWS_AS(singular.inverse(), std::domain_error);
    }
}

TEST_CASE("gdk::matrix4x4 constructors", "[mat4x4]")
{
    using mat = matrix4x4<float>;
    using quat = quaternion<float>;
    using vec = vector3<float>;

    SECTION("the sixteen component constructor fills the matrix in storage order")
    {
        const mat a(
             0,  1,  2,  3,
             4,  5,  6,  7,
             8,  9, 10, 11,
            12, 13, 14, 15);

        for (std::size_t k = 0; k < mat::order * mat::order; ++k)
            REQUIRE(a.get(k / mat::order, k % mat::order) == Approx(static_cast<float>(k)));

        REQUIRE((&a.front())[5] == Approx(5.0f));
    }

    SECTION("the transform constructor composes translation, rotation and scale")
    {
        const auto turn = quat::from_euler({0.3f, 0.6f, 0.4f});
        const mat a(vec(1, 2, 3), turn, vec(2, 2, 2));

        REQUIRE(a.translation() == vec(1, 2, 3));

        const auto scale = a.scale();
        REQUIRE(scale.x == Approx(2.0f));
        REQUIRE(scale.y == Approx(2.0f));
        REQUIRE(scale.z == Approx(2.0f));

        const auto recovered = a.rotation();
        REQUIRE(recovered.x == Approx(turn.x).margin(1e-5));
        REQUIRE(recovered.y == Approx(turn.y).margin(1e-5));
        REQUIRE(recovered.z == Approx(turn.z).margin(1e-5));
        REQUIRE(recovered.w == Approx(turn.w).margin(1e-5));
    }

    SECTION("the scale argument defaults to one")
    {
        const mat a(vec(4, 5, 6), quat::identity);

        REQUIRE(a.translation() == vec(4, 5, 6));
        REQUIRE(a.scale().x == Approx(1.0f));
    }
}
