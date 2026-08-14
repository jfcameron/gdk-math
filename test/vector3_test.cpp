// © 2018 Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/vector3.h>

#include <cmath>
#include <stdexcept>

using namespace gdk;

TEMPLATE_LIST_TEST_CASE("vector3 constructors and constants", "[vector3]", type::floating_point)
{
    using component_type = TestType;
    using vec = vector3<component_type>;

    SECTION("the default is zero")
    {
        REQUIRE(vec() == vec::zero);
        REQUIRE(vec(0, 0, 0) == vec::zero);
    }

    SECTION("the named directions are the axes they claim")
    {
        REQUIRE(vec::right == vec(1, 0, 0));
        REQUIRE(vec::left == vec(-1, 0, 0));
        REQUIRE(vec::up == vec(0, 1, 0));
        REQUIRE(vec::down == vec(0, -1, 0));
        REQUIRE(vec::one == vec(1, 1, 1));

        REQUIRE(vec::forward == vec(0, 0, -1));
        REQUIRE(vec::backward == vec(0, 0, 1));

        REQUIRE(vec::right.cross_product(vec::up) == vec::backward);
    }
}

TEMPLATE_LIST_TEST_CASE("vector3 magnitude", "[vector3]", type::floating_point)
{
    using component_type = TestType;
    using vec = vector3<component_type>;

    SECTION("length and length_squared agree")
    {
        const vec v(3, 4, 0);

        REQUIRE(v.length() == Approx(5.0f));
        REQUIRE(v.length_squared() == Approx(25.0f));
        REQUIRE(v.length_squared() == Approx(v.length() * v.length()));
    }

    SECTION("distance_from is the length of the difference, and is symmetric")
    {
        const vec a(1, 2, 3);
        const vec b(4, 6, 3);

        REQUIRE(a.distance_from(b) == Approx(5.0f));
        REQUIRE(b.distance_from(a) == Approx(5.0f));
        REQUIRE(a.distance_from(a) == Approx(0.0f));
    }

    SECTION("is_effectively_zero tests the SQUARED length against its threshold")
    {
        const auto threshold = numbers::effectively_zero_length_squared_v<component_type>;
        const auto cutoff = std::sqrt(threshold);

        REQUIRE(vec(static_cast<component_type>(cutoff / 10), 0, 0).is_effectively_zero());
        REQUIRE_FALSE(vec(static_cast<component_type>(cutoff * 10), 0, 0).is_effectively_zero());

        REQUIRE(vec::zero.is_effectively_zero());

        REQUIRE(std::sqrt(numbers::effectively_zero_length_squared_v<float>) == Approx(1e-3f));

        REQUIRE(numbers::effectively_zero_length_squared_v<double>
            < numbers::effectively_zero_length_squared_v<float>);
        REQUIRE(numbers::effectively_zero_length_squared_v<long double>
            <= numbers::effectively_zero_length_squared_v<double>);
    }
}

TEMPLATE_LIST_TEST_CASE("vector3 normalisation", "[vector3]", type::floating_point)
{
    using component_type = TestType;
    using vec = vector3<component_type>;

    SECTION("normal returns unit length and leaves the receiver alone")
    {
        const vec v(3, 4, 0);
        const auto unit = v.normal();

        REQUIRE(unit.length() == Approx(1.0f));
        REQUIRE(unit.x == Approx(0.6f));
        REQUIRE(unit.y == Approx(0.8f));
        REQUIRE(v.x == Approx(3.0f));   // unchanged
    }

    SECTION("normalize modifies in place and returns itself")
    {
        vec v(0, 0, 7);
        const auto &returned = v.normalize();

        REQUIRE(v == vec(0, 0, 1));
        REQUIRE(&returned == &v);
    }

    SECTION("normal and normalize agree for every input, including very short vectors")
    {
        const vec inputs[] = {
            {1e-5f, 0, 0}, {1e-4f, 0, 0}, {1e-3f, 0, 0},   
            {0.5f, 0.5f, 0.5f}, {3, 4, 0}, {-7, 0, 2},    
            {0, 0, 0}};                                  

        for (const auto &input : inputs) {
            vec mutated = input;
            mutated.normalize();

            const auto viaConst = input.normal();

            REQUIRE(viaConst.x == Approx(mutated.x));
            REQUIRE(viaConst.y == Approx(mutated.y));
            REQUIRE(viaConst.z == Approx(mutated.z));
        }
    }

    SECTION("a vector too short to have a reliable direction normalises to zero, both ways")
    {
        const auto cutoff = std::sqrt(numbers::effectively_zero_length_squared_v<component_type>);
        const vec tiny(static_cast<component_type>(cutoff / 10), 0, 0);

        REQUIRE(tiny.normal() == vec::zero);

        vec mutated = tiny;
        mutated.normalize();
        REQUIRE(mutated == vec::zero);

        const vec justOver(static_cast<component_type>(cutoff * 2), 0, 0);
        REQUIRE(justOver.normal().length() == Approx(1.0f));

        vec alsoJustOver = justOver;
        alsoJustOver.normalize();
        REQUIRE(alsoJustOver.length() == Approx(1.0f));
    }

    SECTION("normalising exact zero yields zero rather than NaN, both ways")
    {
        REQUIRE(vec::zero.normal() == vec::zero);

        vec zero = vec::zero;
        zero.normalize();
        REQUIRE(std::isfinite(zero.x));
        REQUIRE(zero == vec::zero);
    }
}

