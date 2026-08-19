"""Tag and dependency management dialogs."""

from __future__ import annotations

from collections.abc import Callable

from nicegui import ui

from .models import Task, TaskStore


def open_tags(store: TaskStore, task: Task, on_done: Callable[[str], None]) -> None:
    """Manage a task's tags."""

    def render_tags() -> None:
        tags_box.clear()
        for tag in task.tags:
            with ui.row().classes("w-full items-center gap-2"):
                ui.badge(tag, color="cyan")
                ui.button(icon="close", on_click=lambda name=tag: remove(name)).props("dense flat round size=sm")

    def remove(tag: str) -> None:
        try:
            store.remove_tag(task.id, tag)
        except KeyError:
            pass
        render_tags()
        on_done(f'Removed tag "{tag}"')

    def add() -> None:
        name = new_tag.value.strip()
        if not name:
            return
        try:
            store.add_tag(task.id, name)
        except ValueError:
            ui.notify("Tag already exists", type="warning")
            return
        new_tag.value = ""
        render_tags()
        on_done(f'Added tag "{name}"')

    with ui.dialog() as dialog:
        with dialog:
            with ui.card().classes("w-96 max-w-full"):
                ui.label("Tags").classes("text-lg font-bold")
                tags_box = ui.column().classes("w-full gap-1")
                new_tag = ui.input(placeholder="New tag").props("dense outlined").classes("w-full")
                new_tag.on("keydown.enter", add)
                with ui.row().classes("w-full justify-end gap-2"):
                    ui.button("Add", on_click=add).props("unelevated color=primary").mark("dialog-add")
                    ui.button("Done", on_click=dialog.close).props("flat")
                render_tags()
    dialog.open()


def open_dependencies(store: TaskStore, task: Task, on_done: Callable[[str], None]) -> None:
    """Manage which tasks this task depends on."""

    def render_deps() -> None:
        deps_box.clear()
        for dep_id in task.dependencies:
            dep = store.get_task(dep_id)
            label = dep.description if dep else "(missing)"
            done = dep.completed if dep else False
            color = "green" if done else "orange"
            with ui.row().classes("w-full items-center gap-2"):
                ui.badge(f"#{dep_id}", color=color).tooltip(label)
                ui.label(label).classes("flex-1")
                ui.button(icon="close", on_click=lambda i=dep_id: remove(i)).props("dense flat round size=sm")

    def remove(dep_id: int) -> None:
        task.dependencies.remove(dep_id)
        render_deps()
        on_done(f"Removed dependency on #{dep_id}")

    def add() -> None:
        dep_id = pick.value
        if dep_id is None:
            return
        try:
            store.add_dependency(task.id, int(dep_id))
        except ValueError as exc:
            ui.notify(str(exc), type="warning")
            return
        pick.value = None
        render_deps()
        on_done(f"Task {task.id} now depends on #{dep_id}")

    options = {
        str(t.id): f"#{t.id} — {t.description}"
        for t in store.tasks
        if t.id != task.id and t.id not in task.dependencies
    }
    with ui.dialog() as dialog:
        with dialog:
            with ui.card().classes("w-96 max-w-full"):
                ui.label("Dependencies").classes("text-lg font-bold")
                deps_box = ui.column().classes("w-full gap-1")
                pick = (
                    ui.select(options, value=None, with_input=True, label="Depends on")
                    .props("dense outlined")
                    .classes("w-full")
                )
                with ui.row().classes("w-full justify-end gap-2"):
                    ui.button("Add", on_click=add).props("unelevated color=primary").mark("dialog-add")
                    ui.button("Done", on_click=dialog.close).props("flat")
                render_deps()
    dialog.open()
