# Task Manager Feature Plan

Roadmap and implementation plan for the Python + NiceGUI rewrite. Each
section lists goals, UX, storage changes, new modules/dependencies, and the
test approach.

- `[x]` = implemented
- `[ ]` = planned

## 1. [x] Python + NiceGUI rewrite

**Status:** done. Core logic, JSON storage, the NiceGUI dashboard, and the
full test suite are implemented. See `task_manager/`, `tests/`, and `app.py`.

### Goals

- Rewrite the C `cli_task_manager_system` in Python, keeping every feature:
  add/p-add, list, view, done (with dependency checking), edit, note, delete,
  due dates, tags, findtag, dependencies, sorting (name/pri/date), full-text
  search, color config, and persistent save.
- Ship a **modern GUI only** (no CLI REPL) built with
  [NiceGUI](https://nicegui.io/): pure-Python, Material Design, runs in the
  browser or as a native window.
- Persist tasks as JSON (`tasks.json`) with atomic writes.

### Architecture

- `task_manager/models.py` - public `TaskStore` (combines the mutation and
  query mixins below).
- `task_manager/task.py` - `Task` dataclass, `DependencyError`.
- `task_manager/store_mutation.py` / `store_query.py` - `TaskStore` CRUD and
  query mixins (thread-safe via a lock, fully type-hinted).
- `task_manager/search.py` / `sorting.py` - pure search/filter/sort helpers.
- `task_manager/storage.py` - JSON serialization with atomic writes and
  auto-save support.
- `task_manager/ui.py` - NiceGUI dashboard: task cards, filters, sort
  controls, add form, color theme toggle, auto-save timer.
- `task_manager/ui_card.py`, `ui_dialogs.py`, `ui_relations.py`,
  `ui_confirm.py` - task card and dialog components.
- `app.py` - entry point; launches the NiceGUI app in a browser window.
- `tests/` - pytest suite for models, storage, and UI interactions.

### Storage

- JSON document: `{ "config": { "color": true }, "tasks": [ ... ] }`.
- Tasks use stable monotonic integer IDs (no renumbering on delete, unlike the
  C version's position-based IDs).
- Atomic save (temp file + `os.replace`) so a crash never corrupts data.

### Tests

- `test_models.py` - CRUD, due-date validation, tag add/remove, round-trip.
- `test_dependencies.py` - dependency blocking, force-complete, stale deps,
  stable IDs.
- `test_search_sort.py` - all three sort orders, full-text and tag search,
  color toggle.
- `test_storage.py` - save/load round-trip, atomic write, missing file.
- `test_ui.py` / `test_ui_filters.py` - NiceGUI `user_simulation` interaction
  tests.

## 2. [ ] Data export

### Goals

- Export tasks to portable formats so data is never locked in the app.

### Formats

- **CSV** - header: id, description, notes, status, priority, due_date, tags,
  dependencies.
- **JSON** - a full export of the same fields.

### Tests

- Export output for a sample task list, including Unicode and quoting.

## 3. [ ] Recurring tasks

### Goals

- Mark a task as recurring (daily/weekly/monthly) and auto-reschedule it when
  it is marked complete.

### Tests

- Rescheduling rules for each recurrence type, including leap-year edge cases.

## 4. [ ] Notifications / reminders

### Goals

- Desktop or in-app reminders for tasks due today or overdue.

### Tests

- Due-date classification helpers (overdue, due today, upcoming).

## 5. [ ] CI, packaging & README install guide

### Goals

- GitHub Actions workflow running `pytest`, `ruff check`, and `mypy`.
- Release packaging and clear install instructions for Linux, Windows, and
  macOS.

### Tests

- N/A (tooling and docs).

## Out of scope (for now)

- Cloud sync / multi-user.
- Mobile apps.
- Native desktop build beyond NiceGUI's native mode.