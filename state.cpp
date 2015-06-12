#include "state.h"
#include "simulator.h"
#include <QVariant>
#include <cstring>

State::State() {
	init();
}

void State::init() {
	mem.clear();
	Stat = 0;
	INOP = 0;
	IHALT = 0;
	IRRMOVL = 0;
	IIRMOVL = 0;
	IRMMOVL = 0;
	IMRMOVL = 0;
	IOPL = 0;
	IJXX = 0;
	ICALL = 0;
	IRET = 0;
	IPUSHL = 0;
	IPOPL = 0;
	FNONE = 0;
	RESP = 0;
	RNONE = 0;
	ALUADD = 0;
	SBUB = 0;
	SAOK = 0;
	SADR = 0;
	SINS = 0;
	SHLT = 0;
	F_predPC = 0;
	imem_icode = 0;
	imem_ifun = 0;
	f_pc = 0;
	f_icode = 0;
	f_ifun = 0;
	f_valC = 0;
	f_valP = 0;
	f_stat = 1;
	f_rA = 0;
	f_rB = 0;
	imem_error = 0;
	instr_valid = 0;
	D_stat = 1;
	D_icode = 0;
	D_ifun = 0;
	D_rA = 0;
	D_rB = 0;
	D_valC = 0;
	D_valP = 0;
	d_srcA = 0;
	d_srcB = 0;
	d_rvalA = 0;
	d_rvalB = 0;
	d_valA = 0;
	d_valB = 0;
	d_dstE = 0;
	d_dstM = 0;
	E_stat = 1;
	E_icode = 0;
	E_ifun = 0;
	E_valC = 0;
	E_valA = 0;
	E_valB = 0;
	E_dstE = 0;
	E_dstM = 0;
	E_srcA = 0;
	E_srcB = 0;
	e_valE = 0;
	e_Cnd = 0;
	e_dstE = 0;
	M_stat = 1;
	M_icode = 0;
	M_ifun = 0;
	M_valE = 0;
	M_valA = 0;
	M_dstE = 0;
	M_dstM = 0;
	M_Cnd = 0;
	dmem_error = 0;
	m_valM = 0;
	m_stat = 1;
	W_stat = 1;
	W_icode = 0;
	W_valE = 0;
	W_valM = 0;
	W_dstE = 0;
	W_dstM = 0;
	F_stall = 0;
	F_bubble = 0;
	D_stall = 0;
	D_bubble = 0;
	E_stall = 0;
	E_bubble = 0;
	M_stall = 0;
	M_bubble = 0;
	W_stall = 0;
	W_bubble = 0;
	w_dstE = 0;
	w_valE = 0;
	w_dstM = 0;
	w_valM = 0;


	INOP = 1;
	IHALT = 0;
	IRRMOVL = 2;
	IIRMOVL = 3;
	IRMMOVL = 4;
	IMRMOVL = 5;
	IOPL = 6;
	IJXX = 7;
	ICALL = 8;
	IRET = 9;
	IPUSHL = 10;
	IPOPL = 11;

	FNONE = 0;
	RESP = 4;

	RNONE = 0xf;
	ALUADD = 0;

	SAOK = 1;
	SADR = 2;
	SINS = 3;
	SHLT = 4;

	SBUB = 0;

	eax = ebx = ecx = edx = esi = edi = esp = ebp = 0;
	esp = 1<<30;

	ZF = SF = OF = 0;
	PC = 0;
	Stat = 1;
}


void State::fetch(int p, int l, int &ans, bool &mem_error) {
	mem_error = false;
	ans = 0;
	for (int i = l-1; i >= 0; i--) {
		ans = (ans << 8) + mem[p+i];
	}
}

void State::write(int p, int l, int x, bool &mem_error) {
	mem_error = false;
	for (int i = 0; i < l; i++) {
		mem[p+i] = x & 0xff;
		x >>= 8;
	}
}

