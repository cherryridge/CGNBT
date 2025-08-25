#pragma once
#include <string>
#include <vector>

namespace NBT::Error {
    using std::vector, std::string;

    inline thread_local vector<string> errors;

    //Error vector copied on-purpose.
    [[nodiscard]] inline vector<string> getErrors() noexcept { return vector(errors); }

    inline void clearErrors() noexcept { errors.clear(); }

    [[nodiscard]] inline void pushError(const string& error) noexcept { errors.push_back(error); }
}