#define _XOPEN_SOURCE // For strptime
#include "task_manager.h"
#include <ctype.h> // For tolower

// --- Helper Functions ---

// Helper to get a task by ID (1-based index)
// NOTE: This must be called *inside* a mutex lock!
Task* get_task_by_id(TaskList* list, int task_id) {
    if (task_id < 1 || task_id > list->count) {
        return NULL;
    }
    return list->tasks[task_id - 1];
}

// Helper to safely allocate or reallocate a string
static char* safe_strdup(const char* s, char* existing_ptr) {
    if (s == NULL) return NULL;
    size_t len = strlen(s) + 1;
    char* new_ptr;
    
    if (existing_ptr == NULL) {
        new_ptr = (char*)malloc(len);
    } else {
        new_ptr = (char*)realloc(existing_ptr, len);
    }
    
    if (new_ptr == NULL) {
        perror("Failed to allocate string");
        return existing_ptr;
    }
    memcpy(new_ptr, s, len);
    return new_ptr;
}

// Helper for color output
static const char* get_color(TaskList* list, const char* color) {
    return list->color_enabled ? color : "";
}

// --- List Initialization / Deallocation ---

void init_task_list(TaskList* list) {
    list->tasks = (Task**)malloc(10 * sizeof(Task*));
    if (list->tasks == NULL) {
        perror("Failed to allocate task list");
        exit(1);
    }
    list->count = 0;
    list->capacity = 10;
    list->color_enabled = true; // Color on by default
    pthread_mutex_init(&list->lock, NULL);
}

void free_task_list(TaskList* list) {
    pthread_mutex_lock(&list->lock);
    
    for (int i = 0; i < list->count; i++) {
        Task* t = list->tasks[i];
        free(t->description);
        if (t->notes) free(t->notes);
        
        // Free tags
        for (int j = 0; j < t->tag_count; j++) {
            free(t->tags[j]);
        }
        if (t->tags) free(t->tags);
        
        // Free dependencies
        if (t->dependencies) free(t->dependencies);
        
        free(t);
    }
    
    free(list->tasks);
    pthread_mutex_destroy(&list->lock);
    
    list->tasks = NULL;
    list->count = 0;
    list->capacity = 0;
}

// --- Task Creation / Modification ---

void add_task(TaskList* list, const char* description, bool is_high_priority) {
    pthread_mutex_lock(&list->lock);

    if (list->count >= list->capacity) {
        list->capacity *= 2;
        list->tasks = (Task**)realloc(list->tasks, list->capacity * sizeof(Task*));
        if (list->tasks == NULL) {
            perror("Failed to reallocate task list");
            pthread_mutex_unlock(&list->lock);
            exit(1);
        }
    }

    Task* new_task = (Task*)calloc(1, sizeof(Task)); // calloc zeroes memory
    if (new_task == NULL) {
        perror("Failed to allocate new task");
        pthread_mutex_unlock(&list->lock);
        return;
    }
    
    new_task->description = safe_strdup(description, NULL);
    // all other pointers (notes, tags, dependencies) are NULL
    // all counts (tag_count, dep_count) are 0
    // due_date is 0
    
    if (is_high_priority) {
        new_task->status |= FLAG_HIGH_PRI;
    }
    
    new_task->tag_capacity = 2; // Start with capacity for 2 tags
    new_task->tags = (char**)malloc(new_task->tag_capacity * sizeof(char*));
    
    new_task->dep_capacity = 2; // Start with capacity for 2 dependencies
    new_task->dependencies = (int*)malloc(new_task->dep_capacity * sizeof(int));

    list->tasks[list->count] = new_task;
    list->count++;

    printf("Added task %d: %s\n", list->count, description);
    
    pthread_mutex_unlock(&list->lock);
}

// [Feature 5] Helper to check dependencies
static bool check_dependencies(TaskList* list, Task* task) {
    // Assumes list is already locked
    for (int i = 0; i < task->dep_count; i++) {
        int dep_id = task->dependencies[i];
        Task* dep_task = get_task_by_id(list, dep_id);
        
        if (dep_task != NULL && !(dep_task->status & FLAG_COMPLETED)) {
            // Found an incomplete dependency
            printf("Warning: Task %d (%s) is not complete.\n", dep_id, dep_task->description);
            return false;
        }
    }
    return true; // All dependencies met
}

