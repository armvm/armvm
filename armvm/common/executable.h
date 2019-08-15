#ifndef EXECUTABLE_H
#define EXECUTABLE_H

#include <stdlib.h>

namespace ARMVM
{

class Executable
{
public:

    void *operator new(size_t size);

    void* operator new[](size_t size);

    void operator delete(void* p);

    void  operator delete[](void* p);
};

}

#endif // EXECUTABLE_H
