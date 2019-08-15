#ifndef VMCALLBACK_H
#define VMCALLBACK_H

#include <stdint.h>

namespace ARMVM
{

class VmCallback
{
public:
    virtual void HandleException(uint32_t sig) = 0;

    virtual void HandleIllegalInstruction(uint32_t instruction) = 0;

    virtual void HandleSystemCall(uint32_t swi) = 0;

    virtual void HandleSegmentFault(uint32_t instruction) = 0;

    virtual ~VmCallback() = default;
};

}

#endif // VMCALLBACK_H
