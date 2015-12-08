//
//  tocken.c
//  Compiler
//
//  Created by MacBook on 15/11/21.
//  Copyright © 2015年 MacBook. All rights reserved.
//
#define _CRT_SECURE_NO_WARNINGS
#include "tocken.h"
#include<ctype.h>
#include<stdio.h>
#include "string.h"
#include "error.h"
//global
void init_tocken();
int getsym();
//
FILE *fin,*fout;
int symnumber[256];
int linelen,linep,symtype,num,t,line_num = 1;
char line[MAXLINE],sym[MAXSYM];
char ch;
char *alphabet[] =
{
    "array",
    "begin",
	"case",
    "char",
    "const",
    "do",
    "downto",
    "else",
    "end",
    "for",
    "function",
    "if",
    "integer",
    "of",
    "procedure",
    "read",
	"repeat",
    "then",
    "to",
	"until",
    "var",
    "while",
    "write"
};

void getline2buffer()
{
    if (linep == linelen)
    {
        if (fgets(line,MAXLINE,fin) == NULL)
        {
            ch = EOF;
            return ;
        }
        //	printf("%s",line);
        linep = 0;
        linelen = (int)strlen(line);
    }
    ch = line[linep];
    putchar(ch);
    fflush(stdout);
    linep += 1;
}
void getch1()
{
	if (ch == '\n')  line_num++;
    if( !isspace(ch) && ch!='\"')
    {
        sym[t++] = ch;
        sym[t] = '\0';
    }
    getline2buffer();
}

void getch2()
{
    sym[t++] = ch;
    sym[t] = '\0';
    getline2buffer();
}

void init_tocken()
{
    char inf[256];
    symnumber['+'] = PLUS;
    symnumber['-'] = MINUS;
    symnumber['*'] = MULT;
    symnumber['/'] = DIV;
    symnumber[','] = COMMA;
    symnumber['.'] = PERIOD;
    symnumber[';'] = SEM;
    symnumber[':'] = COLON;
    symnumber['['] = LBP;
    symnumber[']'] = RBP;
    symnumber['('] = LP;
    symnumber[')'] = RP;
    symnumber['='] = EQ;

    scanf("%s",inf);
    fin = fopen(inf,"r");
    while (fin==NULL)
    {
        //until it is ok
        printf("Please input source file name!\n");
        scanf("%s",inf);
        fin = fopen(inf,"r");
    }
    getch1();
}

int searchident()
{
    int t,w,mid =0;
    t = 0;
    w = NALPHABET-1;
    while(t<=w)
    {
        mid = (t + w)/2;
        if (strcmp(sym,alphabet[mid])>=0)
            t = mid+1;
        if (strcmp(sym,alphabet[mid])<=0)
            w = mid-1;
    }
    if (t == w+2)
        return mid+1;
    else
        return 0;
}

int getsym()
{
    int  k;
    t = 0;
    while (isspace(ch))
        getch1();
    if(ch == EOF)
        return -1;
    if (isalpha(ch))
    {
        getch1();
        while (isalnum(ch))
        {
            getch1();
        }
        k = searchident();
        //	printf("%s %d\n",sym,k);
        if (k == 0)
            symtype = T_IDENT;
        else
            symtype = k;
    }
    else if (isdigit(ch))
    {
        num = c2i(ch);
        getch1();
        while (isdigit(ch))
        {
            num = num * 10 + c2i(ch);
            getch1();
        }
        symtype = T_CONST;
    }
    else if (ch =='\'')
    {
        getch1();
        num = ch;
        getch1();
        if (ch !='\'')// missing '
            my_error(1);
        getch1();
        symtype = T_CHAR;
    }
    else if (ch == '\"')
    {
        getch1();
        while(iss(ch))
            getch2();
        if(!(ch =='\"'))
            my_error(2);	// missing "
        symtype = T_STRING;
        getch1();
    }
    else if(ch == ':')
    {
        getch1();
        if(ch == '=')
        {
            symtype = BECOME;
            getch1();
        }
        else
            symtype = COLON;
    }
    else if (ch == '<')
    {
        getch1();
        if(ch == '>')
        {
            symtype = NEQ;
            getch1(); 
        }
        else if( ch =='=')
        {
            symtype = SMOE;
            getch1();
        }
        else
            symtype = SMO;
    }
    else if (ch == '>')
    {
        getch1();
        if(ch == '=')
        {
            symtype = BIGE;
            getch1();
        }
        else
            symtype = BIG;
    }
    else
    {
        symtype = symnumber[ch];
        getch1();
    }
    //	printf("%s\n",sym);
    return 0;
}

char *OUTPUT[]=
{
    "",
    "ARRAYTK",
    "BEGINTK",
    "CHARTK",
    "CONSTTK",
    "DOTK",
    "DOWNTOTK",
    "ELSETK",
    "ENDTK",
    "FORTK",
    "FUNCTK",
    "IFTK",
    "INTTK",
    "OFTK",
    "PROCETK",
    "READTK",
    "REPTTK",
    "THENTK",
    "TOTK",
    "UNLTK",
    "VARTK",
    "WHILETK",
    "WRITETK",
    "PLUS",
    "MINU",
    "MULT",
    "DIV",
    "COMMA",
    "PERIOD",
    "SEMICN",
    "COLON",
    "LBRACK",
    "RBRACK",
    "LPARENT",
    "RPARENT",
    "ASSIGN",
    "EQL",
    "NEQ",
    "LSS",
    "LEQ",
    "GRE",
    "GEQ",
    "IDEN",
    "INTCON",
    "CHARCON",
    "STRCON",
};