TEMPLATE_LIST_TEST_CASE("vector3 products", "[vector3]", type::floating_point)
{
    using component_type = TestType;
    using vec = vector3<component_type>;

    SECTION("dot_product is commutative, and zero for perpendicular vectors")
    {
        const vec a(1, 2, 3);
        const vec b(4, -5, 6);

        REQUIRE(a.dot_product(b) == Approx(12.0f));
        REQUIRE(b.dot_product(a) == Approx(12.0f));
        REQUIRE(vec::right.dot_product(vec::up) == Approx(0.0f));
        REQUIRE(vec::right.dot_product(vec::right) == Approx(1.0f));
        REQUIRE(vec::right.dot_product(vec::left) == Approx(-1.0f));
    }

    SECTION("cross_product is right handed")
    {
        REQUIRE(vec::right.cross_product(vec::up) == vec(0, 0, 1));
        REQUIRE(vec::up.cross_product(vec(0, 0, 1)) == vec::right);
        REQUIRE(vec(0, 0, 1).cross_product(vec::right) == vec::up);
    }

    SECTION("cross_product is anticommutative, and zero for parallel vectors")
    {
        const vec a(1, 2, 3);
        const vec b(4, 5, 6);

        const auto ab = a.cross_product(b);
        const auto ba = b.cross_product(a);

        REQUIRE(ab.x == Approx(-ba.x));
        REQUIRE(ab.y == Approx(-ba.y));
        REQUIRE(ab.z == Approx(-ba.z));

        REQUIRE(a.cross_product(a).is_effectively_zero());

        REQUIRE(ab.dot_product(a) == Approx(0.0f).margin(1e-4));
        REQUIRE(ab.dot_product(b) == Approx(0.0f).margin(1e-4));
    }

    SECTION("element_wise_product multiplies component by component")
    {
        const auto product = vec(2, 3, 4).element_wise_product(vec(5, 6, 7));

        REQUIRE(product == vec(10, 18, 28));
    }
}

TEMPLATE_LIST_TEST_CASE("vector3 operators", "[vector3]", type::floating_point)
{
    using component_type = TestType;
    using vec = vector3<component_type>;

    SECTION("compound assignment")
    {
        vec v(1, 2, 3);

        v += vec(1, 1, 1);
        REQUIRE(v == vec(2, 3, 4));

        v -= vec(1, 1, 1);
        REQUIRE(v == vec(1, 2, 3));

        v *= 2.0f;
        REQUIRE(v == vec(2, 4, 6));

        v /= vec(2, 2, 2);
        REQUIRE(v == vec(1, 2, 3));
    }

    SECTION("dividing by a vector with a zero component throws")
    {
        vec v(1, 2, 3);

        REQUIRE_THROWS_AS(v /= vec(1, 0, 1), std::domain_error);
    }

    SECTION("subscripting reaches the components in x, y, z order")
    {
        vec v(7, 8, 9);

        REQUIRE(v[0] == Approx(7.0f));
        REQUIRE(v[1] == Approx(8.0f));
        REQUIRE(v[2] == Approx(9.0f));

        v[1] = 12.0f;
        REQUIRE(v.y == Approx(12.0f));
    }

    SECTION("equality is exact, component-wise")
    {
        REQUIRE(vec(1, 2, 3) == vec(1, 2, 3));
        REQUIRE(vec(1, 2, 3) != vec(1, 2, 3.0001f));
    }

    SECTION("min and max are component-wise, not by magnitude")
    {
        const vec a(1, 5, 3);
        const vec b(4, 2, 6);

        REQUIRE(vec::min(a, b) == vec(1, 2, 3));
        REQUIRE(vec::max(a, b) == vec(4, 5, 6));
    }
}
