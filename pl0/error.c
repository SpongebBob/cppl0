//
//  error.c
//  Compiler
//
//  Created by MacBook on 15/11/21.
//  Copyright © 2015年 Microsoft. All rights reserved.
//

#include <stdio.h>
char *message[] =
{
    "",
	"1",
	"2",
	"3"
};
void my_error(int id)
{
    char c;
    printf("error:%d",id);
    c=getchar();
	c = getchar();

}