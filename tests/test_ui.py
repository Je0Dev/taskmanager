"""Interaction tests for the NiceGUI dashboard core flows."""

from helpers import add_task
from nicegui import ui


async def test_empty_state(page_user) -> None:
    user, _ = page_user
    await user.should_see("Task Manager")
    await user.should_see("No tasks yet")


async def test_add_task_appears(page_user) -> None:
    user, store = page_user
    add_task(user, store, "buy groceries")
    await user.should_see("buy groceries")
    assert store.tasks[0].description == "buy groceries"
    assert store.tasks[0].high_priority is False


async def test_add_high_priority_task(page_user) -> None:
    user, store = page_user
    add_task(user, store, "ship release", high=True)
    assert store.get_task(1).high_priority is True
    await user.should_see("!")


async def test_mark_done_toggles(page_user) -> None:
    user, store = page_user
    add_task(user, store, "fold laundry")
    assert store.get_task(1).completed is False
    user.find(marker="done-1").click()
    assert store.get_task(1).completed is True


async def test_blocked_dependency_shows_confirm_dialog(page_user) -> None:
    user, store = page_user
    store.add_task("first")
    store.add_task("second")
    store.add_dependency(2, 1)
    await user.open("/")
    user.find(marker="done-2").click()
    await user.should_see("Pending dependencies:")
    await user.should_see("• #1 first")
    user.find(marker="force-cancel").click()
    await user.should_not_see("Pending dependencies:")
    assert store.get_task(2).completed is False


async def test_force_complete_overrides_dependency(page_user) -> None:
    user, store = page_user
    store.add_task("first")
    store.add_task("second")
    store.add_dependency(2, 1)
    await user.open("/")
    user.find(marker="done-2").click()
    user.find(marker="force-complete").click()
    assert store.get_task(2).completed is True


async def test_delete_task(page_user) -> None:
    user, store = page_user
    add_task(user, store, "remove me")
    user.find(marker="delete-1").click()
    user.find(marker="confirm-delete").click()
    await user.should_not_see(kind=ui.button, marker="delete-1")
    assert store.tasks == []


async def test_color_switch_toggles_theme(page_user) -> None:
    user, store = page_user
    assert store.color_enabled is True
    user.find(kind=ui.switch, marker="colors").click()
    assert store.color_enabled is False
    user.find(kind=ui.switch, marker="colors").click()
    assert store.color_enabled is True


async def test_persistence_writes_file(page_user, tmp_path) -> None:
    user, store = page_user
    add_task(user, store, "persist me")
    user.find(kind=ui.button, marker="save-btn").click()
    assert (tmp_path / "tasks.json").is_file()
    data = (tmp_path / "tasks.json").read_text(encoding="utf-8")
    assert "persist me" in data
