// © Joseph Cameron - All Rights Reserved

#include <jfc/catch.hpp>
#include <jfc/types.h>

#include <gdk/math.h>

using namespace gdk;

namespace {
    template<typename T>
    constexpr bool scalar_checks() {
        static_assert(to_radians(T(180)) > T(3.14159), "to_radians");
        static_assert(to_degrees(static_cast<T>(numbers::pi)) > T(179.99), "to_degrees");

        return true;
    }

    template<typename T>
    constexpr bool vector_checks() {
        constexpr vector3<T> a{1, 2, 3};
        constexpr vector3<T> b{4, 5, 6};

        static_assert((a + b).x == T(5), "vector3 addition");
        static_assert((b - a).z == T(3), "vector3 subtraction");
        static_assert((a * T(2)).y == T(4), "vector3 scaling");
        static_assert((b / T(2)).x == T(2), "vector3 division");
        static_assert((-a).x == T(-1), "vector3 negation");
        static_assert(a.length_squared() == T(14), "length_squared avoids the square root");
        static_assert(a.dot_product(b) == T(32), "dot_product");
        static_assert(a.cross_product(b).x == T(-3), "cross_product");
        static_assert(a.element_wise_product(b).y == T(10), "element_wise_product");
        static_assert(a[1] == T(2), "operator[]");
        static_assert(a != b && a == a, "equality");
        static_assert(vector3<T>::min(a, b).x == T(1), "min");
        static_assert(vector3<T>::max(a, b).x == T(4), "max");
        static_assert(lerp(a, b, T(0.5)).x == T(2.5), "lerp");
        static_assert(vector3<T>{0, -1, 0}.reflect(vector3<T>{0, 1, 0}).y == T(1), "reflect");
        static_assert(!vector3<T>{1, 0, 0}.is_effectively_zero(), "is_effectively_zero");
        static_assert(vector3<T>{0, 0, 0}.is_effectively_zero(), "is_effectively_zero, zero case");

        constexpr vector2<T> a2{3, 4};
        static_assert(a2.length_squared() == T(25), "vector2 length_squared");
        static_assert(a2.dot_product(vector2<T>{1, 0}) == T(3), "vector2 dot_product");
        static_assert(lerp(vector2<T>{0, 0}, vector2<T>{4, 8}, T(0.25)).y == T(2), "vector2 lerp");

        return true;
    }

    template<typename T>
    constexpr bool homogeneous_checks() {
        constexpr vector3<T> a{1, 2, 3};
        constexpr vector4<T> point{1, 2, 3, 1};
        constexpr vector4<T> clip{8, 4, -4, 4};

        static_assert(point.w == T(1), "the promoting default is a point");
        static_assert(vector4<T>::direction(a).w == T(0), "direction is w = 0");
        static_assert(point.xyz().y == T(2), "xyz");
        static_assert(clip.to_point().x == T(2), "to_point divides through by w");
        static_assert(vector4<T>::direction(a).to_point().x == T(1), "w = 0 returns xyz undivided");

        return true;
    }

    template<typename T>
    constexpr bool quaternion_checks() {
        constexpr quaternion<T> q{T(0.5), T(0.5), T(0.5), T(0.5)};

        static_assert(q.dot_product(q) == T(1), "quaternion dot_product");
        static_assert(q.inverse_unit().x == T(-0.5), "inverse_unit");
        static_assert((q * q).w == T(-0.5), "quaternion composition");
        static_assert((q * T(2)).w == T(1), "quaternion scaling");
        static_assert((q + q).x == T(1), "quaternion addition");
        static_assert((-q).w == T(-0.5), "quaternion negation");
        static_assert((q * vector3<T>{1, 0, 0}).y == T(1), "rotating a vector is constant evaluable");

        return true;
    }

    template<typename T>
    constexpr bool matrix_checks() {
        constexpr matrix4x4<T> transform = []{
            matrix4x4<T> m;
            m.set_to_identity();
            m.set_translation({10, 20, 30});

            return m;
        }();

        static_assert(transform.get(3, 0) == T(10), "set_translation and get");
        static_assert(transform.translation().z == T(30), "translation");
        static_assert((transform * vector4<T>{1, 2, 3, 1}).x == T(11), "matrix4x4 * vector4");
        static_assert((transform * vector3<T>{1, 2, 3}).y == T(22),
            "matrix4x4 * vector3, w divide included");
        static_assert((transform * vector4<T>::direction(vector3<T>{1, 2, 3})).x == T(1),
            "a direction is not translated");

        constexpr matrix4x4<T> rotated = []{
            matrix4x4<T> m;
            m.set_to_identity();
            m.set_rotation_and_scale(quaternion<T>{0, 0, 0, 1}, vector3<T>{2, 2, 2});
            m.set_translation({1, 1, 1});

            return m;
        }();

        static_assert(rotated.get(0, 0) == T(2), "set_rotation_and_scale");
        static_assert(rotated.translation().x == T(1), "translation survives the rotation");

        constexpr matrix4x4<T> composed = transform * rotated;
        static_assert(composed.get(3, 0) == T(11), "matrix4x4 composition");

        constexpr matrix4x4<T> transposed = transform.transposed();
        static_assert(transposed.get(0, 3) == T(10),
            "transpose, which needed a hand-rolled swap: std::swap is only constexpr from C++20");

        static_assert(transform.determinant() == T(1), "matrix4x4 determinant");
        static_assert(transform.inversed().get(3, 0) == T(-10), "matrix4x4 inversed");

        constexpr matrix3x3<T> scale3{2, 0, 0,
                                      0, 4, 0,
                                      0, 0, 8};

        static_assert(scale3.determinant() == T(64), "matrix3x3 determinant");
        static_assert(scale3.inversed().get(0, 0) == T(0.5), "matrix3x3 inverse");
        static_assert((scale3 * vector3<T>{1, 1, 1}).y == T(4), "matrix3x3 * vector3");
        static_assert(upper_left(transform).get(0, 0) == T(1), "upper_left");
        static_assert(normal_matrix(rotated).get(0, 0) == T(0.5),
            "normal_matrix -- inverse and transpose all the way down, at compile time");

        return true;
    }

    template<typename T>
    constexpr bool everything() {
        return scalar_checks<T>() && vector_checks<T>() && homogeneous_checks<T>()
            && quaternion_checks<T>() && matrix_checks<T>();
    }
}

static_assert(everything<float>(), "constant evaluation holds for float");
static_assert(everything<double>(), "constant evaluation holds for double");
static_assert(everything<long double>(), "constant evaluation holds for long double");

TEMPLATE_LIST_TEST_CASE("gdk-math constexpr", "[constexpr]", type::floating_point)
{
    using vec3 = vector3<TestType>;
    using quat = quaternion<TestType>;

    SECTION("the compile-time answers agree with the runtime ones")
    {
        constexpr vec3 a{1, 2, 3};
        constexpr vec3 b{4, 5, 6};

        REQUIRE(a.dot_product(b) == Approx(32.0));
        REQUIRE(vector4<TestType>{8, 4, -4, 4}.to_point().x == Approx(2.0));
    }

    SECTION("what is not constexpr is not an oversight")
    {
        const vec3 a{1, 2, 3};

        REQUIRE(a.length() == Approx(3.7416575));
        REQUIRE(a.normal().length() == Approx(1.0));
        REQUIRE(quat::from_angle_axis(to_radians(TestType(90)), vec3{0, 1, 0}).angle()
            == Approx(to_radians(90.0)));
    }
}
