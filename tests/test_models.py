"""Tests for the Task dataclass and store CRUD operations."""

import pytest
from task_manager.models import Task, TaskStore


def test_add_task_assigns_incrementing_ids(store: TaskStore) -> None:
    first = store.add_task("write docs")
    second = store.add_task("ship release", high_priority=True)
    assert (first.id, second.id) == (1, 2)
    assert second.high_priority is True
    assert not second.completed


def test_get_task_returns_none_for_missing(store: TaskStore) -> None:
    assert store.add_task("x").id == 1
    assert store.get_task(99) is None


def test_edit_task_updates_description(store: TaskStore) -> None:
    task = store.add_task("old")
    store.edit_task(task.id, "new")
    assert store.get_task(task.id).description == "new"


def test_add_and_update_note(store: TaskStore) -> None:
    task = store.add_task("write code")
    store.add_note(task.id, "use type hints")
    assert store.get_task(task.id).notes == "use type hints"
    store.add_note(task.id, "revised")
    assert store.get_task(task.id).notes == "revised"


def test_due_date_validated_and_stored(store: TaskStore) -> None:
    task = store.add_task("deadline")
    store.set_due_date(task.id, "2026-12-31")
    assert store.get_task(task.id).due_date == "2026-12-31"
    with pytest.raises(ValueError):
        store.set_due_date(task.id, "not-a-date")
    with pytest.raises(ValueError):
        store.set_due_date(task.id, "2026-02-30")


def test_tag_add_duplicate_rejected_and_removed(store: TaskStore) -> None:
    task = store.add_task("taggable")
    store.add_tag(task.id, "work")
    store.add_tag(task.id, "urgent")
    assert store.get_task(task.id).tags == ["work", "urgent"]
    with pytest.raises(ValueError):
        store.add_tag(task.id, "work")
    store.remove_tag(task.id, "work")
    assert store.get_task(task.id).tags == ["urgent"]
    with pytest.raises(KeyError):
        store.remove_tag(task.id, "nope")


def test_missing_task_ops_raise_value_error(store: TaskStore) -> None:
    for operation in (
        lambda: store.edit_task(42, "x"),
        lambda: store.delete_task(42),
        lambda: store.mark_complete(42),
        lambda: store.add_tag(42, "t"),
    ):
        with pytest.raises(ValueError):
            operation()


def test_task_round_trip_dict() -> None:
    task = Task(
        id=7,
        description="hi",
        notes="note",
        completed=True,
        high_priority=True,
        due_date="2026-01-01",
        tags=["x", "y"],
        dependencies=[3],
    )
    assert Task.from_dict(task.to_dict()) == task
