#define REG_STATUS_CARRY        0
#define REG_STATUS_ZERO         1
#define REG_STATUS_IRQ          2
#define REG_STATUS_DEC          3
#define REG_STATUS_BRK          4
#define REG_STATUS_RESERVE      5 
#define REG_STATUS_OVERFLOW     6 
#define REG_STATUS_NEGATIVE     7

#define STATUS_C    0
#define STATUS_Z    1
#define STATUS_I    2 
#define STATUS_D    3
#define STATUS_B    4 
#define STATUS_R    5
#define STATUS_V    6 
#define STATUS_N    7 
const char* status_str[8] = 
{"CARRY", "ZERO", "INTERRUPT", "DECIMAL", "UNUSED", "BREAK", "OVERFLOW", "NEGATIVE"};

#define VEC_NMI     0xFFFA
#define VEC_RESET   0xFFFC
#define VEC_IRQ     0xFFFE
#define VEC_BRK     0xFFFE

#define ADDR_WRAM                 0x0000
#define ADDR_WRAM_MIRROR          0x0800
#define ADDR_PPU_REGS             0x2000
#define ADDR_PPU_REGS_MIRROR      0x2008
#define ADDR_APU_IO_PAD           0x4000
#define ADDR_EXT_ROM              0x4020
#define ADDR_EXT_RAM              0x6000
#define ADDR_PRG_ROM1             0x8000
#define ADDR_PRG_ROM2             0xC000

// addressing mode
#define AM_IMPL            0
#define AM_ACCM            1
#define AM_IMMD            2
#define AM_ZPG             3
#define AM_ZPGX            4
#define AM_ZPGY            5
#define AM_ABS             6
#define AM_ABSX            7
#define AM_ABSY            8
#define AM_REL             9
#define AM_XIND            10            
#define AM_INDY            11            
#define AM_IND             12
#define AM_UNDF            0xFF

// opcode 
#define OP_ADC             0
#define OP_SBC             1
#define OP_AND             2
#define OP_ORA             3
#define OP_EOR             4
#define OP_ASL             5
#define OP_LSR             6
#define OP_ROL             7
#define OP_ROR             8
#define OP_BCC             9
#define OP_BCS             10
#define OP_BEQ             11
#define OP_BNE             12
#define OP_BVC             13
#define OP_BVS             14
#define OP_BPL             15
#define OP_BMI             16
#define OP_BIT             17
#define OP_JMP             18
#define OP_JSR             19
#define OP_RTS             20
#define OP_BRK             21
#define OP_RTI             22
#define OP_CMP             23
#define OP_CPX             24
#define OP_CPY             25
#define OP_INC             26
#define OP_DEC             27
#define OP_INX             28
#define OP_DEX             29
#define OP_INY             30
#define OP_DEY             31
#define OP_CLC             32
#define OP_SEC             33
#define OP_CLI             34
#define OP_SEI             35
#define OP_CLD             36
#define OP_SED             37
#define OP_CLV             38
#define OP_LDA             39
#define OP_LDX             40
#define OP_LDY             41
#define OP_STA             42
#define OP_STX             43
#define OP_STY             44
#define OP_TAX             45
#define OP_TXA             46
#define OP_TAY             47
#define OP_TYA             48
#define OP_TSX             49
#define OP_TXS             50
#define OP_PHA             51
#define OP_PLA             52
#define OP_PHP             53
#define OP_PLP             54
#define OP_NOP             55
#define OP_UND             0xff


