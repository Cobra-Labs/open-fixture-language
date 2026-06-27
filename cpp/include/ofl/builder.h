//
// Created by janwin443 on 6/27/26.
//

#pragma once
#include "nodes.h"
#include "types.h"

namespace ofl {

class Builder {
public:
    static Fixture build(const FixtureNode& node);

private:
    static FixtureChannel build_channel(const ChannelNode& node);
    static Capability     build_cap(const CapNode& node);
    static FixtureMode    build_mode(const ModeNode& node);
    static std::string    generate_id();
};

} // namespace ofl