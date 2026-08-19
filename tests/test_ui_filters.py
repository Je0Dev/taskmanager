"""Interaction tests for dashboard filters, edit, and note dialogs."""

from helpers import add_task
from nicegui import ui


async def test_search_filters(page_user) -> None:
    user, store = page_user
    store.add_task("walk the dog")
    store.add_task("feed the cat")
    await user.open("/")
    user.find(kind=ui.input, marker="search").type("dog")
    await user.should_see("walk the dog")
    await user.should_not_see("feed the cat")


async def test_tag_filter(page_user) -> None:
    user, store = page_user
    first = store.add_task("alpha")
    second = store.add_task("beta")
    store.add_tag(first.id, "work")
    store.add_tag(second.id, "home")
    await user.open("/")
    next(iter(user.find(kind=ui.select, marker="tag-filter").elements)).value = "work"
    await user.should_see("alpha")
    await user.should_not_see("beta")


async def test_sort_by_priority(page_user) -> None:
    user, store = page_user
    store.add_task("aaa")
    store.add_task("zzz", high_priority=True)
    await user.open("/")
    next(iter(user.find(kind=ui.select, marker="sort").elements)).value = "pri"
    await user.should_see("zzz")


async def test_edit_task(page_user) -> None:
    user, store = page_user
    add_task(user, store, "old name")
    user.find(marker="edit-1").click()
    user.find(kind=ui.input, marker="edit-field").type(" now")
    user.find(marker="dialog-save").click()
    await user.should_see("old name now")


async def test_note_badge(page_user) -> None:
    user, store = page_user
    add_task(user, store, "jot down")
    user.find(marker="notes-1").click()
    next(iter(user.find(kind=ui.textarea, marker="note-field").elements)).value = "remember this"
    user.find(marker="dialog-save").click()
    assert store.get_task(1).notes == "remember this"
    await user.should_see("N")
