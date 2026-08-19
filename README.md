# Task Manager

A modern, **GUI-only** task manager rewritten in Python with
[NiceGUI](https://nicegui.io/). The original C version (kept in `legacy/`) is
no longer part of the app; the Python build adds a web dashboard, atomic JSON
persistence, and a full `pytest` test suite while preserving every feature:
tasks with notes, due dates, tags, priorities, dependency graphs, sorting,
search, and color theming.

## Features

- **Task management** - add, edit, delete, mark done; each task can carry a
  description, notes, a due date, tags, and a high/low priority.
- **Dependencies** - a task can depend on other tasks; completing it is
  blocked while any dependency is unfinished, with a "complete anyway"
  override.
- **Sorting** - by description, priority, or due date.
- **Search & filters** - full-text search over descriptions/notes and a
  per-tag filter.
- **Safe persistence** - atomic saves to `tasks.json` (auto-save every 30s,
  save on key actions, plus a Save button).
- **Stable IDs** - tasks keep monotonic integer IDs that are never renumbered
  on delete, so dependency links always stay valid.

## Quick start

```bash
python -m venv .venv
.venv/bin/pip install -r requirements.txt
.venv/bin/python app.py            # dashboard at http://localhost:8080
```

Optional native desktop window (needs `pip install pywebview`):

```bash
TASK_MANAGER_NATIVE=1 .venv/bin/python app.py
```

## Testing & quality

```bash
.venv/bin/python -m pytest -q         # 38 tests
.venv/bin/ruff check task_manager/ tests/ app.py
.venv/bin/mypy task_manager/ app.py
```

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
| `task_manager/ui_card.py` | Single task card rendering + per-task actions. |
| `task_manager/ui_dialogs.py` | Edit / note / due-date dialogs. |
| `task_manager/ui_relations.py` | Tags / dependencies dialogs. |
| `task_manager/ui_confirm.py` | Force-complete / delete confirmation dialogs. |
| `tests/` | `pytest` tests (models, storage, UI). |
| `legacy/` | Original C implementation (reference only). |

## Documentation

- [CONSTRAINTS.md](CONSTRAINTS.md) - code & workflow constraints.
- [CONTRIBUTING.md](CONTRIBUTING.md) - branching, commits, merge process.
- [PLAN.md](PLAN.md) - implementation plan and roadmap.
- [CHANGELOG.md](CHANGELOG.md) - release notes.
- [AGENTS.md](AGENTS.md) - guidance for AI coding agents.

## Feature-to-command mapping (C → Python)

| C command | Python equivalent |
|-----------|-------------------|
| `add` / `p-add` | add form + high-priority toggle |
| `done <id>` | done checkbox with dependency check / force override |
| `edit` / `note` / `delete` | edit / note / delete dialogs |
| `due` / `tag` / `untag` / `findtag` | due-date dialog / tag chips & filter |
| `depend` | dependencies dialog |
| `sort name\|pri\|date` | sort dropdown |
| `find <term>` | search box |
| `config color on/off` | color toggle |
| `save` / background save | auto-save timer + Save button |

## Licence

MIT - see [LICENSE](LICENSE).