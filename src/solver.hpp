#include "ph.h"
#include "cnfparser.hpp"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

extern int is_contradicted; // 是否用冲突的单子句
extern int conflicting_literal; // 用于记录冲突的文字
extern int unit_clause_stack[MAX_CLAUSES]; // 单子句规则
extern int n_unit_clause_stack; // 全局栈，用于记录栈顶位置
extern int depth; // DPLL树中，结点的深度
extern int n_vars; // 变元总个数
extern int dpll_called; // 统计DPLL执行次数
extern int max_clause_len; // 最大字句长度
extern literal_information literal_info[MAX_VARS][2]; // 文字存储数组
extern clause_information* clauses; // 子句存储数组

extern int original_formula_length; // 原长度
extern int current_formula_length; // 现长度
extern changes_information changes_stack[8 * MAX_CLAUSES]; // 记录
extern unsigned int changes_stack_index; // 数组changes的下标
extern unsigned int n_changes[MAX_VARS][2]; // 某一层满足或不满足的句子的个数
extern assign_information result[MAX_VARS]; // 用于回溯操作的记录

//预处理，将单子句加入全局变量
void Preprocesser() {
	for (int i = 0; i < original_formula_length; i++) {
		if (clauses[i].length_original == 1) {//将长度为一的变量加入全局栈
			unit_clause_stack[n_unit_clause_stack] = clauses[i].literals[0];
			++n_unit_clause_stack;
		}
	}
}
//设置子句
void Value(int v) {
	int i;
	int p = abs(v), q = (v > 0) ? SATISFIED : SHRUNK;
	for (i = 0; i < literal_info[p][q].n_number; ++i) {
		int j = literal_info[p][q].literal_clause[i];
		if (clauses[j].is_clause_satisfied) continue;//将j设为满足的
		clauses[j].is_clause_satisfied = YES;//设置为满足后，不改变句子长度，直到回溯后
		--current_formula_length;//更改子句的计数器
		changes_stack[changes_stack_index++].index_of_clause = j;//一个子句被设置为满足，那么记录这个子句的序号
		n_changes[depth][SATISFIED]++;//depth层++
	}
	q = !q;
	for (i = 0; i < literal_info[p][q].n_number; ++i) {
		int j = literal_info[p][q].literal_clause[i];//记录字句序号
		if (clauses[j].is_clause_satisfied) continue;
		int k = literal_info[p][q].literal_clause_pos[i];//记录文字位置
		--clauses[j].length_current;//改变字句长度
		clauses[j].literals_is_assigned[k] = ASSIGNED;//用表示子句中各个文字是否被赋值
		changes_stack[changes_stack_index].index_of_clause = j;//记录改变
		changes_stack[changes_stack_index++].index_of_literal = k;//
		n_changes[depth][SHRUNK]++;
		if (clauses[j].length_current == 1) {//检查字句长度，判断是否为单子句
			int location = -1;
			for (int i_literal = 0; i_literal < clauses[j].length_original; i_literal++) {
				if (clauses[j].literals_is_assigned[i_literal] == UNASSIGNED) {
					location = i_literal;
					break;
				}
			}
			int w = clauses[j].literals[location];//单子句中剩余的文字
			int s = abs(w), t = (w > 0) ? SATISFIED : SHRUNK;
			if (literal_info[s][(!t)].is_in_unit_clause == YES) {//记录冲突
				is_contradicted = TRUE;
				conflicting_literal = w;
			}
			else if (literal_info[s][t].is_in_unit_clause == NO) {//没有冲突,把这个单子句放入全局变量
				unit_clause_stack[n_unit_clause_stack] = clauses[j].unit_clause_literal = w;
				literal_info[s][t].is_in_unit_clause = YES;
				++n_unit_clause_stack;
			}
		}
	}
	literal_info[p][SHRUNK].is_assigned = YES;
	literal_info[p][SATISFIED].is_assigned = YES;
	++depth;
}
//取消设置
void UnValue(int v){
	int i;
	int p = abs(v), q = (v > 0) ? SATISFIED : SHRUNK;
	--depth;
	while (n_changes[depth][SHRUNK]){//还有没从stack中取出来的
		--n_changes[depth][SHRUNK];//
		int j = changes_stack[--changes_stack_index].index_of_clause;
		int k = changes_stack[changes_stack_index].index_of_literal;
		++clauses[j].length_current;//回复子句的长度
		if (clauses[j].length_current == 2) {//将被设置为在单子句中出现的文字设为未出现
			int s = abs(clauses[j].unit_clause_literal);
			int t = (clauses[j].unit_clause_literal > 0) ? SATISFIED : SHRUNK;
			literal_info[s][t].is_in_unit_clause = NO;
			clauses[j].unit_clause_literal = 0;
		}
		clauses[j].literals_is_assigned[k] = UNASSIGNED;////
	}
	while (n_changes[depth][SATISFIED])	{
		--n_changes[depth][SATISFIED];
		int j = changes_stack[--changes_stack_index].index_of_clause;
		clauses[j].is_clause_satisfied = NO;
		++current_formula_length;
	}
	literal_info[p][SATISFIED].is_assigned = NO;
	literal_info[p][SHRUNK].is_assigned = NO;
}

