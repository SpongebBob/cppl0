//
//  tocken.h
//  Compiler
//
//  Created by MacBook on 15/11/21.
//  Copyright © 2015年 MacBook. All rights reserved.
//

#ifndef tocken_h
#define tocken_h


#define NALPHABET 22
#define MAXLINE 255
#define MAXSYM 255
#define MAXVARDEC 100
#define c2i(c) (c-'0')
#define iss(c) ((c)>=32 && (c)<=126 && (c)!=34)

/*
 * reserve words  1-22
 * */
#define ARRAY	1
#define BEGIN	2
#define CASE    3
#define CHAR	4
#define CONST	5
#define DO		6
#define DOWNTO	7
#define ELSE	8
#define END		9
#define FOR		10
#define FUNC	11
#define IF		12
#define INT		13
#define OF		14
#define PROC	15
#define READ	16
#define THEN	17
#define TO		18
#define VAR		19
#define WHILE	20
#define WRITE	21

/*
 * other symbol 23-41
 * */
#define	PLUS	22
#define	MINUS	23
#define	MULT	24
#define	DIV		25
#define	COMMA	26
#define	PERIOD	27
#define	SEM		28
#define	COLON	29
#define	RBP		30
#define	LBP		31
#define	RP		32
#define	LP		33
#define	BECOME	34
#define	EQ		35
#define NEQ		36
#define SMO		37
#define SMOE	38
#define	BIG		39
#define BIGE	40

/*
 * type symbol 42-45
 * */
#define T_IDENT	42
#define T_CONST	43
#define T_CHAR	44
#define T_STRING 45

extern char sym[MAXSYM];
extern void init_tocken();
extern int getsym();
extern int num,symtype;



#endif /* tocken_h */
