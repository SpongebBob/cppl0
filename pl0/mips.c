#define _CRT_SECURE_NO_WARNINGS

#include "four_expression.h"
#include "symtable.h"
#include "mips.h"
#include <stdio.h>

FILE *file_out;
int use[MAXREG];
int get_memory(int i, int nowlevel, int *base);
struct four_expression *four_codes_to_mips;
int use_opt = 0;

int get_REG()
{
	int i;
	for (i = 0; i < MAXREG; i++)
	{
		if (!use[i])
		{
			use[i] = 1;
			return i;
		}
	}
}
void free_reg(int i)
{
	use[i] = 0;
}
int move_into_reg(int i, int nowlevel)
{
	int base, a, b;
	b = get_REG();
	if (sym_tables[i].kind == k_const)
	{
		fprintf(file_out, "\t li,%s,%d\n", r_name[b], sym_tables[i].x);
	}
	else
	{
		//a 是相当于fp的偏移量，找到在运行栈中的保存位置
		a = get_memory(i, nowlevel, &base);
		fprintf(file_out, "\t lw,%s,%d(%s)\n", r_name[b], a, r_name[base]);
		free_reg(base);
	}
	if (sym_tables[i].kind == k_point)
	{
		fprintf(file_out, "\t lw,%s,0(%s)\n", r_name[b], r_name[b]);
	}
	return b;

}
int get_memory(int i, int nowlevel, int *base)
{
	int d_level, a, j;
	d_level = nowlevel - sym_tables[i].level;
	a = get_REG();
	*base = a;
	fprintf(file_out, "\t add,%s,%s,$0\n", r_name[a], r_name[r_fp]);
	if (sym_tables[i].kind == k_func || sym_tables[i].kind == k_proc)
		d_level--;
	for (j = 0; j < d_level; j++)
	{
		fprintf(file_out, "\t lw,%s,-4(%s)\n", r_name[a], r_name[a]);
	}
	if (sym_tables[i].kind == k_func)
		return -OFFSET;
	return -(sym_tables[i].mem + OFFSET);
}
void move_into_memory(int reg, int i, int nowlevel)
{
	int base, a, b;
	a = get_memory(i, nowlevel, &base);
	if (sym_tables[i].kind == k_func)
	{
		fprintf(file_out, "\t sw,%s,-16(%s)\n", r_name[reg], r_name[base]);
	}
	else if (sym_tables[i].kind == k_point)
	{
		b = get_REG();
		// a = offeset ,base = $fp
		fprintf(file_out, "\t lw,%s,%d(%s)\n", r_name[b], a, r_name[base]);
		fprintf(file_out, "\t sw,%s,0(%s)\n", r_name[reg], r_name[b]);
		free_reg(b);
	}
	else
	{
		fprintf(file_out, "\t sw,%s,%d(%s)\n", r_name[reg], a, r_name[base]);
	}
	free_reg(base);

}

void set_string()
{
	int i;
	struct sym_table t;
	fprintf(file_out, "\t.data\n");
	for (i = 1; i < fourtable_p; i++)
	{
		if (four_codes_to_mips[i].type == four_write
			&&sym_tables[four_codes_to_mips[i].des].type == t_string)
		{
			t = sym_tables[four_codes_to_mips[i].des];
			fprintf(file_out, "_str%d:\t.asciiz\t\"%s\"\n", t.x, t.name);
		}
	}
}
void init_memory()
{
	int i, last, temp;
	for (i = 1; i < symtable_p; i++)
	{
		last = sym_tables[i].last;
		if (sym_tables[last].level != sym_tables[i].level)
		{
			//跨越层次的起始地址为0
			sym_tables[i].mem = 0;
		}
		else
			sym_tables[i].mem = sym_tables[last].mem;
		if (sym_tables[i].kind == k_var || sym_tables[i].kind == k_point)
		{
			if (sym_tables[i].x == 0)
			{
				sym_tables[i].mem += 4;
			}
			else
			{
				sym_tables[i].mem += 4 * sym_tables[i].x;
			}
		}
		temp = i;
		while (sym_tables[i].level == sym_tables[temp].level)
		{
			//找到当前层次第一个节点的last，直接浮层节点
			temp = sym_tables[temp].last;
		}
		//最后一个的size保存上一个的最后一个mem
		sym_tables[temp].size = sym_tables[i].mem;

	}
	printf("\nSYM TABLE:\n");
	for (i = 1; i < symtable_p; i++)
	{
		printf("%d\t%s\t%d\t%dL:%d,type:%d\n", i, sym_tables[i].name, sym_tables[i].x, sym_tables[i].mem, sym_tables[i].level,sym_tables[i].type);
	}

}
void init_mips()
{
	if (use_opt == 1) four_codes_to_mips = four_codes_afteropt;
	else four_codes_to_mips = four_codes;
	use[r_zero] = 1;
	use[r_at] = 1;
	use[r_V0] = 1;
	use[r_V1] = 1;
	use[r_a0] = 1;
	use[r_a1] = 1;
	use[r_a2] = 1;
	use[r_a3] = 1;
	use[r_gp] = 1;
	use[r_fp] = 1;
	use[r_sp] = 1;
	use[r_ra] = 1;
	file_out = fopen("result.asm", "w");
	//file_out_opt = fopen("result_opt.asm", "w");
	set_string();
	init_memory();
}

