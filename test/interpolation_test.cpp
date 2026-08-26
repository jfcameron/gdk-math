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
    void require_same_rotation(const quaternion<T> &a, const quaternion<T> &b, const double aMargin = 1e-4) {
        for (const auto &v : {vector3<T>::right, vector3<T>::up, vector3<T>::forward})
            require_near(a * v, b * v, aMargin);
    }

    template<typename T>
    double rotation_error_degrees(const quaternion<double> &reference, const quaternion<T> &actual) {
        const double n = std::sqrt(static_cast<double>(actual.x) * actual.x + static_cast<double>(actual.y) * actual.y +
            static_cast<double>(actual.z) * actual.z + static_cast<double>(actual.w) * actual.w);

        const double x = actual.x / n, y = actual.y / n, z = actual.z / n, w = actual.w / n;

        const auto chord = [&](const double sign) {
            const double dx = reference.x - sign * x, dy = reference.y - sign * y;
            const double dz = reference.z - sign * z, dw = reference.w - sign * w;

            return std::sqrt(dx * dx + dy * dy + dz * dz + dw * dw);
        };

        const double d = std::min(chord(1.0), chord(-1.0));

        return 2.0 * std::asin(std::min(1.0, d * 0.5)) * (180.0 / 3.14159265358979323846);
    }

    quaternion<double> slerp_reference(const quaternion<double> &a, const quaternion<double> &b, const double t) {
        double c = a.dot_product(b);
        auto adjusted = b;

        if (c < 0.0) {
            adjusted = -b;
            c = -c;
        }

        if (c > 0.9995) return nlerp(a, adjusted, t);

        const double theta = std::acos(c), sinTheta = std::sin(theta);

        return a * (std::sin((1.0 - t) * theta) / sinTheta) + adjusted * (std::sin(t * theta) / sinTheta);
    }
}

TEMPLATE_LIST_TEST_CASE("gdk::quaternion axis-angle construction", "[quaternion][interpolation]", type::floating_point)
{
    using vec2 = vector2<TestType>;
    using vec = vector3<TestType>;
    using quat = quaternion<TestType>;

    SECTION("a quarter turn about +Z sends +X to +Y")
    {
        const auto q = quat::from_angle_axis(QUARTER_TURN, vec::up);

        require_near(q * vec::right, vec::forward);
    }

    SECTION("it agrees with the euler spelling")
    {
        require_same_rotation(quat::from_angle_axis(QUARTER_TURN, vec(0, 0, 1)),
            quat::from_euler({0, 0, QUARTER_TURN}));
    }

    SECTION("the axis need not be unit length")
    {
        const auto fromUnit = quat::from_angle_axis(0.7f, vec(0, 1, 0));
        const auto fromLong = quat::from_angle_axis(0.7f, vec(0, 37.5f, 0));

        require_same_rotation(fromUnit, fromLong);
    }

    SECTION("a zero axis gives the identity rather than NaN")
    {
        const auto q = quat::from_angle_axis(1.0f, vec::zero);

        REQUIRE(q == quat::identity);
    }

    SECTION("angle and axis read back what was put in")
    {
        for (const auto angle : {0.1f, 0.9f, QUARTER_TURN, 3.0f}) {
            const auto axis = vec(1, 2, 3).normal();
            const auto q = quat::from_angle_axis(angle, axis);

            REQUIRE(q.angle() == Approx(angle).margin(1e-4f));
            require_near(q.axis(), axis, 1e-4f);
        }
    }

    SECTION("the identity reports a zero angle and a unit axis")
    {
        REQUIRE(quat::identity.angle() == Approx(0.0f).margin(1e-6f));
        REQUIRE(quat::identity.axis().length() == Approx(1.0f));
    }

    SECTION("small angles keep their precision")
    {
        const auto tiny = 1e-4f;
        const auto q = quat::from_angle_axis(tiny, vec::up);

        REQUIRE(q.angle() == Approx(tiny).epsilon(0.01f));
    }
}

