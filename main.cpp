#include "armvm/aarch32/a32_vmlauncher.h"
#include "armvm/common/executable.h"
#include "armvm/aarch32/a32_instruction.h"
#include <android/log.h>

static void foo()
{
    asm("svc 1");
    int *a = (int *)2;
    int b = *a;
}


class TestCallback : public ARMVM::VmCallback
{
public:
    virtual void HandleException(uint32_t sig) override
    {
        __android_log_write(ANDROID_LOG_VERBOSE, "Exception", ".....................");
    }

    virtual void HandleIllegalInstruction(uint32_t instruction) override
    {
        __android_log_write(ANDROID_LOG_VERBOSE, "Illegal Instruction", ".....................");
    }

    virtual void HandleSystemCall(uint32_t instruction) override
    {
        __android_log_write(ANDROID_LOG_VERBOSE, "System Call", ".....................");
    }

    virtual void HandleSegmentFault(uint32_t instruction) override
    {
        __android_log_write(ANDROID_LOG_VERBOSE, "Segment Fault", ".....................");
        auto context = launcher->NewContext();
        launcher->SaveContext(context);
        context->SetProgramCounter(context->GetProgramCounter() + 4);
        launcher->LoadContext(context);
    }

    TestCallback() : launcher(ARMVM::VmLauncher::Create())
    {
    }

    virtual ~TestCallback() override
    {

    }

    std::unique_ptr<ARMVM::VmLauncher> launcher;
};


int main(int argc, char *argv[])
{
    __android_log_write(ANDROID_LOG_VERBOSE, "START", ":.....................");
    uint32_t stack[256000] = { 0 };
    TestCallback* callback = new TestCallback;
    auto context = callback->launcher->NewContext();
    context->SetCpuRegister(13, reinterpret_cast<size_t>(stack) + sizeof(stack));
    context->SetCpuRegister(15, reinterpret_cast<size_t>(foo));
    context->SetFpuRegister(0, 1.0);
    context->SetCpuRegister(0, 0x12);
    context->SetCpuRegister(1, 0x34);
    context->SetCpuRegister(2, 0x56);
    context->SetCpuRegister(3, 0x78);
    context->SetFpscr(0x11223344);
    callback->launcher->LoadContext(context);
    std::unique_ptr<ARMVM::VmCallback> ptr(static_cast<ARMVM::VmCallback *>(callback));
    callback->launcher->Run(ptr);
    callback->launcher->SaveContext(context);
    __android_log_write(ANDROID_LOG_VERBOSE, "FINISH", ".....................");
    return 0;
};
