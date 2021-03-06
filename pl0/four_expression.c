//
//  four_expression.c
//  Compiler
//
//  Created by MacBook on 15/11/21.
//  Copyright © 2015年 MacBook. All rights reserved.
//

#include "four_expression.h"
#include "symtable.h"
#include <stdio.h>


extern int nowlevel;
struct four_expression four_codes[MAXFOURCODE];
struct four_expression four_codes_afteropt[MAXFOURCODE];
struct basic_block basic_blocks[200];
int fourtable_p,four_lable_p,fourtable_afteropt_p=1,basic_blockp;
//计算后继集合
void calc_aftercome()
{
	int i ,t,p,q;
	
	for (i = 0; i < basic_blockp; i++)
	{
		for (t = basic_blocks[i].start; t <= basic_blocks[i].end; t++)
		{
			if (four_codes_afteropt[t].type == four_add || four_codes_afteropt[t].type == four_minus
				|| four_codes_afteropt[t].type == four_div || four_codes_afteropt[t].type == four_mul
				|| four_codes_afteropt[t].type == four_big || four_codes_afteropt[t].type == four_bige
				|| four_codes_afteropt[t].type == four_less || four_codes_afteropt[t].type == four_lesse
				|| four_codes_afteropt[t].type == four_eq || four_codes_afteropt[t].type == four_neq
				|| four_codes_afteropt[t].type == four_bec)
				basic_blocks[i].def[basic_blocks[i].defp++] = four_codes_afteropt[t].des;

			if (four_codes_afteropt[t].type == four_jz)
			{
				basic_blocks[i].houji[0] = i+1;
				for (p = 0; p < fourtable_afteropt_p; p++)
				{
					//four_codes[a.des].des
					if (four_codes_afteropt[p].des == four_codes_afteropt[four_codes_afteropt[t].des].des && four_codes_afteropt[p].type == four_lable)
					{
						//找到p所在的基本块
						for (q = 0; q < basic_blockp; q++)
						{
							if (basic_blocks[q].start == p)
								basic_blocks[i].houji[1] = q;
							basic_blocks[i].houji[0] = i+1;
								
						}
					}
				}
			}
			if (four_codes_afteropt[t].type == four_jmp)
			{
				for (p = 0; p < fourtable_afteropt_p; p++)
				{
					if (four_codes_afteropt[p].des == four_codes_afteropt[four_codes_afteropt[t].des].des&&four_codes_afteropt[p].type == four_lable)
					{
						for (q = 0; q < basic_blockp; q++)
						{
							if (basic_blocks[q].start == p)
							   basic_blocks[i].houji[0] = q;
						}
					}
				}

			}

		}
	}
}
void calc_defuse()
{
	int i, t;
	for (i = 0; i < basic_blockp; i++)
	{
		for (t = basic_blocks[i].start; t <= basic_blocks[i].end; t++)
		{
			switch (four_codes_afteropt[t].type)
			{
				case four_jz:
					if (sym_tables[four_codes_afteropt[t].src1].name[0] != '_')
						basic_blocks[i].use[four_codes_afteropt[t].src1] = 1;
				break;
				case four_add:
				case four_minus:
				case four_mul :
				case  four_div:
				case  four_big:
				case  four_less:
				case  four_bige:
				case  four_lesse:
				case  four_eq:
				case  four_neq:
					if (sym_tables[four_codes_afteropt[t].src1].name[0] != '_')
						basic_blocks[i].use[four_codes_afteropt[t].src1] = 1;
					if (sym_tables[four_codes_afteropt[t].src2].name[0] != '_')
						basic_blocks[i].use[four_codes_afteropt[t].src2] = 1;
					if (sym_tables[four_codes_afteropt[t].des].name[0] != '_'&&four_codes_afteropt[t].des != four_codes_afteropt[t].src1)
						basic_blocks[i].def[four_codes_afteropt[t].des] = 1;
				break;
				case four_read:
				case four_write:
				case four_push:
					if (sym_tables[four_codes_afteropt[t].des].name[0] != '_')
						basic_blocks[i].use[four_codes_afteropt[t].des] = 1;
				break;
				case four_getadd:
					if (sym_tables[four_codes_afteropt[t].src1].name[0] != '_')
						basic_blocks[i].use[four_codes_afteropt[t].src1] = 1;
				break;
				case four_getarr:
					if (sym_tables[four_codes_afteropt[t].src2].name[0] != '_')
						basic_blocks[i].use[four_codes_afteropt[t].src2] = 1;
				break;
				case four_bec:
					if (four_codes_afteropt[t].des != four_codes_afteropt[t].src1)
					{
						if (sym_tables[four_codes_afteropt[t].des].name[0] != '_')
							basic_blocks[i].def[four_codes_afteropt[t].des] = 1;
						if (sym_tables[four_codes_afteropt[t].src1].name[0] != '_')
							basic_blocks[i].use[four_codes_afteropt[t].src1] = 1;
					}
				break;
			}
		}
	}
}
void calc_inout()
{
	int i,t,p;
	int m = 0;
	while (m < 5)
	{
		for (i = basic_blockp; i >= 0; i--)
		{
			if (basic_blocks[i].houji[0] != 0)
			{
				t = basic_blocks[i].houji[0];
				for (p = 0; p < 101; p++)
				{
					if (basic_blocks[t].in[p] != 0)
						basic_blocks[i].out[p] = 1;
				}
			}
			if (basic_blocks[i].houji[1] != 0)
			{
				t = basic_blocks[i].houji[1];
				for (p = 0; p < 101; p++)
				{
					if (basic_blocks[t].in[p] != 0)
						basic_blocks[i].out[p] = 1;
				}
			}
			for (p = 0; p < 101; p++)
			{
				if (basic_blocks[i].use[p] != 0)
					basic_blocks[i].in[p] = 1;
				if (basic_blocks[i].out[p] == 1 && basic_blocks[i].def[p] == 0)
					basic_blocks[i].in[p] = 1;
			}
		}
		m++;
	}
	
}
void print_in()
{
	int i,p;
	printf("\nin:\n");
	for (i = basic_blockp-1; i >= 0; i--)
	{
		printf("\nBlock[%d] in:", i);
		for (p = 0; p < 101; p++)
		{
			if (basic_blocks[i].in[p] == 1 && sym_tables[p].type != t_string)
				printf(" %s", sym_tables[p].name);
		}
		printf("\n");
	}

}
void divide_block()
{
	int i, t;
	for (i = 1; i < fourtable_p; i++)
	{
		if (four_codes[i].type == four_lable || four_codes[i].type == four_enter)
		{
			if (basic_blocks[basic_blockp].start != 0)
			{
				basic_blocks[basic_blockp].end = i - 1;
				basic_blockp++;
			}
			basic_blocks[basic_blockp].start = i;
		}
		if (four_codes[i].type == four_jmp || four_codes[i].type == four_jz)
		{
			if (basic_blocks[basic_blockp].start != 0)
			{
				basic_blocks[basic_blockp].end = i ;
				basic_blockp++;
				basic_blocks[basic_blockp].start = i + 1;
			}
		}
		if (four_codes[i].type == four_end)
		{

			if (basic_blocks[basic_blockp].start != 0)
			{
				basic_blocks[basic_blockp].end = i;
				basic_blockp++;
			}
		}
	}
	if (basic_blocks[basic_blockp].start != 0)
	{
		basic_blocks[basic_blockp].end = fourtable_p - 1;
		basic_blockp++;
	}
	for (i = 0; i < basic_blockp; i++)
	{
		t = basic_blocks[i].start;
		basic_blocks[i].level = four_codes[t].level;
		gen_dag(basic_blocks[i].start,basic_blocks[i].end,basic_blocks[i].level);
	}
	calc_aftercome();
	calc_defuse();
	calc_inout();
	print_in();
	out_all4_afteropt();
	
}

