#include "task_manager.h"

// ========================================
// COMMAND HANDLER FUNCTIONS
// ========================================

// --- New Feature Handler Functions ---

/**
 * Handle the 'due' command to set a task's due date.
 * 
 * This function parses the command arguments and calls the due date
 * setting function with proper validation.
 * 
 * @param list Pointer to the TaskList
 * @param argument Command arguments containing task ID and date
 */
void handle_due(TaskList* list, char* argument) {
    if (argument == NULL) {
        printf("Usage: due <id> <YYYY-MM-DD>\n");
        return;
    }
    char* id_str = strtok(argument, " ");
    char* date_str = strtok(NULL, "");
    
    if (id_str == NULL || date_str == NULL) {
        printf("Usage: due <id> <YYYY-MM-DD>\n");
    } else {
        add_due_date(list, atoi(id_str), date_str);
    }
}

/**
 * Handle the 'tag' command to add a tag to a task.
 * 
 * This function parses the command arguments and calls the tag
 * adding function with proper validation.
 * 
 * @param list Pointer to the TaskList
 * @param argument Command arguments containing task ID and tag name
 */
void handle_tag(TaskList* list, char* argument) {
    if (argument == NULL) {
        printf("Usage: tag <id> <tag_name>\n");
        return;
    }
    char* id_str = strtok(argument, " ");
    char* tag_name = strtok(NULL, "");
    
    if (id_str == NULL || tag_name == NULL) {
        printf("Usage: tag <id> <tag_name>\n");
    } else {
        add_tag(list, atoi(id_str), tag_name);
    }
}

/**
 * Handle the 'untag' command to remove a tag from a task.
 * 
 * This function parses the command arguments and calls the tag
 * removal function with proper validation.
 * 
 * @param list Pointer to the TaskList
 * @param argument Command arguments containing task ID and tag name
 */
void handle_untag(TaskList* list, char* argument) {
    if (argument == NULL) {
        printf("Usage: untag <id> <tag_name>\n");
        return;
    }
    char* id_str = strtok(argument, " ");
    char* tag_name = strtok(NULL, "");
    
    if (id_str == NULL || tag_name == NULL) {
        printf("Usage: untag <id> <tag_name>\n");
    } else {
        remove_tag(list, atoi(id_str), tag_name);
    }
}

/**
 * Handle the 'findtag' command to search for tasks by tag.
 * 
 * This function validates the tag name argument and calls the
 * tag search function.
 * 
 * @param list Pointer to the TaskList
 * @param argument Command argument containing the tag to search for
 */
void handle_findtag(TaskList* list, char* argument) {
    if (argument == NULL) {
        printf("Error: 'findtag' requires a tag name.\n");
    } else {
        find_by_tag(list, argument);
    }
}

/**
 * Handle the 'depend' command to add task dependencies.
 * 
 * This function parses the command arguments and calls the dependency
 * adding function with proper validation.
 * 
 * @param list Pointer to the TaskList
 * @param argument Command arguments containing two task IDs
 */
void handle_depend(TaskList* list, char* argument) {
    if (argument == NULL) {
        printf("Usage: depend <id1> <id2> (task 1 depends on task 2)\n");
        return;
    }
    char* id1_str = strtok(argument, " ");
    char* id2_str = strtok(NULL, "");
    
    if (id1_str == NULL || id2_str == NULL) {
        printf("Usage: depend <id1> <id2>\n");
    } else {
        add_dependency(list, atoi(id1_str), atoi(id2_str));
    }
}

/**
 * Handle the 'config' command to change system configuration.
 * 
 * This function parses the command arguments and calls the
 * appropriate configuration function.
 * 
 * @param list Pointer to the TaskList
 * @param argument Command arguments containing option and value
 */
void handle_config(TaskList* list, char* argument) {
    if (argument == NULL) {
        printf("Usage: config <option> <value> (e.g., config color on)\n");
        return;
    }
    char* option = strtok(argument, " ");
    char* value = strtok(NULL, "");
    
    if (option == NULL || value == NULL) {
        printf("Usage: config <option> <value>\n");
    } else if (strcmp(option, "color") == 0) {
        config_color(list, value);
    } else {
        printf("Unknown config option: %s\n", option);
    }
}

// --- Core Command Handler Functions ---

/**
 * Handle the 'quit' command to exit the program.
 * 
 * This function sets the running flag to false to terminate the main loop.
 * 
 * @param list Pointer to the TaskList (unused)
 * @param argument Command argument (unused)
 */
void handle_quit(TaskList* list, char* argument) {
    (void)list; (void)argument;
}

/**
 * Handle the 'add' command to add a regular priority task.
 * 
 * This function validates the description argument and calls the
 * task addition function with normal priority.
 * 
 * @param list Pointer to the TaskList
 * @param argument Command argument containing the task description
 */
void handle_add(TaskList* list, char* argument) {
    if (argument == NULL) {
        printf("Error: 'add' requires a description.\n");
    } else {
        add_task(list, argument, false);
    }
}

