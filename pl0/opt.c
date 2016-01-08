#include <stdio.h>
#include "four_expression.h"
#include "symtable.h"
#define MAX_P 200
#define MAX_T 200
struct point
{	
	int dag_op;
	int dag_src1;
	int dag_src2;
};
struct table
{
	int sym_src;//保存中间结果或者操作数在符号表中的指针
	int to;//结点表中的这个点在dag图中的指向
	int link;//节点共享。（应该保存一个table中的指针）
};
int dag_point_p, dag_table_p;
struct point dag_points[MAX_P];
struct table dag_tables[MAX_T];
void initdag()
{
	dag_point_p = 1;
	dag_table_p = 1;
	for (int i = 1; i < 200; i++)
	{
		dag_points[i].dag_op = 0;
		dag_points[i].dag_src1 = 0;
		dag_points[i].dag_src2 = 0;
		dag_tables[i].link = 0;
		dag_tables[i].sym_src = 0;
		dag_tables[i].to = 0;

	}
}
int creat_dag_point(int type, int a, int b)
{
	dag_points[dag_point_p].dag_op = type;
	dag_points[dag_point_p].dag_src1 = a;
	dag_points[dag_point_p].dag_src2 = b;
	dag_point_p++;
	return dag_point_p - 1;
}
int creat_dag_table(int src, int to, int link)
{
	dag_tables[dag_table_p].sym_src = src;
	dag_tables[dag_table_p].to = to;
	dag_tables[dag_table_p].link = link;
	dag_table_p++;
	return dag_table_p - 1;
}
//去dag图中找一模一样的节点，找到的话返回节点的编号。
int ifexist_in_dagpoints(int op, int a, int b)
{
	for (int i = dag_point_p - 1; i > 0; i--)
	{
		if (op == dag_points[i].dag_op&&a == dag_points[i].dag_src1&&b == dag_points[i].dag_src2)
		{
			return i;
		}
	}
	return 0;
}
//去dag的节点表中找指向一样的节点。
int find_in_dag_table(int src)
{
	for (int i = dag_table_p - 1; i > 0;i--)
	if (src == dag_tables[i].sym_src)
		return i;
	return 0;
}
//去节点表中寻找指向
int point_to_in_dag_table(int to)
{
	for (int i = dag_table_p - 1; i > 0; i--)
	{
		if (to == dag_tables[i].to)
			return i;
	}
	return 0;
}
//get_dag_point -> create_dag_point变量相关，与其他操作符无关，传入符号表指针，返回dag图指针 
//get_dag_point -> create_dag_table

int get_dag_point(int src)
{
	int i;
	if (src == 0) return 0;
	i = find_in_dag_table(src);
	if (i == 0)
	{
		//dag图中建立节点，生成var,在dag图中生成此节点。
		i = creat_dag_point(four_var, 0, 0);
		//在dag表中填入这个节点的名字(src)和dag图中的指向src.
		i = creat_dag_table(src, i, 0);

	}
	return dag_tables[i].to;
}
//推到一条新的四元式子
void new_four_afteropt(int type, int src1, int src2, int des, int lev)
{
	four_codes_afteropt[fourtable_afteropt_p].type = type;
	four_codes_afteropt[fourtable_afteropt_p].src1 = src1;
	four_codes_afteropt[fourtable_afteropt_p].src2 = src2;
	four_codes_afteropt[fourtable_afteropt_p].level = lev;
	four_codes_afteropt[fourtable_afteropt_p].des = des;
	fourtable_afteropt_p++;
}
//建立dag图包括增长节点表和填入新的节点表
void new_dag_point(int type ,int a,int b,int des)
{
	int i, t;
	switch (type)
	{
		case four_add:case four_minus:case four_mul:case four_div:
		case four_big:case four_bige:case four_less:case four_lesse:
		case four_eq:case four_neq:case four_getadd:case four_getarr:
		case four_bec:
			if ((t = ifexist_in_dagpoints(type, a, b)) == 0)
				t = creat_dag_point(type, a, b);
			//返回了节点表的指针
			i = point_to_in_dag_table(t);
			//
			//dag table 中指向t的点已经存在。创建一个返回点des指向dag图中的t，链接到结点表中的i点。消除了公共子表达式，多了一个赋值语句
			if (i)
				creat_dag_table(des, t, i);
			else
				creat_dag_table(des, t, 0);
			break;
		case four_read:
			t = creat_dag_point(type, 0, 0);
			creat_dag_table(des, t, 0);
			break;
		case four_push:
		case four_write:
			t = creat_dag_point(type, a, b);
			//这两条指令不会对存在的值产修改，所以只需要在dag图中记录节点就可以了。
			break;
	}
}
void r_all(int level)
{
	int a, b, c, i, k;
	struct point t;
	for (i = 1; i < dag_point_p; i++)
	{
		t = dag_points[i];
		switch (t.dag_op)
		{
			case four_add:case four_minus:case four_mul:case four_div:
			case four_big:case four_bige:case four_less:case four_lesse:
			case four_eq:case four_neq:case four_getadd:case four_getarr:
			case four_bec:
			//还原到符号表
			a = dag_tables[point_to_in_dag_table(t.dag_src1)].sym_src;
			b = dag_tables[point_to_in_dag_table(t.dag_src2)].sym_src;
			//第i条四元式
			k = point_to_in_dag_table(i);
			c = dag_tables[k].sym_src;
			new_four_afteropt(t.dag_op, a, b, c, level);
			while (dag_tables[k].link != 0)
			{
				k = dag_tables[k].link;
				//取到link指针，拿到一样的值。产生一条赋值语句
				//c是节点表指向符号表的源节点
				a = dag_tables[k].sym_src;
				if (sym_tables[a].name[0] != '_')
					new_four_afteropt(four_bec, c, 0, a, level);
			}
			break;
			case four_read :
				//通过to找到id.
				a = dag_tables[point_to_in_dag_table(i)].sym_src;
				new_four_afteropt(four_read, 0, 0, a, level);
				break;
			case four_push:
			case four_write:
				a = dag_tables[point_to_in_dag_table(t.dag_src1)].sym_src;
				b = t.dag_src2;
				new_four_afteropt(t.dag_op, b, 0, a, level);
				break;



		}
	}
}

void gen_dag(int start, int end, int level)
{
	int i, a, b;
	struct four_expression t;
	initdag();
	for(i = start; i <= end; i++)
	{
		t = four_codes[i];
		switch (t.type)
		{
			case four_add:case four_minus:case four_mul:case four_div:
			case four_big:case four_bige:case four_less:case four_lesse:
			case four_eq:case four_neq:case four_getadd:case four_getarr:
			case four_bec:
				//在dag图中获取或者创建相应的源操作数
				a = get_dag_point(t.src1);
				b = get_dag_point(t.src2);
				//t.src2 is 0 in bec 
				//dag图中新建一个节点
				new_dag_point(t.type, a, b, t.des);
			break;
			case four_read:
				new_dag_point(t.type, 0, 0, t.des);
				break;
			case four_push:
			case four_write:
				a = get_dag_point(t.des);
				b = t.src1;
				// push i,0,t
				//write 0,0,a
				// a is des in the sym
				new_dag_point(t.type, a, b, 0);
				break;
			default:
				if (t.type == four_call || t.type == four_end ||
					t.type == four_jz || t.type == four_jmp)
				{
					r_all(level);
					initdag();
				}
				new_four_afteropt(four_codes[i].type, four_codes[i].src1, four_codes[i].src2, four_codes[i].des, four_codes[i].level);
				break;
		}
	}
	r_all(level);
}