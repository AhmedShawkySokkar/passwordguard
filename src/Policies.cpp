/**
 * @file Policies.cpp
 * @brief Implementation of password validation strategies and factory method.
 */
#include "passwordguard/Policies.hpp"
#include <cctype>
#include <algorithm>
#include <stdexcept>

namespace passwordguard {

bool BasicLengthPolicy::isValid(const std::string& password) const {
    return password.length() >= 6;
}

bool PinCodePolicy::isValid(const std::string& password) const {
    if (password.length() != 4) {
        return false;
    }
    return std::all_of(password.begin(), password.end(), [](unsigned char c) {
        return std::isdigit(c);
    });
}

bool StrictAdminPolicy::isValid(const std::string& password) const {
    if (password.length() < 8) {
        return false;
    }
    bool has_upper = false;
    bool has_digit = false;
    for (unsigned char c : password) {
        if (std::isupper(c)) has_upper = true;
        if (std::isdigit(c)) has_digit = true;
    }
    return has_upper && has_digit;
}

std::unique_ptr<IPasswordPolicy> PolicyFactory::createPolicy(const std::string& type) {
    if (type == "basic")  return std::make_unique<BasicLengthPolicy>();
    if (type == "pin")    return std::make_unique<PinCodePolicy>();
    if (type == "strict") return std::make_unique<StrictAdminPolicy>();
    throw std::invalid_argument("Unknown policy type: " + type);
}

} // namespace passwordguard
