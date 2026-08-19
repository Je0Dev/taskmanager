"""Entry point for the NiceGUI task manager app.

Run with: python app.py
Opens the dashboard in your default browser at http://localhost:8080.

Optional native desktop window: install pywebview (pip install pywebview)
and run with `TASK_MANAGER_NATIVE=1 python app.py`.
"""

import os

from nicegui import ui
from task_manager.storage import DEFAULT_FILE, load_store
from task_manager.ui import build_page


def main() -> None:
    store = load_store(DEFAULT_FILE)
    build_page(store, save_path=DEFAULT_FILE)
    native = os.environ.get("TASK_MANAGER_NATIVE") == "1"
    ui.run(title="Task Manager", native=native, reload=False)


main()