const uint8_t AM[0x100] = { 
/* 0x00 */ AM_IMPL, AM_XIND, AM_UNDF, AM_UNDF, AM_UNDF,  AM_ZPG,  AM_ZPG, AM_UNDF,
/* 0x08 */ AM_IMPL, AM_IMMD, AM_ACCM, AM_UNDF, AM_UNDF,  AM_ABS,  AM_ABS, AM_UNDF,
/* 0x10 */  AM_REL, AM_INDY, AM_UNDF, AM_UNDF, AM_UNDF, AM_ZPGX, AM_ZPGX, AM_UNDF,
/* 0x18 */ AM_IMPL, AM_ABSY, AM_UNDF, AM_UNDF, AM_UNDF, AM_ABSX, AM_ABSX, AM_UNDF, 
/* 0x20 */  AM_ABS, AM_XIND, AM_UNDF, AM_UNDF,  AM_ZPG,  AM_ZPG,  AM_ZPG, AM_UNDF, 
/* 0x28 */ AM_IMPL, AM_IMMD, AM_ACCM, AM_UNDF,  AM_ABS,  AM_ABS,  AM_ABS, AM_UNDF, 
/* 0x30 */  AM_REL, AM_INDY, AM_UNDF, AM_UNDF, AM_UNDF, AM_ZPGX, AM_ZPGX, AM_UNDF, 
/* 0x38 */ AM_IMPL, AM_ABSY, AM_UNDF, AM_UNDF, AM_UNDF, AM_ABSX, AM_ABSX, AM_UNDF, 
/* 0x40 */ AM_IMPL, AM_XIND, AM_UNDF, AM_UNDF, AM_UNDF,  AM_ZPG,  AM_ZPG, AM_UNDF, 
/* 0x48 */ AM_IMPL, AM_IMMD, AM_ACCM, AM_UNDF,  AM_ABS,  AM_ABS,  AM_ABS, AM_UNDF, 
/* 0x50 */  AM_REL, AM_INDY, AM_UNDF, AM_UNDF, AM_UNDF, AM_ZPGX, AM_ZPGX, AM_UNDF, 
/* 0x58 */ AM_IMPL, AM_ABSY, AM_UNDF, AM_UNDF, AM_UNDF, AM_ABSX, AM_ABSX, AM_UNDF, 
/* 0x60 */ AM_IMPL, AM_XIND, AM_UNDF, AM_UNDF, AM_UNDF,  AM_ZPG,  AM_ZPG, AM_UNDF, 
/* 0x68 */ AM_IMPL, AM_IMMD, AM_ACCM, AM_UNDF,  AM_IND,  AM_ABS,  AM_ABS, AM_UNDF, 
/* 0x70 */  AM_REL, AM_INDY, AM_UNDF, AM_UNDF, AM_UNDF, AM_ZPGX, AM_ZPGX, AM_UNDF, 
/* 0x78 */ AM_IMPL, AM_ABSY, AM_UNDF, AM_UNDF, AM_UNDF, AM_ABSX, AM_ABSX, AM_UNDF, 
/* 0x80 */ AM_UNDF, AM_XIND, AM_UNDF, AM_UNDF,  AM_ZPG,  AM_ZPG,  AM_ZPG, AM_UNDF, 
/* 0x88 */ AM_IMPL, AM_UNDF, AM_IMPL, AM_UNDF,  AM_ABS,  AM_ABS,  AM_ABS, AM_UNDF, 
/* 0x90 */  AM_REL, AM_INDY, AM_UNDF, AM_UNDF, AM_ZPGX, AM_ZPGX, AM_ZPGY, AM_UNDF, 
/* 0x98 */ AM_IMPL, AM_ABSY, AM_IMPL, AM_UNDF, AM_UNDF, AM_ABSX, AM_UNDF, AM_UNDF, 
/* 0xA0 */ AM_IMMD, AM_XIND, AM_IMMD, AM_UNDF,  AM_ZPG,  AM_ZPG,  AM_ZPG, AM_UNDF, 
/* 0xA8 */ AM_IMPL, AM_IMMD, AM_IMPL, AM_UNDF,  AM_ABS,  AM_ABS,  AM_ABS, AM_UNDF, 
/* 0xB0 */  AM_REL, AM_INDY, AM_UNDF, AM_UNDF, AM_ZPGX, AM_ZPGX, AM_ZPGY, AM_UNDF, 
/* 0xB8 */ AM_IMPL, AM_ABSY, AM_IMPL, AM_UNDF, AM_ABSX, AM_ABSX, AM_ABSY, AM_UNDF, 
/* 0xC0 */ AM_IMMD, AM_XIND, AM_UNDF, AM_UNDF,  AM_ZPG,  AM_ZPG,  AM_ZPG, AM_UNDF, 
/* 0xC8 */ AM_IMPL, AM_IMMD, AM_IMPL, AM_UNDF,  AM_ABS,  AM_ABS,  AM_ABS, AM_UNDF, 
/* 0xD0 */  AM_REL, AM_INDY, AM_UNDF, AM_UNDF, AM_UNDF, AM_ZPGX, AM_ZPGX, AM_UNDF, 
/* 0xD8 */ AM_IMPL, AM_ABSY, AM_UNDF, AM_UNDF, AM_UNDF, AM_ABSX, AM_ABSX, AM_UNDF, 
/* 0xE0 */ AM_IMMD, AM_XIND, AM_UNDF, AM_UNDF,  AM_ZPG,  AM_ZPG,  AM_ZPG, AM_UNDF, 
/* 0xE8 */ AM_IMPL, AM_IMMD, AM_IMPL, AM_UNDF,  AM_ABS,  AM_ABS,  AM_ABS, AM_UNDF, 
/* 0xF0 */  AM_REL, AM_INDY, AM_UNDF, AM_UNDF, AM_UNDF, AM_ZPGX, AM_ZPGX, AM_UNDF, 
/* 0xF8 */ AM_IMPL, AM_ABSY, AM_UNDF, AM_UNDF, AM_UNDF, AM_ABSX, AM_ABSX, AM_UNDF 
};

