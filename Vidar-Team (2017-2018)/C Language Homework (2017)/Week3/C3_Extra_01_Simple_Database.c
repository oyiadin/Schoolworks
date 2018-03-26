#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>
#define MAX_LENGTH 500

typedef struct student {
    char *name;
    int math;
    int chinese;
    struct student *next;
} Student;
typedef Student* pStudent;
typedef pStudent* pHead;

void all_to_lower(char *str) {
    while (*str) {
        *str = tolower(*str);
        ++str;
    }
}

pStudent to_end(pHead head) {
    // returns 0 when empty, otherwise returns the last student
    if (*head == 0) return 0;

    pStudent at = *head;
    while (at->next) at = at->next;

    return at;
}

pStudent find(pHead head, char *name) {
    // returns 0 when cannot find it, otherwise returns the specific student required
    if (*head == 0) return 0;

    pStudent at = *head;

    while (strcmp(at->name, name)) {
        if (at->next)
            at = at->next;
        else
            return 0;
    }
    return at;
}

int insert(pHead head, char *name, int math, int chinese) {
    // returns 1 when succeeded, and 0 when failed
    if (find(head, name) != 0) return 0;

    pStudent newStud = (pStudent) malloc(sizeof(Student));
    char *_name = (char *) malloc(sizeof(char [MAX_LENGTH]));
    strcpy(_name, name);
    newStud->name = _name;
    newStud->math = math;
    newStud->chinese = chinese;
    newStud->next = 0;

    pStudent end = to_end(head);
    if (end) end->next = newStud;
    else *head = newStud;

    return 1;
}

int del(pHead head, char *name) {
    if (*head == 0) return 0;

    pStudent last = 0, at = *head;
    while (strcmp(at->name, name)) {
        if (at->next) {
            last = at;
            at = at->next;
        } else return 0;
    }

    if (last == 0) *head = 0;
    else last->next = 0;
    free(at->name);
    free(at);
    return 1;
}

int update(pHead head, char *name0, char *name1, int math, int chinese) {
    if (*head == 0) return 0;

    pStudent selected = find(head, name0);
    if (!selected) return 0;

    free(selected->name);
    char *_name = (char *) malloc(sizeof(char [MAX_LENGTH]));
    strcpy(_name, name1);
    selected->name = _name;
    selected->math = math;
    selected->chinese = chinese;
    return 1;
}

int fsave(pHead head, FILE *f) {
    if (*head == 0) return 1;
    if (f == 0) return 0;

    pStudent at = *head;
    while (at) {
        int result = fprintf(f, "Name:%s, Math:%d, Chinese:%d;\n",
            at->name, at->math, at->chinese);
        if (result < 0) return 0;
        at = at->next;
    }
    return 1;
}

int fread(pHead head, FILE *f) {
    if (f == 0) return 0;

    char line[MAX_LENGTH];
    char name[MAX_LENGTH];
    int math, chinese;
    while (fgets(line, MAX_LENGTH, f) != NULL) {
        sscanf(line, "Name:%[^,], Math:%d, Chinese:%d;", name, &math, &chinese);
        if (find(head, name)) return 0;
        else insert(head, name, math, chinese);
    }
    return 1;
}

void read_until(char *breaking, char *from, char *to, char **for_offset) {
    int pos_break = strcspn(from, breaking);
    from[pos_break] = 0x00;
    if (!pos_break) {
        strncpy(to, from, MAX_LENGTH);
    } else {
        strncpy(to, from, pos_break+1);
    }
    *for_offset = from + pos_break + 1;
}


int main() {
    // I assume that all the input must be legal.
    pStudent head = 0;

    char sCommands[] = "Available commands:\n    insert, delete, find, update, clear, fsave, fread, help, exit\n";
    char sSuccess[] = "success\n";
    char sError[] = "error\n";
    char sWelcome[] = "Welcome. @oyiadin 2017/11/10\n";
    char sUnknown[] = "Unknown command. Please Try again.\n";
    char sGoodbye[] = "goodbye\n";
    char sHelp[] = "Examples:\n"
        "  * insert Name:Xiaoming, Math:88, Chinese:100;\n"
        "  * find Name:Xiaoming;\n"
        "  * update name0:Xiaoming, name:Xiaohong, math:88, chinese:100;\n"
        "  * fsave name:filename\n";
    char sIn[] = ">>> ";
    char sOut[] = "==> ";

    char input[MAX_LENGTH], *remain;
    char command[MAX_LENGTH];
    char temp_name[MAX_LENGTH], temp_value[MAX_LENGTH];
    char name[MAX_LENGTH], name0[MAX_LENGTH];
    int math, chinese;

    printf(sWelcome);
    printf(sCommands);
    printf(sIn);
    while (fgets(input, MAX_LENGTH, stdin)) {
        read_until(" \n", input, command, &remain);

        while (*remain != '\0' && *remain != '\n') {
            read_until(":", remain, temp_name, &remain);
            while (isspace(*remain)) ++remain;
            read_until(",;\n", remain, temp_value, &remain);
            while (isspace(*remain)) ++remain;
            all_to_lower(temp_name);

            if (!strcmp(temp_name, "name")) strcpy(name, temp_value);
            else if (!strcmp(temp_name, "name0")) strcpy(name0, temp_value);
            else if (!strcmp(temp_name, "math")) math = atoi(temp_value);
            else if (!strcmp(temp_name, "chinese")) chinese = atoi(temp_value);
        }

        all_to_lower(command);

        printf(sOut);
        if (!strcmp(command, "insert")) {
            int result = insert(&head, name, math, chinese);
            if (result) printf(sSuccess);
            else printf(sError);
        } else if (!strcmp(command, "find")) {
            pStudent result = find(&head, name);
            if (!result) printf(sError);
            else {
                printf("%s: Math:%d, Chinese:%d;\n",
                    result->name, result->math, result->chinese);
            }
        } else if (!strcmp(command, "delete")) {
            int result = del(&head, name);
            if (result) printf(sSuccess);
            else printf(sError);
        } else if (!strcmp(command, "update")) {
            int result = update(&head, name0, name, math, chinese);
            if (result) printf(sSuccess);
            else printf(sError);
        } else if (!strcmp(command, "fsave")) {
            FILE *f = fopen(name, "w");
            if (fsave(&head, f)) printf(sSuccess);
            else printf(sError);
            fclose(f);
        } else if (!strcmp(command, "fread")) {
            FILE *f = fopen(name, "r");
            if (fread(&head, f)) printf(sSuccess);
            else printf(sError);
            fclose(f);
        } else if (!strcmp(command, "clear")) {
            if (head == 0) printf(sSuccess);
            else {
                pStudent at = head, next;
                while (at) {
                    free(at->name);
                    next = at->next;
                    free(at);
                    at = next;
                }
                head = 0;
                printf(sSuccess);
            }
        } else if (!strcmp(command, "help")) {
            printf(sHelp);
        } else if (!strcmp(command, "exit")) {
            printf(sGoodbye);
            exit(0);
        } else {
            printf(sUnknown);
        }
        printf(sIn);
    }
    return 0;
}
