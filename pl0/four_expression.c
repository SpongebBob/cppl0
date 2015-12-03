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
int fourtable_p,four_lable_p;
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


