// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/math.h>

#include <array>
#include <stdexcept>
#include <vector>

using namespace gdk;

namespace {
    //! an affine transform: rotation, scale and translation, but no perspective row
    template<typename T>
    [[nodiscard]] matrix4x4<T> affine() {
        matrix4x4<T> m;

        m.set_rotation_and_scale(quaternion<T>::from_euler({T(0.3), T(-0.7), T(1.1)}),
            vector3<T>{T(2), T(0.5), T(1.5)});

        m.set_translation({T(3), T(-4), T(5)});

        return m;
    }

    template<typename T>
    [[nodiscard]] std::vector<vector3<T>> sample_points() {
        return {
            {T(0), T(0), T(0)},
            {T(1), T(2), T(3)},
            {T(-4), T(5), T(-6)},
            {T(0.25), T(-0.5), T(0.75)},
            {T(100), T(-200), T(300)},
        };
    }
}

TEMPLATE_LIST_TEST_CASE("gdk-math batch: results match the single-operand operators",
    "[batch]", type::floating_point)
{
    using T = TestType;

    const auto m = affine<T>();
    const auto points = sample_points<T>();

    std::vector<vector3<T>> out(points.size());

    SECTION("project_points is elementwise identical to operator*")
    {
        project_points(m, points, out);

        for (std::size_t i = 0; i < points.size(); ++i) REQUIRE(out[i] == m * points[i]);
    }

    SECTION("transform_points matches operator* for an affine transform")
    {
        transform_points(m, points, out);

        for (std::size_t i = 0; i < points.size(); ++i) REQUIRE(out[i] == m * points[i]);
    }

    SECTION("transform_directions matches the hoisted 3x3")
    {
        const auto linear = upper_left(m);

        transform_directions(m, points, out);

        for (std::size_t i = 0; i < points.size(); ++i) REQUIRE(out[i] == linear * points[i]);
    }

    SECTION("transform_directions ignores translation, which points do not")
    {
        std::vector<vector3<T>> asPoints(points.size());

        transform_points(m, points, asPoints);
        transform_directions(m, points, out);

        REQUIRE(asPoints[1] != out[1]);
        REQUIRE(out[0] == vector3<T>::zero);            
        REQUIRE(asPoints[0] == m.translation());       
    }

    SECTION("transform_normals matches the normal matrix")
    {
        const auto normals = normal_matrix(m);

        transform_normals(m, points, out);

        for (std::size_t i = 0; i < points.size(); ++i) REQUIRE(out[i] == normals * points[i]);
    }

    SECTION("rotate_directions matches quaternion multiplication")
    {
        const auto q = quaternion<T>::from_euler({T(0.3), T(-0.7), T(1.1)});

        rotate_directions(q, points, out);

        for (std::size_t i = 0; i < points.size(); ++i) REQUIRE(out[i] == q * points[i]);
    }
}

TEMPLATE_LIST_TEST_CASE("gdk-math batch: the edges", "[batch]", type::floating_point)
{
    using T = TestType;

    const auto m = affine<T>();
    const auto points = sample_points<T>();

    SECTION("in place gives the same answer as out of place")
    {
        std::vector<vector3<T>> inPlace = points;
        std::vector<vector3<T>> outOfPlace(points.size());

        transform_points(m, inPlace);
        transform_points(m, points, outOfPlace);

        REQUIRE(inPlace == outOfPlace);
    }

    SECTION("a destination smaller than the source is refused")
    {
        std::vector<vector3<T>> tooSmall(points.size() - 1);

        REQUIRE_THROWS_AS(project_points(m, points, tooSmall), std::invalid_argument);
    }

    SECTION("a larger destination is allowed, and the tail is untouched")
    {
        std::vector<vector3<T>> roomy(points.size() + 2, vector3<T>{T(9), T(9), T(9)});

        project_points(m, points, roomy);

        REQUIRE(roomy[points.size()] == vector3<T>(T(9), T(9), T(9)));
    }

    SECTION("an empty batch is a no-op rather than an error")
    {
        std::vector<vector3<T>> none;

        REQUIRE_NOTHROW(project_points(m, none, none));
    }

    SECTION("the spans convert from whatever the caller holds")
    {
        const std::array<vector3<T>, 2> asArray{vector3<T>{T(1), T(2), T(3)},
            vector3<T>{T(4), T(5), T(6)}};

        const vector3<T> asCArray[2] = {{T(1), T(2), T(3)}, {T(4), T(5), T(6)}};

        std::vector<vector3<T>> out(2);

        REQUIRE_NOTHROW(project_points(m, asArray, out));
        REQUIRE_NOTHROW(project_points(m, asCArray, out));
        REQUIRE_NOTHROW(project_points(m, std::span(points).subspan(1, 2), out));
    }
}
