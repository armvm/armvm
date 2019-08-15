#ifndef VMCONTEXT_H
#define VMCONTEXT_H

#include <stdlib.h>

namespace ARMVM
{

class VmContext
{
public:
    virtual ~VmContext() = default;

    virtual void Reset() = 0;
    virtual size_t GetCpuRegister(size_t index) const = 0;
    virtual void SetCpuRegister(size_t index, size_t value) = 0;
    virtual size_t GetCpsr() const = 0;
    virtual void SetCpsr(size_t value) = 0;
    virtual float GetFpuRegister(size_t index) const = 0;
    virtual void SetFpuRegister(size_t index, float value) = 0;
    virtual size_t GetFpscr() const = 0;
    virtual void SetFpscr(size_t value) = 0;
    virtual size_t GetFpexc() const = 0;
    virtual void SetFpexc(size_t value) = 0;

    size_t GetStackPointer() const {
        return GetCpuRegister(13);
    }
    void SetStackPointer(size_t value) {
        return SetCpuRegister(13, value);
    }

    size_t GetLinkRegister() const {
        return GetCpuRegister(14);
    }
    void SetLinkRegister(size_t value) {
        return SetCpuRegister(14, value);
    }

    size_t GetProgramCounter() const {
        return GetCpuRegister(15);
    }
    void SetProgramCounter(size_t value) {
        return SetCpuRegister(15, value);
    }
};

}

#endif // VMCONTEXT_H
