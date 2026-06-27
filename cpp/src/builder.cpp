//
// Created by janwin443 on 6/27/26.
//

#include "ofl/builder.h"
#include <random>
#include <sstream>
#include <iomanip>

namespace ofl {

std::string Builder::generate_id() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dist;

    std::ostringstream ss;
    ss << std::hex << std::setfill('0')
       << std::setw(8) << dist(gen) << "-"
       << std::setw(4) << (dist(gen) & 0xFFFF) << "-"
       << std::setw(4) << ((dist(gen) & 0x0FFF) | 0x4000) << "-"
       << std::setw(4) << ((dist(gen) & 0x3FFF) | 0x8000) << "-"
       << std::setw(8) << dist(gen)
       << std::setw(4) << (dist(gen) & 0xFFFF);
    return ss.str();
}

Fixture Builder::build(const FixtureNode& node) {
    Fixture fixture;
    fixture.id   = generate_id();
    fixture.name = node.name;

    fixture.manufacturer = node.meta.count("manufacturer")
        ? node.meta.at("manufacturer") : "";
    fixture.fixture_type = fixture_type_from_string(
        node.meta.count("type") ? node.meta.at("type") : "");

    if (node.meta.count("weight")) {
        try { fixture.weight_kg = std::stof(node.meta.at("weight")); }
        catch (...) {}
    }
    if (node.meta.count("power")) {
        try { fixture.power_w = std::stof(node.meta.at("power")); }
        catch (...) {}
    }

    for (const auto& child : node.children) {
        if (std::holds_alternative<ChannelNode>(child))
            fixture.channels.push_back(build_channel(std::get<ChannelNode>(child)));
        else if (std::holds_alternative<ModeNode>(child))
            fixture.modes.push_back(build_mode(std::get<ModeNode>(child)));
    }

    return fixture;
}

FixtureChannel Builder::build_channel(const ChannelNode& node) {
    FixtureChannel ch;
    ch.name       = node.name;
    ch.attribute  = attribute_from_string(node.attribute);
    ch.resolution = (node.resolution == "16bit") ? 16 : 8;

    if (node.extra.count("default")) {
        try { ch.default_value = std::stoi(node.extra.at("default")); }
        catch (...) {}
    }
    if (node.extra.count("range")) {
        try { ch.range_degrees = std::stof(node.extra.at("range")); }
        catch (...) {}
    }

    for (const auto& cap : node.caps)
        ch.capabilities.push_back(build_cap(cap));

    return ch;
}

Capability Builder::build_cap(const CapNode& node) {
    Capability cap;
    cap.range_start = node.range_start;
    cap.range_end   = node.range_end;
    cap.cap_type    = cap_type_from_string(node.cap_type);
    cap.label       = node.params.count("name")
        ? std::get<std::string>(node.params.at("name"))
        : node.cap_type;

    // params von ParamValue (variant) nach string konvertieren
    for (const auto& [k, v] : node.params) {
        if (std::holds_alternative<std::string>(v))
            cap.params[k] = std::get<std::string>(v);
        else
            cap.params[k] = std::to_string(std::get<int>(v));
    }

    return cap;
}

FixtureMode Builder::build_mode(const ModeNode& node) {
    FixtureMode mode;
    mode.name          = node.name;
    mode.channel_count = node.channel_count;
    mode.mappings      = node.mappings;
    return mode;
}

} // namespace ofl