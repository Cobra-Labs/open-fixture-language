//
// Created by janwin443 on 6/27/26.
//

#pragma once
#include <string>
#include <vector>
#include <map>
#include <optional>

namespace ofl {

// ─── Enums ────────────────────────────────────────────────

enum class FixtureType {
    MovingHead,
    LedPar,
    Strobe,
    Conventional,
    LedBar,
    Smoke,
    Unknown
};

enum class Attribute {
    Intensity,
    Pan,
    Tilt,
    Color,
    Gobo,
    Strobe,
    Zoom,
    Focus,
    Iris,
    Prism,
    Frost,
    Unknown
};

enum class CapType {
    Open,
    Closed,
    Gobo,
    Color,
    Strobe,
    Variable,
    Unknown
};

// String-Konverter (für Validator, Builder, Writer)
inline FixtureType fixture_type_from_string(const std::string& s) {
    if (s == "moving_head")  return FixtureType::MovingHead;
    if (s == "led_par")      return FixtureType::LedPar;
    if (s == "strobe")       return FixtureType::Strobe;
    if (s == "conventional") return FixtureType::Conventional;
    if (s == "led_bar")      return FixtureType::LedBar;
    if (s == "smoke")        return FixtureType::Smoke;
    return FixtureType::Unknown;
}

inline Attribute attribute_from_string(const std::string& s) {
    if (s == "intensity") return Attribute::Intensity;
    if (s == "pan")       return Attribute::Pan;
    if (s == "tilt")      return Attribute::Tilt;
    if (s == "color")     return Attribute::Color;
    if (s == "gobo")      return Attribute::Gobo;
    if (s == "strobe")    return Attribute::Strobe;
    if (s == "zoom")      return Attribute::Zoom;
    if (s == "focus")     return Attribute::Focus;
    if (s == "iris")      return Attribute::Iris;
    if (s == "prism")     return Attribute::Prism;
    if (s == "frost")     return Attribute::Frost;
    return Attribute::Unknown;
}

inline CapType cap_type_from_string(const std::string& s) {
    if (s == "open")     return CapType::Open;
    if (s == "closed")   return CapType::Closed;
    if (s == "gobo")     return CapType::Gobo;
    if (s == "color")    return CapType::Color;
    if (s == "strobe")   return CapType::Strobe;
    if (s == "variable") return CapType::Variable;
    return CapType::Unknown;
}

inline std::string fixture_type_to_string(FixtureType t) {
    switch (t) {
        case FixtureType::MovingHead:  return "moving_head";
        case FixtureType::LedPar:      return "led_par";
        case FixtureType::Strobe:      return "strobe";
        case FixtureType::Conventional:return "conventional";
        case FixtureType::LedBar:      return "led_bar";
        case FixtureType::Smoke:       return "smoke";
        default:                       return "unknown";
    }
}

inline std::string attribute_to_string(Attribute a) {
    switch (a) {
        case Attribute::Intensity: return "intensity";
        case Attribute::Pan:       return "pan";
        case Attribute::Tilt:      return "tilt";
        case Attribute::Color:     return "color";
        case Attribute::Gobo:      return "gobo";
        case Attribute::Strobe:    return "strobe";
        case Attribute::Zoom:      return "zoom";
        case Attribute::Focus:     return "focus";
        case Attribute::Iris:      return "iris";
        case Attribute::Prism:     return "prism";
        case Attribute::Frost:     return "frost";
        default:                   return "unknown";
    }
}

inline std::string cap_type_to_string(CapType c) {
    switch (c) {
        case CapType::Open:     return "open";
        case CapType::Closed:   return "closed";
        case CapType::Gobo:     return "gobo";
        case CapType::Color:    return "color";
        case CapType::Strobe:   return "strobe";
        case CapType::Variable: return "variable";
        default:                return "unknown";
    }
}

// ─── Structs ──────────────────────────────────────────────

struct Capability {
    int range_start;
    int range_end;
    CapType cap_type;
    std::string label;
    std::map<std::string, std::string> params;
};

struct FixtureChannel {
    std::string name;
    Attribute attribute;
    int resolution;           // 8 oder 16
    std::vector<Capability> capabilities;
    int default_value = 0;
    std::optional<float> range_degrees;
};

struct FixtureMode {
    std::string name;
    int channel_count;
    std::map<int, std::string> mappings;
};

struct Fixture {
    std::string id;
    std::string name;
    std::string manufacturer;
    FixtureType fixture_type;
    std::vector<FixtureChannel> channels;
    std::vector<FixtureMode> modes;
    std::optional<float> weight_kg;
    std::optional<float> power_w;

    // Hilfsmethoden wie im Python
    const FixtureChannel* channel(const std::string& name) const {
        for (const auto& ch : channels)
            if (ch.name == name) return &ch;
        return nullptr;
    }

    const FixtureMode* mode(const std::string& name) const {
        for (const auto& m : modes)
            if (m.name == name) return &m;
        return nullptr;
    }
};

} // namespace ofl