#include "cpu/exec/template-start.h"

#define instr test

static void do_execute(){
	DATA_TYPE r=op_dest->val&op_src->val;
	int len=(DATA_BYTE<<3)-1;
 	cpu.OF=0;
	cpu.CF=0;
	cpu.SF=r>>len;
	cpu.ZF=!r;
	r^=r>>4;
	r^=r>>2;
	r^=r>>1;
	cpu.PF=!(r&1);
	print_asm_no_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"
