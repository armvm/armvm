#ifndef A32_INSTRUCTION_H
#define A32_INSTRUCTION_H

#include <stdint.h>
namespace ARMVM
{
namespace A32
{
union instruction_t
{
    uint32_t code;
    struct
    {
        uint32_t shifter_operand : 12;
        uint32_t Rd : 4;
        uint32_t Rn : 4;
        uint32_t S : 1;
        uint32_t opcode : 7;
        uint32_t cond : 4;
    };
    instruction_t(uint32_t code) : code(code) {}
};

struct Instruction
{
    instruction_t instruction = 0;

    enum Type
    {
        LDR,
        STR,
        VLDR,
        VSTR,
        BX,
        MOV,
        MSR,
        MRS,
        VMSR,
        VMRS,
        BL,
        SUB,
        ADD,
        SVC,
        NOP,
    };

    template<Type T>
    struct Build
    {
        inline instruction_t Generate(Instruction* pc);
    };

    template<Type T>
    Instruction(Build<T> build) : instruction(build.Generate(this + 2)) {}
};

template<uint32_t S>
struct BuildStorageLoadRegisterInstruction
{
    instruction_t instruction = 0xE0000000 | S << 20;

    enum
    {
        FORWARD = 0x2C,
        BACKWARD = 0x28,
    };

    inline BuildStorageLoadRegisterInstruction(uint32_t rd, uint32_t rn, short offset)
    {
        instruction.Rd = rd;
        instruction.Rn = rn;
        instruction.shifter_operand = AdjustOffset(offset);
    }

    inline BuildStorageLoadRegisterInstruction(uint32_t rd, size_t* address) : address(address)
    {
        instruction.Rd = rd;
    }

    inline instruction_t Generate(Instruction* pc)
    {
        if(address)
        {
            instruction.Rn = 15;
            instruction.shifter_operand = AdjustOffset(static_cast<short>(reinterpret_cast<const char *>(address) - reinterpret_cast<const char *>(pc)));
        }
        return instruction;
    }

    const size_t* address = nullptr;

    inline uint32_t AdjustOffset(short offset)
    {
        static const uint32_t mask = 0xFFC;
        if(offset >= 0)
        {
            instruction.opcode = FORWARD;
        }
        else
        {
            instruction.opcode = BACKWARD;
            offset = -offset;
        }
        offset &= mask;
        return static_cast<uint32_t>(offset);
    }
};



template<>
struct Instruction::Build<Instruction::STR> : BuildStorageLoadRegisterInstruction<0>
{
    inline Build(uint32_t rd, uint32_t rn, short offset) : BuildStorageLoadRegisterInstruction(rd, rn, offset) {}
    inline Build(uint32_t rd, size_t* address) : BuildStorageLoadRegisterInstruction(rd, address) {}
};

template<>
struct Instruction::Build<Instruction::LDR> : BuildStorageLoadRegisterInstruction<1>
{
    inline Build(uint32_t rd, uint32_t rn, short offset) : BuildStorageLoadRegisterInstruction(rd, rn, offset) {}
    inline Build(uint32_t rd, size_t* address) : BuildStorageLoadRegisterInstruction(rd, address) {}
};

template<>
struct Instruction::Build<Instruction::BL>
{
    const Instruction *address;
    inline Build(Instruction *address) : address(address) {}

    inline instruction_t Generate(Instruction* pc)
    {
        union
        {
            int offset;
            uint32_t instruction;
            struct
            {
                uint32_t value : 23;
                uint32_t s : 1;
                uint32_t h8 : 8;
            };
        }number;
        number.offset = address - pc;
        number.s = number.h8 >> 7;
        number.h8 = 0xEB;
        return number.instruction;
    }
};

template<uint32_t S>
struct BuildStorageLoadFloatInstruction
{
    instruction_t instruction = 0xED000A00 | S << 20;
    inline BuildStorageLoadFloatInstruction(uint32_t sd, float* address) : address(reinterpret_cast<uint32_t *>(address))
    {
        instruction.Rd = sd >> 1;
        instruction.opcode |= (sd & 1) << 1;
    }

    inline BuildStorageLoadFloatInstruction(uint32_t dd, double* address) : address(reinterpret_cast<uint32_t *>(address))
    {
        instruction.Rd = dd;
        instruction.shifter_operand |= 0x100;
    }

