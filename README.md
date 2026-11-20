# PasswordGuard — Password Policy Auditor

## 1. Overview
`PasswordGuard` is a modular C++17 command-line utility built to demonstrate software engineering practices covered in the course:
- **Modern CMake Build System**: Out-of-source build enforcement, Git SHA1 build provenance injection, and target-based dependency linking.
- **Design Patterns**: Strategy, Factory Method, Observer, and Adapter patterns designed with separation of concerns and RAII resource management.
- **Third-Party Library Integration**: Integration of `nlohmann/json` via CMake `FetchContent` isolated behind an adapter interface.
- **Automated Documentation**: Doxygen HTML API documentation generated via `-DBUILD_DOC=ON`.
- **Testing & Continuous Integration**: Release-safe CTest unit test suite, `cppcheck` static analysis, and `Valgrind` heap leak checks.

## 2. Software Architecture & Design Patterns
- **Strategy Pattern (`IPasswordPolicy`)**: Encapsulates interchangeable password validation algorithms (`BasicLengthPolicy`, `PinCodePolicy`, `StrictAdminPolicy`).
- **Factory Method Pattern (`PolicyFactory`)**: Instantiates concrete policy strategies from CLI identifiers.
- **Observer Pattern (`IValidationObserver`, `ConsoleAuditObserver`)**: Decouples validation event logging (with masked output) and pass/fail counting from string inspection logic.
- **Adapter Pattern (`JsonReportAdapter`)**: Wraps the third-party `nlohmann/json` library behind our domain `IReportExporter` interface.

## 3. Build, Test, Document & Run Instructions

```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DENEBLE_TESTS=ON -DBUILD_DOC=ON ..
cmake --build . -j4
ctest -V	./passwordguard --policy strict Admin2026 weak 12345678A
```

### Security Note on Password Input
Passing passwords directly as command-line arguments (`argv`) is convenient for quick demonstrations, but on multi-user systems CLI arguments can be visible in process listings (`ps`). For safer usage, pipe passwords via standard input using `--stdin`:

```bash
printf "Admin2026\nweak\n" | ./passwordguard --policy strict --stdin
```
