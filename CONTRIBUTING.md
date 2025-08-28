# Contributing

Thanks for your interest! The project goal is strict feature parity with jsolve/templ4docx (text, image, bullet list, table replacements in the main document part). Please keep additions within that scope unless discussed first.

## Development Setup
```
cmake -S . -B build -DQTDOCTXTEMPLATE_BUILD_TESTS=ON
cmake --build build -j
ctest --test-dir build -V
```

## Coding Standards
- C++17
- Prefer explicit over implicit conversions
- Keep public headers minimal and documented (brief Doxygen comments)
- Avoid introducing new dependencies without discussion

## Style
A future .clang-format will define style; until then, follow existing formatting.

## Tests
Add or update tests for every behavior change. Edge cases for run-spanning variables are especially important.

## Commit Messages
- Use concise imperative subject
- Reference issues where relevant

## Pull Requests
- Ensure CI passes
- Include description of change & rationale

## License
By contributing you agree that your contributions are licensed under Apache-2.0.
