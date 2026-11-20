/**
 * @file main.cpp
 * @brief Command-line interface entry point for PasswordGuard.
 */
#include "BuildConfig.hpp"
#include "passwordguard/Utils.hpp"
#include "passwordguard/Policies.hpp"
#include "passwordguard/Observer.hpp"
#include "passwordguard/ReportAdapter.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

void printHelp() {
    std::cout << "PasswordGuard - Password Policy Auditor\n"
              << "Compiler : " << PG_COMPILER << "\n"
              << "Git SHA1 : " << PG_GIT_SHA1 << "\n\n"
              << "Usage:\n"
              << "  ./passwordguard [options] [password1 password2 ...]\n\n"
              << "Options:\n"
              << "  --policy <type>    Policy strategy: basic | pin | strict (default: strict)\n"
              << "  --output <file>    JSON report output path (default: audit_report.json)\n"
              << "  --stdin            Read passwords line-by-line from standard input\n"
              << "  --help, -h         Display this help message\n";
}

int main(int argc, char* argv[]) {
    try {
        std::string policy_type = "strict";
        std::string output_file = "audit_report.json";
        bool read_stdin = false;
        std::vector<std::string> passwords;

        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--help" || arg == "-h") {
                printHelp();
                return 0;
            } else if (arg == "--policy") {
                if (i + 1 >= argc || argv[i + 1][0] == '-') {
                    throw std::invalid_argument("Option '--policy' requires a value (basic | pin | strict).");
                }
                policy_type = argv[++i];
            } else if (arg == "--output") {
                if (i + 1 >= argc || argv[i + 1][0] == '-') {
                    throw std::invalid_argument("Option '--output' requires a file path.");
                }
                output_file = argv[++i];
            } else if (arg == "--stdin") {
                read_stdin = true;
            } else if (!arg.empty() && arg[0] == '-') {
                throw std::invalid_argument("Unknown CLI option: " + arg);
            } else {
                passwords.push_back(arg);
            }
        }

        if (read_stdin) {
            std::string line;
            while (std::getline(std::cin, line)) {
                if (!line.empty()) {
                    passwords.push_back(line);
                }
            }
        } else if (passwords.empty()) {
            passwords = {"1234", "secret", "Admin2026", "gyor99", "SuperSecure9"};
        }

        if (passwords.empty()) {
            throw std::invalid_argument("No passwords provided for evaluation.");
        }

        std::cout << "=== PasswordGuard Auditor (Git: " << PG_GIT_SHA1 << ") ===\n";

        auto policy = passwordguard::PolicyFactory::createPolicy(policy_type);
        std::cout << "Active Strategy: " << policy->name() << "\n";

        passwordguard::PasswordEvaluator evaluator(*policy);
        passwordguard::ConsoleAuditObserver observer(true);
        evaluator.attachObserver(&observer);

        std::vector<std::size_t> lengths;
        for (const auto& pwd : passwords) {
            evaluator.evaluate(pwd);
            lengths.push_back(pwd.length());
        }

        auto len_bounds = tk::extents(lengths);

        passwordguard::AuditSummary summary{
            policy->name(),
            observer.passedCount(),
            observer.failedCount(),
            len_bounds[0],
            len_bounds[1]
        };

        passwordguard::JsonReportAdapter exporter;
        exporter.exportReport(output_file, summary);

        std::cout << "Summary: " << summary.passed << " passed, " << summary.failed
                  << " failed | Length extents: [" << summary.min_length << ", "
                  << summary.max_length << "]\n"
                  << "Report written to '" << output_file << "'.\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }
}
