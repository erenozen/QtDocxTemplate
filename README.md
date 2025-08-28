# QtDocxTemplate (C++ / Qt 6) — Clean-room port of templ4docx

Status: Initial scaffold (stubs). This library aims for 1:1 feature parity with the Java project jsolve/templ4docx (version 2.0.2) — no extra features beyond its documented capabilities.

## High-level Goal
Provide a Qt 6 / C++ API to fill DOCX templates by replacing text variables, inserting images, bullet lists, and table data, mirroring the original Java API.

## Public API (planned)
```
using namespace QtDocxTemplate;
Docx docx("template.docx");
docx.setVariablePattern(VariablePattern{"${","}"});
Variables vars;
vars.add(std::make_shared<TextVariable>("${firstname}", "Lukasz"));
vars.add(std::make_shared<TextVariable>("${lastname}", "Stypka"));
docx.fillTemplate(vars);
docx.save("output.docx");
```

Additional supported variable types (parity with templ4docx):
- TextVariable (style preserved)
- ImageVariable (pixels → EMU @ 96 DPI)
- BulletListVariable (list of text or images)
- TableVariable (column-wise lists, row replication)

Utility methods (parity with blog examples):
- `readTextContent()`
- `findVariables()`

## Build
```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

## Install
```bash
cmake --install build --prefix /desired/prefix
```
Then consume via:
```cmake
find_package(QtDocxTemplate CONFIG REQUIRED)
target_link_libraries(myapp PRIVATE QtDocxTemplate::core)
```

## Implementation Phases (TODO)
- Phase A: OPC package (zip) handling (minizip/libzip)
- Phase B: XML layer (pugixml)
- Phase C: Run model (split runs awareness)
- Phase D: Replacers (text, image, bullet list, table)
- Phase E: Docx façade logic
- Phase F: Examples & tests with real .docx fixtures

## Scope Guardrails
Parity only. Do not add features beyond templ4docx (e.g., headers/footers advanced traversal, hyperlinks, etc.).

## License
Apache-2.0 (clean-room reimplementation). Original Java project remains separately licensed under Apache-2.0.