void mark_task_complete(TaskList* list, int task_id) {
    pthread_mutex_lock(&list->lock);
    Task* task = get_task_by_id(list, task_id);
    if (task == NULL) {
        printf("Error: Invalid task ID.\n");
        pthread_mutex_unlock(&list->lock);
        return;
    }

    // [Feature 5] Check dependencies first
    if (!check_dependencies(list, task)) {
        printf("Task has incomplete dependencies. Mark complete anyway? (y/n): ");
        pthread_mutex_unlock(&list->lock); // Unlock to read input
        
        char confirm[10];
        fgets(confirm, 10, stdin);
        if (tolower(confirm[0]) != 'y') {
            printf("Task not marked as complete.\n");
            return;
        }
        
        pthread_mutex_lock(&list->lock); // Re-lock
        task = get_task_by_id(list, task_id); // Re-get task
        if (task == NULL) { // Check if deleted in the meantime
            printf("Task no longer exists.\n");
            pthread_mutex_unlock(&list->lock);
            return;
        }
    }
    
    task->status |= FLAG_COMPLETED;
    printf("Completed task: %s\n", task->description);
    pthread_mutex_unlock(&list->lock);
}

void delete_task(TaskList* list, int task_id) {
    pthread_mutex_lock(&list->lock);
    
    Task* task_to_delete = get_task_by_id(list, task_id);
    if (task_to_delete == NULL) {
        printf("Error: Invalid task ID.\n");
        pthread_mutex_unlock(&list->lock);
        return;
    }
    
    int index = task_id - 1;
    printf("Deleting task: %s\n", task_to_delete->description);

    // 1. Free all internal dynamic memory
    free(task_to_delete->description);
    if (task_to_delete->notes) free(task_to_delete->notes);
    for (int j = 0; j < task_to_delete->tag_count; j++) {
        free(task_to_delete->tags[j]);
    }
    free(task_to_delete->tags);
    free(task_to_delete->dependencies);
    
    // 2. Free the struct itself
    free(task_to_delete);
    
    // 3. Shift pointers in the main array
    int num_to_move = list->count - index - 1;
    if (num_to_move > 0) {
        memmove(
            &list->tasks[index],
            &list->tasks[index + 1],
            num_to_move * sizeof(Task*)
        );
    }
    
    // 4. Decrement count
    list->count--;
    
    // 5. [Crucial!] Update dependency IDs
    // Any task that depended on a task *after* this one
    // now needs its ID decremented.
    for(int i = 0; i < list->count; i++) {
        Task* t = list->tasks[i];
        for (int j = 0; j < t->dep_count; j++) {
            if (t->dependencies[j] == task_id) {
                // This dependency is now invalid. For simplicity, we'll remove it.
                // (A more complex system might block deletion or re-assign)
                printf("Note: Removing dependency on deleted task %d from task %d.\n", task_id, i+1);
                int num_to_move_deps = t->dep_count - j - 1;
                if (num_to_move_deps > 0) {
                    memmove(&t->dependencies[j], &t->dependencies[j+1], num_to_move_deps * sizeof(int));
                }
                t->dep_count--;
                j--; // Re-check this index
            } else if (t->dependencies[j] > task_id) {
                // Shift down
                t->dependencies[j]--;
            }
        }
    }
    
    pthread_mutex_unlock(&list->lock);
}

void edit_task(TaskList* list, int task_id, const char* new_description) {
    pthread_mutex_lock(&list->lock);
    Task* task = get_task_by_id(list, task_id);
    if (task == NULL) {
        printf("Error: Invalid task ID.\n");
    } else {
        task->description = safe_strdup(new_description, task->description);
        printf("Edited task %d.\n", task_id);
    }
    pthread_mutex_unlock(&list->lock);
}

