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
	int sym_src;//�����м������߲������ڷ��ű��е�ָ��
	int to;//�����е��������dagͼ�е�ָ��
	int link;//�ڵ㹲����Ӧ�ñ���һ��table�е�ָ�룩
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
//ȥdagͼ����һģһ���Ľڵ㣬�ҵ��Ļ����ؽڵ�ı�š�
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
//ȥdag�Ľڵ������ָ��һ���Ľڵ㡣
int find_in_dag_table(int src)
{
	for (int i = dag_table_p - 1; i > 0;i--)
	if (src == dag_tables[i].sym_src)
		return i;
	return 0;
}
//ȥ�ڵ����Ѱ��ָ��
int point_to_in_dag_table(int to)
{
	for (int i = dag_table_p - 1; i > 0; i--)
	{
		if (to == dag_tables[i].to)
			return i;
	}
	return 0;
}
//get_dag_point -> create_dag_point������أ��������������޹أ�������ű�ָ�룬����dagͼָ�� 
//get_dag_point -> create_dag_table

int get_dag_point(int src)
{
	int i;
	if (src == 0) return 0;
	i = find_in_dag_table(src);
	if (i == 0)
	{
		//dagͼ�н����ڵ㣬����var,��dagͼ�����ɴ˽ڵ㡣
		i = creat_dag_point(four_var, 0, 0);
		//��dag������������ڵ������(src)��dagͼ�е�ָ��src.
		i = creat_dag_table(src, i, 0);

	}
	return dag_tables[i].to;
}
//�Ƶ�һ���µ���Ԫʽ��
void new_four_afteropt(int type, int src1, int src2, int des, int lev)
{
	four_codes_afteropt[fourtable_afteropt_p].type = type;
	four_codes_afteropt[fourtable_afteropt_p].src1 = src1;
	four_codes_afteropt[fourtable_afteropt_p].src2 = src2;
	four_codes_afteropt[fourtable_afteropt_p].level = lev;
	four_codes_afteropt[fourtable_afteropt_p].des = des;
	fourtable_afteropt_p++;
}
//����dagͼ���������ڵ��������µĽڵ��
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
			//�����˽ڵ���ָ��
			i = point_to_in_dag_table(t);
			//
			//dag table ��ָ��t�ĵ��Ѿ����ڡ�����һ�����ص�desָ��dagͼ�е�t�����ӵ������е�i�㡣�����˹����ӱ��ʽ������һ����ֵ���
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
			//������ָ���Դ��ڵ�ֵ���޸ģ�����ֻ��Ҫ��dagͼ�м�¼�ڵ�Ϳ����ˡ�
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
			//��ԭ�����ű�
			a = dag_tables[point_to_in_dag_table(t.dag_src1)].sym_src;
			b = dag_tables[point_to_in_dag_table(t.dag_src2)].sym_src;
			//��i����Ԫʽ
			k = point_to_in_dag_table(i);
			c = dag_tables[k].sym_src;
			new_four_afteropt(t.dag_op, a, b, c, level);
			while (dag_tables[k].link != 0)
			{
				k = dag_tables[k].link;
				//ȡ��linkָ�룬�õ�һ����ֵ������һ����ֵ���
				//c�ǽڵ��ָ����ű��Դ�ڵ�
				a = dag_tables[k].sym_src;
				if (sym_tables[a].name[0] != '_')
					new_four_afteropt(four_bec, c, 0, a, level);
			}
			break;
			case four_read :
				//ͨ��to�ҵ�id.
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
				//��dagͼ�л�ȡ���ߴ�����Ӧ��Դ������
				a = get_dag_point(t.src1);
				b = get_dag_point(t.src2);
				//t.src2 is 0 in bec 
				//dagͼ���½�һ���ڵ�
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