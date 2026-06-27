//
// Created by janwin443 on 6/27/26.
//

#pragma once
#include <string>
#include <vector>
#include "nodes.h"
#include "types.h"
#include "error.h"

namespace ofl {

class ValidationError : public std::exception {
public:
    explicit ValidationError(std::vector<std::string> errors)
        : errors_(std::move(errors))
    {
        message_ = "Validation failed:\n";
        for (const auto& e : errors_) message_ += "  - " + e + "\n";
    }
    const char* what() const noexcept override { return message_.c_str(); }
    const std::vector<std::string>& errors() const { return errors_; }

private:
    std::vector<std::string> errors_;
    std::string message_;
};

class Validator {
public:
    void validate(const FixtureNode& node);

private:
    static void validate_attributes(const FixtureNode& node, std::vector<std::string>& errors);
    static void validate_caps(const FixtureNode& node, std::vector<std::string>& errors);
    static void validate_modes(const FixtureNode& node, std::vector<std::string>& errors);
};

} // namespace ofl