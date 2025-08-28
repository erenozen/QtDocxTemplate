# Security Policy

## Supported Versions
Latest main branch (no formal releases yet) receives security updates.

## Reporting a Vulnerability
Please open a private security advisory or email the maintainer (see git commit history) with details:
- Affected versions / commit hash
- Reproduction steps
- Impact assessment
- Suggested fix (if any)

You will receive a response within 5 business days.

## Scope
This project processes DOCX (ZIP + XML) data. Potential classes of issues:
- Malformed ZIP leading to crashes (DoS)
- XML entity expansion (not applicable: pugixml does not process external entities by default)
- Path traversal when writing files (guarded by internal path normalization)

Please highlight any bypass you discover.
