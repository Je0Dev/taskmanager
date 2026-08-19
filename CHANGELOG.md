# Changelog

All notable changes to this project are documented here.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Python rewrite with a NiceGUI web dashboard (replaces the C CLI).
- `TaskStore` model: CRUD, dependencies with force-complete override, sorting,
  full-text search, tag filter, due dates, high-priority tasks.
- Atomic JSON persistence to `tasks.json` (temp file + `os.replace`).
- Auto-save every 30 seconds, save on key actions, and a Save button.
- Edit, note, due-date, tags, dependencies, and delete dialogs.
- Color toggle and sort/filter/search controls in the dashboard header.
- Full `pytest` suite (38 tests) covering models, storage, and UI flows.
- `ruff` (clean) and `mypy` (clean) enforced in CI via `pyproject.toml`.

### Changed

- Original C sources moved to `legacy/` for reference.
- Task IDs now stay stable (monotonic integers, never renumbered on delete)
  instead of the C version's position-based renumbering.

## [0.1.0] - 2026-08-19

- First Python release: all C features ported to the NiceGUI UI.