void add_note(TaskList* list, int task_id, const char* note_text) {
    pthread_mutex_lock(&list->lock);
    Task* task = get_task_by_id(list, task_id);
    if (task == NULL) {
        printf("Error: Invalid task ID.\n");
    } else {
        task->notes = safe_strdup(note_text, task->notes);
        printf("Added/updated note for task %d.\n", task_id);
    }
    pthread_mutex_unlock(&list->lock);
}

// --- Traversal & Viewing ---

void traverse_tasks(TaskList* list, void (*action_func)(Task* task, void* user_data), void* user_data) {
    pthread_mutex_lock(&list->lock);
    for (int i = 0; i < list->count; i++) {
        action_func(list->tasks[i], user_data);
    }
    pthread_mutex_unlock(&list->lock);
}

static void print_task_simple(Task* task, void* user_data) {
    TaskList* list = (TaskList*)user_data;
    static int task_num = 1;
    if (task == NULL) {
        task_num = 1;
        return;
    }
    
    const char* C_RST = get_color(list, COLOR_RESET);
    const char* C_PRI = get_color(list, COLOR_RED);
    const char* C_DON = get_color(list, COLOR_GREEN);
    const char* C_TXT = (task->status & FLAG_COMPLETED) ? get_color(list, COLOR_GRAY) : C_RST;
    const char* C_DATE = get_color(list, COLOR_YELLOW);
    const char* C_TAG = get_color(list, COLOR_CYAN);

    char status_char = (task->status & FLAG_COMPLETED) ? 'X' : ' ';
    char pri_char = (task->status & FLAG_HIGH_PRI) ? '!' : ' ';
    char note_char = (task->notes != NULL) ? 'N' : ' ';
    
    printf("%s%2d. [%c] %s%c%s %c ", C_TXT, task_num++, status_char, 
           (pri_char == '!') ? C_PRI : C_TXT, pri_char, C_TXT, note_char);
           
    // Print Due Date
    if (task->due_date != 0) {
        char date_buf[11];
        strftime(date_buf, 11, "%Y-%m-%d", localtime(&task->due_date));
        printf("%s(%s)%s ", C_DATE, date_buf, C_TXT);
    }
    
    printf("%s", task->description);
    
    // Print Tags
    if (task->tag_count > 0) {
        printf(" %s[", C_TAG);
        for (int i = 0; i < task->tag_count; i++) {
            printf("%s%s", task->tags[i], (i == task->tag_count - 1) ? "" : ", ");
        }
        printf("]%s", C_TXT);
    }
    printf("%s\n", C_RST);
}

void list_tasks(TaskList* list) {
    printf("\n--- Your Tasks ---\n");
    if (list->count == 0) {
        printf("No tasks found.\n");
    } else {
        print_task_simple(NULL, list); // Reset counter
        traverse_tasks(list, print_task_simple, list);
    }
    printf("------------------\n");
}

