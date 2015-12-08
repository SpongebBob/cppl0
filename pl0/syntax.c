//
//  syntax.c
//  Compiler
//
//  Created by MacBook on 15/11/21.
//  Copyright © 2015年 MacBook. All rights reserved.
//
#define _CRT_SECURE_NO_WARNINGS
//#define DEBUG
#include <stdio.h>
#include <string.h>
#include "error.h"
#include "tocken.h"
#include "symtable.h"
#include "four_expression.h"

#define MAXVARDEC 100

int nowlevel;

void get_type(struct sym_table *t);
void var_declare();
int syntax_factor();
int syntax_term();
int syntax_expression();
int condition();
void const_declare();
void function_declare();
void procdure_declare();
void part_procedure(int name);
void init_syntax();
int form_arg();
int get_const_value();
int deal_var();
void real_arg(int func_p);
void statement();



void get_type(struct sym_table *t)
{
    if(symtype == INT)
    {
        t->type = t_integer;
        t->x =0;
    }
    else if (symtype == CHAR)
    {
        t->type = t_char;
        t->x = 0;
    }
    else if (symtype == ARRAY)
    {
        getsym();
        if(symtype == LBP)
        {
            getsym();
            if(symtype==T_CONST)
            {
                t->x =num;
				getsym();
#ifdef DEBUG
				printf("sym:%s\n", sym);
				printf("%d", symtype);
#endif
                if(symtype == RBP)
                    getsym();
                else
                    my_error(1);//missing RBP
                if(symtype == OF)
                    getsym();
                else
                    my_error(2);//missinng OF
                if(symtype == CHAR)
                    t->type = t_char;
                else if (symtype == INT)
                    t->type = t_integer;
                else
                    my_error(3);//unkown type
            }
            else
                my_error(3);//must be unsigned integer
        }
        else
            my_error(4);//missing '['
    }else
        my_error(5);//不是基本类型或者数组
    
	getsym();


}
void var_declare()
{
    int p[MAXVARDEC];
    int vardef_p = 0,i=0;
    struct sym_table t;
    while (symtype == T_IDENT) {
        i = find_symtable(sym);
        if(i==0||sym_tables[i].level!=nowlevel)
            p[vardef_p] = insert_symtable(sym, k_var);
        else
            my_error(6);//redefined
        vardef_p++;
        getsym();
        if(symtype == COMMA)
            getsym();
        else if (symtype == COLON)
        {
            getsym();
            get_type(&t);
            for (i=0; i<vardef_p; i++) 
                settype_symtable(p[i], t.x, t.type);
            break;
        }
        else
            my_error(7);//mssing ',' ';'
    }
}
//factor
int syntax_factor()
{
    int a=0,b,t=0;
#ifdef DEBUG
	//printf("a:%d", a);
	printf("sym:%s", sym);
#endif
    if(symtype == T_IDENT)
    {
        a = find_symtable(sym);
		if (a == 0 || sym_tables[a].kind == k_proc)
		{
#ifdef DEBUG
			printf("a:%d", a);
			printf("sym:%s", sym);
#endif
			my_error(8);//can not get value (is not a factor)
		}
        if(sym_tables[a].kind != k_func)
        {
            getsym();

            if(symtype ==LBP)
            {
                getsym();
                b = syntax_expression();
                if (symtype == RBP) {
                    getsym();
                }else
                    my_error(9);//missing ']'
                if (sym_tables[a].kind == k_var && sym_tables[a].x >0 ) {
                    t = new_temp_var_symtable();
                    insert_4(four_getarr, a, b, t);
                    a = t;
                }
                else
                    my_error(10);//not a arry
            }
        }else //函数调用
        {
            getsym();

			if (sym_tables[a].x != 0 && symtype != LP 
				|| sym_tables[a].x==0&&symtype == LP)
				my_error(100);
            if(symtype == LP)
            {
                getsym();
                real_arg(a);
                t =new_temp_var_symtable();
                insert_4(four_call, a, sym_tables[a].x, t);
				a = t;
                if(symtype == RP)
                    getsym();
                else
                    my_error(11);//can not deal with arg
            }
            else
            {
                t= new_temp_var_symtable();
                insert_4(four_call, a, sym_tables[a].x, t);
				a = t;
            }
        }
        //else if (symtype == LP)
        
    }//expresssion
    else if (symtype == LP)
    {
        getsym();
        a = syntax_expression();
        if (symtype == RP)
            getsym();
        else
            my_error(12);//missing ')'        
    }//unsigned int
    else if (symtype == T_CONST)
    {
        a = new_temp_const_symtable(num);
        getsym();
    }
    else
        my_error(13);//empty factor;
    return a;
}
//term
int syntax_term()
{
    int a,b,f,t =0;
    a = syntax_factor();
    while (symtype == MULT||symtype == DIV) {
        f = symtype;
        getsym();
        b = syntax_factor();
		t = new_temp_var_symtable();
        if(f == MULT)
            insert_4(four_mul, a, b, t);
        else
            insert_4(four_div, a, b, t);
        a = t ;
    }
    return a;
}
//expression return the last des
int syntax_expression()
{
    int a,b,t,f ,c,is_minus = 0;
    if(symtype == PLUS)
        getsym();
    else if(symtype == MINUS)
    {
        getsym();
        is_minus = 1;
    }
    a = syntax_term();
    while (symtype == PLUS||symtype == MINUS) {
        f=symtype;
        getsym();
        b = syntax_term();
        t = new_temp_var_symtable();
        if(f == MINUS)
            insert_4(four_minus, a, b, t);
        else
            insert_4(four_add, a, b, t);
        a = t;
    }
    if (is_minus) {
        t = new_temp_var_symtable();
        c = new_temp_const_symtable(0);
        //if is minus , -this
        insert_4(four_minus, c, a, t);
        a=t;
    }
    return a;
}
int condition()
{
	int f, a,t,b =0;
     a = syntax_expression();
    if(symtype != EQ && symtype != NEQ
       && symtype!= BIG && symtype != BIGE
       &&symtype !=SMO && symtype!= SMOE)
        my_error(14);//not realation sym
     f = symtype;
    getsym();
     t =new_temp_var_symtable();
     b = syntax_expression();
    switch (f) {
        case EQ:
            insert_4(four_eq, a,b,t);
            break;
        case NEQ:
            insert_4(four_neq, a, b, t);
            break;
        case SMO:
            insert_4(four_less, a, b, t);
            break;
        case SMOE:
            insert_4(four_lesse, a, b, t);
            break;
        case BIG:
            insert_4(four_big, a, b,t);
            break;
        case BIGE:
            insert_4(four_bige, a, b, t);
            break;
        default:
            my_error(15);//not guanxiyunsuanfa
    }
    return t;
}