TEMPLATE_LIST_TEST_CASE("gdk::slerp", "[quaternion][interpolation]", type::floating_point)
{
    using vec2 = vector2<TestType>;
    using vec = vector3<TestType>;
    using quat = quaternion<TestType>;

    const auto start = quat::identity;
    const auto end = quat::from_angle_axis(QUARTER_TURN, vec::up);

    SECTION("the endpoints are exact")
    {
        require_same_rotation(slerp(start, end, static_cast<TestType>(0.0)), start);
        require_same_rotation(slerp(start, end, static_cast<TestType>(1.0)), end);
    }

    SECTION("the midpoint is half the rotation")
    {
        require_same_rotation(slerp(start, end, static_cast<TestType>(0.5)),
            quat::from_angle_axis(QUARTER_TURN * 0.5f, vec::up));
    }

    SECTION("it holds unit length throughout")
    {
        for (TestType t = 0; t <= 1; t += static_cast<TestType>(0.05)) {
            const auto q = slerp(start, end, t);

            REQUIRE(std::sqrt(q.dot_product(q)) == Approx(1.0f).margin(1e-5f));
        }
    }

    SECTION("angular velocity is constant, which is what distinguishes it from nlerp")
    {
        const auto wide = quat::from_angle_axis(2.5f, vec::up);

        TestType previous = 0;
        for (int step = 1; step <= 5; ++step) {
            const auto a = slerp(start, wide, static_cast<TestType>((step - 1) * 0.2));
            const auto b = slerp(start, wide, static_cast<TestType>(step * 0.2));

            const auto covered = (a.inverse_unit() * b).angle();

            if (step > 1) REQUIRE(covered == Approx(previous).margin(1e-3f));
            previous = covered;
        }
    }

    SECTION("**it takes the shorter arc when the inputs have opposite signs**")
    {
        const auto negated = -end;

        require_same_rotation(negated, end);   

        const auto viaPositive = slerp(start, end, static_cast<TestType>(0.5));
        const auto viaNegative = slerp(start, negated, static_cast<TestType>(0.5));

        require_same_rotation(viaPositive, viaNegative);

        REQUIRE(viaNegative.angle() == Approx(QUARTER_TURN * 0.5f).margin(1e-3f));
    }

    SECTION("**nearly identical rotations do not divide by zero**")
    {
        const auto almost = quat::from_angle_axis(1e-7f, vec::up);

        const auto q = slerp(start, almost, static_cast<TestType>(0.5));

        REQUIRE(std::isfinite(q.x));
        REQUIRE(std::isfinite(q.y));
        REQUIRE(std::isfinite(q.z));
        REQUIRE(std::isfinite(q.w));
        require_same_rotation(q, start, 1e-3f);
    }

    SECTION("interpolating a rotation with itself is that rotation")
    {
        for (const auto t : {TestType(0), TestType(0.25), TestType(0.5), TestType(1)})
            require_same_rotation(slerp(end, end, t), end);
    }
}

