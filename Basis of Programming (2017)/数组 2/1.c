// 字符串回文
#include <stdio.h>
#include <ctype.h>
#define MAX_lENGTH 100

int huiwen(char*);
int main() {
    char input[MAX_lENGTH];
    while (gets(input))
        printf(huiwen(input) ? "Y\n" : "N\n");

    return 0;
}

int huiwen(char *s) {
    char bypassed[MAX_lENGTH], at=0;

    for (int i=0; s[i]; ++i)
        if (isalpha(s[i]))
            bypassed[at++] = tolower(s[i]);

    for (int i=0; i<at/2; ++i)
        if (bypassed[i]!=bypassed[at-1-i])
            return 0;
    return 1;
}