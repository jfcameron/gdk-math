// © Joseph Cameron - All Rights Reserved

/// \file used to compare the performance of different implementations

#include <gdk/math.h>

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#ifndef GDK_MATH_BENCH_BACKEND
#define GDK_MATH_BENCH_BACKEND "unknown"
#endif

using namespace gdk;

namespace {
    using vec3 = vector3<float>;
    using vec4 = vector4<float>;
    using quat = quaternion<float>;
    using mat3 = matrix3x3<float>;
    using mat4 = matrix4x4<float>;

    inline void clobber() { asm volatile("" : : : "memory"); }

    template <typename value_type>
    inline void escape(value_type &aValue) { asm volatile("" : : "g"(&aValue) : "memory"); }

    struct random_source final {
        std::uint32_t state;

        explicit random_source(const std::uint32_t aSeed) : state(aSeed) {}

        float next() {
            state = state * 1664525u + 1013904223u;

            return static_cast<float>(state >> 8) / static_cast<float>(1u << 24) * 2.0f - 1.0f;
        }
    };

    template <typename body_type>
    double ns_per_op(const std::size_t aCount, const std::size_t aPasses, body_type &&aBody) {
        double best = 1e30;

        for (std::size_t pass = 0; pass < aPasses; ++pass) {
            const auto start = std::chrono::steady_clock::now();

            aBody();

            clobber();

            const auto elapsed = std::chrono::duration<double, std::nano>(
                std::chrono::steady_clock::now() - start).count();

            best = std::min(best, elapsed / static_cast<double>(aCount));
        }

        return best;
    }

    double g_checksum = 0;

    void report(const char *aName, const double aNanoseconds) {
        std::printf("  %-34s %8.2f ns\n", aName, aNanoseconds);
    }
}

