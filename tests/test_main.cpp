/**
 * @file test_main.cpp
 * @brief Unit and error-handling test suite for PasswordGuard.
 */
#include "passwordguard/Utils.hpp"
#include "passwordguard/Policies.hpp"
#include "passwordguard/Observer.hpp"
#include "passwordguard/ReportAdapter.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#define PG_CHECK(cond) \
    do { \
        if (!(cond)) { \
            throw std::runtime_error(std::string("Test assertion failed: ") + #cond + \
                                     " at " + __FILE__ + ":" + std::to_string(__LINE__)); \
        } \
    } while (false)

template<typename ExceptionType, typename Func>
void check_throws(Func fn, const std::string& context) {
    bool threw = false;
    try {
        fn();
    } catch (const ExceptionType&) {
        threw = true;
    }
    if (!threw) {
        throw std::runtime_error("Expected exception was not thrown in: " + context);
    }
}

void test_extents() {
    std::vector<int> v{{ 1, 1, 2, 6, -2, 3, 5, 5 }};
    auto e = tk::extents(v);
    PG_CHECK((e == std::array<int, 2>{{ -2, 6 }}));

    std::vector<std::size_t> w{{ 1, 1, 2, 6, 2, 3, 5, 5 }};
    auto f = tk::extents(w);
    PG_CHECK((f == std::array<std::size_t, 2>{{ 1, 6 }}));

    check_throws<std::invalid_argument>([]() {
        std::vector<int> empty_vec;
        tk::extents(empty_vec);
    }, "tk::extents on empty vector");

    std::cout << "[PASS] Unit Test: tk::extents & empty-input error handling\n";
}

void test_strategies_and_factory() {
    auto basic = passwordguard::PolicyFactory::createPolicy("basic");
    PG_CHECK(basic->isValid("") == false);
    PG_CHECK(basic->isValid("12345") == false);
    PG_CHECK(basic->isValid("123456") == true);

    auto pin = passwordguard::PolicyFactory::createPolicy("pin");
    PG_CHECK(pin->isValid("1234") == true);
    PG_CHECK(pin->isValid("123a") == false);
    PG_CHECK(pin->isValid("12345") == false);

    auto strict = passwordguard::PolicyFactory::createPolicy("strict");
    PG_CHECK(strict->isValid("short1A") == false);
    PG_CHECK(strict->isValid("alllowercase1") == false);
    PG_CHECK(strict->isValid("NoDigitsHere") == false);
    PG_CHECK(strict->isValid("ValidPass1") == true);

    check_throws<std::invalid_argument>([]() {
        passwordguard::PolicyFactory::createPolicy("unknown_policy");
    }, "PolicyFactory unknown policy");

    std::cout << "[PASS] Unit Test: Strategy policies & PolicyFactory\n";
}

void test_observer_and_json_adapter() {
    PG_CHECK(passwordguard::ConsoleAuditObserver::maskPassword("a") == "*");
    PG_CHECK(passwordguard::ConsoleAuditObserver::maskPassword("ab") == "**");
    PG_CHECK(passwordguard::ConsoleAuditObserver::maskPassword("Admin2026") == "Ad*******");

    auto policy = passwordguard::PolicyFactory::createPolicy("pin");
    passwordguard::PasswordEvaluator evaluator(*policy);
    passwordguard::ConsoleAuditObserver spy(false);
    evaluator.attachObserver(&spy);

    evaluator.evaluate("9999");
    evaluator.evaluate("0000");
    evaluator.evaluate("abcd");

    PG_CHECK(spy.passedCount() == 2);
    PG_CHECK(spy.failedCount() == 1);

    passwordguard::AuditSummary out_sum{policy->name(), 2, 1, 4, 4};
    passwordguard::JsonReportAdapter adapter;
    adapter.exportReport("test_audit.json", out_sum);

    auto in_sum = passwordguard::JsonReportAdapter::readReport("test_audit.json");
    PG_CHECK(in_sum.passed == 2);
    PG_CHECK(in_sum.failed == 1);
    PG_CHECK(in_sum.min_length == 4);
    PG_CHECK(in_sum.max_length == 4);

    check_throws<std::runtime_error>([]() {
        passwordguard::JsonReportAdapter::readReport("non_existent_report_12345.json");
    }, "JsonReportAdapter missing file");

    std::cout << "[PASS] Unit Test: Observer masking & JSON Adapter\n";
}

int main() {
    try {
        test_extents();
        test_strategies_and_factory();
        test_observer_and_json_adapter();
        std::cout << "All PasswordGuard unit tests passed successfully!\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "[FAIL] " << e.what() << "\n";
        return 1;
    }
}
