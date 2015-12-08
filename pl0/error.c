//
//  error.c
//  Compiler
//
//  Created by MacBook on 15/11/21.
//  Copyright © 2015年 Microsoft. All rights reserved.
//

#include <stdio.h>
#include "tocken.h"
char *message[] =
{
    "",//0
	"missing RBP",//1
	"missinng OF",//2
	"unkown type",//3
	"missing '['",//4
	"not arry or basic type",//5
	"redefined",//6
	"mssing , ;",//7
	"can not get value (is not a factor)",//8
	"missing ']'",//9
	"not  arry",//10
	"can not deal with arg",//11
	"missing ')'",//12
	"empty factor",//13
	"not realation sym",//14
	"not realation sym",//15
	"redefined",//16
	"missing '='",//17
	"redifine",//18
	"no zero form arg",//19
	"mising ')'",//20
	"mising ';'",//21
	"misising ident",//22
	"missing END",//23
	"no_part_procedure",//24
	"missing colon",//25
	"unknown const_value",//26
	"wrong type ,it should be a k_point",//27
	"wrong array,not a arry",//28
	"wrong type",//29
	"must be ident",//30
	"wrong arg num",//31
	"can't find in sym table",//32
	"can't be const in the left of '='	",//33
	"can't assign to a procedure",//34
	"missing :=",//35
	"not array type",//36
	"no legal :=",//37
	"missing THEN",//38
	"missing lp",//39
	"missing ident",//40
	"can not write to a static ident",//41
	"missing COMMA or RP",//42
	"missing UNTIL",//43
	"missing DO",//44
	"For var is illegal",//45
	"unknown type ,need to or down to"//46
};
void my_error(int id)
{
    char c;
    printf("\nerror:%d,",id);
	printf("%s,", message[id]);
	printf("in line:%d.\n",line_num);
	printf("press any key to skip\n");
    c=getchar();
	c = getchar();

}