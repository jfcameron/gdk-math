// © 2018 Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/quaternion.h>
#include <gdk/vector3.h>

#include <cmath>

using namespace gdk;

namespace {
    template<typename T>
    vector3<T> turn_by(const quaternion<T> &aQ, const vector3<T> &aV) {
        using vec = vector3<T>;
        const vec axis{aQ.x, aQ.y, aQ.z};
        const vec t{
            2 * (axis.y * aV.z - axis.z * aV.y),
            2 * (axis.z * aV.x - axis.x * aV.z),
            2 * (axis.x * aV.y - axis.y * aV.x)};

        return vec{
            aV.x + aQ.w * t.x + (axis.y * t.z - axis.z * t.y),
            aV.y + aQ.w * t.y + (axis.z * t.x - axis.x * t.z),
            aV.z + aQ.w * t.z + (axis.x * t.y - axis.y * t.x)};
    }

    template<typename T>
    T length_of(const quaternion<T> &aQ) {
        return std::sqrt(aQ.w * aQ.w + aQ.x * aQ.x + aQ.y * aQ.y + aQ.z * aQ.z);
    }

    template<typename T>
    T rotation_difference(const quaternion<T> &a, const quaternion<T> &b) {
        const auto plus = std::sqrt((a.w - b.w) * (a.w - b.w) + (a.x - b.x) * (a.x - b.x)
            + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
        const auto minus = std::sqrt((a.w + b.w) * (a.w + b.w) + (a.x + b.x) * (a.x + b.x)
            + (a.y + b.y) * (a.y + b.y) + (a.z + b.z) * (a.z + b.z));

        return std::min(plus, minus);
    }

    constexpr double QUARTER_TURN = to_radians(90.0);
}

TEMPLATE_LIST_TEST_CASE("quaternion constructors", "[quaternion]", type::floating_point)
{
    using component_type = TestType;
    using quat = quaternion<component_type>;
    using vec = vector3<component_type>;

    SECTION("the default is identity, and identity is (0,0,0,1)")
    {
        REQUIRE(quat() == quat::identity);
        REQUIRE(quat::identity.x == Approx(0.0f));
        REQUIRE(quat::identity.y == Approx(0.0f));
        REQUIRE(quat::identity.z == Approx(0.0f));
        REQUIRE(quat::identity.w == Approx(1.0f));
    }

    SECTION("the four-argument constructor takes x, y, z, w -- w last")
    {
        const quat q(0.1f, 0.2f, 0.3f, 0.4f);

        REQUIRE(q.x == Approx(0.1f));
        REQUIRE(q.y == Approx(0.2f));
        REQUIRE(q.z == Approx(0.3f));
        REQUIRE(q.w == Approx(0.4f));
    }

    SECTION("rotation about an axis matches the right-hand rule")
    {
        quat q;
        q.set_from_euler({0, 0, QUARTER_TURN});

        const auto turnedX = turn_by(q, vec{1, 0, 0});
        REQUIRE(turnedX.x == Approx(0.0f).margin(1e-5));
        REQUIRE(turnedX.y == Approx(1.0f).margin(1e-5));

        const auto turnedY = turn_by(q, vec{0, 1, 0});
        REQUIRE(turnedY.x == Approx(-1.0f).margin(1e-5));
        REQUIRE(turnedY.y == Approx(0.0f).margin(1e-5));
    }
}

TEMPLATE_LIST_TEST_CASE("quaternion normalisation", "[quaternion]", type::floating_point)
{
    using component_type = TestType;
    using quat = quaternion<component_type>;
    using vec = vector3<component_type>;

    SECTION("normalized returns unit length and preserves the rotation")
    {
        quat q;
        q.set_from_euler({0.3f, 0.6f, 0.4f});

        quat overlong = q;
        overlong *= 5.0f;

        REQUIRE(length_of(overlong) == Approx(5.0f).margin(1e-4));
        REQUIRE(length_of(overlong.normalized()) == Approx(1.0f).margin(1e-5));
        REQUIRE(rotation_difference(overlong.normalized(), q) < 1e-5);
    }

    SECTION("a zero quaternion normalises to identity rather than dividing by zero")
    {
        const quat zero(0, 0, 0, 0);
        const auto normalised = zero.normalized();

        REQUIRE(std::isfinite(normalised.w));
        REQUIRE(normalised == quat::identity);
    }

    SECTION("normalized does not modify its receiver")
    {
        quat q(1, 2, 3, 4);
        static_cast<void>(q.normalized());

        REQUIRE(q.x == Approx(1.0f));
        REQUIRE(q.w == Approx(4.0f));
    }
}

TEMPLATE_LIST_TEST_CASE("quaternion inversion", "[quaternion]", type::floating_point)
{
    using component_type = TestType;
    using quat = quaternion<component_type>;
    using vec = vector3<component_type>;

    quat q;
    q.set_from_euler({0.3f, 0.6f, 0.4f});

    SECTION("inverse_unit undoes the rotation it inverts")
    {
        const vec probe{0.9f, -0.3f, 0.5f};
        const auto roundTripped = turn_by(q.inverse_unit(), turn_by(q, probe));

        REQUIRE(roundTripped.x == Approx(probe.x).margin(1e-5));
        REQUIRE(roundTripped.y == Approx(probe.y).margin(1e-5));
        REQUIRE(roundTripped.z == Approx(probe.z).margin(1e-5));
    }

    SECTION("q * q.inverse_unit() is identity")
    {
        REQUIRE(rotation_difference(q * q.inverse_unit(), quat::identity) < 1e-5);
    }

    SECTION("inverse_unit is const and does not mutate")
    {
        quat subject = q;
        const auto inverted = subject.inverse_unit();

        REQUIRE(subject.x == Approx(q.x));
        REQUIRE(inverted.x == Approx(-q.x));
    }

    SECTION("inverse handles a non-unit quaternion, where inverse_unit would not")
    {
        quat overlong = q;
        overlong *= 3.0f;

        REQUIRE(rotation_difference(overlong * overlong.inverse(), quat::identity) < 1e-4);
    }

    SECTION("inverting a zero quaternion throws rather than dividing by zero")
    {
        REQUIRE_THROWS(quat(0, 0, 0, 0).inverse());
    }
}

TEMPLATE_LIST_TEST_CASE("quaternion multiplication", "[quaternion]", type::floating_point)
{
    using component_type = TestType;
    using quat = quaternion<component_type>;
    using vec = vector3<component_type>;

    SECTION("composition applies the right operand first")
    {
        quat aboutZ, aboutX;
        aboutZ.set_from_euler({0, 0, QUARTER_TURN});
        aboutX.set_from_euler({QUARTER_TURN, 0, 0});

        const vec probe{1, 0, 0};
        const auto composed = turn_by(aboutX * aboutZ, probe);
        const auto sequential = turn_by(aboutX, turn_by(aboutZ, probe));

        REQUIRE(composed.x == Approx(sequential.x).margin(1e-5));
        REQUIRE(composed.y == Approx(sequential.y).margin(1e-5));
        REQUIRE(composed.z == Approx(sequential.z).margin(1e-5));

        const auto reversed = turn_by(aboutZ * aboutX, probe);
        REQUIRE(std::abs(reversed.y - composed.y) > 0.5f);
    }

    SECTION("identity is the multiplicative identity, on both sides")
    {
        quat q;
        q.set_from_euler({0.3f, 0.6f, 0.4f});

        REQUIRE(rotation_difference(q * quat::identity, q) < 1e-6);
        REQUIRE(rotation_difference(quat::identity * q, q) < 1e-6);
    }

    SECTION("composition is associative")
    {
        quat a, b, c;
        a.set_from_euler({0.3f, 0, 0});
        b.set_from_euler({0, 0.7f, 0});
        c.set_from_euler({0, 0, 1.1f});

        REQUIRE(rotation_difference((a * b) * c, a * (b * c)) < 1e-5);
    }

    SECTION("a unit quaternion composed with itself stays unit length")
    {
        quat q;
        q.set_from_euler({0.3f, 0.6f, 0.4f});

        auto accumulated = quat::identity;
        for (int i = 0; i < 200; ++i) accumulated = q * accumulated;

        REQUIRE(length_of(accumulated) == Approx(1.0f).margin(1e-3));
    }
}

TEMPLATE_LIST_TEST_CASE("quaternion euler conversion", "[quaternion]", type::floating_point)
{
    using component_type = TestType;
    using quat = quaternion<component_type>;
    using vec = vector3<component_type>;

    SECTION("set_from_euler takes {roll about X, pitch about Y, yaw about Z}")
    {
        const vec probe{1, 1, 1};

        quat roll;
        roll.set_from_euler({QUARTER_TURN, 0, 0});
        const auto rolled = turn_by(roll, probe);
        REQUIRE(rolled.x == Approx(1.0f).margin(1e-5));   

        quat pitch;
        pitch.set_from_euler({0, QUARTER_TURN, 0});
        const auto pitched = turn_by(pitch, probe);
        REQUIRE(pitched.y == Approx(1.0f).margin(1e-5)); 

        quat yaw;
        yaw.set_from_euler({0, 0, QUARTER_TURN});
        const auto yawed = turn_by(yaw, probe);
        REQUIRE(yawed.z == Approx(1.0f).margin(1e-5));   
    }

    SECTION("**euler angles compose YXZ: heading about Y, then pitch about X, then roll about Z**")
    {
        const double angles[] = {0.0, 0.3, -0.7, 1.2, 2.5, -2.0};

        for (const auto pitch : angles)
            for (const auto heading : angles)
                for (const auto roll : angles) {
                    quat fromEuler;
                    fromEuler.set_from_euler({static_cast<TestType>(pitch),
                        static_cast<TestType>(heading), static_cast<TestType>(roll)});

                    const auto composed =
                        quat::from_angle_axis(static_cast<TestType>(heading), vec::up)
                      * quat::from_angle_axis(static_cast<TestType>(pitch), vec::right)
                      * quat::from_angle_axis(static_cast<TestType>(roll), vec::backward);

                    REQUIRE(rotation_difference(fromEuler, composed) < 1e-5);
                }
    }

    SECTION("**gimbal lock falls on pitch, not on heading**")
    {
        const auto right = static_cast<TestType>(to_radians(90.0));
        const auto some = static_cast<TestType>(to_radians(20.0));

        quat atPitch, atPitchShifted;
        atPitch.set_from_euler({right, 0, 0});
        atPitchShifted.set_from_euler({right, some, some});
        REQUIRE(rotation_difference(atPitch, atPitchShifted) < 1e-5); 

        quat atHeading, atHeadingShifted;
        atHeading.set_from_euler({0, right, 0});
        atHeadingShifted.set_from_euler({some, right, some});
        REQUIRE(rotation_difference(atHeading, atHeadingShifted) > 1e-2); 
    }

    SECTION("to_euler survives the gimbal lock pole rather than returning NaN")
    {
        // 0.7071068 is a rounded sqrt(0.5): what a 90 degree rotation is written as by hand.
        const quat pole(0, 0.7071068, 0, 0.7071068);

        const auto e = pole.to_euler();

        REQUIRE_FALSE(std::isnan(e.x));
        REQUIRE_FALSE(std::isnan(e.y));
        REQUIRE_FALSE(std::isnan(e.z));

        REQUIRE(e.y == Approx(numbers::pi / 2).margin(1e-6));
    }

    SECTION("to_euler round trips away from the singularity")
    {
        const vec eulers[] = {{0.3f, 0.6f, 0.4f}, {-0.2f, 0.1f, 1.1f}, {1.0f, -0.9f, -0.4f}};

        for (const auto &euler : eulers) {
            quat q;
            q.set_from_euler(euler);

            quat again;
            again.set_from_euler(q.to_euler());

            REQUIRE(rotation_difference(again, q) < 1e-4);
        }
    }

    SECTION("from_euler is the static spelling of the same thing")
    {
        const vec euler{0.3f, 0.6f, 0.4f};

        quat viaSetter;
        viaSetter.set_from_euler(euler);

        REQUIRE(rotation_difference(quat::from_euler(euler), viaSetter) < 1e-6);
        REQUIRE(rotation_difference(quat(euler), viaSetter) < 1e-6);
    }
}
