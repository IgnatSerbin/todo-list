#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_TASKS 100
#define TASK_LEN 128

typedef struct {
    char text[TASK_LEN];
    int done;
} Task;

Task tasks[MAX_TASKS];
int task_count = 0;

void load_tasks() {
    FILE *f = fopen("tasks.txt", "r");
    if (!f) return;

    while (task_count < MAX_TASKS &&
           fscanf(f, "%d %[^\n]", &tasks[task_count].done, tasks[task_count].text) == 2) {
        task_count++;
    }

    fclose(f);
}

void save_tasks() {
    FILE *f = fopen("tasks.txt", "w");
    for (int i = 0; i < task_count; i++) {
        fprintf(f, "%d %s\n", tasks[i].done, tasks[i].text);
    }
    fclose(f);
}

void draw_ui(int selected) {
    clear();

    mvprintw(0, 0, "=== TODO List ===");
    mvprintw(1, 0, "UP/DOWN - Navigate | SPACE - Toggle Done | A - Add | D - Delete | Q - Quit");

    for (int i = 0; i < task_count; i++) {
        char status = tasks[i].done ? 'X' : ' ';
        char line[140];

        snprintf(line, sizeof(line), "[%c] %s", status, tasks[i].text);

        if (i == selected) {
            attron(A_REVERSE);
            mvprintw(3 + i, 0, "%s", line);
            attroff(A_REVERSE);
        } else {
            mvprintw(3 + i, 0, "%s", line);
        }
    }

    refresh();
}

void add_task() {
    echo();
    curs_set(1);
    mvprintw(3 + task_count, 0, "Enter new task: ");
    char new_task[TASK_LEN];
    getnstr(new_task, TASK_LEN - 1);
    curs_set(0);
    noecho();

    if (strlen(new_task) > 0 && task_count < MAX_TASKS) {
        strcpy(tasks[task_count].text, new_task);
        tasks[task_count].done = 0;
        task_count++;
    }
}

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    load_tasks();

    int selected = 0;
    int ch;

    while ((ch = getch()) != 'q') {
        switch (ch) {
            case KEY_UP:
                if (selected > 0) selected--;
                break;
            case KEY_DOWN:
                if (selected < task_count - 1) selected++;
                break;
            case ' ':
                if (task_count > 0)
                    tasks[selected].done = !tasks[selected].done;
                break;
            case 'a':
                add_task();
                break;
            case 'd':
                if (task_count > 0) {
                    for (int i = selected; i < task_count - 1; i++) {
                        tasks[i] = tasks[i + 1];
                    }
                    task_count--;
                    if (selected >= task_count) selected = task_count - 1;
                    if (selected < 0) selected = 0;
                }
                break;
        }
        draw_ui(selected);
    }

    save_tasks();
    endwin();
    return 0;
}