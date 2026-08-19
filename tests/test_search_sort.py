"""Tests for search, tag lookup, sorting, and color settings."""

from task_manager.models import TaskStore


def test_find_tasks_searches_description_and_notes(store: TaskStore) -> None:
    store.add_task("buy milk")
    task = store.add_task("prepare slides")
    store.add_note(task.id, "mention the budget")
    results = store.find_tasks("budget")
    assert [t.id for t in results] == [task.id]
    assert [t.id for t in store.find_tasks("MILK")] == [1]
    assert store.find_tasks("nothing-here") == []


def test_find_by_tag(store: TaskStore) -> None:
    a = store.add_task("a")
    b = store.add_task("b")
    store.add_tag(a.id, "work")
    store.add_tag(b.id, "work")
    store.add_tag(b.id, "home")
    assert {t.id for t in store.find_by_tag("work")} == {a.id, b.id}
    assert {t.id for t in store.find_by_tag("home")} == {b.id}


def test_sort_by_name(store: TaskStore) -> None:
    store.add_task("zeta")
    store.add_task("Alpha")
    store.add_task("beta")
    store.sort_tasks("name")
    assert [t.description for t in store.tasks] == ["Alpha", "beta", "zeta"]


def test_sort_by_priority_then_name(store: TaskStore) -> None:
    store.add_task("alpha")
    store.add_task("zeta", high_priority=True)
    store.add_task("beta", high_priority=True)
    store.sort_tasks("pri")
    assert [t.description for t in store.tasks] == ["beta", "zeta", "alpha"]


def test_sort_by_due_date_no_date_last(store: TaskStore) -> None:
    later = store.add_task("later")
    store.set_due_date(later.id, "2026-03-01")
    store.add_task("no date")
    earlier = store.add_task("earlier")
    store.set_due_date(earlier.id, "2026-01-01")
    store.sort_tasks("date")
    assert [t.description for t in store.tasks] == ["earlier", "later", "no date"]


def test_sort_invalid_falls_back_to_name(store: TaskStore) -> None:
    store.add_task("b")
    store.add_task("a")
    store.sort_tasks("bogus")
    assert [t.description for t in store.tasks] == ["a", "b"]


def test_color_toggle(store: TaskStore) -> None:
    assert store.color_enabled is True
    store.set_color(False)
    assert store.color_enabled is False