void const_declare()
{
    char ident[MAXSYM];
    int t,p = 0;
    if (symtype==T_IDENT) {
        strcpy(ident,sym);
        getsym();
        if(symtype == EQ)
        {
            getsym();
            t = get_const_value();
            p = find_symtable(ident);
            if (p==0||sym_tables[p].level!=nowlevel) {
                p = insert_symtable(ident, k_const);
            }
            else
                my_error(16);//redefined
            if(symtype == T_CHAR)
                settype_symtable(p, t, t_char);
            else
                settype_symtable(p, t, t_integer);
            getsym();
        }
        else
            my_error(17);//missing '='
    }
}
void function_declare()
{
    int p = 0,x = 0,t = 0;
    getsym();
    if(symtype == T_IDENT)
    {
        p = find_symtable(sym);
        if(p == 0|| sym_tables[p].level!=nowlevel)
            p=insert_symtable(sym, k_func);
        else
            my_error(18);//redifine
        getsym();
#ifdef DEBUG
		printf("\t\tNowL:%d", nowlevel);
#endif
        nowlevel++;
        //define
        if(symtype == LP)
        {
            getsym();
			if (symtype == RP) my_error(19); //no zero form arg
            if(symtype == VAR||symtype == T_IDENT)
            {
                x =form_arg();
            }
            if(symtype == RP)
                getsym();
            else
                my_error(20);//mising ')'
        }
        // TODO  there is some defferent
#ifdef DEBUG
		printf("SYM:%s", sym);
#endif
		if (symtype == COLON)
            getsym();
        else
            my_error(21);//mising ';'
        if(symtype == INT)
            t = t_integer;
        else if (symtype == CHAR)
            t = t_char;
        else
            my_error(3);//unknown type
        settype_symtable(p, x, t);
		getsym();
		if (symtype == SEM)
			getsym();
		else
			my_error(22);//misising ident
        part_procedure(p);
        if(symtype == SEM)
            getsym();
        else
            my_error(22);//misising ident
    }
    else
        my_error(22);//missing ident
    
}
void procdure_declare()
{
    int p,x =0;
    getsym();
    if(symtype == T_IDENT)
    {
        p = find_symtable(sym);
        if(p == 0|| sym_tables[p].level!=nowlevel)
            p=insert_symtable(sym, k_proc);
        else
            my_error(16);//redifine
        getsym();
#ifdef DEBUG
		printf("\t\tNowL:%d", nowlevel);
#endif
        nowlevel++;
        //define
        if(symtype == LP)
        {
            getsym();
			if (symtype == RP) my_error(19);//no zero form arg
            if(symtype == VAR||symtype == T_IDENT)
            {
                x =form_arg();
            }
            if(symtype == RP)
                getsym();
            else
                my_error(12);//mising ')'
        }
        settype_symtable(p, x, 0);
        
        // TODO  there is some defferent
        if(symtype == SEM)
            getsym();
        else
            my_error(7);//mising ';'
        part_procedure(p);
#ifdef DEBUG
		printf("\n\npart_procedure_sym:%s\n\n", sym);
#endif // DEBUG

        if(symtype == SEM)
            getsym();
        else
            my_error(22);//misising ident
    }
    else
        my_error(22);//missing ident
    
}

