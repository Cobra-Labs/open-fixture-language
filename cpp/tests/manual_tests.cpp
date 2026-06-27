#include <filesystem>
#include <iostream>
#include "ofl/lexer.h"
#include "ofl/parser.h"
#include "ofl/resolver.h"
#include "ofl/validator.h"
#include "ofl/builder.h"
#include "ofl/writer.h"

int main() {
    const std::string source = R"(fixture Robin600E:
    manufacturer: Robe
    type: moving_head
    weight: 19.5kg
    power: 480W
    @shared/pan_tilt_16bit
    channel dimmer:
        attribute: intensity
        resolution: 16bit
        cap 0..255: open
    channel gobo:
        attribute: gobo
        resolution: 8bit
        cap 0..9: open
        cap 10..19: gobo(slot=1)
        cap 20..29: gobo(slot=1, spin=cw)
    channel color:
        attribute: color
        resolution: 8bit
        cap 0..9: open
        cap 10..19: color(hex=#FF0000, name=Red)
        cap 20..29: color(hex=#0000FF, name=Blue)
    mode standard(16ch):
        1,2: pan
        3,4: tilt
        5,6: dimmer
        7: gobo
        8: color
    )";

    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    ofl::Parser parser(tokens, "test.ofl");
    ofl::FixtureNode fixture = parser.parse();

    std::cout << "Fixture: " << fixture.name << " (line " << fixture.line << ")\n";
    std::cout << "Meta:\n";
    for (const auto& [key, val] : fixture.meta) {
        std::cout << "  " << key << ": " << val << "\n";
    }
    std::cout << "Children: " << fixture.children.size() << "\n";
    for (const auto& child : fixture.children) {
        if (std::holds_alternative<ofl::ChannelNode>(child)) {
            const auto& ch = std::get<ofl::ChannelNode>(child);
            std::cout << "  channel '" << ch.name << "' attr=" << ch.attribute
                      << " res=" << ch.resolution << " caps=" << ch.caps.size() << "\n";
            for (const auto& cap : ch.caps) {
                std::cout << "    cap " << cap.range_start << ".." << cap.range_end
                          << " type=" << cap.cap_type;
                for (const auto& [k, v] : cap.params) {
                    std::cout << " " << k << "=";
                    if (std::holds_alternative<int>(v))
                        std::cout << std::get<int>(v);
                    else
                        std::cout << std::get<std::string>(v);
                }
                std::cout << "\n";
            }
        } else if (std::holds_alternative<ofl::IncludeNode>(child)) {
            const auto& inc = std::get<ofl::IncludeNode>(child);
            std::cout << "  include '" << inc.raw_path << "'\n";
        } else if (std::holds_alternative<ofl::ModeNode>(child)) {
            const auto& mode = std::get<ofl::ModeNode>(child);
            std::cout << "  mode '" << mode.name << "' channels=" << mode.channel_count << "\n";
        } else if (std::holds_alternative<ofl::ModeNode>(child)) {
            const auto& mode = std::get<ofl::ModeNode>(child);
            std::cout << "  mode '" << mode.name << "' channels=" << mode.channel_count << "\n";
            for (const auto& [slot, ch] : mode.mappings) {
                std::cout << "    slot " << slot << " -> " << ch << "\n";
            }
        }
    }

    ofl::Resolver resolver({
        std::filesystem::path("tests")
    });
    resolver.resolve(fixture);

    std::cout << "\nAfter resolve - Children: " << fixture.children.size() << "\n";
    for (const auto& child : fixture.children) {
        if (std::holds_alternative<ofl::ChannelNode>(child)) {
            const auto& ch = std::get<ofl::ChannelNode>(child);
            std::cout << "  channel '" << ch.name << "'\n";
        } else if (std::holds_alternative<ofl::ModeNode>(child)) {
            const auto& mode = std::get<ofl::ModeNode>(child);
            std::cout << "  mode '" << mode.name << "'\n";
        }
    }

    try {
        ofl::Validator validator;
        validator.validate(fixture);
        std::cout << "\nValidation: OK\n";
    } catch (const ofl::ValidationError& e) {
        std::cerr << e.what();
    }

    ofl::Fixture result = ofl::Builder::build(fixture);
    std::cout << "\nBuilt Fixture:\n";
    std::cout << "  id:           " << result.id << "\n";
    std::cout << "  name:         " << result.name << "\n";
    std::cout << "  manufacturer: " << result.manufacturer << "\n";
    std::cout << "  type:         " << ofl::fixture_type_to_string(result.fixture_type) << "\n";
    if (result.weight_kg) std::cout << "  weight:       " << *result.weight_kg << "kg\n";
    if (result.power_w)   std::cout << "  power:        " << *result.power_w << "W\n";
    std::cout << "  channels:     " << result.channels.size() << "\n";
    std::cout << "  modes:        " << result.modes.size() << "\n";

    std::string written = ofl::Writer::write(result);
    std::cout << "\nWritten .ofl:\n" << written;

}
