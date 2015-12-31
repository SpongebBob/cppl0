#include <stdio.h>
#include "four_expression.h"
#include "symtable.h"
#define MAX_P 200
#define MAX_T 200
struct point
{	
	int dag_op;
	int dag_src1;
	int dag_src2;
};
struct point
{
	int sym_src;
	int to;
	int link;
};
