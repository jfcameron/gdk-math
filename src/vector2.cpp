// © 2018 Joseph Cameron - All Rights Reserved

#include <gdk/vector2.h>

#include <nlohmann/json.hpp>

#include <iostream>
#include <cmath>

using namespace gdk;

static constexpr char TAG[] = "Vector2";

/*std::ostream &gdk::operator<<(std::ostream &s, const gdk::Vector2 &a)
{
    return s << nlohmann::json
    {
        {"Type", TAG},
        
        {"x", a.x},
        {"y", a.y},
    }
    .dump();
}*/