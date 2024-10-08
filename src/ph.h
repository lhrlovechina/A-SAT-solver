#ifndef PH_H
#define PH_H

#include <stdio.h>

// 全局变量
#define MAX_VARS 60000 // 最大变元数
#define MAX_CLAUSES 230000 // 最大子句数
#define ASSIGNED -2 // 暂时的赋值
#define UNASSIGNED -1 // 还没有理解的赋值
#define TRUE 1 
#define FALSE 0 
#define YES 1
#define NO 0
#define SATISFIED 1
#define SHRUNK 0
#define SAT 1
#define UNSAT 0

typedef int status;

typedef struct literal_information { // 文字结构
    int* literal_clause; // 文字在公式中的位置
    int* literal_clause_pos; // 文字在子句中的位置
    int n_number; // 个数
    int is_assigned; // 文字是否被赋值
    int is_in_unit_clause; // 记录是否在单子句中出现 NO or YES
} literal_information;


typedef struct clause_information { // 子句结构
    int* literals; // 子句中的文字
    int* literals_is_assigned; // 1 for unassigned 0 for false
    int length_current; // 现在的字句长度
    int length_original; // 最初子句长度
    int is_clause_satisfied; // 是否满足 NO OR YES true 
    int unit_clause_literal; // 存储单子句文字中，未赋值文字
} clause_information;


typedef struct changes_information { // 回溯信息
    int index_of_clause; // 子句序号
    int index_of_literal; // 文字序号
} changes_information; // 子句满足时，记录子句序号。文字被设为假时，记录文字和字句序号


typedef struct assign_information { // 记录赋值操作
    int value; // 赋值类型UNASSIGNED TRUE FALSE
} assign_information;


#endif // PH_H