#include "a32_vmlauncher.h"
#include "a32_vmcontext.h"

#define STACK_SIZE 4096000

namespace ARMVM
{
namespace A32
{

__attribute__((naked)) int VmLauncher::Sigaction(int, struct sigaction *, struct sigaction *)
{
    asm("push {r7}");
    asm("mov r7, 0x43");
    asm("svc 0");
    asm("pop {r7}");
    asm("blx lr");
}

void VmLauncher::HandleException(size_t sig, std::unique_ptr<ARMVM::VmCallback>& callback)
{
    if(callback)
    {
        uint32_t instruction = *(reinterpret_cast<uint32_t *>(use_state.reg[15]) - 1);
        switch (sig)
        {
        case SIGILL:
        {
            callback->HandleIllegalInstruction(instruction);
            break;
        }

        case SIGSYS:
        {
            uint32_t swi = instruction & 0xFFF;
            callback->HandleSystemCall(swi);
            break;
        }
        case SIGSEGV:
        {
            callback->HandleSegmentFault(instruction);
            break;
        }
        default:
        {
            callback->HandleException(sig);
            break;
        }
        }
    }
}

void VmLauncher::Run(std::unique_ptr<ARMVM::VmCallback>& callback)
{
    if(!use_state.reg[15])
    {
        return;
    }
    uint32_t *stack = nullptr;
    if(!use_state.reg[13])
    {
        stack = new uint32_t[STACK_SIZE / sizeof(*stack)];
        use_state.reg[13] = reinterpret_cast<size_t>(stack) + STACK_SIZE;
    }
    Event::Type sandbox_type = Event::BEGIN;
    void* sandbox_pointer = Program;
    for(bool main_enable = true; main_enable; )
    {
        Event main_event = SwitchSandbox(sandbox_type, sandbox_pointer);
        switch (main_event.type)
        {
        case Event::EXCEPTION:
        {
            size_t sig = reinterpret_cast<size_t>(main_event.pointer);
            HandleException(sig, callback);
            sandbox_type = Event::NONE;
            sandbox_pointer = nullptr;
            break;
        }
        case Event::FINISH:
        case Event::STOP:
        {
            main_enable = false;
            break;
        }
        default:
        {
            sandbox_type = Event::NONE;
            sandbox_pointer = nullptr;
            break;
        }
        }
    }
    if(stack)
    {
        delete[] stack;
        use_state.reg[13] = 0;
    }
}



void VmLauncher::SaveContext(const std::unique_ptr<ARMVM::VmContext>& ctx)
{
    VmContext *vmctx = static_cast<VmContext *>(ctx.get());
    memcpy(&vmctx->state, &use_state, sizeof(VmState));
}

void VmLauncher::LoadContext(const std::unique_ptr<ARMVM::VmContext>& ctx)
{
    VmContext *vmctx = static_cast<VmContext *>(ctx.get());
    memcpy(&use_state, &vmctx->state, sizeof(VmState));
}


std::unique_ptr<ARMVM::VmContext> VmLauncher::NewContext() const
{
    return std::unique_ptr<ARMVM::VmContext>(new VmContext);
}

struct vfp_sigframe
{
    unsigned long		magic;
    unsigned long		size;
    struct user_vfp		ufp;
    struct user_vfp_exc	ufp_exc;
};

void VmLauncher::SignalCallback(int sig, ucontext_t* context)
{
    vfp_sigframe *vfp = reinterpret_cast<vfp_sigframe *>(context->uc_regspace);
    memcpy(use_state.reg, &context->uc_mcontext.arm_r0, sizeof(use_state.reg));
    memcpy(use_state.f64, vfp->ufp.fpregs, sizeof(use_state.f64));
    use_state.cspr = context->uc_mcontext.arm_cpsr;
    use_state.fpscr = vfp->ufp.fpscr;
    SwitchMain(Event::EXCEPTION, reinterpret_cast<void *>(sig));
    vfp->ufp.fpscr = use_state.fpscr;
    context->uc_mcontext.arm_cpsr = use_state.cspr;
    memcpy(vfp->ufp.fpregs, use_state.f64, sizeof(use_state.f64));
    memcpy(&context->uc_mcontext.arm_r0, use_state.reg, sizeof(use_state.reg));
}


}
}


