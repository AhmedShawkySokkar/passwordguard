/**
 * @file Policies.hpp
 * @brief Strategy and Factory Method pattern definitions for password validation.
 */
#pragma once

#include <string>
#include <memory>

namespace passwordguard {

/**
 * @class IPasswordPolicy
 * @brief Strategy interface for pluggable password validation rules.
 */
class IPasswordPolicy {
public:
    virtual ~IPasswordPolicy() = default;

    /**
     * @brief Evaluates whether a password satisfies the security policy.
     * @param password The raw password string to inspect.
     * @return true if valid, false otherwise.
     */
    virtual bool isValid(const std::string& password) const = 0;

    /**
     * @brief Returns the human-readable name of the active policy.
     * @return Policy description string.
     */
    virtual std::string name() const = 0;
};

/**
 * @class BasicLengthPolicy
 * @brief Concrete Strategy requiring at least 6 characters.
 */
class BasicLengthPolicy : public IPasswordPolicy {
public:
    bool isValid(const std::string& password) const override;
    std::string name() const override { return "BasicLengthPolicy (len >= 6)"; }
};

/**
 * @class PinCodePolicy
 * @brief Concrete Strategy requiring exactly 4 numeric digits (0-9).
 */
class PinCodePolicy : public IPasswordPolicy {
public:
    bool isValid(const std::string& password) const override;
    std::string name() const override { return "PinCodePolicy (4 digits)"; }
};

/**
 * @class StrictAdminPolicy
 * @brief Concrete Strategy requiring >= 8 chars, at least one uppercase letter, and one digit.
 */
class StrictAdminPolicy : public IPasswordPolicy {
public:
    bool isValid(const std::string& password) const override;
    std::string name() const override { return "StrictAdminPolicy (len >= 8, upper + digit)"; }
};

/**
 * @class PolicyFactory
 * @brief Factory Method creator for instantiating password policies by identifier.
 */
class PolicyFactory {
public:
    /**
     * @brief Creates a concrete password policy instance.
     * @param type Policy identifier ("basic", "pin", or "strict").
     * @return std::unique_ptr<IPasswordPolicy> owning the created strategy.
     * @throws std::invalid_argument if the policy type is unknown.
     */
    static std::unique_ptr<IPasswordPolicy> createPolicy(const std::string& type);
};

} // namespace passwordguard
