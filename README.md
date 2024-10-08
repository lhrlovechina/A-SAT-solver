# A SAT solver

## 1 项目简介

这是一个基于DPLL算法的SAT问题求解器。

## 2 SAT问题描述

为了方便描述，首先做出如下约定：对于任一布尔变元x，“x”与其非“¬x”称为文字。对于多个布尔单元，若干个文字的或运算$l_1∨l_2∨…∨l_k$称为子句。只含一个文字的子句称为单子句。不含任何文字的子句称为空子句。对给定的布尔变元集合${x_1, x_2, ..., x_n}$以及相应的子句集合${c_1, c_2, ..., c_m}$，称 $c_1∧c_2∧...∧c_m$为合取范式。

SAT问题一般可描述为：给定布尔变元集合${x_1, x_2, ..., x_n}$以及相应的子句集合${c_1, c_2, ..., c_m}$，对于合取范式（CNF范式）：$F = c_1∧c_2∧...∧c_m$，判定是否存在对每个布尔变元的一组真值赋值使F为真，当为真时（问题是可满足的，SAT），输出对应的变元赋值（一组解）结果。

对于合取范式F和F中的一个文字“x”，这里约定用F|x表示将文字“x”赋值为真后得到的新的合取范式。详细地说，F|x由执行以下步骤获得：

（1）移除所有包含文字“x”的子句。

（2）从所有的子句中移除文字“¬x”。

（3）将“x”于“¬x”标记为已经赋值。

实际上，如果“x”是单子句中唯一的文字，那么F|x就可以表示单子句规则。

## 3 代码实现思路：DPLL算法

首先，我们要遍历此范式中的所有子句，判断有无单子句；如果有，则判断是否存在单子句冲突，若存在冲突，则返回UNSAT；反之则给单子句中的布尔变元赋值，并重新开始循环。如果没有单子句，则判断剩下的范式是否为空，若是，则返回SAT；反之则根据变元选取策略选择变元x；给x赋值1和-1，若满足，则返回SAT；反之则返回UNSAT。

在细节上，我注意到了以下几点：我实际上没有把任何子句移除，只是用结构clauses_information中的数据项literals_is_true记录文字是否被赋值；如果同时存在x和非x，则一定不满足；我还利用了栈local_stack来记录单子句的赋值状态。

下面给出DPLL算法的伪代码描述：

```pseudocode
DPLL(){
	while(1) {//单子句传播
		if(存在冲突的单子句){
			while(local_stack不为空){
			local_stack出栈，赋值给v;
			UnValue(v); //将F从F|v后恢复
			}
			return UNSAT；
		}
		else if(存在单子句{v}){
			将v入栈local_stack;
			Value(v); //计算操作F|v
		}
		else break;
	}
	if(F为空) return SAT;
	基于某种策略选取文字v;
	Value(v);
	if(DPLL()=SAT) return SAT;
	UnValue(v);
	Value(-v);
	if(DPLL()=SAT) return SAT;
	UnValue(-v);
	while(local_stack不为空){
		local_stack出栈，赋值给v;
		UnValue(v); //将F从操作F|v后恢复
	}
	return UNSAT
}
```

## 4 变元选择思路

如何选择变元呢？一个直观思路就是哪个变元在子句中出现的次数最多，赋值之后得到UNSAT的概率越大。因此我们记$d_k(F,x)$表示文字x在所有长度为k的子句中出现的次数。记二元函数$Ф(x,y)=(x+1)(y+1)$。

这样，变元选择算法可以描述为：

在所有未赋值的变元中，到一个变元v，使得$Ф(d_s(F,v),d_s(F,-v))$取到最大值。其中，s是合取范式中最短子句的长度。如果$d_s(F,v)≧d_s(F,-v)$，则返回v；否则，返回-v。

算法的伪代码可以描述为：

```pseudocode
Branching(){
	s = 所有未满足子句中的最短子句的长度;
	对每一个变元v{
		x = v在所有长度为s的未满足子句中的总的出现次数;
		y = -v在所有长度为s的未满足子句中的总的出现次数;
		L = (x+1)*(y+1);
	}
	找到使L最大的变元;
	if(x>=y) return v;
	else return -v;
}
```

## 5 输入及输出格式

输入格式为cnf的文件名称，如“unif-c500-v250-s453695930.cnf”；

如果合取范式是不满足的，求解结果按照如下的方式显示：
s 1
t DPLL算法执行时间 DPLL函数递归调用次数
UNSAT!

如果合取范式是可满足的，求解结果按照如下的方式显示：
s 1
v 一组使合取范式为真的赋值
t DPLL算法执行时间 DPLL函数递归调用次数
SAT!
