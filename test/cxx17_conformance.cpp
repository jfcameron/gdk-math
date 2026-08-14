// © Joseph Cameron - All Rights Reserved

#include <gdk/math.h>

namespace gdk {
    template class vector2<float>;
    template class vector2<double>;
    template class vector2<long double>;

    template class vector3<float>;
    template class vector3<double>;
    template class vector3<long double>;

    template class vector4<float>;
    template class vector4<double>;
    template class vector4<long double>;

    template class quaternion<float>;
    template class quaternion<double>;
    template class quaternion<long double>;

    template class matrix3x3<float>;
    template class matrix3x3<double>;
    template class matrix3x3<long double>;

    template class matrix4x4<float>;
    template class matrix4x4<double>;
    template class matrix4x4<long double>;
}

namespace {
    template<typename component_type>
    void use_every_free_function() {
        const gdk::vector2<component_type> a2(1, 2), b2(3, 4);
        const gdk::vector3<component_type> a3(1, 2, 3), b3(4, 5, 6);
        const gdk::quaternion<component_type> q(0, 0, 0, 1);

        gdk::matrix4x4<component_type> m;
        m.set_to_identity();
        m.set_translation({1, 2, 3});

        gdk::matrix3x3<component_type> m3;
        m3.set_to_identity();

        (void)lerp(a2, b2, static_cast<component_type>(0.5));
        (void)lerp(a3, b3, static_cast<component_type>(0.5));
        (void)slerp(q, q, static_cast<component_type>(0.5));
        (void)nlerp(q, q, static_cast<component_type>(0.5));
        (void)(q * q);
        (void)(q * a3);
        (void)(m * a3);
        (void)(m * gdk::vector4<component_type>(a3));
        (void)(m3 * a3);
        (void)upper_left(m);
        (void)normal_matrix(m);
        (void)gdk::to_radians(static_cast<component_type>(90));
        (void)gdk::to_degrees(static_cast<component_type>(1));
        (void)gdk::numbers::pi_v<component_type>;
    }

    void use_all() {
        use_every_free_function<float>();
        use_every_free_function<double>();
        use_every_free_function<long double>();
    }
}

namespace gdk_math_conformance {
    void anchor();
    void anchor() { use_all(); }
}
