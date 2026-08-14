// © Joseph Cameron - All Rights Reserved

#ifndef GDK_MATH_VECTOR4_H
#define GDK_MATH_VECTOR4_H

#include <gdk/storage.inl> // varies by implementation

#include <gdk/vector3.h>

#include <iosfwd>

namespace gdk {
    /// \brief a homogeneous coordinate: a 3d position or direction plus w
    /// - w = 1 marks a position, so a transform's translation applies to it
    /// - w = 0 marks a direction, so it does not
    template<typename component_type_param = float>
    class vector4 final : public vector4_storage<component_type_param> {
    public:
        using component_type = component_type_param;

        using vector4_storage<component_type_param>::x;
        using vector4_storage<component_type_param>::y;
        using vector4_storage<component_type_param>::z;
        using vector4_storage<component_type_param>::w;

        //! the three components as they stand, with no divide. Correct for a direction.
        [[nodiscard]] constexpr vector3<component_type> xyz() const;

        //! the euclidean point this represents: xyz divided through by w. A w of 0 divides by nothing.
        [[nodiscard]] constexpr vector3<component_type> to_point() const;

        [[nodiscard]] constexpr bool operator==(const vector4<component_type> &other) const;
        [[nodiscard]] constexpr bool operator!=(const vector4<component_type> &other) const;

        vector4<component_type> &operator=(const vector4<component_type> &) = default;

        constexpr vector4(const component_type &aX, const component_type &aY,
            const component_type &aZ, const component_type &aW = 1.);

        //! extend a vector3, defaulting w to 1
        constexpr vector4(const vector3<component_type> &avector3,
            const component_type &aW = 1.);

        //! make a direction from a vector3 
        [[nodiscard]] static constexpr vector4<component_type> direction(const vector3<component_type> &aVector);

        constexpr vector4();
        vector4(const vector4<component_type> &) = default;
        vector4(vector4<component_type> &&) = default;
        ~vector4() = default;

        //! the origin as a point: {0, 0, 0, 1}. 
        static const vector4<component_type> origin;
    };
}

#include <gdk/vector4.inl> // varies by implementation

#endif
