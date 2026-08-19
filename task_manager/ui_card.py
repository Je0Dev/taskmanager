"""Single task card rendering for the task manager dashboard."""

from __future__ import annotations

from collections.abc import Callable

from nicegui import ui

from .models import DependencyError, Task, TaskStore
from .ui_confirm import confirm_delete, confirm_force_complete
from .ui_dialogs import open_due, open_edit, open_note
from .ui_relations import open_dependencies, open_tags


def render_task_card(
    store: TaskStore, task: Task, on_change: Callable[[], None], on_done: Callable[[str], None]
) -> None:
    """Render one task as a card with status, badges, and action buttons."""

    def toggle_done(checked: bool) -> None:
        if checked:
            try:
                store.mark_complete(task.id)
            except DependencyError as exc:
                confirm_force_complete(store, task, exc, on_done)
                return
        else:
            task.completed = False
        on_change()

    with ui.card().classes("w-full"):
        with ui.row().classes("w-full items-center gap-3"):
            ui.checkbox(
                value=task.completed, on_change=lambda e: toggle_done(e.value)
            ).props("dense").mark(f"done-{task.id}")
            description = ui.label(task.description).classes("flex-1 text-base")
            if task.completed:
                description.classes("line-through text-gray-400")
            elif task.high_priority:
                description.classes("text-red-400 font-semibold")
            if task.high_priority:
                ui.badge("!", color="red").tooltip("High priority")
            if task.notes is not None:
                ui.badge("N", color="blue").tooltip(task.notes)
            if task.due_date:
                ui.badge(task.due_date, color="yellow").tooltip("Due date")
            for tag in task.tags:
                ui.badge(tag, color="cyan").tooltip(f"Tag: {tag}")
        if task.dependencies:
            with ui.row().classes("w-full items-center gap-1"):
                ui.icon("link").classes("text-gray-400 text-sm")
                for dep_id in task.dependencies:
                    dep = store.get_task(dep_id)
                    done = bool(dep and dep.completed)
                    label = dep.description if dep else "(missing)"
                    ui.badge(f"#{dep_id}", color="green" if done else "orange").tooltip(label)
        with ui.row().classes("w-full justify-end gap-1"):
            for icon, action in (
                ("edit", lambda: open_edit(store, task, on_done)),
                ("notes", lambda: open_note(store, task, on_done)),
                ("event", lambda: open_due(store, task, on_done)),
                ("tag", lambda: open_tags(store, task, on_done)),
                ("link", lambda: open_dependencies(store, task, on_done)),
            ):
                ui.button(icon=icon, on_click=action).props("dense flat round").mark(f"{icon}-{task.id}")
            ui.button(icon="delete", on_click=lambda: confirm_delete(store, task, on_done)).props(
                "dense flat round color=negative"
            ).mark(f"delete-{task.id}")
