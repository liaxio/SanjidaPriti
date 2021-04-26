
/*Name : Sanjida Firdaws
  CSc 210 ,Project 1
  Professor : Vulis
  Date : 11/20/2020
  */


#include <stdio.h>
#include <stdint.h>

#include <string.h>

#define RCODE int
#define RCODE_AX 0
#define RCODE_CX 1
#define RCODE_DX 2
#define RCODE_BX 3
#define RCODE_MAX 4

#define Register_name_MAX 20

typedef struct _register_name{
    char low [Register_name_MAX];
    char high[Register_name_MAX];
    char word[Register_name_MAX];
} register_name_p;

register_name_p register_names[RCODE_MAX];

typedef struct _register_data {
        union {
        uint16_t word;
        unsigned char lh[1];
    };
} register_data_p;

register_data_p registers[RCODE_MAX];

#define REGISTER_LOW 0
#define REGISTER_HIGH 1
#define REG_DST_1BYTE_MASK (7 << 3)
#define REG_SRC_1BYTE_MASK 7
#define INCDEC_BITMASK (0x1 << 3)

typedef unsigned char x8086_instruction_type;
typedef unsigned char X8086_INSTR;

typedef enum {
    INSTRUCTION_TYPE_ADD,
    INSTRUCTION_TYPE_MOV,
    INSTRUCTION_TYPE_INC,
    INSTRUCTION_TYPE_DEC,
    INSTRUCTION_TYPE_SUB,
    INSTRUCTION_TYPE_LOOP,
    INSTRUCTION_TYPE_UNDEFINED,
    INSTRUCTION_TYPE_MAX
}   instruction_types_p;

int CHECK_RANGE(int value, int minimum, int maximum)
{
    return ((value >= minimum) && (value <= maximum));
}

void print_register_content(RCODE coding) {
    printf("%s = %d, %s = %d, %s = %d\n", register_names[coding].low, registers[coding].lh[REGISTER_LOW],
        register_names[coding].high, registers[coding].lh[REGISTER_HIGH],
        register_names[coding].word, registers[coding].word);
}

void write_low(RCODE coding, unsigned char val) {
    registers[coding].lh[REGISTER_LOW] = val;
    printf("R: %s *----------->> V = %d (%x)\n\n", register_names[coding].low, val, val);
}

void write_high(RCODE coding, unsigned char val)
{
    registers[coding].lh[REGISTER_HIGH] = val;
    printf_s("R: %s *----------->> V = %d (%x)\n\n", register_names[coding].high, val, val);
}

void write_word(RCODE coding, uint16_t val)
{
    registers[coding].word = val;
    printf_s("R: %s *----------->> V = %d (%x)\n\n", register_names[coding].word, val, val);
}

void set_register_name(RCODE coding, char* low, char* high, char* word)
{
    strcpy_s(register_names[coding].low,  Register_name_MAX, low);
    strcpy_s(register_names[coding].high, Register_name_MAX, high);
    strcpy_s(register_names[coding].word, Register_name_MAX, word);
}

typedef unsigned char msdos_p;

msdos_p* load_msdos_file(char* com_file) {
    FILE* file;

    if (fopen_s(&file, com_file, "r") != 0) return NULL;
    fseek(file, 0, SEEK_END);
    size_t length = ftell(file);
    fseek(file, 0, SEEK_SET);
    unsigned char* bytes = malloc(length);
    fread_s(bytes, length, 1, length, file);
    fclose(file);
    return bytes;
}

unsigned char read_byte(msdos_p* dosfile, size_t offset)
{
    return *(dosfile + offset);
}

int is_program_exit(msdos_p* dosfile, size_t offset)
{
    return (read_byte(dosfile, offset) == 0xCD && read_byte(dosfile, offset + 1) == 0x20);
}

void destroy_msdos(msdos_p* msdos)
{
    free(msdos);
}

void print_address(msdos_p* dosdata, size_t offset) {
    printf_s("%xh: ", offset);
}

