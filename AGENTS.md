# AGENTS.md

Guidance for AI coding agents working in this repository.

## Project overview

A modern Python rewrite of the original C `cli_task_manager_system`. It is a
**GUI-only** task manager built with [NiceGUI](https://nicegui.io/) (web UI),
persisting tasks to `tasks.json`, with `pytest`-driven tests. The original C
sources are preserved under `legacy/` for reference but are not part of the
active app.

## Repository layout

| Path | Purpose |
|------|---------|
| `app.py` | Entry point: loads store, builds page, `ui.run(...)`. |
| `task_manager/models.py` | Public `TaskStore` (combines query + mutation mixins). |
| `task_manager/task.py` | `Task` dataclass, `DependencyError`. |
| `task_manager/store_mutation.py` / `store_query.py` | `TaskStore` CRUD + query mixins. |
| `task_manager/search.py` / `sorting.py` | Pure search/filter/sort helpers. |
| `task_manager/storage.py` | Atomic JSON save/load (`tasks.json`). |
| `task_manager/ui.py` | Dashboard layout, filters, add form, auto-save timer. |
| `task_manager/ui_card.py` | Single task card rendering + per-task action markers. |
| `task_manager/ui_dialogs.py` | Edit / note / due-date dialogs. |
| `task_manager/ui_relations.py` | Tags / dependencies dialogs. |
| `task_manager/ui_confirm.py` | Force-complete / delete confirmation dialogs. |
| `tests/` | `pytest` tests (models, storage, UI via NiceGUI user_simulation). |
| `legacy/` | Original C implementation (read-only reference). |

## Commands

Run from the repo root with the project venv:

```bash
.venv/bin/python -m pytest -q        # all tests (38 currently passing)
.venv/bin/ruff check task_manager/ tests/ app.py   # lint (must be clean)
.venv/bin/mypy task_manager/ app.py  # type-check (must be clean)
.venv/bin/python app.py              # boot the app (http://localhost:8080)
```

## Conventions that matter

- Read `CONSTRAINTS.md` and `CONTRIBUTING.md` **before** editing.
- Never commit directly to `main`; work on a branch (`feat/`, `test/`, `fix/`,
  `docs/`, `chore/`), merge with `--no-ff`.
- Keep every source file **<= 100-120 lines**; split files if they grow past
  the limit.
- Keep functions small and single-purpose. Pure logic (sorting, search,
  filtering, dependency checks) belongs in `search.py` / `sorting.py` / the
  `TaskStore` mixins, not the UI layer.
- Type hints are mandatory on public signatures; run `mypy` to confirm.
- Comments explain *why*; every public function has a short docstring.
- Use the existing `page_user` fixture for UI tests:
  `async def test_x(page_user)` then `user, store = page_user`.
- NiceGUI gotchas that already cost time here:
  - Dialogs use `with dialog:` (no `.card()` in v3).
  - In tests, `user.find(...)`, `.click()`, `.type()` are **sync**;
    `user.open()`, `should_see()`, `should_not_see()` are **async**.
  - `.type()` does not work on `ui.textarea`; set `element.value` directly.
  - Select popups cannot be clicked through `user.scope()`; set
    `select.value` directly to trigger the change handler.
  - Target testable elements with `.mark("...")`.

## Storage

- File: `tasks.json` in the working directory (`task_manager/storage.py`).
- Saves are atomic (write temp file, then `os.replace`), so a crash never
  corrupts data.
- Auto-save every 30s plus save on key actions; the "Save" button forces one.
- IDs are stable monotonic integers (never renumbered on delete), so
  dependency links stay valid.

## Test strategy

- `tests/test_models.py` - Task/TaskStore CRUD: add, get, edit, note, due-date,
  tags, serialization round-trip.
- `tests/test_dependencies.py` - dependency blocking, force-complete,
  stale-dependency cleanup, stable IDs.
- `tests/test_search_sort.py` - full-text/tag search, sorting, color toggle.
- `tests/test_storage.py` - round-trip, missing file, atomic save, corrupted JSON.
- `tests/test_ui.py` - end-to-end UI flows: add, complete (blocked + forced),
  delete, color toggle, auto-save.
- `tests/test_ui_filters.py` - search, tag filter, sort, edit, note dialogs.

Important features need tests; small glue helpers and thin UI wrappers do not.