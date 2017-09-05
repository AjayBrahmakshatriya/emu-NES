#include "nes_decoder.h"


NES_INSTRUCTION nes_instructions[256] = {
	{"BRK", AM_impl},
	{"ORA", AM_X_ind},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"ORA", AM_zpg},
	{"ASL", AM_zpg},
	{NULL , AM_impl},
	{"PHP", AM_impl},
	{"ORA", AM_hash},
	{"ASL", AM_A},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"ORA", AM_abs},
	{"ASL", AM_abs},
	{NULL , AM_impl},
	{"BPL", AM_rel},
	{"ORA", AM_ind_Y},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"ORA", AM_zpg_X},
	{"ASL", AM_zpg_X},
	{NULL , AM_impl},
	{"CLC", AM_impl},
	{"ORA", AM_abs_Y},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"ORA", AM_abs_X},
	{"ASL", AM_abs_X},
	{NULL , AM_impl},
	{"JSR", AM_abs},
	{"AND", AM_X_ind},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"BIT", AM_zpg},
	{"AND", AM_zpg},
	{"ROL", AM_zpg},
	{NULL , AM_impl},
	{"PLP", AM_impl},
	{"AND", AM_hash},
	{"ROL", AM_A},
	{NULL , AM_impl},
	{"BIT", AM_abs},
	{"AND", AM_abs},
	{"ROL", AM_abs},
	{NULL , AM_impl},
	{"BMI", AM_rel},
	{"AND", AM_ind_Y},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"AND", AM_zpg_X},
	{"ROL", AM_zpg_X},
	{NULL , AM_impl},
	{"SEC", AM_impl},
	{"AND", AM_abs_Y},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"AND", AM_abs_X},
	{"ROL", AM_abs_X},
	{NULL , AM_impl},
	{"RTI", AM_impl},
	{"EOR", AM_X_ind},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"EOR", AM_zpg},
	{"LSR", AM_zpg},
	{NULL , AM_impl},
	{"PHA", AM_impl},
	{"EOR", AM_hash},
	{"LSR", AM_A},
	{NULL , AM_impl},
	{"JMP", AM_abs},
	{"EOR", AM_abs},
	{"LSR", AM_abs},
	{NULL , AM_impl},
	{"BVC", AM_rel},
	{"EOR", AM_ind_Y},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"EOR", AM_zpg_X},
	{"LSR", AM_zpg_X},
	{NULL , AM_impl},
	{"CLI", AM_impl},
	{"EOR", AM_abs_Y},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"EOR", AM_abs_X},
	{"LSR", AM_abs_X},
	{NULL , AM_impl},
	{"RTS", AM_impl},
	{"ADC", AM_X_ind},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"ADC", AM_zpg},
	{"ROR", AM_zpg},
	{NULL , AM_impl},
	{"PLA", AM_impl},
	{"ADC", AM_hash},
	{"ROR", AM_A},
	{NULL , AM_impl},
	{"JMP", AM_ind},
	{"ADC", AM_abs},
	{"ROR", AM_abs},
	{NULL , AM_impl},
	{"BVS", AM_rel},
	{"ADC", AM_ind_Y},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"ADC", AM_zpg_X},
	{"ROR", AM_zpg_X},
	{NULL , AM_impl},
	{"SEI", AM_impl},
	{"ADC", AM_abs_Y},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"ADC", AM_abs_X},
	{"ROR", AM_abs_X},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"STA", AM_X_ind},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"STY", AM_zpg},
	{"STA", AM_zpg},
	{"STX", AM_zpg},
	{NULL , AM_impl},
	{"DEY", AM_impl},
	{NULL , AM_impl},
	{"TXA", AM_impl},
	{NULL , AM_impl},
	{"STY", AM_abs},
	{"STA", AM_abs},
	{"STX", AM_abs},
	{NULL , AM_impl},
	{"BCC", AM_rel},
	{"STA", AM_ind_Y},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"STY", AM_zpg_X},
	{"STA", AM_zpg_X},
	{"STX", AM_zpg_Y},
	{NULL , AM_impl},
	{"TYA", AM_impl},
	{"STA", AM_abs_Y},
	{"TXS", AM_impl},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"STA", AM_abs_X},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"LDY", AM_hash},
	{"LDA", AM_X_ind},
	{"LDX", AM_hash},
	{NULL , AM_impl},
	{"LDY", AM_zpg},
	{"LDA", AM_zpg},
	{"LDX", AM_zpg},
	{NULL , AM_impl},
	{"TAY", AM_impl},
	{"LDA", AM_hash},
	{"TAX", AM_impl},
	{NULL , AM_impl},
	{"LDY", AM_abs},
	{"LDA", AM_abs},
	{"LDX", AM_abs},
	{NULL , AM_impl},
	{"BCS", AM_rel},
	{"LDA", AM_ind_Y},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"LDY", AM_zpg_X},
	{"LDA", AM_zpg_X},
	{"LDX", AM_zpg_Y},
	{NULL , AM_impl},
	{"CLV", AM_impl},
	{"LDA", AM_abs_Y},
	{"TSX", AM_impl},
	{NULL , AM_impl},
	{"LDY", AM_abs_X},
	{"LDA", AM_abs_X},
	{"LDX", AM_abs_Y},
	{NULL , AM_impl},
	{"CPY", AM_hash},
	{"CMP", AM_X_ind},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"CPY", AM_zpg},
	{"CMP", AM_zpg},
	{"DEC", AM_zpg},
	{NULL , AM_impl},
	{"INY", AM_impl},
	{"CMP", AM_hash},
	{"DEX", AM_impl},
	{NULL , AM_impl},
	{"CPY", AM_abs},
	{"CMP", AM_abs},
	{"DEC", AM_abs},
	{NULL , AM_impl},
	{"BNE", AM_rel},
	{"CMP", AM_ind_Y},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"CMP", AM_zpg_X},
	{"DEC", AM_zpg_X},
	{NULL , AM_impl},
	{"CLD", AM_impl},
	{"CMP", AM_abs_Y},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"CMP", AM_abs_X},
	{"DEC", AM_abs_X},
	{NULL , AM_impl},
	{"CPX", AM_hash},
	{"SBC", AM_X_ind},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"CPX", AM_zpg},
	{"SBC", AM_zpg},
	{"INC", AM_zpg},
	{NULL , AM_impl},
	{"INX", AM_impl},
	{"SBC", AM_hash},
	{"NOP", AM_impl},
	{NULL , AM_impl},
	{"CPX", AM_abs},
	{"SBC", AM_abs},
	{"INC", AM_abs},
	{NULL , AM_impl},
	{"BEQ", AM_rel},
	{"SBC", AM_ind_Y},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"SBC", AM_zpg_X},
	{"INC", AM_zpg_X},
	{NULL , AM_impl},
	{"SED", AM_impl},
	{"SBC", AM_abs_Y},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{NULL , AM_impl},
	{"SBC", AM_abs_X},
	{"INC", AM_abs_X},
	{NULL , AM_impl}
};