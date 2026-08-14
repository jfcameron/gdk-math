// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/math.h>

#include <stdexcept>

using namespace gdk;

namespace {
    template<typename T>
    struct embedded final {
        matrix3x3<T> three;
        matrix4x4<T> four;

        explicit embedded(const matrix3x3<T> &aSource) : three(aSource) {
            four.set_to_identity();

            for (std::size_t column = 0; column < 3; ++column)
                for (std::size_t row = 0; row < 3; ++row)
                    four.set(column, row, aSource.get(column, row));
        }
    };

    template<typename T>
    matrix3x3<T> made(const T a, const T b, const T c,
        const T d, const T e, const T f,
        const T g, const T h, const T i) {
        matrix3x3<T> m;
        m.set(a, b, c, d, e, f, g, h, i);
        return m;
    }

    template<typename T>
    void require_same_block(const matrix3x3<T> &aExpected, const matrix4x4<T> &aActual,
        const double aMargin = 1e-5) {
        for (std::size_t column = 0; column < 3; ++column)
            for (std::size_t row = 0; row < 3; ++row)
                REQUIRE(aActual.get(column, row) ==
                    Approx(aExpected.get(column, row)).margin(aMargin));
    }
}

TEMPLATE_LIST_TEST_CASE("gdk-math: matrix3x3 and matrix4x4 agree on shared operations",
    "[parity][mat]", type::floating_point)
{
    using mat3 = matrix3x3<TestType>;
    using mat4 = matrix4x4<TestType>;

    const auto source = made<TestType>(
        2, 0, 1,
        1, 3, 0,
        0, 1, 4);

    const embedded<TestType> subject(source);

    SECTION("the identity is the identity in both")
    {
        mat3 a; a.set_to_identity();
        mat4 b; b.set_to_identity();

        require_same_block(a, b);

        REQUIRE(a == mat3::identity);
        REQUIRE(b == mat4::identity);
    }

    SECTION("index convention: get takes the column first in both")
    {
        mat3 a; a.set_to_identity(); a.set(2, 0, static_cast<TestType>(7));
        mat4 b; b.set_to_identity(); b.set(2, 0, static_cast<TestType>(7));

        REQUIRE(a.get(2, 0) == Approx(7.0));
        REQUIRE(b.get(2, 0) == Approx(7.0));

        REQUIRE((&a.front())[2 * mat3::order + 0] == Approx(7.0));
        REQUIRE((&b.front())[2 * mat4::order + 0] == Approx(7.0));
    }

    SECTION("determinant")
    {
        REQUIRE(subject.four.determinant() == Approx(subject.three.determinant()));
        REQUIRE(subject.three.determinant() == Approx(25.0));
    }

    SECTION("transposed")
    {
        require_same_block(subject.three.transposed(), subject.four.transposed());
    }

    SECTION("transpose mutates, transposed does not")
    {
        mat3 a = subject.three;
        mat4 b = subject.four;

        const auto a_copy = a.transposed();
        const auto b_copy = b.transposed();

        REQUIRE(a == subject.three);
        REQUIRE(b == subject.four);

        a.transpose();
        b.transpose();

        REQUIRE(a == a_copy);
        REQUIRE(b == b_copy);
    }

    SECTION("inversed")
    {
        require_same_block(subject.three.inversed(), subject.four.inversed());
    }

    SECTION("inverse mutates, inversed does not")
    {
        mat3 a = subject.three;
        mat4 b = subject.four;

        const auto a_copy = a.inversed();
        const auto b_copy = b.inversed();

        REQUIRE(a == subject.three);
        REQUIRE(b == subject.four);

        a.inverse();
        b.inverse();

        REQUIRE(a == a_copy);
        REQUIRE(b == b_copy);
    }

    SECTION("an inverse times the original is the identity, in both")
    {
        const auto a = subject.three * subject.three.inversed();
        const auto b = subject.four * subject.four.inversed();

        require_same_block(mat3::identity, b);

        for (std::size_t column = 0; column < 3; ++column)
            for (std::size_t row = 0; row < 3; ++row)
                REQUIRE(a.get(column, row) ==
                    Approx(mat3::identity.get(column, row)).margin(1e-5));
    }

    SECTION("**inverting a singular matrix throws the same exception type in both**")
    {
        const auto singularSource = made<TestType>(
            1, 2, 3,
            2, 4, 6,     
            0, 1, 4);

        const embedded<TestType> singular(singularSource);

        REQUIRE(singular.three.determinant() == Approx(0.0).margin(1e-6));
        REQUIRE(singular.four.determinant() == Approx(0.0).margin(1e-6));

        auto a = singular.three;
        auto b = singular.four;

        REQUIRE_THROWS_AS(a.inverse(), std::domain_error);
        REQUIRE_THROWS_AS(b.inverse(), std::domain_error);

        REQUIRE_THROWS_AS(singular.three.inversed(), std::domain_error);
        REQUIRE_THROWS_AS(singular.four.inversed(), std::domain_error);
    }

    SECTION("multiply composes in the same order in both")
    {
        const auto otherSource = made<TestType>(
            1, 2, 0,
            0, 1, 3,
            4, 0, 1);

        const embedded<TestType> other(otherSource);

        const auto a = subject.three * other.three;
        const auto b = subject.four * other.four;

        require_same_block(a, b);

        const auto reversed = other.three * subject.three;
        REQUIRE_FALSE(a == reversed);
    }

    SECTION("multiply and operator*= agree with each other in both")
    {
        mat3 a = subject.three;
        mat4 b = subject.four;

        a.multiply(subject.three);
        b.multiply(subject.four);

        mat3 c = subject.three;
        mat4 d = subject.four;

        c *= subject.three;
        d *= subject.four;

        REQUIRE(a == c);
        REQUIRE(b == d);
    }

    SECTION("equality is exact and != is its negation in both")
    {
        mat3 a = subject.three;
        mat4 b = subject.four;

        REQUIRE(a == subject.three);
        REQUIRE(b == subject.four);
        REQUIRE_FALSE(a != subject.three);
        REQUIRE_FALSE(b != subject.four);

        a.set(0, 0, a.get(0, 0) + static_cast<TestType>(1));
        b.set(0, 0, b.get(0, 0) + static_cast<TestType>(1));

        REQUIRE(a != subject.three);
        REQUIRE(b != subject.four);
        REQUIRE_FALSE(a == subject.three);
        REQUIRE_FALSE(b == subject.four);
    }

    SECTION("order names the side length in both")
    {
        REQUIRE(mat3::order == 3);
        REQUIRE(mat4::order == 4);
    }
}