bool State::inside(int x, int *a, int n) {
	for (int i = 0; i < n; i++)
		if (a[i] == x) return true;
	return false;
}

int State::getRegister(int src) {
	if (src == 0) return eax;
	if (src == 1) return ecx;
	if (src == 2) return edx;
	if (src == 3) return ebx;
	if (src == 4) return esp;
	if (src == 5) return ebp;
	if (src == 6) return esi;
	if (src == 7) return edi;
	return 0xf;
}

void State::putRegister(int src, int val) {
	if (src == 0) eax = val;
	if (src == 1) ecx = val;
	if (src == 2) edx = val;
	if (src == 3) ebx = val;
	if (src == 4) esp = val;
	if (src == 5) ebp = val;
	if (src == 6) esi = val;
	if (src == 7) edi = val;
}

bool State::EndOfProgram() {
	return W_stat == SHLT;
}

void State::Fetch() {

	if (F_stall) return;

	if (M_icode == IJXX && !M_Cnd)
		f_pc = M_valA;
	// Mispredicted branch. Fetch at incremented PC
	else if (W_icode == IRET)
		f_pc = W_valM;
	// Completion of RET instruction.
	else
		f_pc = F_predPC;
	// Default: Use predicted value of PC

	fetch(PC = f_pc, 1, imem_icode, imem_error);
	imem_ifun = imem_icode & 0xf;
	imem_icode >>= 4;

	f_valP = f_pc + 1;

	//Determine icode of fetched instruction
	if (imem_error)
		f_icode = INOP;
	else
		f_icode = imem_icode;

	// Determine ifun
	if (imem_error)
		f_ifun = FNONE;
	else
		f_ifun = imem_ifun;

	//Is instruction valid?
	int a[] = { INOP, IHALT, IRRMOVL, IIRMOVL, IRMMOVL, IMRMOVL,IOPL, IJXX, ICALL, IRET, IPUSHL, IPOPL };
	instr_valid = inside(f_icode, a, sizeof(a)/sizeof(int));

	// Determine status code for fetched instruction
	if (imem_error)
		f_stat = SADR;
	else if (!instr_valid)
		f_stat = SINS;
	else if (f_icode == IHALT)
		f_stat = SHLT;
	else
		f_stat = SAOK;

	// Does fetched instruction require a regid byte?

	int b[] = { IRRMOVL, IOPL, IPUSHL, IPOPL, IIRMOVL, IRMMOVL, IMRMOVL };
	bool need_regids = inside(f_icode, b, sizeof(b)/sizeof(int));
	if (need_regids) {
		fetch(f_valP, 1, f_rA, imem_error);
		f_rB = f_rA & 0xf;
		f_rA >>= 4;
		f_valP ++;
	}

	// Does fetched instruction require a constant word?
	int c[] = { IIRMOVL, IRMMOVL, IMRMOVL, IJXX, ICALL };
	bool need_valC = inside(f_icode, c, sizeof(c)/sizeof(int));
	if (need_valC) {
		fetch(f_valP, 4, f_valC, imem_error);
		f_valP += 4;
	}

	// Predict next value of PC
	int d[] = { IJXX, ICALL };
	if (inside(f_icode, d, sizeof(d)/sizeof(int)))
		F_predPC = f_valC;
	else
		F_predPC = f_valP;
}


