cmake_minimum_required(VERSION 3.20.0)
project(prog)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Set your project compile flags.
# E.g. if using the C++ header files
# you will need to enable C++11 support
# for your compiler.

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# Now build our tools
file(GLOB SOURCES "*.cpp")
add_executable(${PROJECT_NAME}
	${SOURCES}
)

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs
	bitwriter
	core
	executionengine
	mc
	mcjit
	support
	nativecodegen
)

# Link against LLVM libraries
target_link_directories(${PROJECT_NAME} PUBLIC $ENV{LLVM_LIB_DIRS})
target_link_libraries(${PROJECT_NAME}
	${llvm_libs}
)
