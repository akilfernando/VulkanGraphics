# Copilot Instructions

## General Guidelines
- Avoid defining the same function in multiple translation units.
- Keep implementations in .cpp files and declarations in headers.
- If a temp or duplicate .cpp exists, remove it from the build.
- Keep code minimal — remove debug prints and non-essential code; prefer small, readable implementations with only essential diagnostics.

## Code Style
- Use specific formatting rules.
- Follow naming conventions.