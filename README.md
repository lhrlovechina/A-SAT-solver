# A-SAT-solver

## 1项目简介

这是一个基于DPLL算法的SAT问题求解器。

在操作手册.txt中查看目标程序的使用方法。

## 2SAT问题描述

为了方便描述，首先做出如下约定：对于任一布尔变元x，“x”与其非“¬x”称为文字。对于多个布尔单元，若干个文字的或运算$l_1∨l_2∨…∨l_k$称为子句。只含一个文字的子句称为单子句。不含任何文字的子句称为空子句。对给定的布尔变元集合${x_1, x_2, ..., x_n}$以及相应的子句集合${c_1, c_2, ..., c_m}$，称 $c_1∧c_2∧...∧c_m$为合取范式。

SAT问题一般可描述为：给定布尔变元集合${x_1, x_2, ..., x_n}$以及相应的子句集合${c_1, c_2, ..., c_m}$，对于合取范式（CNF范式）：$F = c_1∧c_2∧...∧c_m$，判定是否存在对每个布尔变元的一组真值赋值使F为真，当为真时（问题是可满足的，SAT），输出对应的变元赋值（一组解）结果。

对于合取范式F和F中的一个文字“x”，这里约定用F|x表示将文字“x”赋值为真后得到的新的合取范式。详细地说，F|x由执行以下步骤获得：

（1）移除所有包含文字“x”的子句。

（2）从所有的子句中移除文字“¬x”。

（3）将“x”于“¬x”标记为已经赋值。

实际上，如果“x”是单子句中唯一的文字，那么F|x就可以表示单子句规则。