/**
 * Handle the 'p-add' command to add a high priority task.
 * 
 * This function validates the description argument and calls the
 * task addition function with high priority flag.
 * 
 * @param list Pointer to the TaskList
 * @param argument Command argument containing the task description
 */
void handle_p_add(TaskList* list, char* argument) {
    if (argument == NULL) {
        printf("Error: 'p-add' requires a description.\n");
    } else {
        add_task(list, argument, true);
    }
}

/**
 * Handle the 'list' command to display all tasks.
 * 
 * This function calls the list display function without any arguments.
 * 
 * @param list Pointer to the TaskList
 * @param argument Command argument (unused)
 */
void handle_list(TaskList* list, char* argument) {
    (void)argument;
    list_tasks(list);
}

/**
 * Handle the 'done' command to mark a task as completed.
 * 
 * This function validates the task ID argument and calls the
 * completion marking function.
 * 
 * @param list Pointer to the TaskList
 * @param argument Command argument containing the task ID
 */
void handle_done(TaskList* list, char* argument) {
    if (argument == NULL) {
        printf("Error: 'done' requires a task ID.\n");
    } else {
        mark_task_complete(list, atoi(argument));
    }
}

/**
 * Handle the 'delete' command to remove a task.
 * 
 * This function validates the task ID argument and calls the
 * task deletion function.
 * 
 * @param list Pointer to the TaskList
 * @param argument Command argument containing the task ID
 */
void handle_delete(TaskList* list, char* argument) {
    if (argument == NULL) {
        printf("Error: 'delete' requires a task ID.\n");
    } else {
        delete_task(list, atoi(argument));
    }
}

/**
 * Handle the 'edit' command to modify a task's description.
 * 
 * This function parses the command arguments and calls the
 * task editing function with proper validation.
 * 
 * @param list Pointer to the TaskList
 * @param argument Command arguments containing task ID and new description
 */
void handle_edit(TaskList* list, char* argument) {
    if (argument == NULL) {
        printf("Usage: edit <id> <new description>\n");
        return;
    }
    char* id_str = strtok(argument, " ");
    char* new_desc = strtok(NULL, "");
    
    if (id_str == NULL || new_desc == NULL) {
        printf("Usage: edit <id> <new description>\n");
    } else {
        edit_task(list, atoi(id_str), new_desc);
    }
}

/**
 * Handle the 'note' command to add or update task notes.
 * 
 * This function parses the command arguments and calls the
 * note addition function with proper validation.
 * 
 * @param list Pointer to the TaskList
 * @param argument Command arguments containing task ID and note text
 */
void handle_note(TaskList* list, char* argument) {
    if (argument == NULL) {
        printf("Usage: note <id> <note text>\n");
        return;
    }
    char* id_str = strtok(argument, " ");
    char* note_text = strtok(NULL, "");
    
    if (id_str == NULL || note_text == NULL) {
        printf("Usage: note <id> <note text>\n");
    } else {
        add_note(list, atoi(id_str), note_text);
    }
}

/**
 * Handle the 'view' command to display detailed task information.
 * 
 * This function validates the task ID argument and calls the
 * task viewing function.
 * 
 * @param list Pointer to the TaskList
 * @param argument Command argument containing the task ID
 */
void handle_view(TaskList* list, char* argument) {
    if (argument == NULL) {
        printf("Error: 'view' requires a task ID.\n");
    } else {
        view_task(list, atoi(argument));
    }
}

/**
 * Handle the 'find' command to search for tasks by text.
 * 
 * This function validates the search term argument and calls the
 * task search function.
 * 
 * @param list Pointer to the TaskList
 * @param argument Command argument containing the search term
 */
void handle_find(TaskList* list, char* argument) {
    if (argument == NULL) {
        printf("Error: 'find' requires a search term.\n");
    } else {
        find_tasks(list, argument);
    }
}

/**
 * Handle the 'sort' command to sort tasks by different criteria.
 * 
 * This function passes the sorting criteria to the sort function.
 * 
 * @param list Pointer to the TaskList
 * @param argument Command argument containing the sort criteria
 */
void handle_sort(TaskList* list, char* argument) {
    // This now correctly passes the argument,
    // which matches your task_manager.h
    sort_tasks(list, argument);
}

/**
 * Handle the 'save' command to save tasks to file.
 * 
 * This function creates a background thread to save tasks without
 * blocking the main program execution.
 * 
 * @param list Pointer to the TaskList
 * @param argument Command argument (unused)
 */
void handle_save(TaskList* list, char* argument) {
    (void)argument;
    pthread_t save_thread_id;
    
    if(pthread_create(&save_thread_id, NULL, save_tasks_thread, list) != 0) {
        perror("Failed to create save thread");
    } else {
        pthread_detach(save_thread_id);
        printf("Save command issued. Saving in background...\n");
    }
}

/**
 * Handle the 'help' command to display available commands.
 * 
 * This function calls the menu printing function.
 * 
 * @param list Pointer to the TaskList (unused)
 * @param argument Command argument (unused)
 */
void handle_help(TaskList* list, char* argument);

// ========================================
// COMMAND DISPATCHER SYSTEM
// ========================================