TEMPLATE_LIST_TEST_CASE("gdk::slerp accuracy", "[quaternion][interpolation]", type::floating_point)
{
    using vec = vector3<TestType>;
    using quat = quaternion<TestType>;

#if GDK_MATH_FAST_TRANSCENDENTALS
    constexpr double TOLERANCE_DEGREES = 0.002;
#else
    constexpr double TOLERANCE_DEGREES = 1e-4;
#endif

    SECTION("**it holds its documented error bound across the whole arc**")
    {
        double worst = 0;

        for (const auto angle : {0.02f, 0.06f, 0.3f, 0.9f, 1.5f, 2.2f, 2.8f, 3.1f}) {
            const auto a = quat::from_angle_axis(0.4f, vec(1, 2, 3).normal());
            const auto b = a * quat::from_angle_axis(angle, vec(-2, 1, 0.5f).normal());

            const quaternion<double> aRef(a.x, a.y, a.z, a.w), bRef(b.x, b.y, b.z, b.w);

            for (int step = 0; step <= 20; ++step) {
                const auto t = static_cast<TestType>(step * 0.05);

                worst = std::max(worst, rotation_error_degrees(
                    slerp_reference(aRef, bRef, static_cast<double>(t)), slerp(a, b, t)));
            }
        }

        INFO("worst error " << worst << " degrees against a tolerance of " << TOLERANCE_DEGREES);
        REQUIRE(worst < TOLERANCE_DEGREES);
    }

    SECTION("the endpoints stay exact on either path")
    {
        const auto a = quat::from_angle_axis(0.4f, vec::up);
        const auto b = quat::from_angle_axis(2.4f, vec(1, 1, 0).normal());

        const double margin = TOLERANCE_DEGREES * (3.14159265358979323846 / 180.0);

        require_same_rotation(slerp(a, b, static_cast<TestType>(0)), a, margin);
        require_same_rotation(slerp(a, b, static_cast<TestType>(1)), b, margin);
    }

    SECTION("it is still unit length on either path")
    {
        const auto a = quat::from_angle_axis(0.4f, vec::up);
        const auto b = quat::from_angle_axis(2.9f, vec(0, 1, 3).normal());

        for (TestType t = 0; t <= 1; t += static_cast<TestType>(0.05)) {
            const auto q = slerp(a, b, t);

            REQUIRE(std::sqrt(q.dot_product(q)) == Approx(1.0f).margin(1e-4f));
        }
    }
}

TEMPLATE_LIST_TEST_CASE("gdk::nlerp", "[quaternion][interpolation]", type::floating_point)
{
    using vec2 = vector2<TestType>;
    using vec = vector3<TestType>;
    using quat = quaternion<TestType>;

    const auto start = quat::identity;
    const auto end = quat::from_angle_axis(QUARTER_TURN, vec::up);

    SECTION("the endpoints are exact and the result stays unit length")
    {
        require_same_rotation(nlerp(start, end, static_cast<TestType>(0.0)), start);
        require_same_rotation(nlerp(start, end, static_cast<TestType>(1.0)), end);

        for (TestType t = 0; t <= 1; t += static_cast<TestType>(0.1)) {
            const auto q = nlerp(start, end, t);

            REQUIRE(std::sqrt(q.dot_product(q)) == Approx(1.0f).margin(1e-5f));
        }
    }

    SECTION("it takes the shorter arc too")
    {
        require_same_rotation(nlerp(start, -end, static_cast<TestType>(0.5)), nlerp(start, end, static_cast<TestType>(0.5)));
    }

    SECTION("it follows slerp's arc but not its pacing")
    {
        const auto narrow = quat::from_angle_axis(0.05f, vec::up);
        require_same_rotation(nlerp(start, narrow, static_cast<TestType>(0.5)), slerp(start, narrow, static_cast<TestType>(0.5)), 1e-4);

        const auto wide = quat::from_angle_axis(2.8f, vec::up);
        const auto difference = (nlerp(start, wide, static_cast<TestType>(0.25)).inverse_unit()
            * slerp(start, wide, static_cast<TestType>(0.25))).angle();

        REQUIRE(difference > 0.05f);
    }
}

TEMPLATE_LIST_TEST_CASE("gdk::lerp for vectors", "[vector3][vector2][interpolation]", type::floating_point)
{
    using vec2 = vector2<TestType>;
    using vec = vector3<TestType>;
    using quat = quaternion<TestType>;

    SECTION("endpoints and midpoint")
    {
        const vec a(1, 2, 3), b(5, 6, 11);

        require_near(lerp(a, b, static_cast<TestType>(0.0)), a);
        require_near(lerp(a, b, static_cast<TestType>(1.0)), b);
        require_near(lerp(a, b, static_cast<TestType>(0.5)), vec(3, 4, 7));
    }

    SECTION("it extrapolates rather than clamping")
    {
        require_near(lerp(vec(0, 0, 0), vec(2, 0, 0), static_cast<TestType>(2.0)), vec(4, 0, 0));
        require_near(lerp(vec(0, 0, 0), vec(2, 0, 0), static_cast<TestType>(-1.0)), vec(-2, 0, 0));
    }

    SECTION("vector2 behaves the same way")
    {
        const auto midpoint = lerp(vec2(0, 0), vec2(4, 8), static_cast<TestType>(0.25));

        REQUIRE(midpoint.x == Approx(1.0f));
        REQUIRE(midpoint.y == Approx(2.0f));
    }
}

