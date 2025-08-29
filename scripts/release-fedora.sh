#!/usr/bin/env bash
set -euo pipefail

echo "[QtDocxTemplate] Building Release with tests (minizip fallback)…"
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DQTDOCTXTEMPLATE_BUILD_TESTS=ON \
  -DQTDOCTXTEMPLATE_BUILD_EXAMPLES=OFF \
  -DQDT_BUILD_DOCS=ON \
  -DQDT_FORCE_FETCH_MINIZIP=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
cmake --build build --target docs -j || true
cmake --install build --prefix "${PWD}/install"

echo
echo "Install tree: ${PWD}/install"
if [ -d build/docs/html ]; then
  echo "Docs: build/docs/html (open index.html)"
else
  echo "Docs: (not generated)"
fi

echo
echo "Next steps to tag v1.0.0:"
echo " git add -A && git commit -m 'chore: release v1.0.0'"
echo " git tag -a v1.0.0 -m 'QtDocxTemplate v1.0.0 — parity-only port of templ4docx'"
echo " git push && git push --tags"
echo "Optional GitHub release: gh release create v1.0.0 --title 'QtDocxTemplate v1.0.0' --notes-file CHANGELOG.md --generate-notes"
