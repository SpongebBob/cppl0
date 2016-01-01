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
	int sym_src;
	int to;
	int link;
};
int dag_point_p, dag_table_p;
struct point dag_points[MAX_P];
struct table dag_tables[MAX_T];
void initdag()
{
	dag_point_p = 1;
	dag_table_p = 1;
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
	for (int i = dag_table_p - 1; i > 0; i--)
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
	if (src == dag_tables[dag_table_p].sym_src)
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
int get_dag_point(int src)
{
	int i;
	if (src == 0) return 0;
	if (i = find_in_dag_table(src) == 0)
	{
		//dag图中建立节点，生成var
		i = creat_dag_point(four_var, 0, 0);
		//在dag表中填入src,dag图中的指向
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
			i = point_to_in_dag_table(t);
			//dag table 中指向t的点
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
			a = dag_tables[point_to_in_dag_table(t.dag_src1)].sym_src;
			b = dag_tables[point_to_in_dag_table(t.dag_src2)].sym_src;
			//第i条四元式
			k = point_to_in_dag_table(i);
			c = dag_tables[k].sym_src;
			new_four_afteropt(t.dag_op, a, b, c, level);
			while (dag_tables[k].link != 0)
			{
				k = dag_tables[k].link;
				a = dag_tables[k].sym_src;
				if (sym_tables[a].name[0] != '_')
					new_four_afteropt(four_bec, c, 0, a, level);
			}
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
				a = get_dag_point(t.src1);
				b = get_dag_point(t.src2);
				//t.src2 is 0 in bec 
				new_dag_point(t.type, a, b, t.des);
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