void State::Decode() {

	D_stat = f_stat;
	D_ifun = f_ifun;
	D_icode = f_icode;
	D_valP = f_valP;
	D_valC = f_valC;
	D_rA = f_rA;
	D_rB = f_rB;

	if (D_stall) return;
	if (D_bubble) {
		D_stat = SBUB;
		D_icode = 1;
		D_ifun = 0;
		d_srcA = 0;
		d_srcB = 0;
		d_valA = 0;
		d_valB = 0;
		d_dstE = 0;
		d_dstM = 0;
		return;
	}

	// What register should be used as the A source?
	int a[] = { IRRMOVL, IRMMOVL, IOPL, IPUSHL };
	int b[] = { IPOPL, IRET };
	if (inside(D_icode, a, sizeof(a)/sizeof(int)))
		d_srcA = D_rA;
	else if (inside(D_icode, b, sizeof(b)/sizeof(int)))
		d_srcA = RESP;
	else
		d_srcA = RNONE;
	// Don’t need register

	d_rvalA = getRegister(d_srcA);

	// What register should be used as the B source?

	int c[] = { IOPL, IRMMOVL, IMRMOVL };
	int d[] = { IPUSHL, IPOPL, ICALL, IRET };
	if (inside(D_icode, c, sizeof(c)/sizeof(int)))
		d_srcB = D_rB;
	else if (inside(D_icode, d, sizeof(d)/sizeof(int)))
		d_srcB = RESP;
	else
		d_srcB = RNONE;
	// Don’t need register
	d_rvalB = getRegister(d_srcB);

	// What register should be used as the E destination?
	int e[] = { IRRMOVL, IIRMOVL, IOPL};
	int f[] = { IPUSHL, IPOPL, ICALL, IRET };
	if (inside(D_icode, e, sizeof(e)/sizeof(int)))
		d_dstE = D_rB;
	else if (inside(D_icode, f, sizeof(f)/sizeof(int)))
		d_dstE = RESP;
	else
		d_dstE = RNONE;
	// Don’t write any register

	// What register should be used as the M destination?
	int g[] = { IMRMOVL, IPOPL };
	if (inside(D_icode, g, sizeof(g)/sizeof(int)))
		d_dstM = D_rA;
	else
		d_dstM = RNONE;
	//  Don’t write any register

	// What should be the A value?
	// Forward into decode stage for valA


	int h[] = { ICALL, IJXX };
	if (inside(D_icode, h, sizeof(h)/sizeof(int)))
		d_valA = D_valP;
	// Use incremented PC
	else if (d_srcA == e_dstE)
		d_valA = e_valE;
	// Forward valE from execute
	else if(d_srcA == M_dstM)
		d_valA = m_valM;
	// Forward valM from memory
	else if (d_srcA == M_dstE)
		d_valA = M_valE;
	// Forward valE from memory
	else if (d_srcA == W_dstM)
		d_valA = W_valM;
	// Forward valM from write back
	else if (d_srcA == W_dstE)
		d_valA = W_valE;
	// Forward valE from write back
	else
		d_valA = d_rvalA;
	// Use value read from register file

	if (d_srcB == e_dstE)
		d_valB = e_valE;
	// Forward valE from execute
	else if (d_srcB == M_dstM)
		d_valB = m_valM;
	// Forward valM from memory
	else if (d_srcB == M_dstE)
		d_valB = M_valE;
	// Forward valE from memory
	else if (d_srcB == W_dstM)
		d_valB = W_valM;
	// Forward valM from write back
	else if (d_srcB == W_dstE)
		d_valB = W_valE;
	// Forward valE from write back
	else
		d_valB = d_rvalB;
	// Use value read from register file

}

int State::ALU(int aluA, int aluB, int alufun, bool set_cc) {
	int ans;
	if (alufun == 0)
		ans = aluA + aluB;
	else if (alufun == 1)
		ans = aluA - aluB;
	else if (alufun == 2)
		ans = aluA & aluB;
	else if (alufun == 3)
		ans = aluA ^ aluB;
	if (set_cc) {
		Father->setConditionCode(aluA, aluB, ans);
	}
	return ans;
}

bool State::getCnd(int ifun) {
	if (ifun == 0) return true;
	if (ifun == 1) return (SF^OF) | ZF;
	if (ifun == 2) return (SF^OF);
	if (ifun == 3) return ZF;
	if (ifun == 4) return !ZF;
	if (ifun == 5) return !(SF^OF);
	if (ifun == 6) return !(SF^OF) & !ZF;
	return false;
}

