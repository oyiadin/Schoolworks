#include <math.h>
#include <time.h>
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct Object {
    char ch;
    int x, y;
    struct Object *next;
} object;

const int MAX_X = 47, MAX_Y = 15, SCORE_WHEN_LEVEL_UP = 150;
const double LEVEL_UP_TIME = 2;

const char sHi[] = "Hi, press <Enter> to begin.\n"
    "You can press <Esc> whenever you wanna exit.\n";
const char sLevelMsg[] = "L E V E L  ";
const char sReady[] = "      Ready?";
const char sBottom[] = "[============ "
    "Level %d  Score %04d ============]";
const char sFailed[] = "You failed with the score %d.\n"
    "Why not restart me and try once again?\n";
const char sGoodbye[] = "Your final score is %d. Exiting now.\n";
const char sCongratulations[] = "Congratulations!\n"
    "Your final score is %d. See you!\n";

double time_passed(time_t);
void flush_to_console(void);
void reflush_buffer(void);
void generate_new_letter(void);
void falldown(void);
void keydown_handle(char);
bool check_state(void);
void level_up(void);

object *head = 0;
char buffer[3000];
int level = 0, score = 0;
double generate_new_letter_time = 1.0;

int main(void) {
    sprintf(buffer,  "color F0 && mode con cols=%d lines=%d", MAX_X, MAX_Y+1);
    system(buffer);

    printf(sHi);
    while (getch() != '\r');
    system("cls");
    srand((unsigned) time(0));
    time_t time0_gen = clock(), time0_level=0;
    bool need_reflush = false;

    while (1) {
        if (!check_state()) {
            printf(sFailed, score);
            break;
        } else {
            if ((score / SCORE_WHEN_LEVEL_UP) - level == 1) {
                need_reflush = false;
                time0_level = clock();
                level_up();
            }
            if (level && time_passed(time0_level) < LEVEL_UP_TIME) {
                if (kbhit() && getch() == '\e') {
                    printf(sGoodbye, score);
                    break;
                } else continue;
            }
            if (kbhit()) {
                char ch = getch();
                if (ch == '\e') {
                    printf(sGoodbye, score);
                    break;
                } else if (isalpha(ch)) {
                    keydown_handle(ch);
                    need_reflush = true;
                }
            }
            if (time_passed(time0_gen) >= generate_new_letter_time) {
                time0_gen = clock();
                falldown();
                generate_new_letter();
                need_reflush = true;
            }
            if (need_reflush) {
                need_reflush = false;
                flush_to_console();
            }
        }
    }
    return 0;
}

inline double time_passed(time_t time0) {
    return (clock() - time0) / (double) CLOCKS_PER_SEC;
}

void reflush_buffer(void) {
    int max = (MAX_X+1) * MAX_Y;

    memset(buffer, ' ', sizeof(&buffer)*max);
    for (int i=MAX_X; i<max; i+=(MAX_X+1)) buffer[i] = '\n';
    buffer[(MAX_X+1)*MAX_Y] = '\0';
}

void flush_to_console(void) {
    system("cls");
    reflush_buffer();
    object *at = head;

    while (at) {
        buffer[(at->y)*(MAX_X+1) + at->x] = at->ch;
        at = at->next;
    }
    printf(buffer);
    printf(sBottom, level+1, score);
}

void generate_new_letter() {
    object *p = (object*) malloc(sizeof(object));
    p->ch = rand() % 26 + 65;
    p->x = rand() % MAX_X;
    p->y = 0;
    p->next = 0;

    if (head == 0) head = p;
    else {
        object *at = head;
        while (at->next) at = at->next;
        at->next = p;
    }
}

void falldown() {
    object *at = head;
    while (at) {
        (at->y)++;
        at = at->next;
    }
}

void keydown_handle(char ch) {
    object *at = head, *next = 0, *prev = 0;
    ch = toupper(ch);

    score -= 9;
    while (at) {
        if (at->ch == ch) {
            if (prev == 0) head = at->next;
            else prev->next = at->next;

            free(at);
            score += 19;
            break;
        } else {
            prev = at;
            at = at->next;
        }
    }
}

bool check_state() {
    object *at = head;

    if (score < 0) return false;
    while (at) {
        if (at->y > MAX_Y-1) return false;
        at = at->next;
    }
    return true;
}

void level_up() {
    system("cls");

    object *at = head, *next;
    while (at) {
        next = at->next;
        free(at);
        at = next;
    }
    head = 0;
    reflush_buffer();

    if (level == 8) {
        printf(sCongratulations, score);
        exit(0);
    }

    const char *p[2] = {sLevelMsg, sReady};
    int at_pos;
    for (int i=0; i<2; ++i) {
        at_pos = (MAX_Y-3 + i)*(MAX_X+1);
        strcpy(&buffer[at_pos], p[i]);
        buffer[at_pos+strlen(p[i])] = (i == 0) ? ('0'+level+2) : ' ';
    }
    printf(buffer);
    printf(sBottom, level+1, score);
    ++level;
    generate_new_letter_time -= 0.1;
}
