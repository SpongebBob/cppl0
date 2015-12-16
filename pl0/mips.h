#define OFFSET 16
#define MAXREG 32

#define r_zero 0
#define r_at 1

#define r_V0 2
#define r_V1 3

#define r_a0 4
#define r_a1 5
#define r_a2 6
#define r_a3 7

#define r_t0 8
#define r_t1 9
#define r_t2 10
#define r_t3 11
#define r_t4 12
#define r_t5 13
#define r_t6 14
#define r_t7 15

#define r_s0 16
#define r_s1 17
#define r_s2 18
#define r_s3 19
#define r_s4 20
#define r_s5 21
#define r_s6 22
#define r_s7 23

#define r_t8 24
#define r_t9 25

#define r_k0 26
#define r_k1 27
#define r_gp 28
#define r_sp 29
#define r_fp 30
#define r_ra 31
char *r_name[] =
{
	"$zero",//0
	"$at",//1
	"$v0",//2
	"$v1",//3
	"$a0",//4
	"$a1",//5
	"$a2",//6
	"$a3",//7
	"$t0",//8
	"$t1",//9
	"$t2",//10
	"$t3",
	"$t4",
	"$t5",
	"$t6",
	"$t7",
	"$s0",
	"$s1",
	"$s2",
	"$s3",
	"$s4",
	"$s5",
	"$s6",
	"$s7",
	"$t8",
	"$t9",
	"$k0",
	"$k1",
	"$gp",
	"$sp",
	"$fp",
	"$ra"
};