void view_task(TaskList* list, int task_id) {
    pthread_mutex_lock(&list->lock);
    Task* task = get_task_by_id(list, task_id);
    if (task == NULL) {
        printf("Error: Invalid task ID.\n");
        pthread_mutex_unlock(&list->lock);
        return;
    }
    
    const char* C_RST = get_color(list, COLOR_RESET);
    const char* C_PRI = get_color(list, COLOR_RED);
    const char* C_DON = get_color(list, COLOR_GREEN);
    const char* C_YLW = get_color(list, COLOR_YELLOW);
    const char* C_CYN = get_color(list, COLOR_CYAN);

    printf("\n--- Task %d Details ---%s\n", task_id, C_RST);
    
    printf("Description: %s\n", task->description);
    
    // Status
    char status_char = (task->status & FLAG_COMPLETED) ? 'X' : ' ';
    const char* status_color = (status_char == 'X') ? C_DON : C_YLW;
    printf("Status:      %s[%c] %s%s\n", status_color, status_char, (status_char == 'X' ? "Complete" : "Pending"), C_RST);
    
    // Priority
    char pri_char = (task->status & FLAG_HIGH_PRI) ? '!' : ' ';
    const char* pri_color = (pri_char == '!') ? C_PRI : C_RST;
    printf("Priority:    %s[%c] %s%s\n", pri_color, pri_char, (pri_char == '!' ? "High" : "Normal"), C_RST);
    
    // Due Date
    printf("Due Date:    %s", C_YLW);
    if (task->due_date != 0) {
        char date_buf[20];
        strftime(date_buf, 20, "%Y-%m-%d", localtime(&task->due_date));
        printf("%s\n", date_buf);
    } else {
        printf("(none)\n");
    }
    printf("%s", C_RST);
    
    // Notes
    printf("Notes:       %s\n", task->notes ? task->notes : "(none)");
    
    // Tags
    printf("Tags:        %s", C_CYN);
    if (task->tag_count == 0) {
        printf("(none)\n");
    } else {
        for (int i = 0; i < task->tag_count; i++) {
            printf("%s%s", task->tags[i], (i == task->tag_count - 1) ? "" : ", ");
        }
        printf("\n");
    }
    printf("%s", C_RST);
    
    // Dependencies
    printf("Dependencies:%s", C_PRI);
    if (task->dep_count == 0) {
        printf(" (none)\n");
    } else {
        printf("\n");
        for (int i = 0; i < task->dep_count; i++) {
            Task* dep_task = get_task_by_id(list, task->dependencies[i]);
            if (dep_task) {
                printf("  - (ID %d) %s %s\n", task->dependencies[i], dep_task->description,
                       (dep_task->status & FLAG_COMPLETED) ? "(DONE)" : "(PENDING)");
            }
        }
    }
    
    printf("%s------------------------\n", C_RST);

    pthread_mutex_unlock(&list->lock);
}

// --- Feature Implementation ---

// [Feature 1] Add Due Date
void add_due_date(TaskList* list, int task_id, const char* date_str) {
    pthread_mutex_lock(&list->lock);
    Task* task = get_task_by_id(list, task_id);
    if (task == NULL) {
        printf("Error: Invalid task ID.\n");
        pthread_mutex_unlock(&list->lock);
        return;
    }
    
    struct tm tm = {0};
    // Parse the date string
    if (strptime(date_str, "%Y-%m-%d", &tm) == NULL) {
        printf("Error: Invalid date format. Use YYYY-MM-DD.\n");
    } else {
        task->due_date = mktime(&tm);
        printf("Set due date for task %d.\n", task_id);
    }
    
    pthread_mutex_unlock(&list->lock);
}

// [Feature 2] Add Tag
void add_tag(TaskList* list, int task_id, const char* tag_name) {
    pthread_mutex_lock(&list->lock);
    Task* task = get_task_by_id(list, task_id);
    if (task == NULL) {
        printf("Error: Invalid task ID.\n");
        pthread_mutex_unlock(&list->lock);
        return;
    }
    
    // Check if tag already exists
    for (int i = 0; i < task->tag_count; i++) {
        if (strcmp(task->tags[i], tag_name) == 0) {
            printf("Task already has tag '%s'.\n", tag_name);
            pthread_mutex_unlock(&list->lock);
            return;
        }
    }
    
    // Resize tag array if needed
    if (task->tag_count >= task->tag_capacity) {
        task->tag_capacity *= 2;
        task->tags = (char**)realloc(task->tags, task->tag_capacity * sizeof(char*));
        if (task->tags == NULL) {
            perror("Failed to realloc tags");
            pthread_mutex_unlock(&list->lock);
            return;
        }
    }
    
    // Add the new tag
    task->tags[task->tag_count] = safe_strdup(tag_name, NULL);
    task->tag_count++;
    
    printf("Added tag '%s' to task %d.\n", tag_name, task_id);
    pthread_mutex_unlock(&list->lock);
}

