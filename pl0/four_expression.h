//
//  four_expression.h
//  Compiler
//
//  Created by MacBook on 15/11/21.
//  Copyright © 2015年 MacBook. All rights reserved.
//

#ifndef four_expression_h
#define four_expression_h

#define MAXFOURCODE 2000
#define MAXBLOCK
#define four_var -1
//+-*/
#define four_add 1
#define four_minus 2
#define four_mul 3
#define four_div 4
//> < >= <= == !=
#define four_big 5
#define four_less 6
#define four_bige 7
#define four_lesse 8
#define four_eq 9
#define four_neq 10
//jmp,jz
#define four_jmp 11//
#define four_jz 12//
//bec call
#define four_bec 13//
#define four_call 14//
//read write push pop end
#define four_read 15//
#define four_write 16//
#define four_push 17//
#define four_end 18//
//getarr getadd lable enter
#define four_getarr 19
#define four_getadd 20//
#define four_lable 21//
#define four_enter 22//
//end
struct four_expression
{
    int type;
    int src1;
    int src2;
    int des;
    int level;
};
struct basic_block
{
	int start;
	int end;
	int level;
	int houji[2];
	int def[101];
	int use[101];
	int in[101];
	int out[101];
	int defp;
};
//block is not define
extern struct four_expression four_codes[MAXFOURCODE];
extern struct four_expression four_codes_afteropt[MAXFOURCODE];
extern struct basic_block basic_blocks[200];
extern int fourtable_p, four_lable_p, fourtable_afteropt_p,basic_blockp;
//
int insert_4(int,int,int,int);
int new_lable_4();
extern int new_enter_4(int);
void set_des_4(int,int);
void init_4();
void out_all4();
void out_one_4(struct four_expression);
void out_all4_afteropt();

#endif /* four_expression_h */
