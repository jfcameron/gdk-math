// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_MATH_BATCH_H
#define GDK_MATH_MATH_BATCH_H

#include <gdk/math_ops.h>

#include <span>
#include <type_traits>

#ifndef GDK_MATH_SIMD
#define GDK_MATH_SIMD 1
#endif

#if GDK_MATH_SIMD && (defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2))
#define GDK_MATH_BATCH_SSE2 1
#else
#define GDK_MATH_BATCH_SSE2 0
#endif

namespace gdk {
    /// \brief The batch functions below are the throughput half of this library's interface.
    template<typename component_type>
    using batch_source = std::type_identity_t<std::span<const vector3<component_type>>>;

    template<typename component_type>
    using batch_destination = std::type_identity_t<std::span<vector3<component_type>>>;

    /// \brief transform points by an **affine** transform, without the perspective divide
    template<floating_point_component component_type>
    void transform_points(const matrix4x4<component_type> &aTransform,
        batch_source<component_type> aSource, batch_destination<component_type> aDestination);

    //! in place, for a caller that does not need the originals
    template<floating_point_component component_type>
    void transform_points(const matrix4x4<component_type> &aTransform,
        batch_destination<component_type> aPoints);

    /// \brief transform points by any transform, perspective divide included
    ///
    /// Elementwise identical to `aTransform * aPoint`
    template<floating_point_component component_type>
    void project_points(const matrix4x4<component_type> &aTransform,
        batch_source<component_type> aSource, batch_destination<component_type> aDestination);

    /// \brief transform directions: rotation and scale, no translation
    template<floating_point_component component_type>
    void transform_directions(const matrix4x4<component_type> &aTransform,
        batch_source<component_type> aSource, batch_destination<component_type> aDestination);

    /// \brief transform normals, by the inverse transpose of the upper-left 3x3
    template<floating_point_component component_type>
    void transform_normals(const matrix4x4<component_type> &aTransform,
        batch_source<component_type> aSource, batch_destination<component_type> aDestination);

    //! rotate directions by a unit quaternion
    template<floating_point_component component_type>
    void rotate_directions(const quaternion<component_type> &aRotation,
        batch_source<component_type> aSource, batch_destination<component_type> aDestination);
}

#include <gdk/math_batch.inl> // varies by implementation

#endif