x8086_instruction_type get_opcode_type(X8086_INSTR* instruction) {
    if      (CHECK_RANGE(*instruction, 0xb0, 0xb7)) return INSTRUCTION_TYPE_MOV;
    else if (CHECK_RANGE(*instruction, 0xb8, 0xc0)) return INSTRUCTION_TYPE_MOV;
    else if (CHECK_RANGE(*instruction, 0x88, 0x8e)) return INSTRUCTION_TYPE_MOV;
    else if (CHECK_RANGE(*instruction, 0x0, 0x5)) return INSTRUCTION_TYPE_ADD;
    else if (CHECK_RANGE(*instruction, 0x80, 0x81)) return INSTRUCTION_TYPE_ADD;
    else if (CHECK_RANGE(*instruction, 0x2c, 0x2D)) return INSTRUCTION_TYPE_SUB;
    else if (CHECK_RANGE(*instruction, 0x28, 0x2b)) return INSTRUCTION_TYPE_SUB;
    else if (CHECK_RANGE(*instruction, 0xe2, 0xe2)) return INSTRUCTION_TYPE_LOOP;
    else if (CHECK_RANGE(*instruction, 0xfe, 0xff)) return (*(instruction + 1) & INCDEC_BITMASK) ? INSTRUCTION_TYPE_DEC : INSTRUCTION_TYPE_INC;
    else if (CHECK_RANGE(*instruction, 0x48, 0x50)) return INSTRUCTION_TYPE_DEC;
    else return INSTRUCTION_TYPE_UNDEFINED;
}

unsigned pc;   /* program counter */

void mov(unsigned char* byte);

void add(unsigned char* byte);

void sub(unsigned char* byte);

void loop(unsigned char* byte);

void inc(unsigned char* byte);

void dec(unsigned char* byte);

/* MOV */

void mov(unsigned char* byte) {
    printf("mov...");
    unsigned char opcoding = *byte;

    unsigned char* next_byte_pointer = byte + 1;

    if (CHECK_RANGE(opcoding, 0xb0, 0xb8)) {
        if (CHECK_RANGE(opcoding & 0x7, 0, RCODE_MAX))
            write_low(opcoding & REG_SRC_1BYTE_MASK, *next_byte_pointer);
        else
            write_high((opcoding & REG_SRC_1BYTE_MASK) - RCODE_MAX, *next_byte_pointer);
        pc++;

        return;
    }

    if (CHECK_RANGE(opcoding, 0xb8, 0xc0)) {
        write_word(opcoding & REG_SRC_1BYTE_MASK, *next_byte_pointer);
        pc += 2;

        return;
    }

    if (CHECK_RANGE(opcoding, 0x88, 0x88)) {
        RCODE to = (RCODE)(*next_byte_pointer & REG_DST_1BYTE_MASK) >> 3;
        RCODE from = (RCODE)*next_byte_pointer & REG_SRC_1BYTE_MASK;

        if (CHECK_RANGE(to, 0, RCODE_MAX)) {
            if (CHECK_RANGE(from, 0, RCODE_MAX)) {
                write_low(to, registers[from].lh[REGISTER_LOW]);
            }
            else {
                from -= RCODE_MAX;
                write_low(to, registers[from].lh[REGISTER_HIGH]);
            }
        }
        else {
            to -= RCODE_MAX;
            if (CHECK_RANGE(from, from - RCODE_MAX, RCODE_MAX)) {
                write_low(to, registers[from].lh[REGISTER_LOW]);
            }
            else {
                from -= RCODE_MAX;
                write_low(to, registers[from].lh[REGISTER_HIGH]);
            }
        }
        pc++;
        return;
    }

    if (CHECK_RANGE(opcoding, 0x89, 0x89)) {
        RCODE to = (RCODE)(*next_byte_pointer & REG_DST_1BYTE_MASK) >> 3;
        RCODE from = (RCODE)(*next_byte_pointer & REG_SRC_1BYTE_MASK);
        write_word(to, registers[from].word);
        pc++;
        return;
    }

    if (CHECK_RANGE(opcoding, 0x8A, 0x8A)) {
        RCODE to = (RCODE)(*next_byte_pointer & REG_DST_1BYTE_MASK) >> 3;
        RCODE from = (RCODE)*next_byte_pointer & REG_SRC_1BYTE_MASK;

        if (CHECK_RANGE(to, 0, RCODE_MAX)) {
            if (CHECK_RANGE(from, 0, RCODE_MAX)) {
                write_low(to, registers[from].lh[REGISTER_LOW]);
            }
            else {
                from -= RCODE_MAX;
                write_low(to, registers[from].lh[REGISTER_HIGH]);
            }
        }
        else {
            to -= RCODE_MAX;
            if (CHECK_RANGE(from, from - RCODE_MAX, RCODE_MAX)) {
                write_low(to, registers[from].lh[REGISTER_LOW]);
            }
            else {
                from -= RCODE_MAX;
                write_low(to, registers[from].lh[REGISTER_HIGH]);
            }
        }
        pc++;
        return;
    }

    if (CHECK_RANGE(opcoding, 0x8B, 0x8B)) {
        RCODE to   = (RCODE)(*next_byte_pointer & REG_DST_1BYTE_MASK) >> 3;
        RCODE from = (RCODE)*next_byte_pointer & REG_SRC_1BYTE_MASK;
        write_word(to, registers[from].word);
        pc++;
        return;
    }

}