void out_one_mpis(struct four_expression t)
{
	int a, b, c, d, base;
#ifdef DEBUG
	printf("t.type:%d", t.type);
#endif
	switch (t.type)
	{
		
	case four_add:
	case four_minus:
	case four_mul:
	case four_div:
	case four_big:
	case four_bige:
	case four_less:
	case four_lesse:
	case four_eq:
	case four_neq:
		a = move_into_reg(t.src1, t.level);
		b = move_into_reg(t.src2, t.level);
		c = get_REG();
		switch (t.type)
		{
		case four_add:
			fprintf(file_out, "\t add,%s,%s,%s\n", r_name[c], r_name[a], r_name[b]);
			break;
		case four_minus:
			fprintf(file_out, "\t sub,%s,%s,%s\n", r_name[c], r_name[a], r_name[b]);
			break;
		case four_mul:
			fprintf(file_out, "\t mult,%s,%s\n", r_name[a], r_name[b]);
			fprintf(file_out, "\t mflo,%s\n", r_name[c]);
			break;
		case four_div:
			fprintf(file_out, "\t div,%s,%s\n", r_name[a], r_name[b]);
			fprintf(file_out, "\t mflo,%s\n", r_name[c]);
			break;
		case four_big:
			//>
			fprintf(file_out, "\t sgt,%s,%s,%s\n", r_name[c], r_name[a], r_name[b]);
			break;
		case four_bige:
			//>=
			fprintf(file_out, "\t sge,%s,%s,%s\n", r_name[c], r_name[a], r_name[b]);
			break;
		case four_less:
			//小于则置1
			fprintf(file_out, "\t slt,%s,%s,%s\n", r_name[c], r_name[a], r_name[b]);
			break;
		case four_lesse:
			//小于等于则置1
			fprintf(file_out, "\t sle,%s,%s,%s\n", r_name[c], r_name[a], r_name[b]);
			break;
		case four_eq:
			//seq 相等置1
			fprintf(file_out, "\t seq,%s,%s,%s\n", r_name[c], r_name[a], r_name[b]);
			break;
		case four_neq:
			fprintf(file_out, "\t sne,%s,%s,%s\n", r_name[c], r_name[a], r_name[b]);
			break;
		}
		d = get_memory(t.des, t.level, &base);
		fprintf(file_out, "\t sw,%s,%d(%s)\n", r_name[c], d, r_name[base]);
		free_reg(a);
		free_reg(b);
		free_reg(c);
		free_reg(base);
		break;
	case four_jmp:
		fprintf(file_out, "\t b,$L%d\n", four_codes_to_mips[t.des].des);
		break;
	case four_jz:
		a = move_into_reg(t.src1, t.level);
		//==0 就跳到Lable
		fprintf(file_out, "\t beqz,%s,$L%d\n", r_name[a], four_codes_to_mips[t.des].des);
		free_reg(a);
		break;
	case four_bec:
		if (sym_tables[t.des].x == 0
			|| sym_tables[t.des].kind == k_func
			|| sym_tables[t.des].kind == k_point)
		{
			a = move_into_reg(t.src1, t.level);
			move_into_memory(a, t.des, t.level);
			free_reg(a);
		}
		else
		{
			a = get_memory(t.des, t.level, &base);
			d = get_REG();
			b = move_into_reg(t.src2, t.level);
			c = get_REG();
			fprintf(file_out, "\t addi,%s,%s,%d\n", r_name[d], r_name[base], a);
			fprintf(file_out, "\t li,%s,%d\n", r_name[c], 4);
			fprintf(file_out, "\t mult,%s,%s\n", r_name[c], r_name[b]);
			fprintf(file_out, "\t mflo,%s\n", r_name[c]);
			fprintf(file_out, "\t add,%s,%s,%s\n", r_name[d], r_name[c], r_name[d]);
			a = move_into_reg(t.src1, t.level);
			fprintf(file_out, "\t sw,%s,0(%s)\n", r_name[a], r_name[d]);
			free_reg(a);
			free_reg(b);
			free_reg(c);
			free_reg(d);
			free_reg(base);
		}
		break;
	case four_call:
		a = get_REG();
	//there is 
		if (sym_tables[t.src1].level == t.level)
			fprintf(file_out, "\t add,%s,$fp,$0\n", r_name[a]);
		else
		{
			//如何解决递归调用，调用的不是本层的方法。
			//base存放着调用的基地址，
			get_memory(t.src1, t.level, &base);
			fprintf(file_out, "\t lw,%s,-4(%s)\n", r_name[a], r_name[base]);
			free_reg(base);
		}
		//保存当前$a = $fp8/4
		//-8前一个帧，-4调用目标所在帧
		fprintf(file_out, "\t sw,$fp,-8($sp)\n");
		fprintf(file_out, "\t sw,%s,-4($sp)\n", r_name[a]);
		fprintf(file_out, "\t sw,%s,-12($sp)\n", r_name[r_ra]);
		fprintf(file_out, "\t sw,%s,-16($sp)\n", r_name[r_V0]);
		fprintf(file_out, "\t add,$fp,$sp,$0\n");
		//here the name
		fprintf(file_out, "\t jal,_%s\n", sym_tables[t.src1].name);


		fprintf(file_out, "\t lw,$fp,-8($sp)\n");
		fprintf(file_out, "\t lw,%s,-12($sp)\n", r_name[r_ra]);
		if (t.des != 0)
		{
			b = get_memory(t.des, t.level, &base);
			//返回值不为空，保存到des的内存对应的位置
			fprintf(file_out, "\t sw,$v0,%d(%s)\n", b, r_name[base]);
			free_reg(base);
		}
		free_reg(a);
		fprintf(file_out, "\t lw,%s,-16($sp)\n", r_name[r_V0]);
		break;
	case four_read:
		//系统调用与$a0寄存器
		//$v0是系统调用的类型
		if (sym_tables[t.des].type == t_integer)
			fprintf(file_out, "\t li,$v0,5\n");
		else
			fprintf(file_out, "\t li,$v0,12\n");
		fprintf(file_out, "\t syscall\n");
		a = get_REG();
		fprintf(file_out, "\t add,%s,$v0,$0\n", r_name[a]);
		move_into_memory(a, t.des, t.level);
		free_reg(a);
		break;
	case four_write:
		//$a0 = integer or character to print	
		//$a0 = address of null-terminated string to print	
		if (sym_tables[t.des].kind == k_func||(sym_tables[t.des].kind == k_var
			&&sym_tables[t.des].type == t_integer)
			|| (sym_tables[t.des].kind == k_const
			&&sym_tables[t.des].type == t_integer))
		{
			a = move_into_reg(t.des, t.level);
			fprintf(file_out, "\t li,$v0,1\n");
			fprintf(file_out, "\t add,$a0,%s,$0\n", r_name[a]);
			fprintf(file_out, "\t syscall\n");
			free_reg(a);
		}
		else if (sym_tables[t.des].type == t_char)
		{
			a = move_into_reg(t.des, t.level);
			//write('a') ->this is a 
			fprintf(file_out, "\t li,$v0,4\n");
			fprintf(file_out, "\t sw,%s,-4($sp)\n", r_name[a]);
			//sp指针的位置
			fprintf(file_out, "\t addi,$a0,$sp,-4\n");
			fprintf(file_out, "\t syscall\n");
		}
		else
		{
			//print string
			fprintf(file_out, "\t li,$v0,4\n");
			fprintf(file_out, "\t la,$a0,_str%d\n", sym_tables[t.des].x);
			fprintf(file_out, "\t syscall\n");
		}
		break;
	case four_push:
		b = move_into_reg(t.des, t.level);
		//cover the num
		fprintf(file_out, "\t sw,%s,%d($sp)\n", r_name[b], -(t.src1 * 4 + OFFSET));
		free_reg(b);
		break;
	case four_end:
		if (sym_tables[t.des].kind == k_func)
		{
			a = move_into_reg(t.des, t.level);
			fprintf(file_out, "\t add,$v0,%s,$0\n", r_name[a]);
		}
		fprintf(file_out, "\t addi,$sp,$sp,%d\n", (OFFSET + sym_tables[t.des].size));
		if (t.des)
			fprintf(file_out, "\t jr,$ra\n");
		break;
	case four_getarr:
		a = get_memory(t.src1, t.level, &base);
		b = move_into_reg(t.src2, t.level);
		c = get_REG();
		d = get_REG();
		// r[d] = address src1
		fprintf(file_out, "\t addi,%s,%s,%d\n", r_name[d], r_name[base], a);
		fprintf(file_out, "\t li,%s,%d\n", r_name[c], 4);
		fprintf(file_out, "\t mult,%s,%s\n",r_name[c],r_name[b]);
		fprintf(file_out, "\t mflo,%s\n",r_name[c]);
		// c = srd2 *4  d = src1 +c
		fprintf(file_out, "\t add,%s,%s,%s\n",r_name[d],r_name[c],r_name[d]);
		fprintf(file_out, "\t lw,%s,0(%s)\n",r_name[c],r_name[d]);
		//reg[c] -> t.des
		move_into_memory(c, t.des, t.level);
		free_reg(b);
		free_reg(c);
		free_reg(d);
		free_reg(base);
		break;
	case four_getadd:
		if (t.src2)
		{
			a = get_memory(t.src1, t.level, &base);
			b = move_into_reg(t.src2, t.level);
			c = get_REG();
			d = get_REG();
			fprintf(file_out, "\t addi,%s,%s,%d\n", r_name[d], r_name[base], a);
			fprintf(file_out, "\t li,%s,%d\n", r_name[c], 4);
			fprintf(file_out, "\t mult,%s,%s\n", r_name[c], r_name[b]);
			fprintf(file_out, "\t mflo,%s\n", r_name[c]);
			// c = srd2 *4  d = src1 +c
			fprintf(file_out, "\t add,%s,%s,%s\n", r_name[d], r_name[c], r_name[d]);
			move_into_memory(d, t.des, t.level);
			free_reg(b);
			free_reg(c);
			free_reg(d);
			free_reg(base);
		}
		else
		{
			a = get_memory(t.src1, t.level, &base);
			b = get_REG();
			//r[b] = addr(t.src1)
			fprintf(file_out, "\t addi,%s,%s,%d\n", r_name[b], r_name[base], a);
			move_into_memory(b, t.des, t.level);
			free_reg(b);
			free_reg(base);
		}
		break;
	case four_lable :
		//L_NUM:
		fprintf(file_out, "$L%d:\n", t.des);
		break;
	case four_enter:
		fprintf(file_out, "_%s:\n", sym_tables[t.des].name);
		fprintf(file_out, "\t addi,$sp,$sp,%d\n", -(OFFSET + sym_tables[t.des].size));
		break;
	default:
		printf("four:%d\n", t.type);
		my_error(10001);//what the fuck
	}

}
void out_all_mips()
{
	int i;
	

	fprintf(file_out,"\t.text\n");
	fprintf(file_out, "main:\n");
	fprintf(file_out, "\t add,$fp,$sp,$0\n");
	fprintf(file_out, "\t sw,$fp,-8($sp)\n");
	fprintf(file_out, "\t sw,$fp,-4($sp)\n");
	fprintf(file_out, "\t b,_main\n");
	for (i = 1; i < fourtable_p;i++)
	{
		out_one_mpis(four_codes_to_mips[i]);
	}
	fprintf(file_out, "\t li,$v0,10\n");
	fprintf(file_out, "\t syscall\n");
	fclose(file_out);
}