// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_IMPL_STD_MATH_BATCH_INL
#define GDK_MATH_IMPL_STD_MATH_BATCH_INL

#include <cstddef>
#include <stdexcept>

namespace gdk {
    namespace math_batch_detail {
        inline void require_room(const std::size_t aSourceSize, const std::size_t aDestinationSize) {
            if (aDestinationSize < aSourceSize)
                throw std::invalid_argument(
                    "gdk math batch: destination is smaller than source");
        }
    }

    template<floating_point_component component_type>
    void transform_points(const matrix4x4<component_type> &aTransform,
        batch_source<component_type> aSource, batch_destination<component_type> aDestination) {
        math_batch_detail::require_room(aSource.size(), aDestination.size());

        for (std::size_t i = 0; i < aSource.size(); ++i) {
            const auto &p = aSource[i];

            aDestination[i] = {
                aTransform.get(0, 0) * p.x + aTransform.get(1, 0) * p.y
                    + aTransform.get(2, 0) * p.z + aTransform.get(3, 0),
                aTransform.get(0, 1) * p.x + aTransform.get(1, 1) * p.y
                    + aTransform.get(2, 1) * p.z + aTransform.get(3, 1),
                aTransform.get(0, 2) * p.x + aTransform.get(1, 2) * p.y
                    + aTransform.get(2, 2) * p.z + aTransform.get(3, 2)};
        }
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

        for (std::size_t i = 0; i < aSource.size(); ++i) aDestination[i] = aTransform * aSource[i];
    }

    template<floating_point_component component_type>
    void transform_directions(const matrix4x4<component_type> &aTransform,
        batch_source<component_type> aSource, batch_destination<component_type> aDestination) {
        math_batch_detail::require_room(aSource.size(), aDestination.size());

        const auto linear = upper_left(aTransform); 

        for (std::size_t i = 0; i < aSource.size(); ++i) aDestination[i] = linear * aSource[i];
    }

    template<floating_point_component component_type>
    void transform_normals(const matrix4x4<component_type> &aTransform,
        batch_source<component_type> aSource, batch_destination<component_type> aDestination) {
        math_batch_detail::require_room(aSource.size(), aDestination.size());

        const auto normals = normal_matrix(aTransform); 

        for (std::size_t i = 0; i < aSource.size(); ++i) aDestination[i] = normals * aSource[i];
    }

    template<floating_point_component component_type>
    void rotate_directions(const quaternion<component_type> &aRotation,
        batch_source<component_type> aSource, batch_destination<component_type> aDestination) {
        math_batch_detail::require_room(aSource.size(), aDestination.size());

        for (std::size_t i = 0; i < aSource.size(); ++i) aDestination[i] = aRotation * aSource[i];
    }
}

#endif