/* ADDING */

void add(unsigned char* byte) {
    printf("adding...");

    unsigned char opcoding= *byte;
    unsigned char* next_byte_pointer = byte + 1;

    if (CHECK_RANGE(opcoding, 0x04, 0x04)) {
        uint8_t result = registers[RCODE_AX].lh[REGISTER_LOW] + *(uint8_t*)next_byte_pointer;
        write_low(RCODE_AX, *next_byte_pointer);
        ++pc;
        return;
    }

    if (CHECK_RANGE(opcoding, 0x05, 0x05)) {
        uint16_t result = registers[RCODE_AX].word + *(uint16_t*)next_byte_pointer;
        write_word(RCODE_AX, &result);
        ++pc;
        ++pc;
        return;
    }

    if (CHECK_RANGE(opcoding, 0x80, 0x80)) {
        RCODE to = (RCODE)(*next_byte_pointer & REG_DST_1BYTE_MASK) >> 3;

        if (CHECK_RANGE(to, 0, RCODE_MAX)) {
            uint8_t result = registers[to].lh[REGISTER_LOW] + *next_byte_pointer;
            write_low(to, result);
        }
        else {
            uint8_t result = registers[to].lh[REGISTER_HIGH] + *next_byte_pointer;
            write_high(to - RCODE_MAX, &result);
        }
        ++pc;
        return;
    }

    if (CHECK_RANGE(opcoding, 0x81, 0x81)) {
        RCODE to = (RCODE)(*next_byte_pointer & REG_DST_1BYTE_MASK) >> 3;
        uint16_t result = registers[to].word + *(uint16_t*)next_byte_pointer;
        write_word(to, &result);
        ++pc;
        ++pc;
        return;
    }

    if (CHECK_RANGE(opcoding, 0, 0)) {
        RCODE to = (RCODE)(*next_byte_pointer & REG_DST_1BYTE_MASK) >> 3;
        RCODE from = (RCODE)*next_byte_pointer & REG_SRC_1BYTE_MASK;

        if (CHECK_RANGE(to, 0, RCODE_MAX)) {
            if (CHECK_RANGE(from, 0, RCODE_MAX)) {
                write_low(to, registers[to].lh[REGISTER_LOW] + registers[from].lh[REGISTER_LOW]);
            }
            else {
                from -= RCODE_MAX;
                write_low(to, registers[to].lh[REGISTER_LOW] + registers[from].lh[REGISTER_HIGH]);
            }
        }
        else {
            to -= RCODE_MAX;
            if (CHECK_RANGE(from, from - RCODE_MAX, RCODE_MAX)) {
                write_high(to, registers[to].lh[REGISTER_HIGH] + registers[from].lh[REGISTER_LOW]);
            }
            else {
                from -= RCODE_MAX;
                write_high(to, registers[to].lh[REGISTER_HIGH] + registers[from].lh[REGISTER_HIGH]);
            }
        }
        ++pc;
        return;
    }

    if (CHECK_RANGE(opcoding, 1, 1)) {
        RCODE to = (RCODE)(*next_byte_pointer & REG_DST_1BYTE_MASK) >> 3;
        RCODE from = (RCODE)*next_byte_pointer & REG_SRC_1BYTE_MASK;
        write_word(to, registers[to].word + registers[from].word);
        ++pc;
        return;
    }

    if (CHECK_RANGE(opcoding, 2, 2)) {
        RCODE to   = (RCODE)(*next_byte_pointer & REG_DST_1BYTE_MASK) >> 3;
        RCODE from = (RCODE)*next_byte_pointer & REG_SRC_1BYTE_MASK;

        if (CHECK_RANGE(to, 0, RCODE_MAX)) {
            if (CHECK_RANGE(from, 0, RCODE_MAX)) {
                write_low(to, registers[to].lh[REGISTER_LOW] + registers[from].lh[REGISTER_LOW]);
            }
            else {
                from -= RCODE_MAX;
                write_low(to, registers[to].lh[REGISTER_LOW] + registers[from].lh[REGISTER_HIGH]);
            }
        }
        else {
            to -= RCODE_MAX;
            if (CHECK_RANGE(from, from - RCODE_MAX, RCODE_MAX)) {
                write_high(to, registers[to].lh[REGISTER_HIGH] + registers[from].lh[REGISTER_LOW]);
            }
            else {
                from -= RCODE_MAX;
                write_high(to, registers[to].lh[REGISTER_HIGH] + registers[from].lh[REGISTER_HIGH]);
            }
        }
        ++pc;
        return;
    }

    if (CHECK_RANGE(opcoding, 3, 3)) {
        RCODE to = (RCODE)(*next_byte_pointer & REG_DST_1BYTE_MASK) >> 3;
        RCODE from = (RCODE)*next_byte_pointer & REG_SRC_1BYTE_MASK;
        write_word(to, registers[to].word + registers[from].word);
        ++pc;
        return;
    }
}

