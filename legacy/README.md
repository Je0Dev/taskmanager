# Legacy C Implementation

This directory holds the original C version of the task manager, kept for
reference. It is **not** part of the active Python application.

## Files

- `task.c`, `task_manager.c`, `task_manager.h` - the C source (thread-safe
  task list, pipe-delimited `tasks.dat` persistence, dependency graph, sort,
  search, ANSI colors).
- `task_manager.bin` - the compiled Linux binary (built with
  `gcc task_manager.c task.c -o task_manager -pthread`).
- `.vscode/` - editor config for the C build.

## What moved to Python

The main project was rewritten in Python with a modern NiceGUI interface and
JSON storage:

| C feature                        | Python equivalent                              |
|----------------------------------|------------------------------------------------|
| `add` / `p-add`                  | `TaskStore.add_task(..., high_priority=...)`   |
| `list` / `view`                  | dashboard task cards                           |
| `done <id>` (dependency check)   | `TaskStore.mark_complete(id, force=...)`       |
| `edit` / `note` / `delete`       | `edit_task` / `add_note` / `delete_task`       |
| `due` / `tag` / `untag` / `findtag` | `set_due_date` / `add_tag` / `remove_tag` / `find_by_tag` |
| `depend`                         | `add_dependency`                               |
| `sort name\|pri\|date`           | `sort_tasks(...)`                              |
| `find <term>`                    | `find_tasks(term)`                             |
| `config color on/off`            | `TaskStore.set_color(...)` + theme switch      |
| background save to `tasks.dat`   | atomic JSON save (auto + manual) to `tasks.json` |

## Note on IDs

The C version used position-based 1-based IDs and renumbered tasks on delete.
The Python version uses **stable monotonic IDs**, so deleting a task never
renumbers the rest and dependency links stay valid.