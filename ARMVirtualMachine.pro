TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    armvm/aarch32/a32_vmcontext.cpp \
    armvm/aarch32/a32_vmlauncher.cpp \
    armvm/common/executable.cpp \
    armvm/common/vmlauncherbase.cpp \
    main.cpp


HEADERS += \
    armvm/aarch32/a32_instruction.h \
    armvm/aarch32/a32_vmcontext.h \
    armvm/aarch32/a32_vmlauncher.h \
    armvm/aarch32/a32_vmstate.h \
    armvm/common/executable.h \
    armvm/common/vmlauncherbase.h \
    armvm/vmcallback.h \
    armvm/vmcontext.h \
    armvm/vmlauncher.h
