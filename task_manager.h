#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h> // For due dates

// Bitwise Flags
#define FLAG_COMPLETED   (1 << 0)
#define FLAG_HIGH_PRI    (1 << 1)

#define MAX_LINE 256
#define FILENAME "tasks.dat"

// --- ANSI Color Codes (Feature 6) ---
#define COLOR_RESET   "\x1B[0m"
#define COLOR_RED     "\x1B[31m"
#define COLOR_GREEN   "\x1B[32m"
#define COLOR_YELLOW  "\x1B[33m"
#define COLOR_CYAN    "\x1B[36m"
#define COLOR_GRAY    "\x1B[90m"

// --- Struct Definition (with new features) ---
typedef struct {
    char* description;
    char* notes;
    char status;
    
    // Feature 1: Due Dates
    time_t due_date; // A timestamp
    
    // Feature 2: Tagging
    char** tags;
    int tag_count;
    int tag_capacity;
    
    // Feature 5: Dependencies
    int* dependencies; // Array of task IDs
    int dep_count;
    int dep_capacity;
    
} Task;

// --- Global Task List Structure ---
typedef struct {
    Task** tasks;
    int count;
    int capacity;
    pthread_mutex_t lock;
    
    // Feature 6: Color Config
    bool color_enabled;
    
} TaskList;

// --- Function Prototypes ---

// task.c functions
void init_task_list(TaskList* list);
void free_task_list(TaskList* list);
void list_tasks(TaskList* list);
Task* get_task_by_id(TaskList* list, int task_id); // Helper

// Task modification functions
void add_task(TaskList* list, const char* description, bool is_high_priority);
void mark_task_complete(TaskList* list, int task_id);
void delete_task(TaskList* list, int task_id);
void edit_task(TaskList* list, int task_id, const char* new_description);
void add_note(TaskList* list, int task_id, const char* note_text);
void view_task(TaskList* list, int task_id);

// Feature functions
void add_due_date(TaskList* list, int task_id, const char* date_str);
void add_tag(TaskList* list, int task_id, const char* tag_name);
void remove_tag(TaskList* list, int task_id, const char* tag_name);
void find_by_tag(TaskList* list, const char* search_tag);
void add_dependency(TaskList* list, int task_id, int dependency_id);
void config_color(TaskList* list, const char* value);
void sort_tasks(TaskList* list, const char* sort_by); // <-- THIS IS THE CORRECT PROTOTYPE
void find_tasks(TaskList* list, const char* search_term);

// Traversal
void traverse_tasks(TaskList* list, void (*action_func)(Task* task, void* user_data), void* user_data);

// File I/O
void* save_tasks_thread(void* arg);
void load_tasks(TaskList* list);

#endif // TASK_MANAGER_H