// [Feature 2] Remove Tag
void remove_tag(TaskList* list, int task_id, const char* tag_name) {
    pthread_mutex_lock(&list->lock);
    Task* task = get_task_by_id(list, task_id);
    if (task == NULL) {
        printf("Error: Invalid task ID.\n");
        pthread_mutex_unlock(&list->lock);
        return;
    }
    
    int found_index = -1;
    for (int i = 0; i < task->tag_count; i++) {
        if (strcmp(task->tags[i], tag_name) == 0) {
            found_index = i;
            break;
        }
    }
    
    if (found_index == -1) {
        printf("Tag '%s' not found on task %d.\n", tag_name, task_id);
    } else {
        // Free the string
        free(task->tags[found_index]);
        
        // Shift pointers
        int num_to_move = task->tag_count - found_index - 1;
        if (num_to_move > 0) {
            memmove(
                &task->tags[found_index],
                &task->tags[found_index + 1],
                num_to_move * sizeof(char*)
            );
        }
        task->tag_count--;
        printf("Removed tag '%s' from task %d.\n", tag_name, task_id);
    }
    pthread_mutex_unlock(&list->lock);
}

// [Feature 3] Find by Tag
static void find_tag_action(Task* task, void* user_data) {
    const char* search_tag = (const char*)user_data;
    static int num_found = 0;
    
    if (task == NULL) { // Reset call
        num_found = 0;
        return;
    }
    
    for (int i = 0; i < task->tag_count; i++) {
        if (strcmp(task->tags[i], search_tag) == 0) {
            if (num_found == 0) {
                 print_task_simple(NULL, NULL); // Reset list counter
            }
            num_found++;
            // This is a bit of a hack to get the task ID
            // A better way would be to pass the list and index
            // For now, we just print the task
            // We can't get the "task number" easily, so just list it
            printf("- %s\n", task->description); 
            break; 
        }
    }
}

void find_by_tag(TaskList* list, const char* search_tag) {
    printf("\n--- Tasks tagged '%s' ---\n", search_tag);
    find_tag_action(NULL, NULL); // Reset counter
    traverse_tasks(list, find_tag_action, (void*)search_tag);
    printf("---------------------------\n");
}

// [Feature 4] Multiple Sort Modes
// Comparators
static int compare_name(const void* a, const void* b) {
    Task* task_a = *(Task**)a;
    Task* task_b = *(Task**)b;
    return strcmp(task_a->description, task_b->description);
}

static int compare_priority(const void* a, const void* b) {
    Task* task_a = *(Task**)a;
    Task* task_b = *(Task**)b;
    bool a_high = (task_a->status & FLAG_HIGH_PRI);
    bool b_high = (task_b->status & FLAG_HIGH_PRI);
    if (a_high && !b_high) return -1;
    if (!a_high && b_high) return 1;
    return compare_name(a, b); // Sub-sort by name
}

static int compare_date(const void* a, const void* b) {
    Task* task_a = *(Task**)a;
    Task* task_b = *(Task**)b;
    
    // Tasks with no date go to the end
    if (task_a->due_date == 0 && task_b->due_date != 0) return 1;
    if (task_a->due_date != 0 && task_b->due_date == 0) return -1;
    
    if (task_a->due_date < task_b->due_date) return -1;
    if (task_a->due_date > task_b->due_date) return 1;
    return compare_priority(a, b); // Sub-sort by priority
}

void sort_tasks(TaskList* list, const char* sort_by) {
    pthread_mutex_lock(&list->lock);
    
    // This is the core of the feature:
    // A function pointer to hold the *chosen* comparator
    int (*comparator)(const void*, const void*);
    
    if (sort_by == NULL || strcmp(sort_by, "name") == 0) {
        comparator = compare_name;
        printf("Sorting by name.\n");
    } else if (strcmp(sort_by, "pri") == 0) {
        comparator = compare_priority;
        printf("Sorting by priority (then name).\n");
    } else if (strcmp(sort_by, "date") == 0) {
        comparator = compare_date;
        printf("Sorting by due date (then priority).\n");
    } else {
        printf("Unknown sort type '%s'. Defaulting to name.\n", sort_by);
        comparator = compare_name;
    }
    
    // Call qsort, passing our chosen function pointer
    qsort(list->tasks, list->count, sizeof(Task*), comparator);
    
    pthread_mutex_unlock(&list->lock);
    
    list_tasks(list);
}