void State::Execute() {

	E_stat = D_stat;
	E_icode = D_icode;
	E_ifun = D_ifun;

	E_valC = D_valC;
	E_dstE = d_dstE;
	E_dstM = d_dstM;

	E_srcA = d_srcA;
	E_srcB = d_srcB;

	E_valA = d_valA;
	E_valB = d_valB;

	if (E_bubble) {
		E_stat = SBUB;
		E_icode = 1;
		E_ifun = 0;
		e_valE = 0;
		E_dstM = 0;
		e_Cnd = 0;
		E_valA = 0;
		e_dstE = 0;
		return;
	}

	// Select input A to ALU
	int a[] = { IRRMOVL, IOPL };
	int b[] = { IIRMOVL, IRMMOVL, IMRMOVL };
	int c[] = { ICALL, IPUSHL };
	int d[] = { IRET, IPOPL };
	int aluA = 0;
	if (inside(E_icode, a, sizeof(a)/sizeof(int)))
		aluA = E_valA;
	else if (inside(E_icode, b, sizeof(b)/sizeof(int)))
		aluA = E_valC;
	else if (inside(E_icode, c, sizeof(c)/sizeof(int)))
		aluA =-4;
	else if (inside(E_icode, d, sizeof(d)/sizeof(int)))
		aluA = 4;
	// Other instructions don’t need ALU

	// Select input B to ALU
	int e[] = { IRMMOVL, IMRMOVL, IOPL, ICALL, IPUSHL, IRET, IPOPL };
	int f[] = { IRRMOVL, IIRMOVL };
	int aluB = 0;
	if (inside(E_icode, e, sizeof(e)/sizeof(int)))
		aluB = E_valB;
	else if (inside(E_icode, f, sizeof(f)/sizeof(int)))
		aluB = 0;
	// Other instructions don’t need ALU

	// Set the ALU function
	int  alufun;
	if (E_icode == IOPL)
		alufun = E_ifun;
	else
		alufun = ALUADD;

	// Should the condition codes be updated?
	int g[] = { SADR, SINS, SHLT };
	int h[] =  { SADR, SINS, SHLT };
	bool set_cc = (E_icode == IOPL) && (!inside(m_stat, g, sizeof(g)/sizeof(int))) && (!inside(W_stat, h, sizeof(h)/sizeof(int)));
	// State changes only during normal operation
	e_valE = ALU(aluA, aluB, alufun, set_cc);
	e_Cnd = getCnd(E_ifun);

	// Generate valA in execute stage
	//int e_valA = E_valA;
	// Pass valA through stage

	// Set dstE to RNONE in event of not-taken conditional move
	if (E_icode == IRRMOVL && !e_Cnd)
		e_dstE = RNONE;
	else
		e_dstE = E_dstE;
}

void State::Memory() {
	M_stat = E_stat;
	M_icode = E_icode;
	M_Cnd = getCnd(E_ifun);
	M_valE = e_valE;
	M_valA = E_valA;
	M_dstE = e_dstE;
	M_dstM = E_dstM;

	if (M_bubble) {
		M_stat = SBUB;
		M_icode = 1;
		M_ifun = 0;

		w_valE = 0;
		m_valM = 0;
		w_dstE = 0;
		w_dstM = 0;
		return;
	}

	int a[] = { IRMMOVL, IPUSHL, ICALL, IMRMOVL };
	int b[] = { IPOPL, IRET };
	// Select memory address
	int mem_addr;
	if (inside(M_icode, a, sizeof(a)/sizeof(int)))
		mem_addr = M_valE;
	else if (inside(M_icode, b, sizeof(b)/sizeof(int)))
		mem_addr = M_valA;
	// Other instructions don’t need address

	// Set read control signal
	int c[] = { IMRMOVL, IPOPL, IRET };
	bool mem_read = inside(M_icode, c, sizeof(c)/sizeof(int));

	// Set write control signal
	int d[] = { IRMMOVL, IPUSHL, ICALL };
	bool mem_write = inside(M_icode, d, sizeof(d)/sizeof(int));
	if (mem_write) {
		write(mem_addr, 4, M_valA, dmem_error);
	}

	//* $begin pipe-m_stat-hcl */
	// Update the status
	if (dmem_error)
		m_stat = SADR;
	else
		m_stat = M_stat;
	//* $end pipe-m_stat-hcl */

	// Set E port register ID
	w_dstE = M_dstE;

	// Set E port value
	w_valE = M_valE;

	// Set M port register ID
	w_dstM = M_dstM;

	// Set M port value
	if (mem_read) {
		fetch(mem_addr, 4, m_valM, dmem_error);
	}
}

