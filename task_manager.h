#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h> // For due dates

// ========================================
// CONSTANTS AND MACROS
// ========================================

// Bitwise flags for task status and priority
#define FLAG_COMPLETED   (1 << 0)  // Task completion status flag
#define FLAG_HIGH_PRI    (1 << 1)  // High priority task flag

// System configuration constants
#define MAX_LINE 256               // Maximum length for input lines
#define FILENAME "tasks.dat"       // Default filename for task persistence

// ========================================
// ANSI COLOR CODES (Feature 6)
// ========================================

// Color codes for terminal output formatting
#define COLOR_RESET   "\x1B[0m"    // Reset all color formatting
#define COLOR_RED     "\x1B[31m"   // Red text (used for high priority)
#define COLOR_GREEN   "\x1B[32m"   // Green text (used for completed tasks)
#define COLOR_YELLOW  "\x1B[33m"   // Yellow text (used for due dates)
#define COLOR_CYAN    "\x1B[36m"   // Cyan text (used for tags)
#define COLOR_GRAY    "\x1B[90m"   // Gray text (used for completed task text)

// ========================================
// DATA STRUCTURES
// ========================================

/**
 * Task structure representing a single to-do item.
 * 
 * This structure contains all information related to a task including
 * description, notes, status flags, due dates, tags, and dependencies.
 */
typedef struct {
    char* description;            // Task description text
    char* notes;                  // Optional notes for the task
    char status;                  // Bitwise flags for status and priority
    
    // Feature 1: Due Dates
    time_t due_date;              // Due date as Unix timestamp
    
    // Feature 2: Tagging System
    char** tags;                  // Array of tag strings
    int tag_count;                // Number of current tags
    int tag_capacity;             // Allocated capacity for tags array
    
    // Feature 5: Dependencies
    int* dependencies;            // Array of dependent task IDs
    int dep_count;                // Number of current dependencies
    int dep_capacity;             // Allocated capacity for dependencies array
    
} Task;

/**
 * TaskList structure representing the complete task management system.
 * 
 * This structure manages the collection of all tasks with thread-safe
 * operations and configuration settings.
 */
typedef struct {
    Task** tasks;                 // Array of task pointers
    int count;                    // Current number of tasks
    int capacity;                 // Allocated capacity for tasks array
    pthread_mutex_t lock;         // Mutex for thread-safe operations
    
    // Feature 6: Color Configuration
    bool color_enabled;           // Flag to enable/disable color output
    
} TaskList;

// ========================================
// FUNCTION PROTOTYPES
// ========================================

// --- System Management Functions ---

/**
 * Initialize the task list with default settings.
 * Allocates initial memory and sets up the mutex lock.
 * 
 * @param list Pointer to the TaskList to initialize
 */
void init_task_list(TaskList* list);

/**
 * Free all memory associated with the task list.
 * Destroys mutex and deallocates all tasks and their data.
 * 
 * @param list Pointer to the TaskList to free
 */
void free_task_list(TaskList* list);

/**
 * Display all tasks in the list with formatted output.
 * Shows task ID, status, priority, notes indicator, due date, and tags.
 * 
 * @param list Pointer to the TaskList to display
 */
void list_tasks(TaskList* list);

/**
 * Helper function to get a task by its ID (1-based index).
 * Must be called within a mutex lock for thread safety.
 * 
 * @param list Pointer to the TaskList
 * @param task_id The 1-based ID of the task to retrieve
 * @return Task* Pointer to the task, or NULL if not found
 */
Task* get_task_by_id(TaskList* list, int task_id);

// --- Task Modification Functions ---

/**
 * Add a new task to the list.
 * 
 * @param list Pointer to the TaskList
 * @param description The task description text
 * @param is_high_priority Flag indicating if task is high priority
 */
void add_task(TaskList* list, const char* description, bool is_high_priority);

/**
 * Mark a task as completed with dependency checking.
 * 
 * @param list Pointer to the TaskList
 * @param task_id The ID of the task to mark complete
 */
void mark_task_complete(TaskList* list, int task_id);

/**
 * Delete a task from the list and update dependencies.
 * 
 * @param list Pointer to the TaskList
 * @param task_id The ID of the task to delete
 */
void delete_task(TaskList* list, int task_id);

/**
 * Edit the description of an existing task.
 * 
 * @param list Pointer to the TaskList
 * @param task_id The ID of the task to edit
 * @param new_description The new description text
 */
void edit_task(TaskList* list, int task_id, const char* new_description);

/**
 * Add or update notes for a task.
 * 
 * @param list Pointer to the TaskList
 * @param task_id The ID of the task
 * @param note_text The note text to add
 */
void add_note(TaskList* list, int task_id, const char* note_text);

/**
 * Display detailed information about a specific task.
 * Shows description, status, priority, due date, notes, tags, and dependencies.
 * 
 * @param list Pointer to the TaskList
 * @param task_id The ID of the task to view
 */
void view_task(TaskList* list, int task_id);

// --- Feature Implementation Functions ---

/**
 * Set a due date for a task.
 * 
 * @param list Pointer to the TaskList
 * @param task_id The ID of the task
 * @param date_str Date string in YYYY-MM-DD format
 */
void add_due_date(TaskList* list, int task_id, const char* date_str);

/**
 * Add a tag to a task.
 * 
 * @param list Pointer to the TaskList
 * @param task_id The ID of the task
 * @param tag_name The tag to add
 */
void add_tag(TaskList* list, int task_id, const char* tag_name);

/**
 * Remove a tag from a task.
 * 
 * @param list Pointer to the TaskList
 * @param task_id The ID of the task
 * @param tag_name The tag to remove
 */
void remove_tag(TaskList* list, int task_id, const char* tag_name);

/**
 * Find and display all tasks that have a specific tag.
 * 
 * @param list Pointer to the TaskList
 * @param search_tag The tag to search for
 */
void find_by_tag(TaskList* list, const char* search_tag);

/**
 * Add a dependency relationship between two tasks.
 * 
 * @param list Pointer to the TaskList
 * @param task_id The ID of the task that depends on another
 * @param dependency_id The ID of the task that must be completed first
 */
void add_dependency(TaskList* list, int task_id, int dependency_id);

/**
 * Configure color output settings.
 * 
 * @param list Pointer to the TaskList
 * @param value "on" to enable colors, "off" to disable
 */
void config_color(TaskList* list, const char* value);

/**
 * Sort tasks by different criteria.
 * 
 * @param list Pointer to the TaskList
 * @param sort_by "name", "pri", or "date" for sorting method
 */
void sort_tasks(TaskList* list, const char* sort_by);

/**
 * Find tasks by searching description and notes.
 * 
 * @param list Pointer to the TaskList
 * @param search_term The text to search for
 */
void find_tasks(TaskList* list, const char* search_term);

// --- Utility Functions ---

/**
 * Traverse all tasks and apply a function to each.
 * 
 * @param list Pointer to the TaskList
 * @param action_func Function pointer to apply to each task
 * @param user_data Optional data to pass to the action function
 */
void traverse_tasks(TaskList* list, void (*action_func)(Task* task, void* user_data), void* user_data);

// --- File I/O Functions ---

/**
 * Save all tasks to a file in the background using a separate thread.
 * 
 * @param arg Pointer to the TaskList to save
 * @return void* Always returns NULL
 */
void* save_tasks_thread(void* arg);

/**
 * Load tasks from a file and restore the system state.
 * 
 * @param list Pointer to the TaskList to populate
 */
void load_tasks(TaskList* list);

#endif // TASK_MANAGER_H
