# CMakeLists.txt
cmake_minimum_required(VERSION 3.0)
project(SIMHYD)  # Set your project name here
enable_language(C)

# Add your source files here
set(SOURCE_FILES
    main.c
    Func_Snow.c
    Func_IO.c
    Func_View.c
    Func_Para.c
    Func_SIMHYD.c
    Func_Metrics.c
)


# Add the executable target
add_executable(SIMHYD ${SOURCE_FILES})
# Link against the math library
target_link_libraries(SIMHYD m)


## cmake -G "MinGW Makefiles" ..
## mingw32-make
