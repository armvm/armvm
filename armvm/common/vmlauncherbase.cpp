#include "vmlauncherbase.h"

#include "../aarch32/a32_vmlauncher.h"

#include <ucontext.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include <signal.h>
#include <sys/prctl.h>
#include <linux/filter.h>
#include <linux/seccomp.h>
#include <sys/mman.h>

namespace ARMVM
{

template<size_t bit>
std::unique_ptr<VmLauncher> CreateVmLoader();

template<>
std::unique_ptr<VmLauncher> CreateVmLoader<32>()
{
    return std::unique_ptr<ARMVM::VmLauncher>(new ARMVM::A32::VmLauncher);
}

std::unique_ptr<VmLauncher> VmLauncher::Create()
{
    return CreateVmLoader<sizeof(void *) * 8>();
}

VmLauncherBase::~VmLauncherBase()
{
    sandbox_thread->detach();
    sandbox_event = Event(Event::STOP, nullptr);
    main_event = Event(Event::STOP, nullptr);
    sandbox_flag.clear();
    main_flag.clear();
    sandbox_thread.reset();
    munmap(sig_stack.ss_sp, SIGSTKSZ);
}

VmLauncherBase::VmLauncherBase(void* sigcallback, int(*sigaction)(int, struct sigaction*, struct sigaction*)) : main_event(Event::NONE, nullptr), sandbox_event(Event::NONE, nullptr)
{
    sig_stack.ss_sp = mmap(nullptr, SIGSTKSZ, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    sig_stack.ss_size = SIGSTKSZ;
    sig_stack.ss_flags = 0;
    main_flag.test_and_set();
    sandbox_flag.test_and_set();
    sandbox_thread = std::unique_ptr<std::thread>(new std::thread(&VmLauncherBase::Sandbox, this, sigcallback, sigaction));
    WaitForSandboxReady();
}

void VmLauncherBase::WaitForSandboxReady()
{
    while (main_flag.test_and_set());
}

VmLauncherBase::Event VmLauncherBase::SwitchMain(Event::Type type, void* pointer)
{
    main_event = Event(type, pointer);
    main_flag.clear();
    while (sandbox_flag.test_and_set());
    return sandbox_event;
}

VmLauncherBase::Event VmLauncherBase::SwitchSandbox(Event::Type type, void* pointer)
{
    sandbox_event = Event(type, pointer);
    sandbox_flag.clear();
    while (main_flag.test_and_set());
    return main_event;
}

void VmLauncherBase::SandboxLoop()
{
    Event::Type main_type = Event::NONE;
    void* main_pointer = nullptr;
    for(bool sandbox_enable = true; sandbox_enable;)
    {
        Event sandbox_event = SwitchMain(main_type, main_pointer);
        switch (sandbox_event.type)
        {
        case Event::BEGIN:
        {
            reinterpret_cast<void(*)()>(sandbox_event.pointer)();
            main_type = Event::FINISH;
            main_pointer = sandbox_event.pointer;
            break;
        }
        case Event::STOP:
        {
            sandbox_enable = false;
            break;
        }
        default:
        {
            main_type = Event::NONE;
            main_pointer = nullptr;
            break;
        }
        }
    }
}


void VmLauncherBase::Sandbox(void* sigcallback, int(*sigaction)(int, struct sigaction*, struct sigaction*))
{
    sigaltstack(&sig_stack, nullptr);
    static const int sigs[] =
    {
        SIGSEGV,
        SIGSYS,
        SIGILL,
        SIGABRT,
    };
    struct sigaction siga;
    siga.sa_mask = 0;
    siga.sa_flags = SA_SIGINFO | SA_ONSTACK | SA_RESTART;
    siga.sa_restorer = nullptr;
    siga.sa_sigaction = reinterpret_cast<void(*)(int, struct siginfo*, void*)>(sigcallback);
    for(size_t i = 0; i < sizeof(sigs) / sizeof(*sigs); ++i)
    {
        sigaction(sigs[i], &siga, nullptr);
    }
    if(0 == prctl(PR_SET_NO_NEW_PRIVS,1,0,0,0))
    {

        struct sock_filter sfi[] =
        {
            {0x20,0x00,0x00,0x00000000}, // A = sys_number
            {0x15,0x00,0x09,0x000000AD}, // skip ((A == 0xAD) ? 0 : 9) instruction
            {0x20,0x00,0x00,0x00000010}, // A = arg0 (0x10)
            {0x15,0x00,0x07,svc_key[0]}, // skip ((A == svc_key[0]) ? 0 : 7) instruction
            {0x20,0x00,0x00,0x00000018}, // A = arg1 (0x18)
            {0x15,0x00,0x05,svc_key[1]}, // skip ((A == svc_key[1]) ? 0 : 5) instruction
            {0x20,0x00,0x00,0x00000020}, // A = arg2 (0x20)
            {0x15,0x00,0x03,svc_key[2]}, // skip ((A == svc_key[2]) ? 0 : 3) instruction
            {0x20,0x00,0x00,0x00000028}, // A = arg3 (0x28)
            {0x15,0x00,0x01,svc_key[3]}, // skip ((A == svc_key[3]) ? 0 : 1) instruction
            {0x06,0x00,0x00,0x7FFF0000}, // retrun ALLOW
            {0x06,0x00,0x00,0x00030000}, // return TRAP
        };
        struct sock_fprog sfp = { sizeof(sfi) / sizeof(*sfi), sfi};
        if(0 == prctl(PR_SET_SECCOMP,SECCOMP_MODE_FILTER,&sfp))
        {
            SandboxLoop();
            return;
        }
    }
    delete this;
}

}
