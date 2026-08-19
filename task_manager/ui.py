"""Main page layout for the task manager NiceGUI app."""

from __future__ import annotations

from nicegui import ui

from .models import Task, TaskStore
from .storage import save_store
from .ui_card import render_task_card

SORT_LABELS = {"name": "Name", "pri": "Priority", "date": "Due date"}


class ViewState:
    """Mutable view state bound to the filter controls."""

    def __init__(self) -> None:
        self.search = ""
        self.tag = ""
        self.sort = "name"


def build_page(store: TaskStore, save_path: str | None = None) -> None:
    """Render the full dashboard for the given store."""
    state = ViewState()
    dark = ui.dark_mode(store.color_enabled)

    def _toggle_color(e) -> None:
        store.set_color(e.value)
        dark.value = e.value

    def all_tags() -> list[str]:
        return sorted({tag for task in store.tasks for tag in task.tags})

    def on_search(value: str | None) -> None:
        state.search = value or ""
        refresh()

    def on_sort(value: str | None) -> None:
        state.sort = value or "name"
        refresh()

    def on_tag_filter(value: str | None) -> None:
        state.tag = value or ""
        refresh()

    def visible_tasks() -> list[Task]:
        store.sort_tasks(state.sort)
        tasks = store.tasks
        if state.search:
            needle = state.search.lower()
            tasks = [t for t in tasks if needle in (t.description + " " + (t.notes or "")).lower()]
        if state.tag:
            tasks = [t for t in tasks if state.tag in t.tags]
        return tasks

    with ui.header().classes("items-center gap-3 px-4"):
        ui.label("Task Manager").classes("text-xl font-bold")
        save_status = ui.label("").classes("text-sm text-gray-400")
        ui.space()
        search_box = ui.input(placeholder="Search tasks…").props("dense outlined").classes("w-52").mark("search")
        search_box.on_value_change(lambda e: on_search(e.value))
        sort_box = ui.select(SORT_LABELS, value=state.sort, label="Sort").props("dense outlined").mark("sort")
        sort_box.on_value_change(lambda e: on_sort(e.value))
        tag_options = {"": "All tags", **{t: t for t in all_tags()}}
        tag_filter = ui.select(tag_options, value="", label="Tag").props("dense outlined").mark("tag-filter")
        tag_filter.on_value_change(lambda e: on_tag_filter(e.value))
        color_switch = ui.switch("Colors", value=store.color_enabled).mark("colors")
        color_switch.on_value_change(_toggle_color)
        ui.button("Save", on_click=lambda: persist("Saved")).props("dense unelevated").mark("save-btn")

    tasks_box = ui.column().classes("w-full max-w-4xl mx-auto p-4 gap-3")

    def persist(message: str = "Saved") -> None:
        if save_path:
            save_store(store, save_path)
        from datetime import datetime

        save_status.set_text(f"{message} at {datetime.now():%H:%M:%S}")

    def on_done(message: str) -> None:
        ui.notify(message, type="positive")
        persist()
        refresh()

    def refresh() -> None:
        tag_filter.options = {"": "All tags", **{t: t for t in all_tags()}}
        tasks_box.clear()
        with tasks_box:
            if not store.tasks:
                ui.label("No tasks yet — add one below.").classes("text-gray-400")
            else:
                with ui.card().classes("w-full px-4 py-2"):
                    ui.label(f"{len(store.tasks)} task(s)").classes("text-sm text-gray-400")
                for task in visible_tasks():
                    render_task_card(store, task, refresh, on_done)

    def do_add() -> None:
        text = add_input.value.strip()
        if not text:
            ui.notify("Description required", type="warning")
            return
        store.add_task(text, high_priority=high_switch.value)
        add_input.value = ""
        high_switch.value = False
        persist()
        refresh()

    with ui.column().classes("w-full max-w-4xl mx-auto p-4 gap-3"):
        with ui.card().classes("w-full"):
            with ui.row().classes("w-full items-center gap-2"):
                add_input = (
                    ui.input(placeholder="What needs doing?").props("outlined").classes("flex-1").mark("add-input")
                )
                add_input.on("keydown.enter", do_add)
                high_switch = ui.switch("High priority").props("dense").mark("high-priority")
                ui.button("Add", on_click=do_add).props("unelevated color=primary").mark("add-btn")

    refresh()
