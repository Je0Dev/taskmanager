# C - A Command-Line To-Do List

## 🚀 Key Features

* **Task Management:** Add, delete, edit, and list all sorts of different tasks.
* **Multithreaded Persistent Storage:** Everything can be saved to `tasks.dat` and it is then loaded on start. The `save` command uses `pthread` to save in the background, so you can keep working.
* **Task Details:** Add notes, priority status, due dates, and tags to each task.
* **Dependencies:** Define task dependencies (e.g., Task 1 must be done before Task 2, which has to be done before Task 4, etc.).
* **Sorting:** Sort your list by name, priority, or due date.
* **Searching:** Full-text search on descriptions/notes, or find tasks by a specific tag.
* **Safe & Dynamic:** All data is dynamically allocated using `malloc`/`realloc`, with `pthread_mutex_t` locks to ensure thread safety.

## ⚙️ Compile & Run

```bash
gcc task_manager.c task.c -o task_manager -pthread # POSIX threads library
./task_manager #Linux,MacOs
./task_manager.exe #Windows
```

## 📋 Command Reference

### Basic Task Management
- `add <description>` - Add a regular priority task
- `p-add <description>` - Add a high priority task
- `list` - Display all tasks with formatted output
- `view <id>` - Show detailed information about a specific task
- `done <id>` - Mark a task as completed (with dependency checking)
- `edit <id> <new description>` - Modify a task's description
- `note <id> <note text>` - Add or update notes for a task
- `delete <id>` - Remove a task and update dependencies

### Advanced Features
- `due <id> <YYYY-MM-DD>` - Set a due date for a task
- `tag <id> <tag>` - Add a tag to a task
- `untag <id> <tag>` - Remove a tag from a task
- `findtag <tag>` - Find all tasks with a specific tag
- `depend <id1> <id2>` - Make task id1 depend on task id2
- `sort <name|pri|date>` - Sort tasks by different criteria
- `find <term>` - Search for tasks by text in description or notes

### System Management
- `save` - Save all tasks to file in the background
- `config color <on|off>` - Enable or disable color output
- `help` - Display the complete command menu
- `quit` - Exit the program

## 🏗️ System Architecture

### Thread Safety
The task manager uses `pthread_mutex_t` locks to ensure thread-safe operations across all functions. The main data structure (`TaskList`) contains a mutex that protects access to the task array and all task data.

### Memory Management
- All strings are dynamically allocated using `malloc`/`realloc`
- Memory is properly freed when tasks are deleted or the program exits
- Helper functions like `safe_strdup()` handle both allocation and reallocation safely

### Data Persistence
- Tasks are saved to `tasks.dat` in a structured text format
- The save operation runs in a background thread to avoid blocking the main program
- Configuration settings (like color preferences) are also persisted

### File Format
The save file uses a pipe-delimited (`|`) format with comma-separated lists for arrays:
```
status|due_date|dep_count|dep1,dep2|tag_count|tag1,tag2|description|notes
```

## 🎨 Color Output

The system supports optional color output for better visual distinction:
- **Red** for high priority tasks
- **Green** for completed tasks  
- **Yellow** for due dates
- **Cyan** for tags
- **Gray** for completed task text

Color output can be toggled with `config color on/off`.

## 🔒 Dependency Management

The system implements a dependency graph where tasks can depend on other tasks being completed first. When marking a task as complete, the system checks all dependencies and warns if any are incomplete. Users can override this check if needed.

## 📊 Sorting Capabilities

Tasks can be sorted by three different criteria:
1. **Name** - Alphabetical order by description
2. **Priority** - High priority tasks first, then by name
3. **Due Date** - Earliest dates first, tasks without dates go to the end

## 🔍 Search Functionality

Two search methods are available:
1. **Full-text search** (`find`) - Searches both descriptions and notes
2. **Tag search** (`findtag`) - Finds tasks by specific tags

## 📝 Task Structure

Each task contains:
- **Description** - The main task text
- **Notes** - Optional additional information
- **Status flags** - Completion status and priority level
- **Due date** - Optional deadline (Unix timestamp)
- **Tags** - Array of string tags for categorization
- **Dependencies** - Array of task IDs that must be completed first

## 🔄 Threading Model

The save operation uses a separate thread to avoid blocking the main command loop. This allows users to continue working while their data is being saved in the background. The mutex ensures that the save thread has exclusive access to the data during the save operation.