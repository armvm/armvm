#ifndef VMLAUNCHER_H
#define VMLAUNCHER_H

#include "vmcallback.h"
#include "vmcontext.h"

#include <memory>

namespace ARMVM
{
    class VmLauncher
    {
    public:
        static std::unique_ptr<VmLauncher> Create();

        virtual void Run(std::unique_ptr<VmCallback>& callback) = 0;

        virtual void SaveContext(const std::unique_ptr<VmContext>& ctx) = 0;

        virtual void LoadContext(const std::unique_ptr<VmContext>& ctx) = 0;

        virtual std::unique_ptr<VmContext> NewContext() const = 0;

        virtual ~VmLauncher() = default;
    };
}



#endif // VMLAUNCHER_H
