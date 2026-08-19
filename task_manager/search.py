"""Pure full-text and tag search helpers over a task list."""

from __future__ import annotations

from .task import Task


def find_text(tasks: list[Task], term: str) -> list[Task]:
    """Return tasks whose description or notes contain the term (case-insensitive)."""
    term = term.lower()
    return [
        t
        for t in tasks
        if term in t.description.lower() or (t.notes and term in t.notes.lower())
    ]


def find_by_tag(tasks: list[Task], tag: str) -> list[Task]:
    """Return all tasks carrying the exact given tag."""
    return [t for t in tasks if tag in t.tags]
