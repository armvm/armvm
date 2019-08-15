#ifndef A32_VMSTATE_H
#define A32_VMSTATE_H

#include <stdint.h>
#include <stdlib.h>

namespace ARMVM
{
namespace A32
{

struct VmState
{
    size_t reg[16] = { 0 };
    size_t cspr = 0;
    union
    {
        double f64[16] = { 0 };
        float f32[32];
    };
    size_t fpscr = 0;
    size_t fpexc = 0;
};

}
}

#endif // A32_VMSTATE_H
