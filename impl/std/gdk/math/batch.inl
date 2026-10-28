// © Joseph Cameron - MIT License

#ifndef GDK_MATH_IMPL_STD_BATCH_INL
#define GDK_MATH_IMPL_STD_BATCH_INL

#include <cstddef>
#include <stdexcept>
#include <type_traits>

#if GDK_MATH_BATCH_SSE2
#include <emmintrin.h>
#elif GDK_MATH_BATCH_NEON
#include <arm_neon.h>
#endif

namespace gdk::math {
    namespace math_batch_detail {
        inline void require_room(const std::size_t aSourceSize, const std::size_t aDestinationSize) {
            if (aDestinationSize < aSourceSize)
                throw std::invalid_argument(
                    "gdk math batch: destination is smaller than source");
        }

        template<typename component_type>
        struct affine_rows { component_type c[12]; };

        template<typename component_type>
        affine_rows<component_type> rows_of(const matrix4x4<component_type> &aMatrix) {
            return {{aMatrix.get(0, 0), aMatrix.get(1, 0), aMatrix.get(2, 0), aMatrix.get(3, 0),
                     aMatrix.get(0, 1), aMatrix.get(1, 1), aMatrix.get(2, 1), aMatrix.get(3, 1),
                     aMatrix.get(0, 2), aMatrix.get(1, 2), aMatrix.get(2, 2), aMatrix.get(3, 2)}};
        }

        template<typename component_type>
        affine_rows<component_type> rows_of(const matrix3x3<component_type> &aMatrix) {
            return {{aMatrix.get(0, 0), aMatrix.get(1, 0), aMatrix.get(2, 0), component_type(0),
                     aMatrix.get(0, 1), aMatrix.get(1, 1), aMatrix.get(2, 1), component_type(0),
                     aMatrix.get(0, 2), aMatrix.get(1, 2), aMatrix.get(2, 2), component_type(0)}};
        }

        template<bool translate, typename component_type>
        void affine_batch_scalar(const affine_rows<component_type> &aRows,
            const vector3<component_type> *aSource, vector3<component_type> *aDestination,
            const std::size_t aCount) {
            const auto *const c = aRows.c;

            for (std::size_t i = 0; i < aCount; ++i) {
                const auto p = aSource[i];

                if constexpr (translate)
                    aDestination[i] = {c[0] * p.x + c[1] * p.y + c[2] * p.z + c[3],
                                       c[4] * p.x + c[5] * p.y + c[6] * p.z + c[7],
                                       c[8] * p.x + c[9] * p.y + c[10] * p.z + c[11]};
                else
                    aDestination[i] = {c[0] * p.x + c[1] * p.y + c[2] * p.z,
                                       c[4] * p.x + c[5] * p.y + c[6] * p.z,
                                       c[8] * p.x + c[9] * p.y + c[10] * p.z};
            }
        }

#if GDK_MATH_BATCH_SSE2
        template<bool translate>
        void affine_batch_simd(const affine_rows<float> &aRows, const vector3<float> *aSource,
            vector3<float> *aDestination, const std::size_t aCount) {
            const auto *const c = aRows.c;

            const __m128 x0 = _mm_set1_ps(c[0]), x1 = _mm_set1_ps(c[1]), x2 = _mm_set1_ps(c[2]);
            const __m128 y0 = _mm_set1_ps(c[4]), y1 = _mm_set1_ps(c[5]), y2 = _mm_set1_ps(c[6]);
            const __m128 z0 = _mm_set1_ps(c[8]), z1 = _mm_set1_ps(c[9]), z2 = _mm_set1_ps(c[10]);
            const __m128 tx = _mm_set1_ps(c[3]), ty = _mm_set1_ps(c[7]), tz = _mm_set1_ps(c[11]);

            std::size_t i = 0;

            for (; i + 4 <= aCount; i += 4) {
                const __m128 px = _mm_setr_ps(aSource[i].x, aSource[i + 1].x, aSource[i + 2].x, aSource[i + 3].x);
                const __m128 py = _mm_setr_ps(aSource[i].y, aSource[i + 1].y, aSource[i + 2].y, aSource[i + 3].y);
                const __m128 pz = _mm_setr_ps(aSource[i].z, aSource[i + 1].z, aSource[i + 2].z, aSource[i + 3].z);

                __m128 rx = _mm_add_ps(_mm_add_ps(_mm_mul_ps(x0, px), _mm_mul_ps(x1, py)), _mm_mul_ps(x2, pz));
                __m128 ry = _mm_add_ps(_mm_add_ps(_mm_mul_ps(y0, px), _mm_mul_ps(y1, py)), _mm_mul_ps(y2, pz));
                __m128 rz = _mm_add_ps(_mm_add_ps(_mm_mul_ps(z0, px), _mm_mul_ps(z1, py)), _mm_mul_ps(z2, pz));

                if constexpr (translate) {
                    rx = _mm_add_ps(rx, tx);
                    ry = _mm_add_ps(ry, ty);
                    rz = _mm_add_ps(rz, tz);
                }

                alignas(16) float bx[4], by[4], bz[4];

                _mm_store_ps(bx, rx);
                _mm_store_ps(by, ry);
                _mm_store_ps(bz, rz);

                for (std::size_t k = 0; k < 4; ++k) aDestination[i + k] = {bx[k], by[k], bz[k]};
            }

            affine_batch_scalar<translate>(aRows, aSource + i, aDestination + i, aCount - i);
        }
#endif

#if GDK_MATH_BATCH_SSE2
        inline __m128 select(const __m128 aMask, const __m128 aWhenSet, const __m128 aWhenClear) {
            return _mm_or_ps(_mm_and_ps(aMask, aWhenSet), _mm_andnot_ps(aMask, aWhenClear));
        }

