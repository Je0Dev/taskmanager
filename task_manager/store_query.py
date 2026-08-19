"""Query operations for the task store."""

from __future__ import annotations

import threading
from typing import TYPE_CHECKING, Any

from .search import find_by_tag, find_text
from .sorting import sort_task_list
from .task import Task

if TYPE_CHECKING:
    from .models import TaskStore


class StoreQueryMixin:
    """Read-only queries and settings for the task store."""

    _tasks: list[Task]
    _lock: threading.RLock
    _next_id: int
    color_enabled: bool

    @property
    def tasks(self) -> list[Task]:
        """Return a snapshot copy of the task list."""
        with self._lock:
            return list(self._tasks)

    def get_task(self, task_id: int) -> Task | None:
        """Return the task with the given stable ID, or None."""
        with self._lock:
            for task in self._tasks:
                if task.id == task_id:
                    return task
            return None

    def find_tasks(self, term: str) -> list[Task]:
        """Full-text search over descriptions and notes (case-insensitive)."""
        return find_text(self.tasks, term)

    def find_by_tag(self, tag: str) -> list[Task]:
        """Return all tasks carrying the exact given tag."""
        return find_by_tag(self.tasks, tag)

    def sort_tasks(self, sort_by: str = "name") -> None:
        """Sort tasks by name, priority, or due date (invalid keys fall back to name)."""
        with self._lock:
            sort_task_list(self._tasks, sort_by)

    def set_color(self, enabled: bool) -> None:
        """Enable or disable color/theme output."""
        self.color_enabled = enabled

    @classmethod
    def restore(cls: type[Any], tasks: list[dict[str, Any]], color_enabled: bool = True) -> TaskStore:
        """Rebuild a store from serialized task dicts, restoring stable IDs."""
        store = cls(color_enabled=color_enabled)
        store._tasks = [Task.from_dict(data) for data in tasks]
        store._next_id = max((task.id for task in store._tasks), default=0) + 1
        return store

    def _pending_dependencies(self, task: Task) -> list[tuple[int, str]]:
        pending = []
        for dep_id in task.dependencies:
            dep = self.get_task(dep_id)
            if dep is not None and not dep.completed:
                pending.append((dep_id, dep.description))
        return pending
