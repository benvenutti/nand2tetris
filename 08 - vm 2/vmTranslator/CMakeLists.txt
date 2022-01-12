file(GLOB sources src/*.cpp main.cpp)
include_directories(include)
add_executable(VMTranslator ${sources})
