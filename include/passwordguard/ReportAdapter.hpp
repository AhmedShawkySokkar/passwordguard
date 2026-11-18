/**
 * @file ReportAdapter.hpp
 * @brief Structural Adapter pattern wrapping nlohmann::json for report export.
 */
#pragma once

#include <string>
#include <cstddef>

namespace passwordguard {

/**
 * @struct AuditSummary
 * @brief Data transfer structure holding aggregate password audit metrics.
 */
struct AuditSummary {
    std::string policy_name;    ///< Name of the policy strategy used
    int passed = 0;             ///< Count of valid passwords
    int failed = 0;             ///< Count of rejected passwords
    std::size_t min_length = 0; ///< Minimum password length in batch
    std::size_t max_length = 0; ///< Maximum password length in batch
};

/**
 * @class IReportExporter
 * @brief Target domain interface for serializing audit summaries.
 */
class IReportExporter {
public:
    virtual ~IReportExporter() = default;

    /**
     * @brief Writes the audit summary to a file.
     * @param filepath Destination file path.
     * @param summary Aggregated audit metrics.
     */
    virtual void exportReport(const std::string& filepath, const AuditSummary& summary) const = 0;
};

/**
 * @class JsonReportAdapter
 * @brief Adapter integrating third-party nlohmann::json behind IReportExporter.
 */
class JsonReportAdapter : public IReportExporter {
public:
    void exportReport(const std::string& filepath, const AuditSummary& summary) const override;

    /**
     * @brief Reads an AuditSummary back from a JSON file for verification.
     * @param filepath Source JSON file path.
     * @return Deserialized AuditSummary struct.
     */
    static AuditSummary readReport(const std::string& filepath);
};

} // namespace passwordguard
