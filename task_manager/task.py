"""Task data model and the dependency error type."""

from __future__ import annotations

from dataclasses import dataclass, field
from typing import Any


class DependencyError(Exception):
    """Raised when a task cannot be completed because its dependencies are pending."""

    def __init__(self, task_id: int, pending: list[tuple[int, str]]) -> None:
        super().__init__(f"Task {task_id} has incomplete dependencies")
        self.task_id = task_id
        self.pending = pending


@dataclass
class Task:
    """A single to-do item."""

    id: int
    description: str
    notes: str | None = None
    completed: bool = False
    high_priority: bool = False
    due_date: str | None = None
    tags: list[str] = field(default_factory=list)
    dependencies: list[int] = field(default_factory=list)

    def to_dict(self) -> dict[str, Any]:
        """Serialize to a plain dict for JSON storage."""
        return {
            "id": self.id,
            "description": self.description,
            "notes": self.notes,
            "completed": self.completed,
            "high_priority": self.high_priority,
            "due_date": self.due_date,
            "tags": list(self.tags),
            "dependencies": list(self.dependencies),
        }

    @classmethod
    def from_dict(cls, data: dict[str, Any]) -> Task:
        """Build a Task from a serialized dict."""
        return cls(
            id=int(data["id"]),
            description=str(data["description"]),
            notes=data.get("notes"),
            completed=bool(data.get("completed", False)),
            high_priority=bool(data.get("high_priority", False)),
            due_date=data.get("due_date"),
            tags=[str(tag) for tag in data.get("tags", [])],
            dependencies=[int(dep) for dep in data.get("dependencies", [])],
        )
