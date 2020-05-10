#include <stdio.h>
#include <stdlib.h>

typedef struct LinkNode {
    struct LinkNode *next;
    int code, ID;
} LNode, *LinkList;

int main() {
    int m, n;
    printf("[>]    m = ");
    scanf("%d", &m);
    printf("[>]    n = ");
    scanf("%d", &n);
    
    printf("[>] code = ");
    
    LinkList head = (LinkList) malloc(sizeof(LNode));
    // 为了方便，先创建一个头节点，稍后会删除
    LNode *at = head;
    
    for (int i = 0; i < n; ++i) {
        int code;
        scanf("%d", &code);
        LNode *new_node = (LNode *) malloc(sizeof(LNode));
        new_node->code = code;
        new_node->ID = i+1;
        // new_node->next = NULL;
        // 没必要，后边会正确覆盖
        at->next = new_node;
        at = new_node;
    }
    at->next = head->next;
    free(head);
    // 构成循环
    
    printf("[<]  seq = [ ");
    
    LNode *prev = at;
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j < m; ++j)
            prev = prev->next;
        
        // 更新 m 值
        m = prev->next->code;
        printf("%d ", prev->next->ID);
        // 删除对应节点
        LNode *nextnext = prev->next->next;
        free(prev->next);
        prev->next = nextnext;
    }
    
    puts("]");
}
