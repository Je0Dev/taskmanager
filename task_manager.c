#include "task_manager.h"

// --- [FEATURE 5: Command Handler Functions] ---

// --- New Handlers for New Features ---

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

void handle_findtag(TaskList* list, char* argument) {
    if (argument == NULL) {
        printf("Error: 'findtag' requires a tag name.\n");
    } else {
        find_by_tag(list, argument);
    }
}

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

// --- Handler Functions from previous step ---

void handle_quit(TaskList* list, char* argument) {
    (void)list; (void)argument;
}

void handle_add(TaskList* list, char* argument) {
    if (argument == NULL) {
        printf("Error: 'add' requires a description.\n");
    } else {
        add_task(list, argument, false);
    }
}

void handle_p_add(TaskList* list, char* argument) {
    if (argument == NULL) {
        printf("Error: 'p-add' requires a description.\n");
    } else {
        add_task(list, argument, true);
    }
}

void handle_list(TaskList* list, char* argument) {
    (void)argument;
    list_tasks(list);
}

void handle_done(TaskList* list, char* argument) {
    if (argument == NULL) {
        printf("Error: 'done' requires a task ID.\n");
    } else {
        mark_task_complete(list, atoi(argument));
    }
}

void handle_delete(TaskList* list, char* argument) {
    if (argument == NULL) {
        printf("Error: 'delete' requires a task ID.\n");
    } else {
        delete_task(list, atoi(argument));
    }
}

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

void handle_view(TaskList* list, char* argument) {
    if (argument == NULL) {
        printf("Error: 'view' requires a task ID.\n");
    } else {
        view_task(list, atoi(argument));
    }
}

void handle_find(TaskList* list, char* argument) {
    if (argument == NULL) {
        printf("Error: 'find' requires a search term.\n");
    } else {
        find_tasks(list, argument);
    }
}

void handle_sort(TaskList* list, char* argument) {
    // This now correctly passes the argument,
    // which matches your task_manager.h
    sort_tasks(list, argument);
}

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

void handle_help(TaskList* list, char* argument);

// --- [FEATURE 5: The Command Dispatcher Table] ---
typedef struct {
    const char* name;
    void (*handler)(TaskList* list, char* argument);
} Command;

static Command commands[] = {
    // Old commands
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
    // New commands
    {"due", handle_due},
    {"tag", handle_tag},
    {"untag", handle_untag},
    {"findtag", handle_findtag},
    {"depend", handle_depend},
    {"config", handle_config},
};
static const int num_commands = sizeof(commands) / sizeof(Command);

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

void handle_help(TaskList* list, char* argument) {
    (void)list; (void)argument;
    print_menu();
}


int main() {
    TaskList task_list;
    char buffer[MAX_LINE];
    char* command;
    char* argument;
    bool running = true;
    
    init_task_list(&task_list);
    load_tasks(&task_list); 

    print_menu();
    
    do {
        printf("Enter command: ");
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break; 
        }
        
        buffer[strcspn(buffer, "\n")] = 0;
        
        // Make a copy of the buffer for strtok to modify
        char* temp_buffer = malloc(strlen(buffer) + 1);
        if (temp_buffer == NULL) {
            perror("Failed to allocate command buffer");
            continue;
        }
        strcpy(temp_buffer, buffer);

        command = strtok(temp_buffer, " ");
        argument = strtok(NULL, ""); 
        
        if (command == NULL) {
            free(temp_buffer);
            continue;
        }

        if (strcmp(command, "quit") == 0) {
            running = false;
        } else {
            bool found_command = false;
            for (int i = 0; i < num_commands; i++) {
                if (strcmp(command, commands[i].name) == 0) {
                    // Call the handler function from the array
                    commands[i].handler(&task_list, argument);
                    found_command = true;
                    break;
                }
            }
            if (!found_command) {
                printf("Error: Unknown command '%s'. Type 'help' for a list.\n", command);
            }
        }
        
        // We must free the buffer we malloc'd
        free(temp_buffer); 

    } while (running);

    printf("Quitting... freeing memory.\n");
    free_task_list(&task_list);
    printf("Goodbye!\n");
    
    return 0;
}