/**
 * Command structure defining the mapping between command names and handler functions.
 */
typedef struct {
    const char* name;                    // Command name string
    void (*handler)(TaskList* list, char* argument); // Function pointer to handler
} Command;

/**
 * Array of all available commands with their corresponding handlers.
 * This enables dynamic command dispatching based on user input.
 */
static Command commands[] = {
    // Core task management commands
    {"add", handle_add},
    {"p-add", handle_p_add},
    {"list", handle_list},
    {"done", handle_done},
    {"delete", handle_delete},
    {"edit", handle_edit},
    {"note", handle_note},
    {"view", handle_view},
    {"find", handle_find},
    {"sort", handle_sort},
    {"save", handle_save},
    {"help", handle_help},
    
    // Advanced feature commands
    {"due", handle_due},
    {"tag", handle_tag},
    {"untag", handle_untag},
    {"findtag", handle_findtag},
    {"depend", handle_depend},
    {"config", handle_config},
};

/**
 * Number of commands in the commands array.
 * Calculated automatically to avoid maintenance issues.
 */
static const int num_commands = sizeof(commands) / sizeof(Command);

/**
 * Display the complete menu of available commands.
 * 
 * This function shows all available commands with brief descriptions
 * to help users understand the system's capabilities.
 */
void print_menu() {
    printf("\n--- C Task Manager (v3) ---\n");
    printf("  add <desc>     - Add a task\n");
    printf("  p-add <desc>   - Add a high-priority task\n");
    printf("  list           - List all tasks\n");
    printf("  view <id>      - View task details\n");
    printf("  done <id>      - Mark task as complete\n");
    printf("  edit <id> <d>  - Edit a task's description\n");
    printf("  note <id> <t>  - Add/edit a task's notes\n");
    printf("  delete <id>    - Delete a task\n");
    printf("  find <term>    - Find tasks by term (in desc/notes)\n");
    printf("  due <id> <date>  - Set due date (YYYY-MM-DD)\n");
    printf("  tag <id> <tag>   - Add a tag to a task\n");
    printf("  untag <id> <tag> - Remove a tag from a task\n");
    printf("  findtag <tag>  - Find tasks by tag\n");
    printf("  depend <id1> <id2> - Task <id1> depends on <id2>\n");
    printf("  sort <name|pri|date> - Sort tasks\n");
    printf("  config <opt> <val> - Set config (e.g., config color on)\n");
    printf("  save           - Save tasks (in background)\n");
    printf("  help           - Show this menu\n");
    printf("  quit           - Exit the program\n");
    printf("----------------------------------\n");
}

/**
 * Handle the 'help' command by displaying the menu.
 * 
 * @param list Pointer to the TaskList (unused)
 * @param argument Command argument (unused)
 */
void handle_help(TaskList* list, char* argument) {
    (void)list; (void)argument;
    print_menu();
}

// ========================================
// MAIN PROGRAM EXECUTION
// ========================================

/**
 * Main function - Entry point of the task manager application.
 * 
 * This function initializes the system, loads existing data, handles
 * the main command loop, and performs cleanup on exit.
 * 
 * @return int Exit status (0 for success)
 */
int main() {
    TaskList task_list;              // Main task list structure
    char buffer[MAX_LINE];           // Input buffer for user commands
    char* command;                   // Parsed command name
    char* argument;                  // Parsed command arguments
    bool running = true;             // Main loop control flag
    
    // Initialize the task list system
    init_task_list(&task_list);
    load_tasks(&task_list); 

    // Display the help menu
    print_menu();
    
    // Main command processing loop
    do {
        printf("Enter command: ");
        
        // Read user input
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break; 
        }
        
        // Remove newline character from input
        buffer[strcspn(buffer, "\n")] = 0;
        
        // Create a copy of the buffer for tokenization
        char* temp_buffer = malloc(strlen(buffer) + 1);
        if (temp_buffer == NULL) {
            perror("Failed to allocate command buffer");
            continue;
        }
        strcpy(temp_buffer, buffer);

        // Parse command and arguments
        command = strtok(temp_buffer, " ");
        argument = strtok(NULL, ""); 
        
        // Skip empty commands
        if (command == NULL) {
            free(temp_buffer);
            continue;
        }

        // Check for quit command
        if (strcmp(command, "quit") == 0) {
            running = false;
        } else {
            // Search for and execute the appropriate command handler
            bool found_command = false;
            for (int i = 0; i < num_commands; i++) {
                if (strcmp(command, commands[i].name) == 0) {
                    // Call the handler function from the array
                    commands[i].handler(&task_list, argument);
                    found_command = true;
                    break;
                }
            }
            
            // Handle unknown commands
            if (!found_command) {
                printf("Error: Unknown command '%s'. Type 'help' for a list.\n", command);
            }
        }
        
        // Free the temporary buffer
        free(temp_buffer); 

    } while (running);

    // Cleanup and exit
    printf("Quitting... freeing memory.\n");
    free_task_list(&task_list);
    printf("Goodbye!\n");
    
    return 0;
}