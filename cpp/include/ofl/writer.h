//
// Created by janwin443 on 6/27/26.
//

#pragma once
#include <string>
#include "types.h"

namespace ofl {

class Writer {
public:
    static std::string write(const Fixture& fixture);

private:
    static std::string write_channel(const FixtureChannel& ch);
    static std::string write_cap(const Capability& cap);
    static std::string write_mode(const FixtureMode& mode);

    static constexpr const char* INDENT = "    ";
};

} // namespace ofl