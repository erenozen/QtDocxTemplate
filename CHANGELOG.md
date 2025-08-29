# Changelog

## [1.0.0] - 2025-08-29
Parity-only port of jsolve/templ4docx.

Features: text, image, bullet list, table replacements; configurable VariablePattern; main document part only.
Run-aware replacements with style preserved from first overlapped run.
Packaging: CMake config package; libzip or minizip backend exported via feature flags; CI matrix, sanitizers, docs check.

No public API changes planned for further parity maintenance releases.

## [0.1.0] - Unreleased
### Added
- Initial implementation: package handling (libzip / minizip-ng fallback)
- XML parsing (pugixml) and variable detection with custom patterns
- Robust run model with style preservation across fragmented runs
- Text variable replacement with edge-case handling (overlaps, whitespace, duplicates)
- ImageVariable insertion (drawing relationships + EMU sizing)
- BulletListVariable (paragraph cloning) support
- TableVariable (row replication) support
- CMake options: QDT_BUILD_DOCS, QDT_FORCE_SYSTEM_LIBZIP, QDT_FORCE_FETCH_MINIZIP
- Version header generation, uninstall target, Doxygen scaffolding
- Exported feature flags (QtDocxTemplate_WITH_LIBZIP / WITH_MINIZIP) in config file
- Basic repository docs: README, LICENSE, THIRD_PARTY_NOTICES, CONTRIBUTING, SECURITY

### Pending
- CI workflows (build matrix, sanitizers)
- Formatting & static analysis configs
- Doxygen comment coverage expansion
