"""Tests for JSON persistence."""

import json

from task_manager.models import TaskStore
from task_manager.storage import load_store, save_store


def test_round_trip_preserves_all_fields(tmp_path) -> None:
    store = TaskStore()
    task = store.add_task("design system", high_priority=True)
    store.add_note(task.id, "check the notes")
    store.set_due_date(task.id, "2026-05-01")
    store.add_tag(task.id, "work")
    store.add_tag(task.id, "urgent")
    second = store.add_task("gather feedback")
    store.add_dependency(second.id, task.id)
    store.set_color(False)

    path = tmp_path / "tasks.json"
    save_store(store, path)

    loaded = load_store(path)
    assert loaded.color_enabled is False
    assert [t.id for t in loaded.tasks] == [1, 2]
    restored = loaded.get_task(1)
    assert restored.description == "design system"
    assert restored.high_priority is True
    assert restored.completed is False
    assert restored.notes == "check the notes"
    assert restored.due_date == "2026-05-01"
    assert restored.tags == ["work", "urgent"]
    assert loaded.get_task(2).dependencies == [1]


def test_restore_resumes_next_id(tmp_path) -> None:
    store = TaskStore()
    store.add_task("a")
    store.add_task("b")
    path = tmp_path / "tasks.json"
    save_store(store, path)
    loaded = load_store(path)
    fresh = loaded.add_task("c")
    assert fresh.id == 3


def test_missing_file_yields_empty_store(tmp_path) -> None:
    loaded = load_store(tmp_path / "nope.json")
    assert loaded.tasks == []
    assert loaded.add_task("x").id == 1


def test_file_is_valid_json_and_atomic(tmp_path) -> None:
    store = TaskStore()
    store.add_task("hello world")
    path = tmp_path / "tasks.json"
    save_store(store, path)
    data = json.loads(path.read_text(encoding="utf-8"))
    assert data["tasks"][0]["description"] == "hello world"
    assert "config" in data
    leftovers = list(tmp_path.glob("*.tmp"))
    assert leftovers == []
