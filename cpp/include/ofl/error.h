//
// Created by janwin443 on 6/27/26.
//

#ifndef OFL_CPP_ERROR_H
#define OFL_CPP_ERROR_H

#endif //OFL_CPP_ERROR_H

#pragma once
#include <string>
#include <stdexcept>

namespace ofl {

enum class ErrorCode {
    // OFL-Parser
    UnexpectedToken,       // LUM-OFL-001
    UnexpectedIndent,      // LUM-OFL-002
    UnknownKeyword,        // LUM-OFL-003
    MissingRequiredField,  // LUM-OFL-004
    InvalidRange,          // LUM-OFL-005
    DuplicateChannel,      // LUM-OFL-006
    IncludeNotFound,       // LUM-OFL-007
    InvalidColor,          // LUM-OFL-008
};

inline std::string error_code_string(ErrorCode code) {
    switch (code) {
        case ErrorCode::UnexpectedToken:      return "LUM-OFL-001";
        case ErrorCode::UnexpectedIndent:     return "LUM-OFL-002";
        case ErrorCode::UnknownKeyword:       return "LUM-OFL-003";
        case ErrorCode::MissingRequiredField: return "LUM-OFL-004";
        case ErrorCode::InvalidRange:         return "LUM-OFL-005";
        case ErrorCode::DuplicateChannel:     return "LUM-OFL-006";
        case ErrorCode::IncludeNotFound:      return "LUM-OFL-007";
        case ErrorCode::InvalidColor:         return "LUM-OFL-008";
    }
    return "LUM-OFL-???";
}

struct SourceLocation {
    std::string file;
    int line   = 0;
    int column = 0;
};

class OflError : public std::runtime_error {
public:
    OflError(ErrorCode code,
             const std::string& message,
             const std::string& hint,
             SourceLocation loc)
        : std::runtime_error(format(code, message, hint, loc))
        , code_(code)
        , hint_(hint)
        , loc_(loc)
    {}

    ErrorCode       code() const { return code_; }
    const std::string& hint() const { return hint_; }
    const SourceLocation& location() const { return loc_; }

private:
    static std::string format(ErrorCode code,
                               const std::string& message,
                               const std::string& hint,
                               const SourceLocation& loc)
    {
        return "error[" + error_code_string(code) + "]: " + message + "\n"
             + "  --> " + loc.file + ":" + std::to_string(loc.line)
                        + ":" + std::to_string(loc.column) + "\n"
             + "   = hint: " + hint;
    }

    ErrorCode      code_;
    std::string    hint_;
    SourceLocation loc_;
};

} // namespace ofl