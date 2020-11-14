#include "cpu/exec/template-start.h"

#define instr or

static void do_execute () {
	DATA_TYPE r = op_dest->val | op_src->val;
	int len=(DATA_BYTE<<3)-1;
	cpu.CF=0;
	cpu.OF=0;
	cpu.SF=r>>len;
	cpu.ZF=!r;
	OPERAND_W(op_dest, r);
	r^=r>>4;
	r^=r>>2;
	r^=r>>1;
	cpu.PF=!(r&1);
	/* TODO: Update EFLAGS. */
	//panic("please implement me");

	print_asm_template2();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
