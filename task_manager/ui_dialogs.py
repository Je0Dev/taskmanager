"""Dialogs for editing a task, its note, and its due date."""

from __future__ import annotations

from collections.abc import Callable

from nicegui import ui

from .models import Task, TaskStore


def open_edit(store: TaskStore, task: Task, on_done: Callable[[str], None]) -> None:
    """Edit a task's description."""

    def save() -> None:
        new_desc = field.value.strip()
        if not new_desc:
            ui.notify("Description cannot be empty", type="warning")
            return
        store.edit_task(task.id, new_desc)
        dialog.close()
        on_done("Description updated")

    with ui.dialog() as dialog:
        with dialog:
            with ui.card().classes("w-96 max-w-full"):
                ui.label("Edit task").classes("text-lg font-bold")
                field = (
                    ui.input(value=task.description).props("outlined").classes("w-full").mark("edit-field")
                )
                field.on("keydown.enter", save)
                with ui.row().classes("w-full justify-end gap-2"):
                    ui.button("Cancel", on_click=dialog.close).props("flat")
                    ui.button("Save", on_click=save).props("unelevated color=primary").mark("dialog-save")
    dialog.open()


def open_note(store: TaskStore, task: Task, on_done: Callable[[str], None]) -> None:
    """Add or update a task's note."""

    def save() -> None:
        store.add_note(task.id, field.value.strip())
        dialog.close()
        on_done("Note saved")

    with ui.dialog() as dialog:
        with dialog:
            with ui.card().classes("w-96 max-w-full"):
                ui.label("Note").classes("text-lg font-bold")
                field = (
                    ui.textarea(value=task.notes or "").props("outlined autogrow").classes("w-full").mark("note-field")
                )
                with ui.row().classes("w-full justify-end gap-2"):
                    ui.button("Cancel", on_click=dialog.close).props("flat")
                    ui.button("Save", on_click=save).props("unelevated color=primary").mark("dialog-save")
    dialog.open()


def open_due(store: TaskStore, task: Task, on_done: Callable[[str], None]) -> None:
    """Set or clear a task's due date."""

    def save() -> None:
        try:
            store.set_due_date(task.id, field.value)
        except ValueError as exc:
            ui.notify(str(exc), type="warning")
            return
        dialog.close()
        on_done("Due date set")

    def clear() -> None:
        task.due_date = None
        dialog.close()
        on_done("Due date cleared")

    with ui.dialog() as dialog:
        with dialog:
            with ui.card().classes("w-96 max-w-full"):
                ui.label("Due date").classes("text-lg font-bold")
                field = (
                    ui.input(value=task.due_date or "").props("outlined type=date").classes("w-full").mark("due-field")
                )
                with ui.row().classes("w-full justify-end gap-2"):
                    ui.button("Clear", on_click=clear).props("flat")
                    ui.button("Cancel", on_click=dialog.close).props("flat")
                    ui.button("Save", on_click=save).props("unelevated color=primary").mark("dialog-save")
    dialog.open()
