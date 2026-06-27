//
// Created by janwin443 on 6/27/26.
//

#pragma once
#include <filesystem>
#include <vector>
#include "nodes.h"
#include "error.h"

namespace ofl {

class Resolver {
public:
    // search_paths: Liste von Verzeichnissen wo .ofl-Includes gesucht werden
    explicit Resolver(std::vector<std::filesystem::path> search_paths);

    // Löst alle IncludeNodes in fixture.children auf (in-place)
    void resolve(FixtureNode& fixture);

private:
    std::vector<std::filesystem::path> search_paths_;

    // Lädt und parst eine .ofl-Datei, gibt ihre Children zurück
    std::vector<ChildNode> load_include(const std::string& raw_path, int ref_line);

    // Sucht die Datei in allen search_paths
    std::filesystem::path find_file(const std::string& raw_path, int ref_line);
};

} // namespace ofl