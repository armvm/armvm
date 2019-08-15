#ifndef VMLAUNCHERBASE_H
#define VMLAUNCHERBASE_H

#include "executable.h"
#include "../vmlauncher.h"
#include "../vmcallback.h"

#include <thread>


template<typename T1, typename T2>
inline T1 force_cast(T2 value)
{
    return *reinterpret_cast<T1 *>(&value);
}

namespace ARMVM
{

class VmLauncherBase : public ARMVM::VmLauncher, public Executable
{
public:
    VmLauncherBase(void* sigcallback, int(*sigaction)(int, struct sigaction *, struct sigaction *));
    virtual ~VmLauncherBase();

protected:
    struct Event
    {
        enum Type
        {
            NONE,
            BEGIN,
            FINISH,
            EXCEPTION,
            RESUME,
            STOP,
        }type;
        void * pointer;
        Event(Type type, void *pointer) : type(type), pointer(pointer) {}
    };


    Event SwitchMain(Event::Type type, void* pointer);
    Event SwitchSandbox(Event::Type type, void* pointer);

    size_t svc_key[4] = {0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF};
private:


    void Sandbox(void* sigcallback, int(*sigaction)(int, struct sigaction *, struct sigaction *));
    void WaitForSandboxReady();
    void SandboxLoop();
    stack_t sig_stack;
    std::unique_ptr<std::thread> sandbox_thread;
    std::atomic_flag main_flag, sandbox_flag;
    Event main_event, sandbox_event;
};

}


#endif // VMLAUNCHERBASE_H