int main(int argc, char **argv) {
    const std::uint32_t seed = argc > 1 ? static_cast<std::uint32_t>(std::atoi(argv[1])) : 12345u;

    constexpr std::size_t COUNT = 1u << 16;
    constexpr std::size_t PASSES = 7;

    random_source rng(seed);

    std::vector<vec3> a3(COUNT), b3(COUNT);
    std::vector<vec4> a4(COUNT);
    std::vector<quat> aq(COUNT), bq(COUNT);
    std::vector<mat4> am(COUNT), bm(COUNT);
    std::vector<mat3> a33(COUNT);

    for (std::size_t i = 0; i < COUNT; ++i) {
        a3[i] = vec3(rng.next(), rng.next(), rng.next());
        b3[i] = vec3(rng.next(), rng.next(), rng.next());
        a4[i] = vec4(rng.next(), rng.next(), rng.next(), rng.next() + 2.0f);

        aq[i] = quat::from_euler(vec3(rng.next(), rng.next(), rng.next()));
        bq[i] = quat::from_euler(vec3(rng.next(), rng.next(), rng.next()));

        am[i].set_to_identity();
        am[i].set_rotation_and_scale(aq[i], vec3(1 + std::abs(rng.next()), 1, 1));
        am[i].set_translation(a3[i]);

        bm[i].set_to_identity();
        bm[i].set_rotation_and_scale(bq[i], vec3::one);
        bm[i].set_translation(b3[i]);

        a33[i] = upper_left(am[i]);
    }

    std::printf("gdk-math bench   backend=%s   count=%zu   passes=%zu   seed=%u\n\n",
        GDK_MATH_BENCH_BACKEND, COUNT, PASSES, seed);

    double sink = 0;

    std::puts("vector3");
    report("operator+", ns_per_op(COUNT, PASSES, [&]{
        vec3 acc;
        for (std::size_t i = 0; i < COUNT; ++i) acc = acc + a3[i];
        sink += acc.x + acc.y + acc.z;
    }));
    report("dot_product", ns_per_op(COUNT, PASSES, [&]{
        float acc = 0;
        for (std::size_t i = 0; i < COUNT; ++i) acc += a3[i].dot_product(b3[i]);
        sink += acc;
    }));
    report("cross_product", ns_per_op(COUNT, PASSES, [&]{
        float acc = 0;
        for (std::size_t i = 0; i < COUNT; ++i) { auto r = a3[i].cross_product(b3[i]); escape(r); acc += r.x; }
        sink += acc;
    }));
    report("length  (sqrt)", ns_per_op(COUNT, PASSES, [&]{
        float acc = 0;
        for (std::size_t i = 0; i < COUNT; ++i) acc += a3[i].length();
        sink += acc;
    }));
    report("normal  (sqrt)", ns_per_op(COUNT, PASSES, [&]{
        float acc = 0;
        for (std::size_t i = 0; i < COUNT; ++i) { auto r = a3[i].normal(); escape(r); acc += r.x; }
        sink += acc;
    }));

    std::puts("\nquaternion");
    report("operator* (compose)", ns_per_op(COUNT, PASSES, [&]{
        float acc = 0;
        for (std::size_t i = 0; i < COUNT; ++i) { auto r = aq[i] * bq[i]; escape(r); acc += r.w; }
        sink += acc;
    }));
    report("operator* vector3 (rotate)", ns_per_op(COUNT, PASSES, [&]{
        float acc = 0;
        for (std::size_t i = 0; i < COUNT; ++i) { auto r = aq[i] * a3[i]; escape(r); acc += r.x; }
        sink += acc;
    }));
    report("slerp   (acos, sin)", ns_per_op(COUNT, PASSES, [&]{
        float acc = 0;
        for (std::size_t i = 0; i < COUNT; ++i) { auto r = slerp(aq[i], bq[i], 0.35f); escape(r); acc += r.w; }
        sink += acc;
    }));
    report("nlerp   (sqrt)", ns_per_op(COUNT, PASSES, [&]{
        float acc = 0;
        for (std::size_t i = 0; i < COUNT; ++i) { auto r = nlerp(aq[i], bq[i], 0.35f); escape(r); acc += r.w; }
        sink += acc;
    }));

    std::puts("\nmatrix4x4");
    report("operator* (compose)", ns_per_op(COUNT, PASSES, [&]{
        float acc = 0;
        for (std::size_t i = 0; i < COUNT; ++i) { auto r = am[i] * bm[i]; escape(r); acc += r.get(3, 0); }
        sink += acc;
    }));
    report("operator* vector3 (point)", ns_per_op(COUNT, PASSES, [&]{
        float acc = 0;
        for (std::size_t i = 0; i < COUNT; ++i) { auto r = am[i] * a3[i]; escape(r); acc += r.x; }
        sink += acc;
    }));
    report("operator* vector4", ns_per_op(COUNT, PASSES, [&]{
        float acc = 0;
        for (std::size_t i = 0; i < COUNT; ++i) { auto r = am[i] * a4[i]; escape(r); acc += r.x; }
        sink += acc;
    }));
    report("set_rotation_and_scale", ns_per_op(COUNT, PASSES, [&]{
        mat4 m;
        float acc = 0;
        for (std::size_t i = 0; i < COUNT; ++i) {
            m.set_rotation_and_scale(aq[i], vec3::one);
            acc += m.get(0, 0);
        }
        sink += acc;
    }));
    report("inversed", ns_per_op(COUNT, PASSES, [&]{
        float acc = 0;
        for (std::size_t i = 0; i < COUNT; ++i) { auto r = am[i].inversed(); escape(r); acc += r.get(3, 0); }
        sink += acc;
    }));
    report("rotation  (4 sqrt + 9 div)", ns_per_op(COUNT, PASSES, [&]{
        float acc = 0;
        for (std::size_t i = 0; i < COUNT; ++i) { auto r = am[i].rotation(); escape(r); acc += r.w; }
        sink += acc;
    }));
    report("scale     (3 sqrt)", ns_per_op(COUNT, PASSES, [&]{
        float acc = 0;
        for (std::size_t i = 0; i < COUNT; ++i) { auto r = am[i].scale(); escape(r); acc += r.x; }
        sink += acc;
    }));

    std::puts("\nmatrix3x3");
    report("operator* (compose)", ns_per_op(COUNT, PASSES, [&]{
        float acc = 0;
        for (std::size_t i = 0; i < COUNT; ++i) { auto r = a33[i] * a33[i]; escape(r); acc += r.get(0, 0); }
        sink += acc;
    }));
    report("inversed", ns_per_op(COUNT, PASSES, [&]{
        float acc = 0;
        for (std::size_t i = 0; i < COUNT; ++i) { auto r = a33[i].inversed(); escape(r); acc += r.get(0, 0); }
        sink += acc;
    }));
    report("normal_matrix", ns_per_op(COUNT, PASSES, [&]{
        float acc = 0;
        for (std::size_t i = 0; i < COUNT; ++i) { auto r = normal_matrix(am[i]); escape(r); acc += r.get(0, 0); }
        sink += acc;
    }));

    std::puts("\nbatch: one matrix over many points");
    const mat4 single = am[0];
    report("transform N points, per point", ns_per_op(COUNT, PASSES, [&]{
        float acc = 0;
        for (std::size_t i = 0; i < COUNT; ++i) { auto r = single * a3[i]; escape(r); acc += r.x; }
        sink += acc;
    }));
    report("transform N directions, per one", ns_per_op(COUNT, PASSES, [&]{
        float acc = 0;
        const auto linear = upper_left(single);
        for (std::size_t i = 0; i < COUNT; ++i) { auto r = linear * a3[i]; escape(r); acc += r.x; }
        sink += acc;
    }));

    g_checksum = sink;

    std::printf("\nchecksum %.6f   (must differ for a different seed argument)\n", g_checksum);

    return 0;
}
