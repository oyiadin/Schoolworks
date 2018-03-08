// 结构类型处理学生数据
#include <stdio.h>

typedef struct {
    int id, score[3], total;
    double average;
    char name[20];
} student;

int main() {
    student stu[5];

    for (int i=0; i<5; ++i) {
        scanf("%d%s%d%d%d", &stu[i].id, &stu[i].name,
            &stu[i].score[0], &stu[i].score[1], &stu[i].score[2]);
        stu[i].total = stu[i].score[0]+stu[i].score[1]+stu[i].score[2];
        stu[i].average = stu[i].total / 3.0;
    }
    
    printf("姓名 总分 平均成绩\n");
    student *max = &stu[0];
    for (int i=0; i<5; ++i) {
        printf("%s %d %.1lf\n", stu[i].name, stu[i].total, stu[i].average);
        if (stu[i].total > max->total) max = &stu[i];
    }
    printf("总分数最高的学生信息如下：\n");
    printf("%d %s %d %d %d\n", max->id, max->name,
        max->score[0], max->score[1], max->score[2]);

    return 0;
}