    inline instruction_t Generate(Instruction* pc)
    {
        if(address)
        {
            instruction.Rn = 15;
            int offset = address - &pc->instruction.code;
            instruction.opcode |= (offset >= 0) << 2;
            instruction.shifter_operand |= ((offset >= 0) ? offset : -offset) & 0xFF;
        }
        return instruction;
    }

    const uint32_t* address = nullptr;
};

template<>
struct Instruction::Build<Instruction::VSTR> : BuildStorageLoadFloatInstruction<0>
{
    inline Build(uint32_t sd, float* address) : BuildStorageLoadFloatInstruction(sd, address) {}
    inline Build(uint32_t dd, double* address) : BuildStorageLoadFloatInstruction(dd, address) {}
};

template<>
struct Instruction::Build<Instruction::VLDR> : BuildStorageLoadFloatInstruction<1>
{
    inline Build(uint32_t sd, float* address) : BuildStorageLoadFloatInstruction(sd, address) {}
    inline Build(uint32_t dd, double* address) : BuildStorageLoadFloatInstruction(dd, address) {}
};

struct IndependentInstruction
{
    instruction_t instruction;
    IndependentInstruction(uint32_t instruction) : instruction(instruction) {}
    inline instruction_t Generate(Instruction* pc)
    {
        return instruction;
    }
};

template<>
struct Instruction::Build<Instruction::MSR> : IndependentInstruction
{
    inline Build(uint32_t rn, bool c = true, bool x = true, bool s = true, bool f = true) : IndependentInstruction(0xE120F000)
    {
        instruction.shifter_operand = rn;
        instruction.Rn = static_cast<uint32_t>(f);
        instruction.Rn = static_cast<uint32_t>(x | instruction.Rn << 1);
        instruction.Rn = static_cast<uint32_t>(s | instruction.Rn << 1);
        instruction.Rn = static_cast<uint32_t>(c | instruction.Rn << 1);
    }
};

template<>
struct Instruction::Build<Instruction::MRS> : IndependentInstruction
{
    inline Build(uint32_t rd) : IndependentInstruction(0xE10F0000)
    {
        instruction.Rd = rd;
    }
};

template<>
struct Instruction::Build<Instruction::SUB> : IndependentInstruction
{
    inline Build(uint32_t rd, uint32_t rn, uint32_t value) : IndependentInstruction(0xE2400000)
    {
        instruction.Rd = rd;
        instruction.Rn = rn;
        instruction.shifter_operand = value;
    }
};

template<>
struct Instruction::Build<Instruction::ADD> : IndependentInstruction
{
    inline Build(uint32_t rd, uint32_t rn, uint32_t value) : IndependentInstruction(0xE2800000)
    {
        instruction.Rd = rd;
        instruction.Rn = rn;
        instruction.shifter_operand = value;
    }
};


enum FP
{
    FPSCR = 1,
    FPEXC = 8,
};

template<>
struct Instruction::Build<Instruction::VMSR> : IndependentInstruction
{

    inline Build(uint32_t reg, FP fp) : IndependentInstruction(0xEEE00A10)
    {
        instruction.Rn = fp;
        instruction.Rd = reg;
    }
};



template<>
struct Instruction::Build<Instruction::VMRS> : IndependentInstruction
{
    inline Build(uint32_t reg, FP fp) : IndependentInstruction(0xEEF00A10)
    {
        instruction.Rn = fp;
        instruction.Rd = reg;
    }
};


template<>
struct Instruction::Build<Instruction::MOV> : IndependentInstruction
{
    inline Build(uint32_t rd, uint32_t rn)  : IndependentInstruction(0xE1A00000)
    {
        instruction.Rd = rd;
        instruction.shifter_operand = rn;
    }
};

template<>
struct Instruction::Build<Instruction::BX> : IndependentInstruction
{
    inline Build(uint32_t rd) : IndependentInstruction(0xE12FFF10)
    {
        instruction.code |= rd;
    }
};

template<>
struct Instruction::Build<Instruction::NOP> : IndependentInstruction
{
    inline Build() : IndependentInstruction(0xE1A00000) {}
};

template<>
struct Instruction::Build<Instruction::SVC> : IndependentInstruction
{
    inline Build(uint32_t swi) : IndependentInstruction(0xEF000000)
    {
        instruction.shifter_operand = swi;
    }
};

}
}

#endif // A32_INSTRUCTION_H
