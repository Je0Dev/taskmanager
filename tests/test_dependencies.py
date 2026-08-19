"""Tests for dependency blocking, force-complete, and stable IDs."""

import pytest
from task_manager.models import DependencyError, TaskStore


def test_add_dependency_rules(store: TaskStore) -> None:
    task = store.add_task("depends")
    prereq = store.add_task("prereq")
    store.add_dependency(task.id, prereq.id)
    assert store.get_task(task.id).dependencies == [prereq.id]
    with pytest.raises(ValueError):
        store.add_dependency(task.id, task.id)
    with pytest.raises(ValueError):
        store.add_dependency(task.id, prereq.id)
    with pytest.raises(ValueError):
        store.add_dependency(task.id, 999)


def test_mark_complete_blocks_on_pending_dependency(store: TaskStore) -> None:
    task = store.add_task("dependent")
    prereq = store.add_task("must come first")
    store.add_dependency(task.id, prereq.id)
    with pytest.raises(DependencyError) as exc:
        store.mark_complete(task.id)
    assert exc.value.pending == [(prereq.id, "must come first")]
    assert not store.get_task(task.id).completed
    store.mark_complete(prereq.id)
    store.mark_complete(task.id)
    assert store.get_task(task.id).completed


def test_mark_complete_force_overrides_dependencies(store: TaskStore) -> None:
    task = store.add_task("dependent")
    store.add_task("prereq")
    store.add_dependency(task.id, 2)
    store.mark_complete(task.id, force=True)
    assert store.get_task(task.id).completed


def test_delete_removes_stale_dependencies(store: TaskStore) -> None:
    task = store.add_task("keep me")
    doomed = store.add_task("delete me")
    store.add_dependency(task.id, doomed.id)
    store.delete_task(doomed.id)
    assert store.get_task(doomed.id) is None
    assert store.get_task(task.id).dependencies == []
    assert store.get_task(task.id).id == 1


def test_delete_stable_ids_not_renumbered(store: TaskStore) -> None:
    store.add_task("a")
    b = store.add_task("b")
    store.add_task("c")
    store.delete_task(2)
    remaining = [t.id for t in store.tasks]
    assert remaining == [1, 3]
    assert store.get_task(b.id) is None
