#include "cpu/exec/template-start.h"

#define instr sar

static void do_execute () {
	DATA_TYPE src = op_src->val;
	DATA_TYPE_S dest = op_dest->val;
	DATA_TYPE_S r=op_dest->val;
	uint8_t count = src & 0x1f;
	dest>>=count;
	r=dest;
	int len=(DATA_BYTE<<3)-1;
	cpu.CF=0;
	cpu.OF=0;
	cpu.SF=r>>len;
	cpu.ZF=!r;
	OPERAND_W(op_dest, dest);
	r^=r>>4;
	r^=r>>2;
	r^=r>>1;
	cpu.PF=!(r&1);
	/* TODO: Update EFLAGS. */
	//panic("please implement me");

	print_asm_template2();
}

make_instr_helper(rm_1)
make_instr_helper(rm_cl)
make_instr_helper(rm_imm)

#include "cpu/exec/template-end.h"