const uint8_t OPCODE[0x100] = { 
/* 0x00 */ OP_BRK, OP_ORA, OP_UND, OP_UND, OP_UND, OP_ORA, OP_ASL, OP_UND,
/* 0x08 */ OP_PHP, OP_ORA, OP_ASL, OP_UND, OP_UND, OP_ORA, OP_ASL, OP_UND,
/* 0x10 */ OP_BPL, OP_ORA, OP_UND, OP_UND, OP_UND, OP_ORA, OP_ASL, OP_UND,
/* 0x18 */ OP_CLC, OP_ORA, OP_UND, OP_UND, OP_UND, OP_ORA, OP_ASL, OP_UND,
/* 0x20 */ OP_JSR, OP_AND, OP_UND, OP_UND, OP_BIT, OP_AND, OP_ROL, OP_UND,
/* 0x28 */ OP_PLP, OP_AND, OP_ROL, OP_UND, OP_BIT, OP_AND, OP_ROL, OP_UND,
/* 0x30 */ OP_BMI, OP_AND, OP_UND, OP_UND, OP_UND, OP_AND, OP_ROL, OP_UND,
/* 0x38 */ OP_SEC, OP_AND, OP_UND, OP_UND, OP_UND, OP_AND, OP_ROL, OP_UND,
/* 0x40 */ OP_RTI, OP_EOR, OP_UND, OP_UND, OP_UND, OP_EOR, OP_LSR, OP_UND,
/* 0x48 */ OP_PHA, OP_EOR, OP_LSR, OP_UND, OP_JMP, OP_EOR, OP_LSR, OP_UND,
/* 0x50 */ OP_BVC, OP_EOR, OP_UND, OP_UND, OP_UND, OP_EOR, OP_LSR, OP_UND,
/* 0x58 */ OP_CLI, OP_EOR, OP_UND, OP_UND, OP_UND, OP_EOR, OP_LSR, OP_UND,
/* 0x60 */ OP_RTS, OP_ADC, OP_UND, OP_UND, OP_UND, OP_ADC, OP_ROR, OP_UND,
/* 0x68 */ OP_PLA, OP_ADC, OP_ROR, OP_UND, OP_JMP, OP_ADC, OP_ROR, OP_UND,
/* 0x70 */ OP_BVS, OP_ADC, OP_UND, OP_UND, OP_UND, OP_ADC, OP_ROR, OP_UND,
/* 0x78 */ OP_SEI, OP_ADC, OP_UND, OP_UND, OP_UND, OP_ADC, OP_ROR, OP_UND,
/* 0x80 */ OP_UND, OP_STA, OP_UND, OP_UND, OP_STY, OP_STA, OP_STX, OP_UND,
/* 0x88 */ OP_DEY, OP_UND, OP_TXA, OP_UND, OP_STY, OP_STA, OP_STX, OP_UND,
/* 0x90 */ OP_BCC, OP_STA, OP_UND, OP_UND, OP_STY, OP_STA, OP_STX, OP_UND,
/* 0x98 */ OP_TYA, OP_STA, OP_TXS, OP_UND, OP_UND, OP_STA, OP_UND, OP_UND,
/* 0xA0 */ OP_LDY, OP_LDA, OP_LDX, OP_UND, OP_LDY, OP_LDA, OP_LDX, OP_UND,
/* 0xA8 */ OP_TAY, OP_LDA, OP_TAX, OP_UND, OP_LDY, OP_LDA, OP_LDX, OP_UND,
/* 0xB0 */ OP_BCS, OP_LDA, OP_UND, OP_UND, OP_LDY, OP_LDA, OP_LDX, OP_UND,
/* 0xB8 */ OP_CLV, OP_LDA, OP_TSX, OP_UND, OP_LDY, OP_LDA, OP_LDX, OP_UND,
/* 0xC0 */ OP_CPY, OP_CMP, OP_UND, OP_UND, OP_CPY, OP_CMP, OP_DEC, OP_UND,
/* 0xC8 */ OP_INY, OP_CMP, OP_DEX, OP_UND, OP_CPY, OP_CMP, OP_DEC, OP_UND,
/* 0xD0 */ OP_BNE, OP_CMP, OP_UND, OP_UND, OP_UND, OP_CMP, OP_DEC, OP_UND,
/* 0xD8 */ OP_CLD, OP_CMP, OP_UND, OP_UND, OP_UND, OP_CMP, OP_DEC, OP_UND,
/* 0xE0 */ OP_CPX, OP_SBC, OP_UND, OP_UND, OP_CPX, OP_SBC, OP_INC, OP_UND,
/* 0xE8 */ OP_INX, OP_SBC, OP_NOP, OP_UND, OP_CPX, OP_SBC, OP_INC, OP_UND,
/* 0xF0 */ OP_BEQ, OP_SBC, OP_UND, OP_UND, OP_UND, OP_SBC, OP_INC, OP_UND,
/* 0xF8 */ OP_SED, OP_SBC, OP_UND, OP_UND, OP_UND, OP_SBC, OP_INC, OP_UND
};

void init_cpu();
void init_regs();
void reset();
uint8_t fetch();
uint8_t get_status_at(uint8_t);
void set_reg_P(uint8_t, bool);
bool check_overflow(uint8_t, uint8_t, uint8_t);
uint16_t get_operand(uint8_t);
void run();

void debug();
void dump(uint16_t, int32_t);