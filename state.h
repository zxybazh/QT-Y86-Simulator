#ifndef STATE_H
#define STATE_H
#include <QMainWindow>
#include <QImage>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QMainWindow>
#include <map>

class Simulator;

class State {
public:
	State();
	int  INOP;
	int  IHALT;
	int  IRRMOVL;
	int  IIRMOVL;
	int  IRMMOVL;
	int  IMRMOVL;
	int  IOPL;
	int  IJXX;
	int  ICALL;
	int  IRET;
	int  IPUSHL;
	int  IPOPL;
	int  FNONE;
	int  RESP;
	int  RNONE;
	int  ALUADD;
	int  SBUB;
	int  SAOK;
	int  SADR;
	int  SINS;
	int  SHLT;

	int  F_predPC;

	int  imem_icode;
	int  imem_ifun;
	int  f_pc;
	int  f_icode;
	int  f_ifun;
	int  f_valC;
	int  f_valP;
	int  f_stat;
	int  f_rA;
	int  f_rB;
	bool  imem_error;
	bool  instr_valid;

	int  D_stat;
	int  D_icode;
	int  D_ifun;
	int  D_rA;
	int  D_rB;
	int  D_valC;
	int  D_valP;

	int  d_srcA;
	int  d_srcB;
	int  d_rvalA;
	int  d_rvalB;
	int  d_valA;
	int  d_valB;
	int  d_dstE;
	int  d_dstM;

	int  E_stat;
	int  E_icode;
	int  E_ifun;
	int  E_valC;
	int  E_valA;
	int  E_valB;
	int  E_dstE;
	int  E_dstM;
	int  E_srcA;
	int  E_srcB;

	int  e_valE;
	bool  e_Cnd;
	int  e_dstE;

	int  M_stat;
	int  M_icode;
	int  M_ifun;
	int  M_valE;
	int  M_valA;
	int  M_dstE;
	int  M_dstM;
	bool  M_Cnd;

	bool  dmem_error;
	int  m_valM;
	int  m_stat;

	int  W_stat;
	int  W_icode;
	int  W_valE;
	int  W_valM;
	int  W_dstE;
	int  W_dstM;
	int  w_dstE;
	int  w_valE;
	int  w_dstM;
	int  w_valM;

	bool F_stall, F_bubble;
	bool D_stall, D_bubble;
	bool E_stall, E_bubble;
	bool M_stall, M_bubble;
	bool W_stall, W_bubble;

	Simulator *Father;
	void init();
	int getRegister(int);
	void putRegister(int, int);
	int eax, ebx, ecx, edx, esi, edi, esp, ebp;
	int ZF, SF, OF;
	int Stat, PC;

	void Fetch();
	void Decode();
	void Execute();
	void Memory();
	void Write();
	void Register_Control();
	bool EndOfProgram();
	int ALU(int, int, int, bool);
	bool getCnd(int);

	bool inside(int, int*, int);

	void fetch(int, int, int&, bool&);
	void write(int, int, int, bool&);
	std::map<int, int> mem;
};
#include "simulator.h"
#endif // STATE_H
