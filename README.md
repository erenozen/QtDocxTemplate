# QtDocxTemplate (C++ / Qt 6) — Clean-room port of templ4docx

![CI](https://github.com/erenozen/QtDocxTemplate/actions/workflows/ci.yml/badge.svg)
![Sanitizers](https://github.com/erenozen/QtDocxTemplate/actions/workflows/sanitizers.yml/badge.svg)
![License](https://img.shields.io/badge/license-Apache--2.0-blue.svg)

Clean-room (Apache-2.0) reimplementation of the Java library jsolve/templ4docx (v2.0.2). Intentional parity: text, image, bullet list, and table variable expansion in the main document part. No extra features beyond upstream scope.

## Quick Start
```bash
cmake -S . -B build -DQTDOCTXTEMPLATE_BUILD_TESTS=ON
cmake --build build -j
ctest --test-dir build -V
```

Install & consume:
```bash
cmake --install build --prefix /tmp/qdt-prefix
```
```cmake
find_package(QtDocxTemplate CONFIG REQUIRED)
add_executable(app main.cpp)
target_link_libraries(app PRIVATE QtDocxTemplate::core Qt6::Core Qt6::Gui)
```

## Minimal Usage
```cpp
using namespace QtDocxTemplate;
Docx doc("template.docx");
Variables vars;
vars.add(std::make_shared<TextVariable>("${firstname}", "Lukasz"));
vars.add(std::make_shared<TextVariable>("${lastname}",  "Stypka"));
doc.fillTemplate(vars);
doc.save("output.docx");
```

## Variable Types
- TextVariable (style preserved, run fragmentation neutral)
- ImageVariable (pixels → EMU @ 96 DPI)
- BulletListVariable (list of text items)
- TableVariable (column-wise lists; row replication)

Utility helpers: `Docx::readTextContent()`, `Docx::findVariables()`.

## Extended Examples

Custom pattern:
```cpp
doc.setVariablePattern({"#{","}"});
vars.add(std::make_shared<TextVariable>("#{city}", "Berlin"));
```

Image insertion:
```cpp
QImage logo(80,80,QImage::Format_ARGB32); logo.fill(Qt::red);
vars.add(std::make_shared<ImageVariable>("${logo}", logo, 80, 80));
```

Bullet list:
```cpp
auto bl = std::make_shared<BulletListVariable>("${skills}");
bl->addItem(std::make_shared<TextVariable>("${i1}", "C++"));
bl->addItem(std::make_shared<TextVariable>("${i2}", "Qt"));
vars.add(bl);
```

Table (4 columns: name, age, avatar image, skills bullet list):
```cpp
TableVariable table("${students}");
// columns are parallel vectors of VariablePtr (same length)
std::vector<VariablePtr> colName { TV("${n1}", "Alice"), TV("${n2}", "Bob") };
std::vector<VariablePtr> colAge  { TV("${a1}", "30"),    TV("${a2}", "25") };
QImage avatar(32,32,QImage::Format_ARGB32); avatar.fill(Qt::blue);
std::vector<VariablePtr> colImg  { IV("${i1}", avatar,32,32), IV("${i2}", avatar,32,32) };
auto mkSkills = [](QString a, QString b){
	auto bl = std::make_shared<BulletListVariable>("${skills}");
	bl->addItem(std::make_shared<TextVariable>("${k1}", a));
	bl->addItem(std::make_shared<TextVariable>("${k2}", b));
	return std::static_pointer_cast<Variable>(bl);
};
std::vector<VariablePtr> colSkills { mkSkills("C++","Qt"), mkSkills("Python","Docs") };
table.addColumn(colName); table.addColumn(colAge); table.addColumn(colImg); table.addColumn(colSkills);
vars.add(std::make_shared<TableVariable>(table));
```
Helper aliases for brevity (user-defined):
```cpp
auto TV = [](QString key, QString v){ return std::make_shared<TextVariable>(key, v); };
auto IV = [&](QString key, const QImage &img, int w, int h){ return std::make_shared<ImageVariable>(key, img, w, h); };
```

## Build Options
| Option | Default | Description |
|--------|---------|-------------|
| QDT_FORCE_SYSTEM_LIBZIP | OFF | Require system libzip or fail |
| QDT_FORCE_FETCH_MINIZIP | OFF | Force FetchContent minizip-ng |
| QDT_BUILD_DOCS | OFF | Build Doxygen docs target `docs` |
| QTDOCTXTEMPLATE_BUILD_TESTS | ON | Build tests (ctest) |
| QTDOCTXTEMPLATE_BUILD_EXAMPLES | ON | Build examples |

Feature flags exported in `QtDocxTemplateConfig.cmake`:
`QtDocxTemplate_WITH_LIBZIP`, `QtDocxTemplate_WITH_MINIZIP`.

## Style & Replacement Semantics (Parity Rules)
- Variable detection spans run boundaries; duplicates deduped in `findVariables()`.
- Replacement preserves leading/trailing whitespace and first run styling.
- Non-text (images) inserted structurally (not aggregated into surrounding text runs).
- Table replication uses the first row under the table variable cell as a template.

## Roadmap / Non-Goals
Strict parity first. Out of scope (for now): headers/footers, numbering overrides, hyperlinks, advanced drawing objects, nested tables beyond simple row replication.

## Contributing
See CONTRIBUTING.md.

## License & Notices
Apache-2.0. See LICENSE and THIRD_PARTY_NOTICES.md. This project is independent from the original Java implementation.

## Parity Statement
Behavior limited intentionally to templ4docx scope; deviations are treated as bugs unless mandated by platform differences (Qt / C++ specifics).
