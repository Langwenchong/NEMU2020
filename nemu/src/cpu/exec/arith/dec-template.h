#include "cpu/exec/template-start.h"

#define instr dec

static void do_execute () {
	DATA_TYPE r = op_src->val - 1;
	int len=(DATA_BYTE<<3)-1;
	cpu.CF=op_src->val<1;
	cpu.SF=r>>len;
	int s1=op_src->val>>len;
	int s2=0;
	cpu.OF=(s1!=s2&&s2==cpu.SF);
	cpu.ZF=!r;
	OPERAND_W(op_src, r);
	/* TODO: Update EFLAGS. */
//	panic("please implement me");
	r^=r>>4;
	r^=r>>2;
	r^=r>>1;
	cpu.PF=!(r&1);
	print_asm_template1();
}

make_instr_helper(rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