/* SUBTRACTING */

void sub(unsigned char* byte) {
    printf("subtracting... ");

    unsigned char opcoding = *byte;
    unsigned char* next_byte_pointer = byte + 1;

    if (CHECK_RANGE(opcoding, 0x2d, 0x2d)) {
        uint16_t result = registers[RCODE_AX].word - *(uint16_t*)next_byte_pointer;
        write_word(RCODE_AX, &result);
        ++pc;
        ++pc;
        return;
    }

    if (CHECK_RANGE(opcoding, 0x28, 0x28)) {
        RCODE to = (RCODE)(*next_byte_pointer & REG_DST_1BYTE_MASK) >> 3;
        RCODE from = (RCODE)*next_byte_pointer & REG_SRC_1BYTE_MASK;

        if (CHECK_RANGE(to, 0, RCODE_MAX)) {
            if (CHECK_RANGE(from, 0, RCODE_MAX)) {
                write_low(to, registers[to].lh[REGISTER_LOW] - registers[from].lh[REGISTER_LOW]);
            }
            else {
                from -= RCODE_MAX;
                write_low(to, registers[to].lh[REGISTER_LOW] - registers[from].lh[REGISTER_HIGH]);
            }
        }
        else {
            to -= RCODE_MAX;
            if (CHECK_RANGE(from, from - RCODE_MAX, RCODE_MAX)) {
                write_high(to, registers[to].lh[REGISTER_HIGH] - registers[from].lh[REGISTER_LOW]);
            }
            else {
                from -= RCODE_MAX;
                write_high(to, registers[to].lh[REGISTER_HIGH] - registers[from].lh[REGISTER_HIGH]);
            }
        }
        ++pc;
        return;
    }

    if (CHECK_RANGE(opcoding, 0x2c, 0x2c)) {
        uint8_t result = registers[RCODE_AX].lh[REGISTER_LOW] - *(uint8_t*)next_byte_pointer;
        write_low(RCODE_AX, result);
        ++pc;
        return;
    }

    if (CHECK_RANGE(opcoding, 0x29, 0x29)) {
        RCODE to = (RCODE)(*next_byte_pointer & REG_DST_1BYTE_MASK) >> 3;
        RCODE from = (RCODE)*next_byte_pointer & REG_SRC_1BYTE_MASK;
        write_word(to, registers[to].word - registers[from].word);
        ++pc;
        return;
    }

    if (CHECK_RANGE(opcoding, 0x2A, 0x2A)) {
        RCODE to = (RCODE)(*next_byte_pointer & REG_DST_1BYTE_MASK) >> 3;
        RCODE from = (RCODE)*next_byte_pointer & REG_SRC_1BYTE_MASK;

        if (CHECK_RANGE(to, 0, RCODE_MAX)) {
            if (CHECK_RANGE(from, 0, RCODE_MAX)) {
                write_low(to, registers[to].lh[REGISTER_LOW] - registers[from].lh[REGISTER_LOW]);
            }
            else {
                from -= RCODE_MAX;
                write_low(to, registers[to].lh[REGISTER_LOW] - registers[from].lh[REGISTER_HIGH]);
            }
        }
        else {
            to -= RCODE_MAX;
            if (CHECK_RANGE(from, from - RCODE_MAX, RCODE_MAX)) {
                write_high(to, registers[to].lh[REGISTER_HIGH] - registers[from].lh[REGISTER_LOW]);
            }
            else {
                from -= RCODE_MAX;
                write_high(to, registers[to].lh[REGISTER_HIGH] - registers[from].lh[REGISTER_HIGH]);
            }
        }
        ++pc;
        return;
    }

    if (CHECK_RANGE(opcoding, 0x2b, 0x2b)) {
        RCODE to   = (RCODE)(*next_byte_pointer & REG_DST_1BYTE_MASK) >> 3;
        RCODE from = (RCODE)*next_byte_pointer & REG_SRC_1BYTE_MASK;
        write_word(to, registers[to].word - registers[from].word);
        ++pc;
        return;
    }
}

