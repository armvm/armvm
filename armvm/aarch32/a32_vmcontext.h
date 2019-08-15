#ifndef A32_VMCONTEXT_H
#define A32_VMCONTEXT_H


/*
namespace ARMVM
{

namespace A32
{

}

}
*/

#include "../vmcontext.h"
#include "a32_vmstate.h"


namespace ARMVM
{

namespace A32
{
    class VmContext : public ARMVM::VmContext
    {
    public:
        virtual ~VmContext();

        virtual void Reset();
        virtual size_t GetCpuRegister(size_t index) const;
        virtual void SetCpuRegister(size_t index, size_t value);
        virtual size_t GetCpsr() const;
        virtual void SetCpsr(size_t value);
        virtual float GetFpuRegister(size_t index) const;
        virtual void SetFpuRegister(size_t index, float value);
        virtual size_t GetFpscr() const;
        virtual void SetFpscr(size_t value);
        virtual size_t GetFpexc() const;
        virtual void SetFpexc(size_t value);
    private:
        VmState state;
        friend class VmLauncher;
    };
}

}

#endif // A32_VMCONTEXT_H
