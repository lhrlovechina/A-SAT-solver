#include "ph.h"
#include "cnfparser.hpp"
#include "solver.hpp"
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <bits/stdc++.h>

using namespace std;
double t;//用来记录运行时间

void PrintToScreen(int value,double time) {
	if (value == SAT) {
		printf("s 1\n");
		printf( "v");
		for (int i = 1; i <= n_vars; i++) {
			if (result[i].value == TRUE) {
				printf(" %d", i);
			}
			else {
				printf( " -%d", i);
			}
		}
		printf( "\n");
		printf("t %.2f %d\n", time, dpll_called);
		printf("SAT!\n");
	}
	else if (value == UNSAT) {
		printf("s 0\n");
		printf("t %.2f %d\n", time, dpll_called);
		printf("UNSAT!\n");
	}
}

int main(){
    char filename[100];//文件名
	scanf("%s", filename);
	FILE* fp = fopen(filename, "r");//输入文件名
	if (fp == NULL) {
		printf("文件打开失败\n");
	}
	int is_cnf = CNFparser(fp);//创建新的内部结构
	if (is_cnf == NO) {
		printf("文件格式不正确\n");
	}
	//CheckCNF();//将解析完的公式打印出来
	clock_t starttime, endtime;
	starttime = clock();
	Preprocesser();//预处理
	int value =dpll();
	endtime = clock();
	t = (double)(endtime - starttime) * 1000 / CLOCKS_PER_SEC;//时间的单位为毫秒
	PrintToScreen(value,t);//打印结果
	Free();

    return 0;
}