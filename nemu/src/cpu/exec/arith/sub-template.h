#include "cpu/exec/template-start.h"

#define instr sub

static void do_execute(){
	DATA_TYPE r=op_dest->val-op_src->val;
	int len=(DATA_BYTE<<3)-1;
	cpu.CF=op_dest->val<op_src->val;
	cpu.SF=r>>len;
	int s1=op_dest->val>>len;
	int s2=op_src->val>>len;
	cpu.OF=(s1!=s2&&s2==cpu.SF);
	cpu.ZF=!r;
	OPERAND_W(op_dest,r);
	r^=r>>4;
	r^=r>>2;
	r^=r>>1;
	cpu.PF=!(r&1);
	print_asm_no_template2();
}

#if DATA_BYTE==2||DATA_BYTE==4
make_instr_helper(si2rm)
#endif

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
