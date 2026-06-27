//
// Created by janwin443 on 6/27/26.
//

#include "ofl/writer.h"
#include <sstream>

namespace ofl {

std::string Writer::write(const Fixture& fixture) {
    std::ostringstream out;

    out << "fixture " << fixture.name << ":\n";
    out << INDENT << "manufacturer: " << fixture.manufacturer << "\n";

    if (fixture.fixture_type != FixtureType::Unknown)
        out << INDENT << "type: " << fixture_type_to_string(fixture.fixture_type) << "\n";
    if (fixture.weight_kg)
        out << INDENT << "weight: " << *fixture.weight_kg << "kg\n";
    if (fixture.power_w)
        out << INDENT << "power: " << *fixture.power_w << "W\n";

    for (const auto& ch : fixture.channels)
        out << write_channel(ch);

    for (const auto& mode : fixture.modes)
        out << write_mode(mode);

    return out.str();
}

std::string Writer::write_channel(const FixtureChannel& ch) {
    std::ostringstream out;
    out << INDENT << "channel " << ch.name << ":\n";
    out << INDENT << INDENT << "attribute: " << attribute_to_string(ch.attribute) << "\n";
    out << INDENT << INDENT << "resolution: " << (ch.resolution == 16 ? "16bit" : "8bit") << "\n";

    if (ch.range_degrees)
        out << INDENT << INDENT << "range: " << *ch.range_degrees << "\n";
    if (ch.default_value != 0)
        out << INDENT << INDENT << "default: " << ch.default_value << "\n";

    for (const auto& cap : ch.capabilities)
        out << write_cap(cap);

    return out.str();
}

std::string Writer::write_cap(const Capability& cap) {
    std::ostringstream out;
    out << INDENT << INDENT << "cap "
        << cap.range_start << ".." << cap.range_end << ": "
        << cap_type_to_string(cap.cap_type);

    // Parameter ausgeben, aber "name" weglassen wenn es gleich dem cap_type ist
    std::map<std::string, std::string> filtered_params = cap.params;
    if (filtered_params.count("name") &&
        filtered_params.at("name") == cap_type_to_string(cap.cap_type))
        filtered_params.erase("name");

    if (!filtered_params.empty()) {
        out << "(";
        bool first = true;
        for (const auto& [k, v] : filtered_params) {
            if (!first) out << ", ";
            out << k << "=" << v;
            first = false;
        }
        out << ")";
    }
    out << "\n";
    return out.str();
}

std::string Writer::write_mode(const FixtureMode& mode) {
    std::ostringstream out;
    out << INDENT << "mode " << mode.name
        << "(" << mode.channel_count << "ch):\n";

    for (const auto& [slot, ch_name] : mode.mappings)
        out << INDENT << INDENT << slot << ": " << ch_name << "\n";

    return out.str();
}

} // namespace ofl