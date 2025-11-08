# C-Task-Manager: A Multi-Threaded Command-Line To-Do List

This is a comprehensive, high-performance task manager written entirely in C. It demonstrates a wide range of C programming concepts, from basic structs and file I/O to advanced topics like multithreading, function pointers, and complex dynamic memory management.

It is persistent (saves to `tasks.dat`), supports background saving, and is managed through a simple command-line interface.

## 🚀 Features

* **Task Management:** Add, delete, edit, and list tasks.
* **Persistent Storage:** Tasks are automatically saved to `tasks.dat` and loaded on start.
* **Multi-Threaded Saving:** The `save` command uses `pthread` to save in the background, so you can keep working.
* **Task Details:** Add multi-line notes, priority status, due dates, and tags.
* **Dependencies:** Define task dependencies (e.g., Task 1 must be done before Task 2).
* **Sorting:** Sort your list by name, priority, or due date.
* **Searching:** Full-text search on descriptions/notes, or find tasks by a specific tag.
* **Safe & Dynamic:** All data is dynamically allocated using `malloc`/`realloc`, with `pthread_mutex_t` locks to ensure thread safety.
* **Configurable:** Toggle ANSI color output on or off.

## ⚙️ How to Compile

This project uses the POSIX threads library (`pthread`). You must link it during compilation.

```bash
# Compile both .c files and link the pthread library
gcc task_manager.c task.c -o task_manager -pthread