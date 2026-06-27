//
// Created by janwin443 on 6/27/26.
//

#include "ofl/resolver.h"
#include "ofl/lexer.h"
#include "ofl/parser.h"
#include <fstream>
#include <sstream>

namespace ofl {

Resolver::Resolver(std::vector<std::filesystem::path> search_paths)
    : search_paths_(std::move(search_paths))
{}

std::filesystem::path Resolver::find_file(const std::string& raw_path, int ref_line) {
    for (const auto& base : search_paths_) {
        std::filesystem::path candidate = base / (raw_path + ".ofl");
        if (std::filesystem::exists(candidate)) {
            return candidate;
        }
    }
    throw OflError(
        ErrorCode::IncludeNotFound,
        "Cannot find include '" + raw_path + "'",
        "Add the directory containing '" + raw_path + ".ofl' to your fixture search paths",
        {"<resolver>", ref_line, 0}
    );
}

std::vector<ChildNode> Resolver::load_include(const std::string& raw_path, int ref_line) {
    const auto path = find_file(raw_path, ref_line);

    std::ifstream file(path);
    if (!file) {
        throw OflError(
            ErrorCode::IncludeNotFound,
            "Cannot open file '" + path.string() + "'",
            "Check file permissions",
            {"<resolver>", ref_line, 0}
        );
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    const std::string source = ss.str();

    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(tokens, path.string());
    FixtureNode included = parser.parse();

    // Nur die Children (Channels) übernehmen, kein Meta
    return included.children;
}

void Resolver::resolve(FixtureNode& fixture) {
    std::vector<ChildNode> resolved;

    for (auto& child : fixture.children) {
        if (std::holds_alternative<IncludeNode>(child)) {
            const auto& inc = std::get<IncludeNode>(child);
            auto included_children = load_include(inc.raw_path, inc.line);
            for (auto& c : included_children) {
                resolved.push_back(std::move(c));
            }
        } else {
            resolved.push_back(std::move(child));
        }
    }

    fixture.children = std::move(resolved);
}

} // namespace ofl