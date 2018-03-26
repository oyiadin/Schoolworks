#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 由于暂时无法参透 C 语言里怎么处理编码问题，没能力修中文的 bug
// Orz
char *base64(const char *origin) {
    const char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int origin_len = strlen(origin);
    int converted_len = 4 * (origin_len / 3) + 1;  // 留一个给空字符
    if (origin_len % 3) converted_len += 4;

    char *converted = (char *) malloc(converted_len);

    // 处理不需要补全的部分（若有），3 bytes 一组地处理
    int i = 0, bytes;
    if (origin_len >= 3) {
        for (; i < (origin_len / 3); ++i) {
            bytes = (origin[3*i+0] << 16) + (origin[3*i+1] << 8) + origin[3*i+2];
            converted[4*i+0] = table[(bytes & 0xFC0000) >> 18];
            converted[4*i+1] = table[(bytes & 0x03F000) >> 12];
            converted[4*i+2] = table[(bytes & 0x000FC0) >> 6];
            converted[4*i+3] = table[bytes & 0x00003F];
        }
    }

    // 处理最后的补全
    if (origin_len % 3) {
        bytes = (origin[3*i+0] << 16) + ((origin_len % 3 == 2) ? (origin[3*i+1] << 8) : 0);
        converted[4*i+0] = table[(bytes & 0xFC0000) >> 18];
        converted[4*i+1] = table[(bytes & 0x03F000) >> 12];
        if (origin_len % 3 == 1) converted[4*i+2] = '=';
        else if (origin_len % 3 == 2) converted[4*i+2] = table[(bytes & 0x000FC0) >> 6];
        converted[4*i+3] = '=';
    }

    converted[converted_len-1] = 0x00;
    return converted;
}


int main() {
    char a[10000];
    while (gets(a)) {
        char *b64 = base64(a);
        printf("[%s] to [%s]\n", a, b64);
        free(b64);
    }

    return 0;
}