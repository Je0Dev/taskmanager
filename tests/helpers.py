"""Shared helpers for UI interaction tests."""

from __future__ import annotations

from nicegui import ui
from task_manager.models import TaskStore


def add_task(user, store: TaskStore, text: str, *, high: bool = False) -> None:
    """Type into the add form and click Add."""
    user.find(kind=ui.input, marker="add-input").type(text)
    if high:
        user.find(kind=ui.switch, marker="high-priority").click()
    user.find(kind=ui.button, marker="add-btn").click()