void part_procedure(int name)
{

    int t;
    if (symtype == CONST) {
        getsym();
        const_declare();
        while (symtype == COMMA) {
            getsym();
            const_declare();
        }
        if (symtype == SEM) {
            getsym();
        }
        else
            my_error(21);//missing ';'
    }
    if (symtype == VAR) {
        getsym();
        while (symtype == T_IDENT) {
            var_declare();
			
            if(symtype == SEM)
                getsym();
            else
                my_error(21);//missing ';'
#ifdef DEBUG
			printf("symtype,%d",symtype);
#endif // DEBUG
        }
    }
    while (symtype == PROC||symtype ==FUNC) {
        if (symtype == PROC) {
            procdure_declare();
        }else if(symtype == FUNC)
            function_declare();
    }
    t = new_enter_4(name);
    if(symtype == BEGIN)
    {
        getsym();
#ifdef DEBUG
		printf("\npart_sym:%s", sym);
#endif // DEBUG
        statement();
        while (symtype == SEM) {
            getsym();
            statement();
        }

        if(symtype == END)
            getsym();
        else
            my_error(23);//missing END
    }else
        my_error(24); //no_part_procedure
    nowlevel--;
    insert_4(four_end, 0, 0, name);
    
}
void init_syntax()
{
    nowlevel = 0;
    sym_tables[0].name = "main";
    sym_tables[0].x = 0;
    sym_tables[0].type = 0;
    sym_tables[0].level = -1;
    sym_tables[0].size = 0;
}