inline int get_length_of() {
	int i, s,type, C, min = max_clause_len;
	if (min == 2) return min;//如果最长子句的长度为2，那么min同样为
	for (i = 1; i <= n_vars; ++i) {
		if (result[i].value == UNASSIGNED) {
			for (type = 0; type < 2; ++type) {
				for (s = 0; s < literal_info[i][type].n_number; ++s) {
					C = literal_info[i][type].literal_clause[s];//每一个文字，在每一个句子中的位置
					if (!clauses[C].is_clause_satisfied &&
						clauses[C].length_current < min) {
						min = clauses[C].length_current;
						if (min == 2) return 2;
					}
				}
			}
		}
	}
	return min;
}
void get_weight(int x, int k, unsigned int& s, unsigned int& t) {
	int j, c;
	s = t = 0;
	for (j = 0; j < literal_info[x][SATISFIED].n_number; ++j) {
		c = literal_info[x][SATISFIED].literal_clause[j];//子句序号
		if (clauses[c].length_current == k)
			s += 1 - clauses[c].is_clause_satisfied;
	}
	for (j = 0; j < literal_info[x][SHRUNK].n_number; ++j) {// 子句序号
		c = literal_info[x][SHRUNK].literal_clause[j];
		if (clauses[c].length_current == k)
			t += 1 - clauses[c].is_clause_satisfied;
	}
}
inline int GetMinLenOfLiteral() {
	unsigned int i, k;
	unsigned int max = 0, r, s, t;
	int u;
	for (i = 1; i <= n_vars; ++i) {
		if (result[i].value == UNASSIGNED) {
			k = get_length_of();//最短子句
			get_weight(i, k, s, t);//i 变元 k  长度
			r = (s + 1) * (t + 1);
			if (r > max) {
				max = r;
				if (s >= t) u = i;
				else u = -(int)i;
			}
		}
	}
	return u;
}
//DPLL递归算法
int dpll() {
	++dpll_called;//用于输出调试信息
	int* local_stack = NULL;
	unsigned int n_local_stack = 0;
	while (true) {
		if (is_contradicted) {//return
			int cl = abs(conflicting_literal);
			while (n_local_stack) {
				UnValue(local_stack[--n_local_stack]);
				int s = abs(local_stack[n_local_stack]);
				int t = local_stack[n_local_stack] > 0 ? TRUE : FALSE;
				result[s].value = UNASSIGNED;
			}
			is_contradicted = FALSE;
			free(local_stack);
			n_unit_clause_stack = 0;
			return UNSAT;
		}
		else if (n_unit_clause_stack) {//
			local_stack = (int*)realloc(local_stack, (n_local_stack + 1) * sizeof(int));
			int implied_lit = unit_clause_stack[--n_unit_clause_stack];//将要进行单子句规则的文字
			local_stack[n_local_stack++] = implied_lit;
			result[abs(implied_lit)].value = implied_lit > 0 ? TRUE : FALSE;
			Value(implied_lit);
		}
		else break;
	}
	if (!current_formula_length) return SAT;//如果当前子句数为0,则返回SAT
	int v = GetMinLenOfLiteral();//变元选取策略
	result[abs(v)].value = v > 0 ? TRUE : FALSE;
	Value(v);
	if (dpll()) return SAT;
	UnValue(v);
	int i, j, k, m ;
	result[abs(v)].value = !result[abs(v)].value;
	Value(-v);
	if (dpll()) return SAT;
	UnValue(-v);
	result[abs(v)].value = UNASSIGNED;
	while (n_local_stack) {
		int z = local_stack[--n_local_stack];
		UnValue(z);
		result[abs(z)].value = UNASSIGNED;
	}
	free(local_stack);
	is_contradicted = FALSE;
	return UNSAT;
}
//释放分配的内存
void Free() {
	for (int i = 1; i <= n_vars; i++) {
		if (literal_info[i][0].literal_clause) {
			free(literal_info[i][0].literal_clause);
			literal_info[i][0].literal_clause = NULL;
		}
		if (literal_info[i][0].literal_clause_pos) {
			free(literal_info[i][0].literal_clause_pos);
			literal_info[i][0].literal_clause_pos = NULL;
		}
		if (literal_info[i][1].literal_clause) {
			free(literal_info[i][0].literal_clause);
			literal_info[i][0].literal_clause_pos = NULL;
		}
		if (literal_info[i][1].literal_clause_pos) {
			free(literal_info[i][0].literal_clause_pos);
			literal_info[i][0].literal_clause_pos = NULL;
		}
	}
	for (int i = 0; i < original_formula_length; i++) {
		if (clauses[i].literals) {
			free(clauses[i].literals);
		}
		if (clauses[i].literals_is_assigned) {
			free(clauses[i].literals_is_assigned);
		}
	}
	free(clauses);
}