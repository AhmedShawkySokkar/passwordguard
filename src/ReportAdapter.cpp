/**
 * @file ReportAdapter.cpp
 * @brief Implementation of JsonReportAdapter using nlohmann::json.
 */
#include "passwordguard/ReportAdapter.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>

namespace passwordguard {

void JsonReportAdapter::exportReport(const std::string& filepath, const AuditSummary& summary) const {
    std::ofstream out(filepath);
    if (!out.is_open()) {
        throw std::runtime_error("Cannot open output file for writing: " + filepath);
    }

    nlohmann::json j;
    j["policy"]     = summary.policy_name;
    j["passed"]     = summary.passed;
    j["failed"]     = summary.failed;
    j["total"]      = summary.passed + summary.failed;
    j["min_length"] = summary.min_length;
    j["max_length"] = summary.max_length;

    out << j.dump(2) << "\n";
}

AuditSummary JsonReportAdapter::readReport(const std::string& filepath) {
    std::ifstream in(filepath);
    if (!in.is_open()) {
        throw std::runtime_error("Cannot open JSON report file: " + filepath);
    }

    nlohmann::json j;
    in >> j;

    AuditSummary s;
    s.policy_name = j.at("policy").get<std::string>();
    s.passed      = j.at("passed").get<int>();
    s.failed      = j.at("failed").get<int>();
    s.min_length  = j.at("min_length").get<std::size_t>();
    s.max_length  = j.at("max_length").get<std::size_t>();
    return s;
}

} // namespace passwordguard