int form_arg()
{
    int kind,count =0;
    int t = 0,i,num_dec = 0;
    int p[MAXVARDEC];
    while (symtype ==VAR||symtype == T_IDENT) {
        num_dec =0;
        kind = k_var;
        if (symtype == VAR) {
            getsym();
            kind = k_point;
        }
        if(symtype != T_IDENT)
            my_error(22);//there is no ident
        while (symtype == T_IDENT) {
            t = find_symtable(sym);
            if (t==0 || sym_tables[t].level!=nowlevel) {
                p[num_dec] = insert_symtable(sym, kind);
            }
            else
                my_error(18);//redefind
            num_dec++;
            count++;
            getsym();
            if(symtype == COMMA)
                getsym();
        }
        if (symtype == COLON) {
            getsym();
        }else
            my_error(25);//missing colon
        if (symtype == INT) {
            t = t_integer;
        }
        else if (symtype == CHAR)
            t = t_char;
        else
            my_error(3);//unknown type
        getsym();
        for(i =0;i<num_dec;i++)
        {
            settype_symtable(p[i], 0, t);
        }
        if(symtype == SEM)
            getsym();
        else
            break;
    }
	if (count == 0) 
		my_error(19);//cannot  be zero form arg
    return count;

}

int get_const_value()
{
    int t = 0;
    if(symtype == PLUS)
    {
        getsym();
        t=num;
    }
    else if(symtype == MINUS)
    {
        getsym();
        t =-num;
    }
    else if (symtype == T_CHAR||symtype == T_CONST)
    {
        //not get a sym?
        t=num;
    }else
        my_error(26);//unknown const_value
#ifdef DEBUG
	printf("\n~~~~~~~~~~~~~~~NUM:%d~~~~~~~~~~~~~~~~\n", num);
#endif
    return t;
}

//
int deal_var()
{
    int a,b = 0,t = 0;
#ifdef DEBUG
	printf("VAR_SYM:%s\n", sym);
#endif
    if(symtype == T_IDENT)
    {
        a = find_symtable(sym);
        if(sym_tables[a].kind == k_const||sym_tables[a].kind == k_proc)
            my_error(27);//wrong type ,it should be a k_point
        getsym();
        if(symtype == LBP)
        {
            getsym();
            b=syntax_expression();
            if(symtype == RBP)
            {
                if(sym_tables[a].x == 0)
                    my_error(28);//wrong array,not a arry;
                t = new_temp_var_symtable();
                insert_4(four_getadd, a, b, t);
				getsym();
            }
            else
                my_error(9);//missing ']'
        }
        else if(symtype == COMMA||symtype == RP)
        {
            t = new_temp_var_symtable();
            insert_4(four_getadd, a, b, t);
        }
        else
            my_error(29);//wrong type;
    }else
        my_error(30);//must be ident
    return t;
}
//real_arg()
void real_arg(int func_p)
{
    int i,t = 0;
    for(i = 1;i <= sym_tables[func_p].x;i++)
    {
#ifdef DEBUG
		printf("\nNUM:%d", sym_tables[func_p].x);
#endif

        if (sym_tables[func_p+i].kind == k_point) {
#ifdef DEBUG
			printf("\nSYM:%s", sym);
			printf("DEAL VAR\n");
#endif
            t = deal_var();

            insert_4(four_push, i, 0, t);
        }else{
            t = syntax_expression();
            insert_4(four_push, i, 0, t);
        }
        if(symtype == COMMA)
            getsym();
		else if (symtype == RP)
		{
			if (i != sym_tables[func_p].x)
				my_error(31);//wrong arg num
		} //need a error deal?
		else
			my_error(12);
    
    }

}


