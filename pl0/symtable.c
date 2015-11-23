//
//  symtable.c
//  Compiler
//
//  Created by MacBook on 15/11/21.
//  Copyright © 2015年 MacBook. All rights reserved.
//
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "string.h"
#include "symtable.h"
//need a nowlevel
extern int nowlevel;
//init

 char sym_name[MAXSYMNAME];
 int sym_namep,symtable_p,symstring_p;
 struct sym_table sym_tables[MAXSYMTABLE];

void init_symtable()
{
    sym_namep = 1;
    symtable_p = 1;
    symstring_p = 1;
}
//create a sym_name
char *new_sym_name(char a[])
{
    int i = 0;
    char *r = &sym_name[sym_namep];
    while (a[i]) {
        sym_name[sym_namep] = a[i];
        sym_namep++;
        i++;
    }
    sym_name[sym_namep] = '\0';
    sym_namep++;
    return r;
}

int insert_symtable(char *a,int kind)
{
    int i = symtable_p-1;
    sym_tables[symtable_p].kind = kind;
    sym_tables[symtable_p].x = 0;
    sym_tables[symtable_p].level = nowlevel;
    sym_tables[symtable_p].name = new_sym_name(a);
    //set last pointer
    //if not same_level
    while (sym_tables[i].level>nowlevel) {
        i = sym_tables[i].last;
    }
    //set last pointer = last i
    sym_tables[symtable_p].last = i;
    symtable_p++;
    return symtable_p-1;
}

void settype_symtable(int insert_p,int x,int type)
{
    sym_tables[insert_p].type = type;
    if (type == t_string) {
        sym_tables[insert_p].x = symstring_p++;
    }
    else{
        sym_tables[insert_p].x = x;
    }
}


int new_temp_var_symtable()
{
    static int i=0;
    int t;
    char s[20];
    sprintf(s,"_t%d",i++);
    //what is var
    t=insert_symtable(s,k_var);
    settype_symtable(t, 0, t_integer);
    return t;
}

int new_temp_const_symtable(int x)
{
    // I don't care what is called
    int i = insert_symtable("", k_const);
    settype_symtable(i, x, t_integer);
    return i;
}
int find_symtable(char *a)
{
    int i =symtable_p-1;
    while (sym_tables[i].level>nowlevel) {
        i=sym_tables[i].last;
    }
    while (i) {
        if (strcmp(sym_tables[i].name,a)==0) {
            return i;
        }
        i = sym_tables[i].last;
    }
    return 0;
}

















