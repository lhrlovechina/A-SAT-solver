#include "ph.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#pragma once

int is_contradicted; // 是否用冲突的单子句
int conflicting_literal; // 用于记录冲突的文字
int unit_clause_stack[MAX_CLAUSES]; // 单子句规则
int n_unit_clause_stack; // 全局栈，用于记录栈顶位置
int depth; // DPLL树中，结点的深度
int n_vars; // 变元总个数
int dpll_called; // 统计DPLL执行次数
int max_clause_len; // 最大字句长度
literal_information literal_info[MAX_VARS][2]; // 文字存储数组
clause_information* clauses; // 子句存储数组

int original_formula_length; // 原长度
int current_formula_length; // 现长度
changes_information changes_stack[8 * MAX_CLAUSES]; // 记录
unsigned int changes_stack_index; // 数组changes的下标
unsigned int n_changes[MAX_VARS][2]; // 某一层满足或不满足的句子的个数
assign_information result[MAX_VARS]; // 用于回溯操作的记录

int CNFparser(FILE* fp) {
	//重置全局变量，以确保每次读取文件之前都是干净的
	dpll_called = 0;
	max_clause_len = 0;
	for (int i = 0; i < MAX_VARS; i++) {
		literal_info[i][SATISFIED].is_assigned = UNASSIGNED;
		literal_info[i][SATISFIED].is_in_unit_clause = NO;
		literal_info[i][SATISFIED].literal_clause = NULL;
		literal_info[i][SATISFIED].literal_clause_pos = NULL;
		literal_info[i][SATISFIED].n_number = 0;
		literal_info[i][SHRUNK].is_assigned = UNASSIGNED;
		literal_info[i][SHRUNK].is_in_unit_clause = NO;
		literal_info[i][SHRUNK].literal_clause = NULL;
		literal_info[i][SHRUNK].literal_clause_pos = NULL;
		literal_info[i][SHRUNK].n_number = 0;
	}
	for (int i = 0; i < 8 * MAX_CLAUSES; i++) {
		changes_stack[i].index_of_clause = 0;
		changes_stack[i].index_of_literal = 0;
	}
	for (int i = 0; i < MAX_VARS; i++) {
		n_changes[i][0] = 0;
		n_changes[i][1] = 0;
	}
	changes_stack_index = 0;
	for (int i = 0; i < MAX_VARS; i++) {
		result[i].value = UNASSIGNED;
	}
	is_contradicted = 0;//是否用冲突的单子句
	conflicting_literal = 0;//用于记录冲突的文字
	n_unit_clause_stack = 0;//全局栈，用于记录栈顶位置
	depth = 0;//DPLL树中，结点的深度
	char c;
	char s[5];
	int x = 1;
	while ((c = fgetc(fp)) == 'c')
		while ((c = fgetc(fp)) != '\n')
			;  //跳过注释部分
	if (feof(fp)) {
		fclose(fp);
		return NULL;
	}
	if (c != 'p') {
		fclose(fp);
		return NO;
	}
	fscanf(fp, "%s%d%d", s, &n_vars, &original_formula_length);
	if (strcmp(s, "cnf")) {
		fclose(fp);
		return NO;
	}
	current_formula_length = original_formula_length;//读取文件的格式、变量数量和子句数量
	clauses = (clause_information*)malloc(sizeof(clause_information) * original_formula_length);//动态分配内存存储子句信息
	for (int i = 0; i < original_formula_length; i++) {//一共有i个子句
		clauses[i].literals = NULL;
		clauses[i].length_current = 0;
		clauses[i].is_clause_satisfied = NO;
		clauses[i].literals_is_assigned = NULL;
		clauses[i].unit_clause_literal = 0;//首先初始化
		while (1) {
			fscanf(fp, "%d", &x);
			if (x == 0) break;//0是分开两个子句的标志，故结束本轮读取；
			clauses[i].literals = (int*)realloc(clauses[i].literals, (++clauses[i].length_current)*(sizeof(int)));
			clauses[i].literals[clauses[i].length_current - 1] = x;//将x添加到literals的末尾
			clauses[i].literals_is_assigned = (int*)realloc(clauses[i].literals_is_assigned, (clauses[i].length_current) * (sizeof(int)));;
			clauses[i].literals_is_assigned[clauses[i].length_current - 1] = UNASSIGNED;
			int t = x > 0 ? SATISFIED : SHRUNK;//t表示变量是正还是负
			int s = abs(x);//s表示变量是哪个

			++literal_info[s][t].n_number;
			literal_info[s][t].literal_clause = (int*)realloc(literal_info[s][t].literal_clause, literal_info[s][t].n_number * (sizeof(int)));
			literal_info[s][t].literal_clause[literal_info[s][t].n_number - 1] = i;
			literal_info[s][t].literal_clause_pos = (int*)realloc(literal_info[s][t].literal_clause_pos, literal_info[s][t].n_number * (sizeof(int)));
			literal_info[s][t].literal_clause_pos[literal_info[s][t].n_number - 1] = clauses[i].length_current - 1;
		}
		clauses[i].length_original = clauses[i].length_current;
		if (clauses[i].length_original > max_clause_len) {
			max_clause_len = clauses[i].length_original;
		}
	}
	fclose(fp);
	return YES;
}
void CheckCNF() {//打印cnf以检查
	for (int i = 0; i < original_formula_length; i++) {
		for (int j = 0; j < clauses[i].length_current - 1; j++) {
			printf("%d∨", clauses[i].literals[j]);
		}
		if (clauses[i].length_current) {
			printf("%d", clauses[i].literals[clauses[i].length_current - 1]);
		}
		putchar('\n');
	}
}