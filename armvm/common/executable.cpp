#include "executable.h"
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>

namespace ARMVM
{

void *Executable::operator new(size_t size)
{
    const static size_t pagesize = static_cast<size_t>(getpagesize());
    void *buf = memalign(size, 4);
    size_t pointer = reinterpret_cast<size_t>(buf);
    size += pointer % pagesize;
    mprotect(reinterpret_cast<void *>(pointer / pagesize * pagesize), size, PROT_READ | PROT_WRITE | PROT_EXEC);
    return buf;
}

void* Executable::operator new[](size_t size)
{
    return operator new(size);
}

void Executable::operator delete(void* p)
{
    free(p);
}

void Executable::operator delete[](void* p)
{
    operator delete(p);
}

}