        inline void project_batch_simd(const matrix4x4<float> &aTransform,
            const vector3<float> *aSource, vector3<float> *aDestination, const std::size_t aCount) {
            const __m128 x0 = _mm_set1_ps(aTransform.get(0, 0)), x1 = _mm_set1_ps(aTransform.get(1, 0));
            const __m128 x2 = _mm_set1_ps(aTransform.get(2, 0)), x3 = _mm_set1_ps(aTransform.get(3, 0));
            const __m128 y0 = _mm_set1_ps(aTransform.get(0, 1)), y1 = _mm_set1_ps(aTransform.get(1, 1));
            const __m128 y2 = _mm_set1_ps(aTransform.get(2, 1)), y3 = _mm_set1_ps(aTransform.get(3, 1));
            const __m128 z0 = _mm_set1_ps(aTransform.get(0, 2)), z1 = _mm_set1_ps(aTransform.get(1, 2));
            const __m128 z2 = _mm_set1_ps(aTransform.get(2, 2)), z3 = _mm_set1_ps(aTransform.get(3, 2));
            const __m128 w0 = _mm_set1_ps(aTransform.get(0, 3)), w1 = _mm_set1_ps(aTransform.get(1, 3));
            const __m128 w2 = _mm_set1_ps(aTransform.get(2, 3)), w3 = _mm_set1_ps(aTransform.get(3, 3));

            const __m128 zero = _mm_setzero_ps(), one = _mm_set1_ps(1.0f);

            std::size_t i = 0;

            for (; i + 4 <= aCount; i += 4) {
                const __m128 px = _mm_setr_ps(aSource[i].x, aSource[i + 1].x, aSource[i + 2].x, aSource[i + 3].x);
                const __m128 py = _mm_setr_ps(aSource[i].y, aSource[i + 1].y, aSource[i + 2].y, aSource[i + 3].y);
                const __m128 pz = _mm_setr_ps(aSource[i].z, aSource[i + 1].z, aSource[i + 2].z, aSource[i + 3].z);

                const __m128 rx = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(x0, px), _mm_mul_ps(x1, py)), _mm_mul_ps(x2, pz)), x3);
                const __m128 ry = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(y0, px), _mm_mul_ps(y1, py)), _mm_mul_ps(y2, pz)), y3);
                const __m128 rz = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(z0, px), _mm_mul_ps(z1, py)), _mm_mul_ps(z2, pz)), z3);
                const __m128 rw = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(w0, px), _mm_mul_ps(w1, py)), _mm_mul_ps(w2, pz)), w3);

                const __m128 degenerate = _mm_cmpeq_ps(rw, zero);
                const __m128 divisor = select(degenerate, one, rw);

                alignas(16) float bx[4], by[4], bz[4];

                _mm_store_ps(bx, select(degenerate, rx, _mm_div_ps(rx, divisor)));
                _mm_store_ps(by, select(degenerate, ry, _mm_div_ps(ry, divisor)));
                _mm_store_ps(bz, select(degenerate, rz, _mm_div_ps(rz, divisor)));

                for (std::size_t k = 0; k < 4; ++k) aDestination[i + k] = {bx[k], by[k], bz[k]};
            }

            for (; i < aCount; ++i) aDestination[i] = aTransform * aSource[i];
        }

        inline void rotate_batch_simd(const quaternion<float> &aRotation,
            const vector3<float> *aSource, vector3<float> *aDestination, const std::size_t aCount) {
            const __m128 ax = _mm_set1_ps(aRotation.x), ay = _mm_set1_ps(aRotation.y);
            const __m128 az = _mm_set1_ps(aRotation.z), aw = _mm_set1_ps(aRotation.w);
            const __m128 two = _mm_set1_ps(2.0f);

            std::size_t i = 0;

            for (; i + 4 <= aCount; i += 4) {
                const __m128 vx = _mm_setr_ps(aSource[i].x, aSource[i + 1].x, aSource[i + 2].x, aSource[i + 3].x);
                const __m128 vy = _mm_setr_ps(aSource[i].y, aSource[i + 1].y, aSource[i + 2].y, aSource[i + 3].y);
                const __m128 vz = _mm_setr_ps(aSource[i].z, aSource[i + 1].z, aSource[i + 2].z, aSource[i + 3].z);

                // t = cross(axis, v) * 2
                const __m128 tx = _mm_mul_ps(_mm_sub_ps(_mm_mul_ps(ay, vz), _mm_mul_ps(az, vy)), two);
                const __m128 ty = _mm_mul_ps(_mm_sub_ps(_mm_mul_ps(az, vx), _mm_mul_ps(ax, vz)), two);
                const __m128 tz = _mm_mul_ps(_mm_sub_ps(_mm_mul_ps(ax, vy), _mm_mul_ps(ay, vx)), two);

                // v + t * w + cross(axis, t)
                const __m128 cx = _mm_sub_ps(_mm_mul_ps(ay, tz), _mm_mul_ps(az, ty));
                const __m128 cy = _mm_sub_ps(_mm_mul_ps(az, tx), _mm_mul_ps(ax, tz));
                const __m128 cz = _mm_sub_ps(_mm_mul_ps(ax, ty), _mm_mul_ps(ay, tx));

                alignas(16) float bx[4], by[4], bz[4];

                _mm_store_ps(bx, _mm_add_ps(_mm_add_ps(vx, _mm_mul_ps(tx, aw)), cx));
                _mm_store_ps(by, _mm_add_ps(_mm_add_ps(vy, _mm_mul_ps(ty, aw)), cy));
                _mm_store_ps(bz, _mm_add_ps(_mm_add_ps(vz, _mm_mul_ps(tz, aw)), cz));

                for (std::size_t k = 0; k < 4; ++k) aDestination[i + k] = {bx[k], by[k], bz[k]};
            }

            for (; i < aCount; ++i) aDestination[i] = aRotation * aSource[i];
        }
