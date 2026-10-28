// © Joseph Cameron - MIT License

#include <gdk/math.h>

#include <array>
#include <cmath>

int main() {
    using namespace gdk::math;

    const auto rotation = quaternion<float>::from_angle_axis(numbers::pi_f / 2, vector3<float>::up);
    const matrix4x4<float> transform({1, 2, 3}, rotation, {2, 2, 2});

    std::array<vector3<float>, 5> points{};
    transform_points(transform, points);

    const auto back = transform.inversed() * points[4];

    return back.length() < 1e-5f ? 0 : 1;
}
