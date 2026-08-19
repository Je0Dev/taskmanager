"""Mutation operations for the task store."""

from __future__ import annotations

import threading
from abc import ABC, abstractmethod
from datetime import datetime

from .task import DependencyError, Task


class StoreMutationMixin(ABC):
    """Task mutations: create, delete, edit, due dates, tags, dependencies."""

    _tasks: list[Task]
    _lock: threading.RLock
    _next_id: int

    @abstractmethod
    def get_task(self, task_id: int) -> Task | None:
        """Return the task with the given stable ID, or None."""

    @abstractmethod
    def _pending_dependencies(self, task: Task) -> list[tuple[int, str]]:
        """Return (id, description) pairs for still-pending dependencies."""

    def add_task(self, description: str, high_priority: bool = False) -> Task:
        """Add a new task and return it."""
        with self._lock:
            task = Task(id=self._next_id, description=description, high_priority=high_priority)
            self._next_id += 1
            self._tasks.append(task)
            return task

    def mark_complete(self, task_id: int, force: bool = False) -> Task:
        """Mark a task complete, refusing while dependencies are pending unless forced."""
        with self._lock:
            task = self._get(task_id)
            if not force:
                pending = self._pending_dependencies(task)
                if pending:
                    raise DependencyError(task_id, pending)
            task.completed = True
            return task

    def delete_task(self, task_id: int) -> Task:
        """Delete a task and strip any dependencies that point at it."""
        with self._lock:
            task = self._get(task_id)
            self._tasks.remove(task)
            for other in self._tasks:
                other.dependencies = [d for d in other.dependencies if d != task_id]
            return task

    def edit_task(self, task_id: int, new_description: str) -> Task:
        """Replace a task's description."""
        with self._lock:
            task = self._get(task_id)
            task.description = new_description
            return task

    def add_note(self, task_id: int, note_text: str) -> Task:
        """Set or replace a task's note."""
        with self._lock:
            task = self._get(task_id)
            task.notes = note_text
            return task

    def set_due_date(self, task_id: int, date_str: str) -> Task:
        """Set a task's due date (YYYY-MM-DD), raising ValueError on bad input."""
        try:
            datetime.strptime(date_str, "%Y-%m-%d")
        except ValueError as exc:
            raise ValueError(f"Invalid date {date_str!r}; use YYYY-MM-DD.") from exc
        with self._lock:
            task = self._get(task_id)
            task.due_date = date_str
            return task

    def add_tag(self, task_id: int, tag: str) -> Task:
        """Add a tag to a task, raising ValueError if it already exists."""
        with self._lock:
            task = self._get(task_id)
            if tag in task.tags:
                raise ValueError(f"Task {task_id} already has tag {tag!r}.")
            task.tags.append(tag)
            return task

    def remove_tag(self, task_id: int, tag: str) -> Task:
        """Remove a tag from a task, raising KeyError if it is absent."""
        with self._lock:
            task = self._get(task_id)
            try:
                task.tags.remove(tag)
            except ValueError as exc:
                raise KeyError(f"Task {task_id} has no tag {tag!r}.") from exc
            return task

    def add_dependency(self, task_id: int, dependency_id: int) -> Task:
        """Make one task depend on another, rejecting self/duplicate/invalid links."""
        with self._lock:
            task = self._get(task_id)
            self._get(dependency_id)
            if task_id == dependency_id:
                raise ValueError("A task cannot depend on itself.")
            if dependency_id in task.dependencies:
                raise ValueError(f"Task {task_id} already depends on task {dependency_id}.")
            task.dependencies.append(dependency_id)
            return task

    def _get(self, task_id: int) -> Task:
        task = self.get_task(task_id)
        if task is None:
            raise ValueError(f"Invalid task ID {task_id}.")
        return task
