set(CMAKE_SYSTEM_NAME    Windows)
set(CMAKE_SYSROOT        /usr/x86_64-w64-mingw32)
set(CMAKE_C_COMPILER    x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER  x86_64-w64-mingw32-g++)
# Toujours utiliser le sysroot pour includes et librairies
set(CMAKE_C_FLAGS         "--sysroot=${CMAKE_SYSROOT}")
set(CMAKE_CXX_FLAGS       "--sysroot=${CMAKE_SYSROOT}")
set(CMAKE_EXE_LINKER_FLAGS "--sysroot=${CMAKE_SYSROOT}")
# Ne chercher QUE dans le sysroot
set(CMAKE_FIND_ROOT_PATH       ${CMAKE_SYSROOT})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