TEMPLATE_LIST_TEST_CASE("gdk::vector3 reflect and angle_between", "[vector3]", type::floating_point)
{
    using vec2 = vector2<TestType>;
    using vec = vector3<TestType>;
    using quat = quaternion<TestType>;

    SECTION("a vector reflects off a surface")
    {
        require_near(vec(1, -1, 0).reflect(vec::up), vec(1, 1, 0));
    }

    SECTION("reflecting twice returns the original")
    {
        const vec v(0.3f, -0.9f, 0.2f);
        const auto n = vec(1, 2, 3).normal();

        require_near(v.reflect(n).reflect(n), v);
    }

    SECTION("a vector along the normal reverses")
    {
        require_near(vec(0, -2, 0).reflect(vec::up), vec(0, 2, 0));
    }

    SECTION("a vector across the normal is untouched")
    {
        require_near(vec(3, 0, 0).reflect(vec::up), vec(3, 0, 0));
    }

    SECTION("angle_between covers the whole range")
    {
        REQUIRE(vec::right.angle_between(vec::up) == Approx(QUARTER_TURN));
        REQUIRE(vec::right.angle_between(vec::right) == Approx(0.0f).margin(1e-6f));
        REQUIRE(vec::right.angle_between(vec::left) == Approx(numbers::pi_f).margin(1e-5f));
    }

    SECTION("it is unaffected by length")
    {
        REQUIRE(vec(5, 0, 0).angle_between(vec(0, 0.001f, 0)) == Approx(QUARTER_TURN));
    }

    SECTION("**the parallel and antiparallel cases do not produce NaN**")
    {
        for (const auto &v : {vec(1, 2, 3), vec(0.001f, 0, 0), vec(1e6f, 1e6f, 1e6f)}) {
            REQUIRE(std::isfinite(v.angle_between(v)));
            REQUIRE(std::isfinite(v.angle_between(v * -1.0f)));
        }
    }

    SECTION("a zero vector reports zero rather than NaN")
    {
        REQUIRE(vec::zero.angle_between(vec::up) == Approx(0.0f));
        REQUIRE(std::isfinite(vec::zero.angle_between(vec::zero)));
    }
}

TEMPLATE_LIST_TEST_CASE("gdk::to_radians and to_degrees", "[math_constants]", type::floating_point)
{
    using vec2 = vector2<TestType>;
    using vec = vector3<TestType>;
    using quat = quaternion<TestType>;

    SECTION("the landmarks")
    {
        REQUIRE(to_radians(180.0f) == Approx(numbers::pi_f));
        REQUIRE(to_radians(90.0f) == Approx(QUARTER_TURN));
        REQUIRE(to_degrees(numbers::pi_f) == Approx(180.0f));
    }

    SECTION("they round trip")
    {
        for (const auto degrees : {0.0f, 1.0f, 45.0f, 90.0f, 179.9f, 360.0f, -30.0f})
            REQUIRE(to_degrees(to_radians(degrees)) == Approx(degrees).margin(1e-3f));
    }

    SECTION("they are usable in a constant expression")
    {
        static_assert(to_radians(0.0) == 0.0, "");
        constexpr auto quarter = to_radians(90.0);

        REQUIRE(quarter == Approx(QUARTER_TURN));
    }
}
