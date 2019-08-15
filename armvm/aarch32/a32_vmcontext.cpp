#include "a32_vmcontext.h"
#include <memory>

namespace ARMVM
{

namespace A32
{

VmContext::~VmContext()
{

}

void VmContext::Reset()
{
    memset(&state, 0, sizeof(state));
}
size_t VmContext::GetCpuRegister(size_t index) const
{
    return state.reg[index];
}
void VmContext::SetCpuRegister(size_t index, size_t value)
{
    state.reg[index] = value;
}
size_t VmContext::GetCpsr() const
{
    return state.cspr;
}
void VmContext::SetCpsr(size_t value)
{
    state.cspr = value;
}

float VmContext::GetFpuRegister(size_t index) const
{
    return state.f32[index];
}
void VmContext::SetFpuRegister(size_t index, float value)
{
    state.f32[index] = value;
}
size_t VmContext::GetFpscr() const
{
    return state.fpscr;
}
void VmContext::SetFpscr(size_t value)
{
    state.fpscr = value;
}
size_t VmContext::GetFpexc() const
{
    return 0;//state.fpexc;
}

void VmContext::SetFpexc(size_t value)
{
    //state.fpexc = value;
}

}

}
