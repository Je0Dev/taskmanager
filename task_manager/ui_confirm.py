"""Confirmation dialogs for force-complete and delete actions."""

from __future__ import annotations

from collections.abc import Callable

from nicegui import ui

from .models import DependencyError, Task, TaskStore


def confirm_force_complete(
    store: TaskStore, task: Task, error: DependencyError, on_done: Callable[[str], None]
) -> None:
    """Ask whether to complete a task despite pending dependencies."""

    def mark_anyway() -> None:
        store.mark_complete(task.id, force=True)
        dialog.close()
        on_done(f'Completed "{task.description}" anyway')

    def cancel() -> None:
        dialog.close()
        on_done("Cancelled")

    with ui.dialog() as dialog:
        with dialog:
            with ui.card().classes("w-96 max-w-full"):
                ui.label(f'Complete "{task.description}"?').classes("text-lg font-bold")
                ui.label("Pending dependencies:")
                for dep_id, description in error.pending:
                    ui.label(f"• #{dep_id} {description}")
                with ui.row().classes("w-full justify-end gap-2"):
                    ui.button("Cancel", on_click=cancel).props("flat").mark("force-cancel")
                    ui.button(
                        "Complete anyway",
                        on_click=mark_anyway,
                    ).props("unelevated color=primary").mark("force-complete")
    dialog.open()


def confirm_delete(store: TaskStore, task: Task, on_done: Callable[[str], None]) -> None:
    """Confirm before deleting a task."""

    def do_delete() -> None:
        store.delete_task(task.id)
        dialog.close()
        on_done(f'Deleted "{task.description}"')

    with ui.dialog() as dialog:
        with dialog:
            with ui.card().classes("w-96 max-w-full"):
                ui.label(f'Delete "{task.description}"?').classes("text-lg font-bold")
                ui.label("This cannot be undone.")
                with ui.row().classes("w-full justify-end gap-2"):
                    ui.button("Cancel", on_click=dialog.close).props("flat")
                    ui.button("Delete", on_click=do_delete).props("unelevated color=negative").mark("confirm-delete")
    dialog.open()
