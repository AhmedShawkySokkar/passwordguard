/**
 * @file test_main.cpp
 * @brief Unit and verification test suite for PasswordGuard.
 */
#include "passwordguard/Utils.hpp"
#include "passwordguard/Policies.hpp"
#include "passwordguard/Observer.hpp"
#include "passwordguard/ReportAdapter.hpp"
#include <cassert>
#include <iostream>
#include <vector>

void test_extents() {
    std::vector<int> v{{ 1, 1, 2, 6, -2, 3, 5, 5 }};
    auto e = tk::extents(v);
    assert((e == std::array<int, 2>{{ -2, 6 }}));

    std::vector<std::size_t> w{{ 1, 1, 2, 6, 2, 3, 5, 5 }};
    auto f = tk::extents(w);
    assert((f == std::array<std::size_t, 2>{{ 1, 6 }}));
    std::cout << "[PASS] Unit Test: tk::extents\n";
}

void test_strategies_and_factory() {
    auto basic = passwordguard::PolicyFactory::createPolicy("basic");
    assert(basic->isValid("12345") == false);
    assert(basic->isValid("123456") == true);

    auto pin = passwordguard::PolicyFactory::createPolicy("pin");
    assert(pin->isValid("1234") == true);
    assert(pin->isValid("123a") == false);
    assert(pin->isValid("12345") == false);

    auto strict = passwordguard::PolicyFactory::createPolicy("strict");
    assert(strict->isValid("short1A") == false);
    assert(strict->isValid("alllowercase1") == false);
    assert(strict->isValid("NoDigitsHere") == false);
    assert(strict->isValid("ValidPass1") == true);

    bool threw = false;
    try {
        passwordguard::PolicyFactory::createPolicy("unknown_policy");
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    assert(threw);
    std::cout << "[PASS] Unit Test: Strategy policies & PolicyFactory\n";
}

void test_observer_and_json_adapter() {
    auto policy = passwordguard::PolicyFactory::createPolicy("pin");
    passwordguard::PasswordEvaluator evaluator(*policy);
    passwordguard::ConsoleAuditObserver spy(false);
    evaluator.attachObserver(&spy);

    evaluator.evaluate("9999");
    evaluator.evaluate("0000");
    evaluator.evaluate("abcd");

    assert(spy.passedCount() == 2);
    assert(spy.failedCount() == 1);

    passwordguard::AuditSummary out_sum{policy->name(), 2, 1, 4, 4};
    passwordguard::JsonReportAdapter adapter;
    adapter.exportReport("test_audit.json", out_sum);

    auto in_sum = passwordguard::JsonReportAdapter::readReport("test_audit.json");
    assert(in_sum.passed == 2);
    assert(in_sum.failed == 1);
    assert(in_sum.min_length == 4);
    assert(in_sum.max_length == 4);
    std::cout << "[PASS] Unit Test: Observer notifications & JSON Adapter\n";
}

int main() {
    test_extents();
    test_strategies_and_factory();
    test_observer_and_json_adapter();
    std::cout << "All PasswordGuard unit tests passed successfully!\n";
    return 0;
}
