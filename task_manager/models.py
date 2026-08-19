"""Public task store model, combining mutation and query mixins."""

from __future__ import annotations

import threading

from .store_mutation import StoreMutationMixin
from .store_query import StoreQueryMixin
from .task import DependencyError, Task

__all__ = ["DependencyError", "Task", "TaskStore"]


class TaskStore(StoreQueryMixin, StoreMutationMixin):
    """Thread-safe collection of tasks with all task-management operations."""

    def __init__(self, color_enabled: bool = True) -> None:
        self._tasks: list[Task] = []
        self._next_id = 1
        self.color_enabled = color_enabled
        self._lock = threading.RLock()
