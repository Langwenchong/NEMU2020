#include "cpu/exec/template-start.h"

#define instr scas

make_helper(concat(scas_n_,SUFFIX)){
	swaddr_t dest=REG(R_EAX);
	swaddr_t src=MEM_R(reg_l(R_EDI));
	DATA_TYPE r=dest-src;
	if(cpu.DF==0){ reg_l(R_EDI)+=DATA_BYTE;}
	else{ reg_l(R_EDI)-=DATA_BYTE;}
	cpu.CF=src>dest;
	cpu.SF=MSB(r);
	cpu.ZF=!r;
	int s1=MSB(dest);
	int s2=MSB(src);
	cpu.OF=(s1!=s2 && s2==cpu.SF);
	r^=r>>4;
	r^=r>>2;
	r^=r>>1;
	cpu.PF=!(r&1);
	print_asm("scas%s",str(SUFFIX));
	return 1;
}

#include "cpu/exec/template-end.h"