void State::Write() {
	W_icode = M_icode;
	W_stat = m_stat;
	W_valE = w_valE;
	W_valM = m_valM;
	W_dstE = w_dstE;
	W_dstM = w_dstM;

	// Update processor status
	if (W_stat == SBUB)
		Stat = SAOK;
	else
		Stat = W_stat;
	if (W_stall) return;

	putRegister(W_dstE, W_valE);
	putRegister(W_dstM, W_valM);
}

void State::Register_Control() {

	// Should I stall or inject a bubble into Pipeline Register F?
	// At most one of these can be true.
	int a[] = { IMRMOVL, IPOPL };
	int b[] = { d_srcA, d_srcB };
	int c[] = { D_icode, E_icode, M_icode };
	F_bubble = 0;
	F_stall = (inside(E_icode, a, sizeof(a)/sizeof(int))  && inside(E_dstM, b, sizeof(b)/sizeof(int)))  || inside(IRET, c, sizeof(c)/sizeof(int));
	// Conditions for a load/use hazard
	// Stalling at fetch while ret passes through pipeline

	// Should I stall or inject a bubble into Pipeline Register D?
	// At most one of these can be true.
	int d[] = { IMRMOVL, IPOPL };
	int e[] = { d_srcA, d_srcB };
	D_stall = inside(E_icode, d, sizeof(d)/sizeof(int)) && inside(E_dstM, e, sizeof(e)/sizeof(int));
	// Conditions for a load/use hazard
	int f[] = { IMRMOVL, IPOPL };
	int g[] = { d_srcA, d_srcB };
	int h[] = { D_icode, E_icode, M_icode };
	D_bubble = (E_icode == IJXX && !e_Cnd) || (!(inside(E_icode,f, sizeof(f)/sizeof(int))&& inside(E_dstM, g, sizeof(g)/sizeof(int))) && inside(IRET, h, sizeof(h)/sizeof(int)));
	// Mispredicted branch
	// Stalling at fetch while ret passes through pipeline
	// but not condition for a load/use hazard

	// Should I stall or inject a bubble into Pipeline Register E?
	// At most one of these can be true.
	int i[] = { IMRMOVL, IPOPL };
	int j[] = { d_srcA, d_srcB};
	E_stall = 0;
	E_bubble = (E_icode == IJXX && !e_Cnd) || (inside(E_icode, i, sizeof(i)/sizeof(int)) && inside(E_dstM, j, sizeof(j)/sizeof(int)));
	// Mispredicted branch
	// Conditions for a load/use hazard

	// Should I stall or inject a bubble into Pipeline Register M?
	// At most one of these can be true.
	M_stall = 0;
	// Start injecting bubbles as soon as exception passes through memory stage
	int k[] = { SADR, SINS, SHLT };
	int l[] = { SADR, SINS, SHLT };
	M_bubble = inside(m_stat, k, sizeof(k)/sizeof(int)) || inside(W_stat, l, sizeof(l)/sizeof(int));
	// Should I stall or inject a bubble into Pipeline Register W?
	int m[] = { SADR, SINS, SHLT };
	W_stall = inside(W_stat, m, sizeof(m)/sizeof(int));
	W_bubble = 0;
	//* $end pipe-all-hcl */
}