void statement()
{
    int a,b,c,d,t1,t2,isTO = 0;
    if(symtype == T_IDENT)
    {
        a = find_symtable(sym);
        if(a == 0)
            my_error(32);//can't find in sym table
        if(sym_tables[a].kind == k_const)
            my_error(33);//can't be const in the left of '='	
		if (sym_tables[a].kind != k_proc) 
		{
            getsym();
			
#ifdef DEBUG
			printf("&???********%d", symtype);
#endif
            if(symtype == BECOME)
            {
                getsym();
                b = syntax_expression();
#ifdef DEBUG
				printf("&********%d", symtype);
#endif
                if(sym_tables[a].kind != k_proc)
                {
                    insert_4(four_bec, b, 0, a);
                }else
                    my_error(34);//can't assign to a procedure
            }
            else if(symtype == LBP)
            {
#ifdef DEBUG
				printf("SYM:%s", sym);
#endif
                getsym();//array

                b = syntax_expression();
                if(symtype == RBP)
                    getsym();
                else
                    my_error(9);//missing ]
                if(symtype == BECOME)
                    getsym();
                else
                    my_error(35);//missing :=
                c = syntax_expression();
                if(sym_tables[a].kind == k_var && sym_tables[a].x > 0)
                    insert_4(four_bec, c, b, a);

                else
                    my_error(36);//not array type
            }
			else

                my_error(37);//no legal := or no function
		}
        else
        {
			getsym();
#ifdef DEBUG
			printf("Here is lp:%s", sym);
#endif		
            if(symtype == LP)
            {
                getsym();
                real_arg(a);
                if (symtype == RP) {
                    getsym();
                }
                else
                    my_error(12);//missing rp
            }
			//else
				//getsym();
            b = sym_tables[a].x;
            insert_4(four_call, a, b, 0);
        }
    }
    else if (symtype == IF)
    {
        getsym();
        a = condition();
        t1 = insert_4(four_jz, a, 0, 0);
        if(symtype == THEN)
            getsym();
        else
            my_error(38);//missing THEN
        statement();
        if(symtype == ELSE)
        {
            t2 = insert_4(four_jmp, 0, 0, 0);
            a = new_lable_4();
            set_des_4(t1, a);
            getsym();
            statement();
            a = new_lable_4();
            set_des_4(t2, a);
        }
        else
        {
            a = new_lable_4();
            set_des_4(t1, a);
        }
    }
    else if (symtype == BEGIN)
    {
        getsym();
#ifdef DEBUG
		printf("\nSYM_statement:%s", sym);
#endif
        statement();
        while (symtype == SEM) {
            getsym();
            statement();
        }
#ifdef DEBUG
		printf("SYM_begin:%s", sym);
#endif
        if(symtype == END)
            getsym();
        else
            my_error(23); // missing END
    }
    else if(symtype == READ)
    {
        getsym();
        if(symtype == LP)
            getsym();
        else
            my_error(39);//missing lp
        if(symtype!=T_IDENT)
            my_error(40);//missing ident
        while (symtype == T_IDENT) {
            a = find_symtable(sym);
            if(a!= 0&&(sym_tables[a].kind == k_var
                       ||sym_tables[a].kind == k_func
                       ||sym_tables[a].kind == k_point))
                insert_4(four_read, 0, 0, a);
            else
                my_error(41);//can not write to a static ident
            getsym();
            if(symtype == COMMA)
                getsym();
            else if (symtype == RP)
            {
                getsym();
                break;
            }else
                my_error(42);//missing COMMA or RP
        }
    }
	else if (symtype == REPEAT)
	{
		getsym();
		t1 = new_lable_4();
		statement();
		if (symtype == UNTIL)
			getsym();
		else
			my_error(43);//missing UNTIL
		a = condition();
		insert_4(four_jz, a, 0, t1);
		// do some thing for REPEAT-UNTIL
	}
	else if (symtype == WHILE)
	{
		getsym();
		t1 = new_lable_4();
		a = condition();
		b =insert_4(four_jz, a, 0, 0);
		if (symtype == DO)
			getsym();
		else
			my_error(44);//missing DO
		statement();
		insert_4(four_jmp, 0, 0, t1);
		t2 = new_lable_4();
		set_des_4(b, t2);
	}
    else if (symtype == WRITE)
    {
        getsym();
        if(symtype == LP)
            getsym();
        else
            my_error(39);//error missing lp
        if(symtype == T_STRING)
        {
            a =insert_symtable(sym, k_const);
            settype_symtable(a, 0, t_string);
            insert_4(four_write, 0, 0, a);
            getsym();
            if(symtype == COMMA)
            {
                getsym();
                a = syntax_expression();
                insert_4(four_write, 0, 0, a);
            }
            if(symtype == RP)
                getsym();
            else
                my_error(20);//missing rp
#ifdef DEBUG
			printf("\nafter_write RP%s", sym);
#endif // DEBUG
        }
        else
        {
            a =syntax_expression();
            insert_4(four_write, 0, 0, a);


            if(symtype == RP)
                getsym();
            else
                my_error(20);//missing rp
#ifdef DEBUG
			printf("\nafter_write2 RP%s", sym);
#endif // DEBUG
        }
    }
    else if (symtype == FOR)
    {
        getsym();
        if(symtype == T_IDENT)
        {
            a = find_symtable(sym);
            if(a == 0||sym_tables[a].kind!=k_var)
                my_error(45);//For var is illegal
            getsym();
            if(symtype == BECOME)
                getsym();
            else
                my_error(35);//need :=
            b = syntax_expression();
            insert_4(four_bec, b, 0, a);
            if(symtype == TO)
            {
                isTO = 1;
            }else if(symtype == DOWNTO)
                isTO = -1;
            else
                my_error(46);//unknown type ,need to or down to
            getsym();
            c = new_temp_const_symtable(isTO);
            t1 = new_lable_4();
            d = syntax_expression();
            t2 = new_temp_var_symtable();
            if(isTO == 1)
                insert_4(four_lesse, a, d, t2);
            else
                insert_4(four_bige, a, d, t2);
            b = insert_4(four_jz, t2, 0, 0);
            if(symtype ==DO)
                getsym();
            else
                my_error(44);//missing do
            statement();
            insert_4(four_add, a, c, a);
            insert_4(four_jmp, 0, 0, t1);
            t2 = new_lable_4();
            set_des_4(b, t2);
        }
        else
            my_error(40);//missing ident to store;
    }
    else if (symtype == CASE)
    {
        // case
        int a=0,b=0,c=0,t1,l;
		getsym();
        a = syntax_expression();
        if(symtype == OF)
        {
			getsym();
            b = get_const_value();
			c = new_temp_const_symtable(b);
            int t = new_temp_var_symtable();
            insert_4(four_eq, a, c, t);
            t1 = insert_4(four_jz, t, 0, 0);
            getsym();
			if (symtype == COLON)
			{
				getsym();
				statement();
			}
            else
                my_error(21);//missing ':'
            l = new_lable_4();
            set_des_4(t1, l);
            while (symtype == SEM) {
				getsym();
                b = get_const_value();
				c = new_temp_const_symtable(b);

                t = new_temp_var_symtable();
                insert_4(four_eq, a, c, t);
                t1 = insert_4(four_jz, t, 0, 0);
                getsym();
				if (symtype == COLON)
				{
					getsym();
					statement();
				}
                else
                    my_error(21); //missing ";"
	               l = new_lable_4();
                set_des_4(t1, l);
            }
			getsym();
        }
        else
            my_error(68);//missing of
    }
}

int main(int argc, const char * argv[]) {
    // insert code here...
	char c;
	printf("************************\n");
	printf("Please Input File Name:\n");
	printf("************************\n");
	init_tocken();
    init_syntax();
    init_symtable();
    init_4();
    getsym();
    part_procedure(0);
    if(symtype == PERIOD)
        getsym();
    else
        my_error(21);//missing '.'
	printf("************************\n");
	printf("Compile Success\n");
	printf("************************\n");
	out_all4();
	init_mips();
	printf("************************\n");
	printf("Gen to the result.asm \n");
	printf("************************\n");
	out_all_mips();
	printf("************************\n");
	printf("presse any key to exit\n");
	printf("************************\n");
	c = getchar();
	c = getchar();
    return 0;
}