#elif GDK_MATH_BATCH_NEON
        template<bool translate>
        void affine_batch_simd(const affine_rows<float> &aRows, const vector3<float> *aSource,
            vector3<float> *aDestination, const std::size_t aCount) {
            const auto *const c = aRows.c;

            const float32x4_t x0 = vdupq_n_f32(c[0]), x1 = vdupq_n_f32(c[1]), x2 = vdupq_n_f32(c[2]);
            const float32x4_t y0 = vdupq_n_f32(c[4]), y1 = vdupq_n_f32(c[5]), y2 = vdupq_n_f32(c[6]);
            const float32x4_t z0 = vdupq_n_f32(c[8]), z1 = vdupq_n_f32(c[9]), z2 = vdupq_n_f32(c[10]);
            const float32x4_t tx = vdupq_n_f32(c[3]), ty = vdupq_n_f32(c[7]), tz = vdupq_n_f32(c[11]);

            std::size_t i = 0;

            for (; i + 4 <= aCount; i += 4) {
                const float32x4x3_t p = vld3q_f32(&aSource[i].x);

                float32x4x3_t r;

                if constexpr (translate) {
                    r.val[0] = x0 * p.val[0] + x1 * p.val[1] + x2 * p.val[2] + tx;
                    r.val[1] = y0 * p.val[0] + y1 * p.val[1] + y2 * p.val[2] + ty;
                    r.val[2] = z0 * p.val[0] + z1 * p.val[1] + z2 * p.val[2] + tz;
                } else {
                    r.val[0] = x0 * p.val[0] + x1 * p.val[1] + x2 * p.val[2];
                    r.val[1] = y0 * p.val[0] + y1 * p.val[1] + y2 * p.val[2];
                    r.val[2] = z0 * p.val[0] + z1 * p.val[1] + z2 * p.val[2];
                }

                vst3q_f32(&aDestination[i].x, r);
            }

            affine_batch_scalar<translate>(aRows, aSource + i, aDestination + i, aCount - i);
        }

        inline void project_batch_simd(const matrix4x4<float> &aTransform,
            const vector3<float> *aSource, vector3<float> *aDestination, const std::size_t aCount) {
            const float32x4_t x0 = vdupq_n_f32(aTransform.get(0, 0)), x1 = vdupq_n_f32(aTransform.get(1, 0));
            const float32x4_t x2 = vdupq_n_f32(aTransform.get(2, 0)), x3 = vdupq_n_f32(aTransform.get(3, 0));
            const float32x4_t y0 = vdupq_n_f32(aTransform.get(0, 1)), y1 = vdupq_n_f32(aTransform.get(1, 1));
            const float32x4_t y2 = vdupq_n_f32(aTransform.get(2, 1)), y3 = vdupq_n_f32(aTransform.get(3, 1));
            const float32x4_t z0 = vdupq_n_f32(aTransform.get(0, 2)), z1 = vdupq_n_f32(aTransform.get(1, 2));
            const float32x4_t z2 = vdupq_n_f32(aTransform.get(2, 2)), z3 = vdupq_n_f32(aTransform.get(3, 2));
            const float32x4_t w0 = vdupq_n_f32(aTransform.get(0, 3)), w1 = vdupq_n_f32(aTransform.get(1, 3));
            const float32x4_t w2 = vdupq_n_f32(aTransform.get(2, 3)), w3 = vdupq_n_f32(aTransform.get(3, 3));

            const float32x4_t zero = vdupq_n_f32(0.0f), one = vdupq_n_f32(1.0f);

            std::size_t i = 0;

            for (; i + 4 <= aCount; i += 4) {
                const float32x4x3_t p = vld3q_f32(&aSource[i].x);

                const float32x4_t rx = x0 * p.val[0] + x1 * p.val[1] + x2 * p.val[2] + x3 * one;
                const float32x4_t ry = y0 * p.val[0] + y1 * p.val[1] + y2 * p.val[2] + y3 * one;
                const float32x4_t rz = z0 * p.val[0] + z1 * p.val[1] + z2 * p.val[2] + z3 * one;
                const float32x4_t rw = w0 * p.val[0] + w1 * p.val[1] + w2 * p.val[2] + w3 * one;

                const uint32x4_t degenerate = vceqq_f32(rw, zero);
                const float32x4_t divisor = vbslq_f32(degenerate, one, rw);

                float32x4x3_t r;
                r.val[0] = vbslq_f32(degenerate, rx, rx / divisor);
                r.val[1] = vbslq_f32(degenerate, ry, ry / divisor);
                r.val[2] = vbslq_f32(degenerate, rz, rz / divisor);

                vst3q_f32(&aDestination[i].x, r);
            }

            for (; i < aCount; ++i) aDestination[i] = aTransform * aSource[i];
        }

        inline void rotate_batch_simd(const quaternion<float> &aRotation,
            const vector3<float> *aSource, vector3<float> *aDestination, const std::size_t aCount) {
            const float32x4_t ax = vdupq_n_f32(aRotation.x), ay = vdupq_n_f32(aRotation.y);
            const float32x4_t az = vdupq_n_f32(aRotation.z), aw = vdupq_n_f32(aRotation.w);
            const float32x4_t two = vdupq_n_f32(2.0f);

            std::size_t i = 0;

            for (; i + 4 <= aCount; i += 4) {
                const float32x4x3_t v = vld3q_f32(&aSource[i].x);

                const float32x4_t cx = ay * v.val[2] - az * v.val[1];
                const float32x4_t cy = az * v.val[0] - ax * v.val[2];
                const float32x4_t cz = ax * v.val[1] - ay * v.val[0];

                const float32x4_t tx = cx * two, ty = cy * two, tz = cz * two;

                const float32x4_t wx = tx * aw, wy = ty * aw, wz = tz * aw;

                const float32x4_t sx = v.val[0] + wx, sy = v.val[1] + wy, sz = v.val[2] + wz;

                const float32x4_t ux = ay * tz - az * ty;
                const float32x4_t uy = az * tx - ax * tz;
                const float32x4_t uz = ax * ty - ay * tx;

                float32x4x3_t r;
                r.val[0] = sx + ux;
                r.val[1] = sy + uy;
                r.val[2] = sz + uz;

                vst3q_f32(&aDestination[i].x, r);
            }

            for (; i < aCount; ++i) aDestination[i] = aRotation * aSource[i];
        }
