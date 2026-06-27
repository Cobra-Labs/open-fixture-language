//
// Created by janwin443 on 6/27/26.
//

#include "ofl/validator.h"
#include <unordered_set>

namespace ofl {

    static const std::unordered_set<std::string> VALID_ATTRIBUTES = {
        "intensity", "pan", "tilt", "color", "gobo",
        "strobe", "zoom", "focus", "iris", "prism", "frost"
    };

    static const std::unordered_set<std::string> VALID_CAP_TYPES = {
        "open", "closed", "gobo", "color", "strobe", "variable"
    };

    static const std::unordered_set<std::string> VALID_RESOLUTIONS = {
        "8bit", "16bit"
    };

    void Validator::validate(const FixtureNode& node) {
        std::vector<std::string> errors;
        validate_attributes(node, errors);
        validate_caps(node, errors);
        validate_modes(node, errors);
        if (!errors.empty()) throw ValidationError(errors);
    }

    void Validator::validate_attributes(const FixtureNode& node, std::vector<std::string>& errors) {
        for (const auto& child : node.children) {
            if (!std::holds_alternative<ChannelNode>(child)) continue;
            const auto& ch = std::get<ChannelNode>(child);

            if (VALID_ATTRIBUTES.find(ch.attribute) == VALID_ATTRIBUTES.end()) {
                errors.push_back(
                    "Invalid attribute '" + ch.attribute + "' in channel '" + ch.name +
                    "' (line " + std::to_string(ch.line) + ")"
                    " - valid: intensity, pan, tilt, color, gobo, strobe, zoom, focus, iris, prism, frost"
                );
            }

            if (VALID_RESOLUTIONS.find(ch.resolution) == VALID_RESOLUTIONS.end()) {
                errors.push_back(
                    "Invalid resolution '" + ch.resolution + "' in channel '" + ch.name +
                    "' (line " + std::to_string(ch.line) + ")"
                    " - valid: 8bit, 16bit"
                );
            }
        }
    }

    void Validator::validate_caps(const FixtureNode& node, std::vector<std::string>& errors) {
        for (const auto& child : node.children) {
            if (!std::holds_alternative<ChannelNode>(child)) continue;
            const auto& ch = std::get<ChannelNode>(child);
            for (const auto& cap : ch.caps) {
                if (cap.range_start > cap.range_end) {
                    errors.push_back(
                        "Invalid range " + std::to_string(cap.range_start) +
                        ".." + std::to_string(cap.range_end) +
                        " in channel '" + ch.name + "' (line " + std::to_string(cap.line) + ")"
                        " - range_start must be <= range_end"
                    );
                }
                if (VALID_CAP_TYPES.find(cap.cap_type) == VALID_CAP_TYPES.end()) {
                    errors.push_back(
                        "Invalid cap type '" + cap.cap_type +
                        "' in channel '" + ch.name + "' (line " + std::to_string(cap.line) + ")"
                        " - valid: open, closed, gobo, color, strobe, variable"
                    );
                }
            }
        }
    }

    void Validator::validate_modes(const FixtureNode& node, std::vector<std::string>& errors) {
        std::unordered_set<std::string> channel_names;
        for (const auto& child : node.children) {
            if (std::holds_alternative<ChannelNode>(child))
                channel_names.insert(std::get<ChannelNode>(child).name);
        }

        for (const auto& child : node.children) {
            if (!std::holds_alternative<ModeNode>(child)) continue;
            const auto& mode = std::get<ModeNode>(child);
            for (const auto& [slot, ch_name] : mode.mappings) {
                if (channel_names.find(ch_name) == channel_names.end()) {
                    errors.push_back(
                        "Mode '" + mode.name + "': channel '" + ch_name +
                        "' (slot " + std::to_string(slot) + ") is not defined"
                    );
                }
            }
        }
    }

} // namespace ofl