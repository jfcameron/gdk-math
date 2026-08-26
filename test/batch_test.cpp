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

TEMPLATE_LIST_TEST_CASE("gdk-math batch: the SIMD kernel agrees with the scalar one",
    "[batch]", type::floating_point)
{
    using T = TestType;

    const auto m = affine<T>();

    std::vector<vector3<T>> points;
    for (int i = 0; i < 40; ++i)
        points.push_back({T(i * 0.37 - 5.0), T(i * -0.91 + 2.0), T(i * 1.13 - 8.0)});

    SECTION("**every batch length matches operator* exactly, tail included**")
    {
        for (std::size_t n = 0; n <= points.size(); ++n) {
            const auto source = std::vector<vector3<T>>(points.begin(), points.begin() + n);

            std::vector<vector3<T>> transformed(n), directions(n), normals(n), projected(n), rotated(n);

            auto perspective = m;
            perspective.set(0, 3, T(0.2));
            perspective.set(1, 3, T(-0.1));
            perspective.set(2, 3, T(0.35));

            const auto rotation = quaternion<T>::from_euler({T(0.3), T(-0.7), T(1.1)});

            transform_points(m, source, transformed);
            transform_directions(m, source, directions);
            transform_normals(m, source, normals);
            project_points(perspective, source, projected);
            rotate_directions(rotation, source, rotated);

            const auto linear = upper_left(m);
            const auto normalMatrix = normal_matrix(m);

            for (std::size_t i = 0; i < n; ++i) {
                INFO("length " << n << ", element " << i);

                REQUIRE(transformed[i] == m * source[i]);
                REQUIRE(directions[i] == linear * source[i]);
                REQUIRE(normals[i] == normalMatrix * source[i]);
                REQUIRE(projected[i] == perspective * source[i]);
                REQUIRE(rotated[i] == rotation * source[i]);
            }
        }
    }

    SECTION("**a point projecting to w == 0 is passed through, as to_point does**")
    {
        auto degenerate = matrix4x4<T>::identity;
        degenerate.set(0, 3, T(0));
        degenerate.set(1, 3, T(0));
        degenerate.set(2, 3, T(0));
        degenerate.set(3, 3, T(0));

        for (std::size_t n = 1; n <= 9; ++n) {
            const auto source = std::vector<vector3<T>>(points.begin(), points.begin() + n);

            std::vector<vector3<T>> out(n);

            project_points(degenerate, source, out);

            for (std::size_t i = 0; i < n; ++i) {
                INFO("length " << n << ", element " << i);
                REQUIRE(out[i] == degenerate * source[i]);
            }
        }
    }

    SECTION("in place matches out of place at every length")
    {
        for (std::size_t n = 0; n <= points.size(); ++n) {
            std::vector<vector3<T>> inPlace(points.begin(), points.begin() + n);
            std::vector<vector3<T>> outOfPlace(n);

            transform_points(m, inPlace, outOfPlace);
            transform_points(m, inPlace);

            INFO("length " << n);
            REQUIRE(inPlace == outOfPlace);
        }
    }

    SECTION("it does not write past the source length into a longer destination")
    {
        const auto sentinel = vector3<T>{T(-999), T(-999), T(-999)};

        for (std::size_t n = 1; n <= 9; ++n) {
            const auto source = std::vector<vector3<T>>(points.begin(), points.begin() + n);

            std::vector<vector3<T>> destination(n + 3, sentinel);

            transform_points(m, source, destination);

            INFO("length " << n);
            for (std::size_t i = n; i < destination.size(); ++i) REQUIRE(destination[i] == sentinel);
        }
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
