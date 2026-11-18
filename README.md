# PasswordGuard — Production Password Policy Auditor

## 1. Overview
`PasswordGuard` is a modular C++17 command-line utility demonstrating production software engineering practices:
- **Modern CMake Build System**: Out-of-source build enforcement, Git SHA1 build provenance injection, and automated target management.
- **Gang of Four (GoF) Design Patterns**: Strategy, Factory Method, Observer, and Adapter patterns adhering to SOLID principles.
- **Third-Party Library Integration**: Seamless integration of `nlohmann/json` via CMake `FetchContent` and the Adapter pattern.
- **Automated Documentation**: Full Doxygen API documentation built automatically via `-DBUILD_DOC=ON`.
- **Verification & Continuous Integration**: CTest unit test suite, `cppcheck` static analysis, and `Valgrind` heap leak checks.

## 2. Software Architecture & Design Patterns
- **Strategy Pattern (`IPasswordPolicy`)**: Encapsulates interchangeable password validation algorithms (`BasicLengthPolicy`, `PinCodePolicy`, `StrictAdminPolicy`).
- **Factory Method Pattern (`PolicyFactory`)**: Instantiates concrete policy strategies from CLI identifiers, satisfying the Open-Closed Principle.
- **Observer Pattern (`IValidationObserver`, `ConsoleAuditObserver`)**: Decouples validation event logging and pass/fail counting from string inspection logic.
- **Adapter Pattern (`JsonReportAdapter`)**: Wraps the third-party `nlohmann/json` library behind our domain `IReportExporter` interface.

## 3. Build, Test, Document & Run Instructions
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON -DBUILD_DOC=ON ..
cmake --build . -j4
ctest -V
./passwordguard --policy strict Admin2026 weak 12345678A
