//
// Created by janwin443 on 6/27/26.
//

#pragma once
#include <string>
#include <vector>
#include <map>
#include <variant>

namespace ofl {

// ─── Capability ───────────────────────────────────────────

using ParamValue = std::variant<int, std::string>;

struct CapNode {
    int range_start;
    int range_end;
    std::string cap_type;
    std::map<std::string, ParamValue> params;
    int line = 0;
};

// ─── Channel ──────────────────────────────────────────────

struct ChannelNode {
    std::string name;
    std::string attribute;
    std::string resolution;
    std::vector<CapNode> caps;
    std::map<std::string, std::string> extra;
    int line = 0;
};

// ─── Mode ─────────────────────────────────────────────────

struct ModeNode {
    std::string name;
    int channel_count = 0;
    std::map<int, std::string> mappings;
    int line = 0;
};

// ─── Include ──────────────────────────────────────────────

struct IncludeNode {
    std::string raw_path;
    int line = 0;
};

// ─── Child-Variant ────────────────────────────────────────

using ChildNode = std::variant<ChannelNode, ModeNode, IncludeNode>;

// ─── Fixture (Root) ───────────────────────────────────────

struct FixtureNode {
    std::string name;
    std::map<std::string, std::string> meta;
    std::vector<ChildNode> children;
    int line = 0;
};

} // namespace ofl
