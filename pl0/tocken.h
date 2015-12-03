//
//  tocken.h
//  Compiler
//
//  Created by MacBook on 15/11/21.
//  Copyright © 2015年 MacBook. All rights reserved.
//

#ifndef tocken_h
#define tocken_h


#define NALPHABET 23
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
#define REPEAT  17 
#define THEN	18
#define TO		19
#define UNTIL   20
#define VAR		21
#define WHILE	22
#define WRITE	23

/*
 * other symbol 23-41
 * */
#define	PLUS	24
#define	MINUS	25
#define	MULT	26
#define	DIV		27
#define	COMMA	28
#define	PERIOD	29
#define	SEM		30
#define	COLON	31
#define	RBP		32
#define	LBP		33
#define	RP		34
#define	LP		35
#define	BECOME	36
#define	EQ		37
#define NEQ		38
#define SMO		39
#define SMOE	40
#define	BIG		41
#define BIGE	42

/*
 * type symbol 42-45
 * */
#define T_IDENT	43
#define T_CONST	44
#define T_CHAR	45
#define T_STRING 46

extern char sym[MAXSYM];
extern void init_tocken();
extern int getsym();
extern int num,symtype,linep;



#endif /* tocken_h */
