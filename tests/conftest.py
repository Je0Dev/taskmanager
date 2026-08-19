"""Shared pytest fixtures for the task manager."""

from collections.abc import AsyncGenerator

import pytest
from nicegui.testing.user import User
from nicegui.testing.user_simulation import user_simulation
from task_manager.models import TaskStore
from task_manager.ui import build_page


@pytest.fixture
def store() -> TaskStore:
    """A fresh task store for model-level tests."""
    return TaskStore()


@pytest.fixture
async def page_user(tmp_path) -> AsyncGenerator[tuple[User, TaskStore], None]:
    """Spin up the app in a simulated user session against an isolated store."""
    store = TaskStore()

    def root() -> None:
        build_page(store, save_path=str(tmp_path / "tasks.json"))

    async with user_simulation(root=root) as user:
        await user.open("/")
        yield user, store
