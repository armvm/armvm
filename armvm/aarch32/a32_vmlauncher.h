#ifndef A32_VMLAUNCHER_H
#define A32_VMLAUNCHER_H

#include "a32_vmstate.h"
#include "a32_instruction.h"
#include "../vmcallback.h"
#include "../common/vmlauncherbase.h"

#include <atomic>
#include <thread>
#include <mutex>
#include <queue>



namespace ARMVM
{
namespace A32
{


class VmLauncher : public ARMVM::VmLauncherBase
{
public:

    virtual void Run(std::unique_ptr<VmCallback>& callback) override;

    virtual void SaveContext(const std::unique_ptr<VmContext>& ctx) override;

    virtual void LoadContext(const std::unique_ptr<VmContext>& ctx) override;

    virtual std::unique_ptr<VmContext> NewContext() const override;

    virtual ~VmLauncher() override = default;

    VmLauncher() : ARMVM::VmLauncherBase(SignalCallbackInstruction, &Sigaction){}


private:
    void SignalCallback(int sig, ucontext_t* context);
    void HandleException(size_t sig, std::unique_ptr<ARMVM::VmCallback>& callback);
    static int Sigaction(int, struct sigaction *, struct sigaction *);

    VmState use_state;
    VmState save_state;

    alignas(4) Instruction Program[141] =
    {
        Instruction::Build<Instruction::VSTR>(0, &save_state.f64[0]),
        Instruction::Build<Instruction::VSTR>(1, &save_state.f64[1]),
        Instruction::Build<Instruction::VSTR>(2, &save_state.f64[2]),
        Instruction::Build<Instruction::VSTR>(3, &save_state.f64[3]),
        Instruction::Build<Instruction::VSTR>(4, &save_state.f64[4]),
        Instruction::Build<Instruction::VSTR>(5, &save_state.f64[5]),
        Instruction::Build<Instruction::VSTR>(6, &save_state.f64[6]),
        Instruction::Build<Instruction::VSTR>(7, &save_state.f64[7]),
        Instruction::Build<Instruction::VSTR>(8, &save_state.f64[8]),
        Instruction::Build<Instruction::VSTR>(9, &save_state.f64[9]),
        Instruction::Build<Instruction::VSTR>(10, &save_state.f64[10]),
        Instruction::Build<Instruction::VSTR>(11, &save_state.f64[11]),
        Instruction::Build<Instruction::VSTR>(12, &save_state.f64[12]),
        Instruction::Build<Instruction::VSTR>(13, &save_state.f64[13]),
        Instruction::Build<Instruction::VSTR>(14, &save_state.f64[14]),
        Instruction::Build<Instruction::VSTR>(15, &save_state.f64[15]),

        Instruction::Build<Instruction::STR>(0, &save_state.reg[0]),
        Instruction::Build<Instruction::STR>(1, &save_state.reg[1]),
        Instruction::Build<Instruction::STR>(2, &save_state.reg[2]),
        Instruction::Build<Instruction::STR>(3, &save_state.reg[3]),
        Instruction::Build<Instruction::STR>(4, &save_state.reg[4]),
        Instruction::Build<Instruction::STR>(5, &save_state.reg[5]),
        Instruction::Build<Instruction::STR>(6, &save_state.reg[6]),
        Instruction::Build<Instruction::STR>(7, &save_state.reg[7]),
        Instruction::Build<Instruction::STR>(8, &save_state.reg[8]),
        Instruction::Build<Instruction::STR>(9, &save_state.reg[9]),
        Instruction::Build<Instruction::STR>(10, &save_state.reg[10]),
        Instruction::Build<Instruction::STR>(11, &save_state.reg[11]),
        Instruction::Build<Instruction::STR>(12, &save_state.reg[12]),
        Instruction::Build<Instruction::STR>(13, &save_state.reg[13]),
        Instruction::Build<Instruction::STR>(14, &save_state.reg[14]),
        Instruction::Build<Instruction::MRS>(0),
        Instruction::Build<Instruction::STR>(0, &save_state.cspr),
        Instruction::Build<Instruction::VMRS>(0, FP::FPSCR),
        Instruction::Build<Instruction::STR>(0, &save_state.fpscr),

        Instruction::Build<Instruction::VLDR>(0, &use_state.f64[0]),
        Instruction::Build<Instruction::VLDR>(1, &use_state.f64[1]),
        Instruction::Build<Instruction::VLDR>(2, &use_state.f64[2]),
        Instruction::Build<Instruction::VLDR>(3, &use_state.f64[3]),
        Instruction::Build<Instruction::VLDR>(4, &use_state.f64[4]),
        Instruction::Build<Instruction::VLDR>(5, &use_state.f64[5]),
        Instruction::Build<Instruction::VLDR>(6, &use_state.f64[6]),
        Instruction::Build<Instruction::VLDR>(7, &use_state.f64[7]),
        Instruction::Build<Instruction::VLDR>(8, &use_state.f64[8]),
        Instruction::Build<Instruction::VLDR>(9, &use_state.f64[9]),
        Instruction::Build<Instruction::VLDR>(10, &use_state.f64[10]),
        Instruction::Build<Instruction::VLDR>(11, &use_state.f64[11]),
        Instruction::Build<Instruction::VLDR>(12, &use_state.f64[12]),
        Instruction::Build<Instruction::VLDR>(13, &use_state.f64[13]),
        Instruction::Build<Instruction::VLDR>(14, &use_state.f64[14]),
        Instruction::Build<Instruction::VLDR>(15, &use_state.f64[15]),

        Instruction::Build<Instruction::LDR>(0, &use_state.cspr),
        Instruction::Build<Instruction::MSR>(0),
        Instruction::Build<Instruction::LDR>(0, &use_state.fpscr),
        Instruction::Build<Instruction::VMSR>(0, FP::FPSCR),
        Instruction::Build<Instruction::LDR>(0, &use_state.reg[0]),
        Instruction::Build<Instruction::LDR>(1, &use_state.reg[1]),
        Instruction::Build<Instruction::LDR>(2, &use_state.reg[2]),
        Instruction::Build<Instruction::LDR>(3, &use_state.reg[3]),
        Instruction::Build<Instruction::LDR>(4, &use_state.reg[4]),
        Instruction::Build<Instruction::LDR>(5, &use_state.reg[5]),
        Instruction::Build<Instruction::LDR>(6, &use_state.reg[6]),
        Instruction::Build<Instruction::LDR>(7, &use_state.reg[7]),
        Instruction::Build<Instruction::LDR>(8, &use_state.reg[8]),
        Instruction::Build<Instruction::LDR>(9, &use_state.reg[9]),
        Instruction::Build<Instruction::LDR>(10, &use_state.reg[10]),
        Instruction::Build<Instruction::LDR>(11, &use_state.reg[11]),
        Instruction::Build<Instruction::LDR>(12, &use_state.reg[12]),
        Instruction::Build<Instruction::LDR>(13, &use_state.reg[13]),
        Instruction::Build<Instruction::MOV>(14, 15),
        Instruction::Build<Instruction::LDR>(15, &use_state.reg[15]),

        Instruction::Build<Instruction::VSTR>(0, &use_state.f64[0]),
        Instruction::Build<Instruction::VSTR>(1, &use_state.f64[1]),
        Instruction::Build<Instruction::VSTR>(2, &use_state.f64[2]),
        Instruction::Build<Instruction::VSTR>(3, &use_state.f64[3]),
        Instruction::Build<Instruction::VSTR>(4, &use_state.f64[4]),
        Instruction::Build<Instruction::VSTR>(5, &use_state.f64[5]),
        Instruction::Build<Instruction::VSTR>(6, &use_state.f64[6]),
        Instruction::Build<Instruction::VSTR>(7, &use_state.f64[7]),
        Instruction::Build<Instruction::VSTR>(8, &use_state.f64[8]),
        Instruction::Build<Instruction::VSTR>(9, &use_state.f64[9]),
        Instruction::Build<Instruction::VSTR>(10, &use_state.f64[10]),
        Instruction::Build<Instruction::VSTR>(11, &use_state.f64[11]),
        Instruction::Build<Instruction::VSTR>(12, &use_state.f64[12]),
        Instruction::Build<Instruction::VSTR>(13, &use_state.f64[13]),
        Instruction::Build<Instruction::VSTR>(14, &use_state.f64[14]),
        Instruction::Build<Instruction::VSTR>(15, &use_state.f64[15]),

        Instruction::Build<Instruction::STR>(0, &use_state.reg[0]),
        Instruction::Build<Instruction::STR>(1, &use_state.reg[1]),
        Instruction::Build<Instruction::STR>(2, &use_state.reg[2]),
        Instruction::Build<Instruction::STR>(3, &use_state.reg[3]),
        Instruction::Build<Instruction::STR>(4, &use_state.reg[4]),
        Instruction::Build<Instruction::STR>(5, &use_state.reg[5]),
        Instruction::Build<Instruction::STR>(6, &use_state.reg[6]),
        Instruction::Build<Instruction::STR>(7, &use_state.reg[7]),
        Instruction::Build<Instruction::STR>(8, &use_state.reg[8]),
        Instruction::Build<Instruction::STR>(9, &use_state.reg[9]),
        Instruction::Build<Instruction::STR>(10, &use_state.reg[10]),
        Instruction::Build<Instruction::STR>(11, &use_state.reg[11]),
        Instruction::Build<Instruction::STR>(12, &use_state.reg[12]),
        Instruction::Build<Instruction::STR>(13, &use_state.reg[13]),
        Instruction::Build<Instruction::MRS>(0),
        Instruction::Build<Instruction::STR>(0, &use_state.cspr),
        Instruction::Build<Instruction::VMRS>(0, FP::FPSCR),
        Instruction::Build<Instruction::STR>(0, &use_state.fpscr),

        Instruction::Build<Instruction::VLDR>(0, &save_state.f64[0]),
        Instruction::Build<Instruction::VLDR>(1, &save_state.f64[1]),
        Instruction::Build<Instruction::VLDR>(2, &save_state.f64[2]),
        Instruction::Build<Instruction::VLDR>(3, &save_state.f64[3]),
        Instruction::Build<Instruction::VLDR>(4, &save_state.f64[4]),
        Instruction::Build<Instruction::VLDR>(5, &save_state.f64[5]),
        Instruction::Build<Instruction::VLDR>(6, &save_state.f64[6]),
        Instruction::Build<Instruction::VLDR>(7, &save_state.f64[7]),
        Instruction::Build<Instruction::VLDR>(8, &save_state.f64[8]),
        Instruction::Build<Instruction::VLDR>(9, &save_state.f64[9]),
        Instruction::Build<Instruction::VLDR>(10, &save_state.f64[10]),
        Instruction::Build<Instruction::VLDR>(11, &save_state.f64[11]),
        Instruction::Build<Instruction::VLDR>(12, &save_state.f64[12]),
        Instruction::Build<Instruction::VLDR>(13, &save_state.f64[13]),
        Instruction::Build<Instruction::VLDR>(14, &save_state.f64[14]),
        Instruction::Build<Instruction::VLDR>(15, &save_state.f64[15]),

        Instruction::Build<Instruction::LDR>(0, &save_state.cspr),
        Instruction::Build<Instruction::MSR>(0),
        Instruction::Build<Instruction::LDR>(0, &save_state.fpscr),
        Instruction::Build<Instruction::VMSR>(0, FP::FPSCR),
        Instruction::Build<Instruction::LDR>(0, &save_state.reg[0]),
        Instruction::Build<Instruction::LDR>(1, &save_state.reg[1]),
        Instruction::Build<Instruction::LDR>(2, &save_state.reg[2]),
        Instruction::Build<Instruction::LDR>(3, &save_state.reg[3]),
        Instruction::Build<Instruction::LDR>(4, &save_state.reg[4]),
        Instruction::Build<Instruction::LDR>(5, &save_state.reg[5]),
        Instruction::Build<Instruction::LDR>(6, &save_state.reg[6]),
        Instruction::Build<Instruction::LDR>(7, &save_state.reg[7]),
        Instruction::Build<Instruction::LDR>(8, &save_state.reg[8]),
        Instruction::Build<Instruction::LDR>(9, &save_state.reg[9]),
        Instruction::Build<Instruction::LDR>(10, &save_state.reg[10]),
        Instruction::Build<Instruction::LDR>(11, &save_state.reg[11]),
        Instruction::Build<Instruction::LDR>(12, &save_state.reg[12]),
        Instruction::Build<Instruction::LDR>(13, &save_state.reg[13]),
        Instruction::Build<Instruction::LDR>(14, &save_state.reg[14]),

        Instruction::Build<Instruction::BX>(14),
    };

    size_t self = reinterpret_cast<size_t>(this);
    size_t callback = force_cast<size_t>(&VmLauncher::SignalCallback);
    Instruction SignalCallbackInstruction[13] =
    {
        Instruction::Build<Instruction::MOV>(1,0),
        Instruction::Build<Instruction::LDR>(0, &self),
        Instruction::Build<Instruction::STR>(14, 13, -4),
        Instruction::Build<Instruction::SUB>(13,13,8),
        Instruction::Build<Instruction::MOV>(14, 15),
        Instruction::Build<Instruction::LDR>(15, &callback),
        Instruction::Build<Instruction::ADD>(13,13,8),
        Instruction::Build<Instruction::LDR>(14, 13, -4),
        Instruction::Build<Instruction::LDR>(0,&svc_key[0]),
        Instruction::Build<Instruction::LDR>(1,&svc_key[1]),
        Instruction::Build<Instruction::LDR>(2,&svc_key[2]),
        Instruction::Build<Instruction::LDR>(3,&svc_key[3]),
        Instruction::Build<Instruction::BX>(14),
    };
};

}
}

#endif // A32_VMLAUNCHER_H