#endif

        template<typename component_type>
        void project_batch_scalar(const matrix4x4<component_type> &aTransform,
            const vector3<component_type> *aSource, vector3<component_type> *aDestination,
            const std::size_t aCount) {
            for (std::size_t i = 0; i < aCount; ++i) aDestination[i] = aTransform * aSource[i];
        }

        template<typename component_type>
        void rotate_batch_scalar(const quaternion<component_type> &aRotation,
            const vector3<component_type> *aSource, vector3<component_type> *aDestination,
            const std::size_t aCount) {
            for (std::size_t i = 0; i < aCount; ++i) aDestination[i] = aRotation * aSource[i];
        }

        //! picks the kernel: SIMD where the target has it and the component type is the one it
        //! is written for, the plain loop everywhere else
        template<bool translate, typename component_type>
        void affine_batch(const affine_rows<component_type> &aRows,
            const vector3<component_type> *aSource, vector3<component_type> *aDestination,
            const std::size_t aCount) {
#if GDK_MATH_BATCH_SIMD
            if constexpr (std::is_same_v<component_type, float>)
                affine_batch_simd<translate>(aRows, aSource, aDestination, aCount);
            else
                affine_batch_scalar<translate>(aRows, aSource, aDestination, aCount);
#else
            affine_batch_scalar<translate>(aRows, aSource, aDestination, aCount);
#endif
        }
    }

    template<floating_point_component component_type>
    void transform_points(const matrix4x4<component_type> &aTransform,
        batch_source<component_type> aSource, batch_destination<component_type> aDestination) {
        math_batch_detail::require_room(aSource.size(), aDestination.size());

        math_batch_detail::affine_batch<true>(math_batch_detail::rows_of(aTransform),
            aSource.data(), aDestination.data(), aSource.size());
    }

    template<floating_point_component component_type>
    void transform_points(const matrix4x4<component_type> &aTransform,
        batch_destination<component_type> aPoints) {
        transform_points(aTransform, batch_source<component_type>(aPoints), aPoints);
    }

    template<floating_point_component component_type>
    void project_points(const matrix4x4<component_type> &aTransform,
        batch_source<component_type> aSource, batch_destination<component_type> aDestination) {
        math_batch_detail::require_room(aSource.size(), aDestination.size());

#if GDK_MATH_BATCH_SIMD
        if constexpr (std::is_same_v<component_type, float>)
            math_batch_detail::project_batch_simd(aTransform, aSource.data(), aDestination.data(),
                aSource.size());
        else
            math_batch_detail::project_batch_scalar(aTransform, aSource.data(), aDestination.data(),
                aSource.size());
#else
        math_batch_detail::project_batch_scalar(aTransform, aSource.data(), aDestination.data(),
            aSource.size());
#endif
    }

    template<floating_point_component component_type>
    void transform_directions(const matrix4x4<component_type> &aTransform,
        batch_source<component_type> aSource, batch_destination<component_type> aDestination) {
        math_batch_detail::require_room(aSource.size(), aDestination.size());

        math_batch_detail::affine_batch<false>(math_batch_detail::rows_of(upper_left(aTransform)),
            aSource.data(), aDestination.data(), aSource.size());
    }

    template<floating_point_component component_type>
    void transform_normals(const matrix4x4<component_type> &aTransform,
        batch_source<component_type> aSource, batch_destination<component_type> aDestination) {
        math_batch_detail::require_room(aSource.size(), aDestination.size());

        math_batch_detail::affine_batch<false>(math_batch_detail::rows_of(normal_matrix(aTransform)),
            aSource.data(), aDestination.data(), aSource.size());
    }

    template<floating_point_component component_type>
    void rotate_directions(const quaternion<component_type> &aRotation,
        batch_source<component_type> aSource, batch_destination<component_type> aDestination) {
        math_batch_detail::require_room(aSource.size(), aDestination.size());

#if GDK_MATH_BATCH_SIMD
        if constexpr (std::is_same_v<component_type, float>)
            math_batch_detail::rotate_batch_simd(aRotation, aSource.data(), aDestination.data(),
                aSource.size());
        else
            math_batch_detail::rotate_batch_scalar(aRotation, aSource.data(), aDestination.data(),
                aSource.size());
#else
        math_batch_detail::rotate_batch_scalar(aRotation, aSource.data(), aDestination.data(),
            aSource.size());
#endif
    }
}

#endif