// [Feature 5] Add Dependency
void add_dependency(TaskList* list, int task_id, int dependency_id) {
    if (task_id == dependency_id) {
        printf("Error: A task cannot depend on itself.\n");
        return;
    }
    
    pthread_mutex_lock(&list->lock);
    Task* task = get_task_by_id(list, task_id);
    Task* dep_task = get_task_by_id(list, dependency_id);
    
    if (task == NULL || dep_task == NULL) {
        printf("Error: Invalid task ID.\n");
        pthread_mutex_unlock(&list->lock);
        return;
    }
    
    // Check for existing
    for (int i = 0; i < task->dep_count; i++) {
        if (task->dependencies[i] == dependency_id) {
            printf("Task %d already depends on task %d.\n", task_id, dependency_id);
            pthread_mutex_unlock(&list->lock);
            return;
        }
    }
    
    // Resize if needed
    if (task->dep_count >= task->dep_capacity) {
        task->dep_capacity *= 2;
        task->dependencies = (int*)realloc(task->dependencies, task->dep_capacity * sizeof(int));
    }
    
    task->dependencies[task->dep_count] = dependency_id;
    task->dep_count++;
    
    printf("Task %d now depends on task %d.\n", task_id, dependency_id);
    pthread_mutex_unlock(&list->lock);
}

// [Feature 6] Config Color
void config_color(TaskList* list, const char* value) {
    pthread_mutex_lock(&list->lock);
    if (strcmp(value, "on") == 0) {
        list->color_enabled = true;
        printf("Color output %sENABLED%s.\n", COLOR_GREEN, COLOR_RESET);
    } else if (strcmp(value, "off") == 0) {
        list->color_enabled = false;
        printf("Color output DISABLED.\n");
    } else {
        printf("Usage: config color <on|off>\n");
    }
    pthread_mutex_unlock(&list->lock);
}

// --- Find Function (Original) ---

static void search_task_action(Task* task, void* user_data) {
    const char* search_term = (const char*)user_data;
    static int num_found = 0;

    if (task == NULL) { // Reset call
        num_found = 0;
        return;
    }
    
    if (strstr(task->description, search_term) || (task->notes && strstr(task->notes, search_term))) {
         if (num_found == 0) {
             print_task_simple(NULL, NULL); // Reset list counter
         }
         num_found++;
         // This is still a hack, but it's consistent
         printf("- %s\n", task->description);
    }
}

void find_tasks(TaskList* list, const char* search_term) {
    printf("\n--- Search Results for '%s' ---\n", search_term);
    search_task_action(NULL, NULL); // Reset
    traverse_tasks(list, search_task_action, (void*)search_term);
    printf("--------------------------------\n");
}

// --- File I/O (HEAVILY Updated) ---

#define FILE_DELIM "|"
#define LIST_DELIM ","
#define NULL_MARKER "!"

void* save_tasks_thread(void* arg) {
    TaskList* list = (TaskList*)arg;
    
    printf("Save thread started: Saving tasks...\n");
    
    pthread_mutex_lock(&list->lock);
    
    FILE* file = fopen(FILENAME, "w");
    if (file == NULL) {
        perror("Could not open file for writing");
        pthread_mutex_unlock(&list->lock);
        return NULL;
    }
    
    // Save config first
    fprintf(file, "config%scolor%s%s\n", FILE_DELIM, FILE_DELIM, list->color_enabled ? "on" : "off");
    
    // Save tasks
    for (int i = 0; i < list->count; i++) {
        Task* t = list->tasks[i];
        
        // Format:
        // status|due_date|dep_count|dep1,dep2|tag_count|tag1,tag2|description|notes
        
        fprintf(file, "%d%s", t->status, FILE_DELIM);
        fprintf(file, "%ld%s", t->due_date, FILE_DELIM);
        
        // Dependencies
        fprintf(file, "%d%s", t->dep_count, FILE_DELIM);
        for(int j = 0; j < t->dep_count; j++) {
            fprintf(file, "%d%s", t->dependencies[j], (j == t->dep_count - 1) ? "" : LIST_DELIM);
        }
        fprintf(file, "%s", FILE_DELIM);
        
        // Tags
        fprintf(file, "%d%s", t->tag_count, FILE_DELIM);
        for(int j = 0; j < t->tag_count; j++) {
            fprintf(file, "%s%s", t->tags[j], (j == t->tag_count - 1) ? "" : LIST_DELIM);
        }
        fprintf(file, "%s", FILE_DELIM);
        
        // Description and Notes
        fprintf(file, "%s%s", t->description, FILE_DELIM);
        fprintf(file, "%s\n", t->notes ? t->notes : NULL_MARKER);
    }
    
    fclose(file);
    sleep(1); 
    pthread_mutex_unlock(&list->lock);
    
    printf("Save thread finished: Tasks saved.\n");
    return NULL;
}

