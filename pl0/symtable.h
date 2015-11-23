//
//  sytable.h
//  Compiler
//
//  Created by MacBook on 15/11/21.
//  Copyright © 2015年 MacBook. All rights reserved.
//

#ifndef sytable_h
#define sytable_h

#define MAXSYMTABLE 200
#define MAXSYMNAME 1000

//this is sym.kind
#define k_var 1 
#define k_const 2
#define k_func 3
#define k_proc 4
#define k_point 5
//this is sym.type
#define t_char 1
#define t_integer 2
#define t_string 3

struct sym_table
{
    char *name;
    int kind;
    int type;
    int x;//the value,=
    int level;
    int last;//the last pointer
    int size;//frame size
    int reg;
    int mem;
};

extern char sym_name[MAXSYMNAME];
extern int sym_namep,symtable_p,symstring_p;
extern struct sym_table sym_tables[MAXSYMTABLE];

void init_symtable();
char *new_sym_name(char *);
int insert_symtable(char *,int);
void settype_symtable(int,int,int);
int new_temp_var_symtable();
int new_temp_const_symtable(int);
int find_symtable(char *);




#endif /* sytable_h */
