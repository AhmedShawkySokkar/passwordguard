/**
 * @file Observer.hpp
 * @brief Behavioral Observer pattern classes for password audit logging.
 */
#pragma once

#include "passwordguard/Policies.hpp"
#include <string>
#include <vector>
#include <iostream>

namespace passwordguard {

/**
 * @class IValidationObserver
 * @brief Observer interface receiving notifications after each password check.
 */
class IValidationObserver {
public:
    virtual ~IValidationObserver() = default;

    /**
     * @brief Callback triggered when a password is evaluated.
     * @param password The evaluated password string.
     * @param passed True if the password satisfied the policy, false otherwise.
     */
    virtual void onPasswordChecked(const std::string& password, bool passed) = 0;
};

/**
 * @class ConsoleAuditObserver
 * @brief Concrete Observer that prints check outcomes and tallies pass/fail counts.
 */
class ConsoleAuditObserver : public IValidationObserver {
public:
    /**
     * @brief Constructs a console observer.
     * @param verbose If true, prints each check outcome to standard output.
     */
    explicit ConsoleAuditObserver(bool verbose = true) : m_verbose(verbose) {}

    void onPasswordChecked(const std::string& password, bool passed) override {
        if (passed) {
            ++m_passed_count;
        } else {
            ++m_failed_count;
        }
        if (m_verbose) {
            std::cout << "  [" << (passed ? "PASS" : "FAIL") << "] Password: \""
                      << password << "\" (length: " << password.length() << ")\n";
        }
    }

    /** @brief Returns total number of passed passwords. */
    int passedCount() const noexcept { return m_passed_count; }

    /** @brief Returns total number of failed passwords. */
    int failedCount() const noexcept { return m_failed_count; }

private:
    bool m_verbose;
    int m_passed_count = 0;
    int m_failed_count = 0;
};

/**
 * @class PasswordEvaluator
 * @brief Context class executing an IPasswordPolicy and notifying attached observers.
 */
class PasswordEvaluator {
public:
    /**
     * @brief Binds the evaluator to a specific validation strategy.
     * @param policy Reference to the active password policy strategy.
     */
    explicit PasswordEvaluator(const IPasswordPolicy& policy) : m_policy(policy) {}

    /**
     * @brief Subscribes an observer to receive validation events.
     * @param obs Pointer to an IValidationObserver instance.
     */
    void attachObserver(IValidationObserver* obs) {
        if (obs) m_observers.push_back(obs);
    }

    /**
     * @brief Validates a password and broadcasts the outcome to all observers.
     * @param password The password string to validate.
     * @return true if valid, false otherwise.
     */
    bool evaluate(const std::string& password) {
        bool ok = m_policy.isValid(password);
        for (auto* obs : m_observers) {
            obs->onPasswordChecked(password, ok);
        }
        return ok;
    }

private:
    const IPasswordPolicy& m_policy;
    std::vector<IValidationObserver*> m_observers;
};

} // namespace passwordguard