void loop(unsigned char* byte) {
    pc++;
    printf_s("loop... [%d] ", ((int8_t) * (byte + 1)));
    if (registers[RCODE_CX].word != 0) pc += ((int8_t) * (byte + 1));
    else { printf_s("\n"); return; }
    write_word(RCODE_CX, registers[RCODE_CX].word - 1);
}

void inc(unsigned char* byte) {
    unsigned char opcoding = *byte;
    switch (opcoding)
    {

    }
}

void dec(unsigned char* byte) {
    unsigned char opcoding = *byte;
    switch (opcoding)
    {

    }
}

void undefined(unsigned char* byte) {
    printf_s(stderr, "?????\n");
}

void(*instr_dispatcher[INSTRUCTION_TYPE_MAX])(unsigned char* byte);

int setup();

int setup() {
    set_register_name(RCODE_AX, "al", "ah", "ax");
    set_register_name(RCODE_CX, "cl", "ch", "cx");
    set_register_name(RCODE_DX, "dl", "dh", "dx");
    set_register_name(RCODE_BX, "bl", "bh", "bx");

    instr_dispatcher[INSTRUCTION_TYPE_MOV] = mov;
    instr_dispatcher[INSTRUCTION_TYPE_ADD] = add;
    instr_dispatcher[INSTRUCTION_TYPE_SUB] = sub;
    instr_dispatcher[INSTRUCTION_TYPE_INC] = inc;
    instr_dispatcher[INSTRUCTION_TYPE_DEC] = dec;
    instr_dispatcher[INSTRUCTION_TYPE_LOOP] = loop;
    instr_dispatcher[INSTRUCTION_TYPE_UNDEFINED] = undefined;
}

void execute(msdos_p* data) {
    pc = 0;

    memset(registers, 0, sizeof(registers));

    while (!is_program_exit(data, pc)) {
        unsigned char operation_coding = *data;

        print_address(data, pc);

        x8086_instruction_type opcoding_type = get_opcoding_type(data + pc);

        instr_dispatcher[opcoding_type](data + pc);

        if (opcoding_type == INSTRUCTION_TYPE_UNDEFINED) break;
        pc++;
    }

    printf("exiting...\n");
}

int main(int argc, char* argv[])
{
    setup();
    char file[255];
    while (file != "") {
        memset(file, 0, 255);
        printf_s(".COM filename: ");
        scanf_s("%s", file, sizeof(file));
        msdos_p* data = load_msdos_file(file);

        if (data == NULL) {
            fprintf_s(stderr, "file %s not found\n", file);
            continue;
        }

        execute(data);
        destroy_msdos(data);
    }
}
