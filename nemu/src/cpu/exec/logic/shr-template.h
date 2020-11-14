#include "cpu/exec/template-start.h"

#define instr shr

static void do_execute () {
	DATA_TYPE src = op_src->val;
	DATA_TYPE dest = op_dest->val;

	uint8_t count = src & 0x1f;
	dest >>= count;
	OPERAND_W(op_dest, dest);
	DATA_TYPE r=dest;
	/* TODO: Update EFLAGS. */
//	panic("please implement me");
//	concat(update_,SUFFIX)(dest);
	cpu.CF=0;
	cpu.OF=0;
	cpu.ZF=!r;
	cpu.SF=MSB(r);
	r^=r>>4;
	r^=r>>2;
	r^=r>>1;
	cpu.PF=!(r&1);
	print_asm_template2();
}

make_instr_helper(rm_1)
make_instr_helper(rm_cl)
make_instr_helper(rm_imm)

#include "cpu/exec/template-end.h"