//dag flag not now
int insert_4(int type,int src1,int src2,int des){
    four_codes[fourtable_p].type = type;
    four_codes[fourtable_p].src1 = src1;
    four_codes[fourtable_p].src2 = src2;
    four_codes[fourtable_p].des = des;
    four_codes[fourtable_p].level = nowlevel;
    fourtable_p++;
    return fourtable_p - 1;
}
int new_lable_4(){
    int i = insert_4(four_lable, 0, 0 , four_lable_p);
    four_lable_p++;
    return i;
}
int new_enter_4(int func){
    int i =insert_4(four_enter, 0, 0, func);
    return i;
}


void set_des_4(int four_p,int des){
    four_codes[four_p].des = des;
}

void init_4(){
    fourtable_p = 1;
    four_lable_p = 0;
    //dag flag block
}

char *out4[]=
{
    "",
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "BIG",
    "LESS",
    "BIGE",
    "LESSE",
    "EQ",
    "NEQ",
    "JMP",
    "JZ",
    "BEC",
    "CALL",
    "READ",
    "WRITE",
    "PUSH",
    "END",
    "GETARR",
    "GETADD",
    "LABLE",
    "ENTER"
};
void out_all4_afteropt()
{	
	printf("\nfour expressions after opt:\n");
	for (int i = 1; i < fourtable_afteropt_p; i++)
	{
		printf("%d", i);
		out_one_4(four_codes_afteropt[i]);
	}
}
void out_all4(){
	printf("\nfour expressions:\n");
    for (int i = 1; i<fourtable_p; i++) {
        printf("%d",i);
        out_one_4(four_codes[i]);
    }
}

void out_4_var(int x){
    printf(",");
    if(x == 0)
        printf("-");
    else if (sym_tables[x].kind == k_var
             ||sym_tables[x].kind == k_func
             ||sym_tables[x].kind == k_proc)
    {
        printf("%s",sym_tables[x].name);
    }
    else if (sym_tables[x].kind == k_const)
    {
        if(sym_tables[x].type == t_string)
            printf("\"%s\"",sym_tables[x].name);
        else if (sym_tables[x].type == t_char)
            printf("%c",sym_tables[x].x);
        else
            printf("%d",sym_tables[x].x);
    }
    else if (sym_tables[x].kind == k_point)
        printf("(%s)",sym_tables[x].name);
}

void out_one_4(struct four_expression a){
    //format label and enter
    switch (a.type) {
        case four_lable:
            printf("\t%s",out4[a.type]);
            printf(" L%d:",a.des);
            break;
        case four_enter:
            printf("\t%s",out4[a.type]);
            printf(" %s:",sym_tables[a.des].name);
            break;
        case four_jmp:
        case four_jz:
            printf("\t\t%s",out4[a.type]);
            out_4_var(a.src1);
            printf(",L%d",four_codes[a.des].des);
            break;
        case four_call:
            printf("\t\t%s",out4[a.type]);
            printf(",%s,%d",sym_tables[a.src1].name,a.src2);
            out_4_var(a.des);
            break;
        default:
            printf("\t\t%s",out4[a.type]);
            if(a.type!=four_push)
            {
                out_4_var(a.src1);
                out_4_var(a.src2);
            }
            out_4_var(a.des);
            break;
    }
    printf("\n");
}
//划分基本快


