"""Sorting keys and helpers matching the C version's comparators."""

from __future__ import annotations

from collections.abc import Callable

from .task import Task

SORT_KEYS = ("name", "pri", "date")


def task_sort_key(sort_by: str) -> Callable[[Task], tuple]:
    """Build a sort key function for a named sort mode."""

    def key(task: Task) -> tuple:
        if sort_by == "name":
            return (0, task.description.lower())
        if sort_by == "pri":
            return (0 if task.high_priority else 1, task.description.lower())
        return (
            1 if task.due_date is None else 0,
            task.due_date or "",
            0 if task.high_priority else 1,
            task.description.lower(),
        )

    return key


def sort_task_list(tasks: list[Task], sort_by: str) -> None:
    """Sort a task list in place, falling back to name on invalid keys."""
    key = sort_by if sort_by in SORT_KEYS else "name"
    tasks.sort(key=task_sort_key(key))
