"""JSON persistence for the task store."""

from __future__ import annotations

import json
import os
import tempfile
from pathlib import Path

from .models import TaskStore

DEFAULT_FILE = "tasks.json"


def save_store(store: TaskStore, path: str | Path = DEFAULT_FILE) -> None:
    """Write the store to a JSON file using an atomic temp-file + replace."""
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    data = {
        "config": {"color": store.color_enabled},
        "tasks": [task.to_dict() for task in store.tasks],
    }
    fd, tmp_name = tempfile.mkstemp(dir=path.parent, prefix=path.name + ".", suffix=".tmp")
    try:
        with os.fdopen(fd, "w", encoding="utf-8") as handle:
            json.dump(data, handle, indent=2, ensure_ascii=False)
            handle.write("\n")
        os.replace(tmp_name, path)
    except BaseException:
        try:
            os.unlink(tmp_name)
        except OSError:
            pass
        raise


def load_store(path: str | Path = DEFAULT_FILE) -> TaskStore:
    """Load a store from a JSON file, returning an empty store if absent."""
    path = Path(path)
    if not path.is_file():
        return TaskStore()
    with path.open("r", encoding="utf-8") as handle:
        data = json.load(handle)
    return TaskStore.restore(
        [dict(t) for t in data.get("tasks", [])],
        color_enabled=bool(data.get("config", {}).get("color", True)),
    )