void load_tasks(TaskList* list) {
    FILE* file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("No previous save file found.\n");
        return;
    }
    
    char line[MAX_LINE * 4]; // Larger buffer
    int loaded_count = 0;
    
    while (fgets(line, sizeof(line), file) != NULL) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) == 0) continue;

        // Check for config line
        if (strncmp(line, "config|", 7) == 0) {
            char* type = strtok(line, FILE_DELIM);
            char* option = strtok(NULL, FILE_DELIM);
            char* value = strtok(NULL, "");
            if (option && value && strcmp(option, "color") == 0) {
                config_color(list, value);
            }
            continue;
        }
        
        // --- Parse Task Line ---
        
        // Add a new blank task
        // We lock/unlock inside add_task
        add_task(list, "LOADING...", false);
        Task* t = list->tasks[list->count - 1]; // Get pointer to the new task
        
        pthread_mutex_lock(&list->lock); // Lock for manual editing
        
        char* token;
        int field = 0;
        
        token = strtok(line, FILE_DELIM);
        while (token != NULL) {
            switch(field) {
                case 0: // Status
                    t->status = atoi(token);
                    break;
                case 1: // Due Date
                    t->due_date = atol(token);
                    break;
                case 2: // Dep Count
                    t->dep_count = atoi(token);
                    if (t->dep_count > t->dep_capacity) {
                        t->dep_capacity = t->dep_count;
                        t->dependencies = (int*)realloc(t->dependencies, t->dep_capacity * sizeof(int));
                    }
                    break;
                case 3: // Dependencies
                    if (t->dep_count > 0) {
                        char* dep_list = token;
                        char* dep_token = strtok(dep_list, LIST_DELIM);
                        for (int i = 0; i < t->dep_count && dep_token != NULL; i++) {
                            t->dependencies[i] = atoi(dep_token);
                            dep_token = strtok(NULL, LIST_DELIM);
                        }
                    }
                    break;
                case 4: // Tag Count
                    t->tag_count = atoi(token);
                     if (t->tag_count > t->tag_capacity) {
                        t->tag_capacity = t->tag_count;
                        t->tags = (char**)realloc(t->tags, t->tag_capacity * sizeof(char*));
                    }
                    break;
                case 5: // Tags
                    if (t->tag_count > 0) {
                        char* tag_list = token;
                        char* tag_token = strtok(tag_list, LIST_DELIM);
                        for (int i = 0; i < t->tag_count && tag_token != NULL; i++) {
                            t->tags[i] = safe_strdup(tag_token, NULL);
                            tag_token = strtok(NULL, LIST_DELIM);
                        }
                    }
                    break;
                case 6: // Description
                    // free the "LOADING..." description and replace it
                    free(t->description);
                    t->description = safe_strdup(token, NULL);
                    break;
                case 7: // Notes
                    if (strcmp(token, NULL_MARKER) != 0) {
                        t->notes = safe_strdup(token, NULL);
                    }
                    break;
            }
            field++;
            token = strtok(NULL, FILE_DELIM);
        }
        pthread_mutex_unlock(&list->lock);
        loaded_count++;
    }
    
    fclose(file);
    // Suppress the "Added task..." messages from load
    printf("Loaded %d tasks from file.\n", loaded_count);
}