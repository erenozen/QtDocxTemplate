# Changelog

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
