cmake_minimum_required(VERSION 3.8.2)
project(Vulkan_Test)

IF (CMAKE_COMPILER_IS_GNUCC)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexceptions")
ELSEIF (MSVC71 OR MSVC80)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /EHc-")
ENDIF (CMAKE_COMPILER_IS_GNUCC)

# Creates C resources file from files in given directory
function(create_resources dir coutput houtput)
    # Create empty output file
    file(REMOVE ${coutput})
    file(REMOVE ${houtput})

    file(WRITE ${coutput} "")
    file(WRITE ${houtput} "")

    # Collect input files
    file(GLOB bins ${dir}/*)
    # Iterate through input files
    file(APPEND ${houtput} "#ifndef VULKAN_TEST_SHADERS_H\n#define VULKAN_TEST_SHADERS_H\n")

    foreach (bin ${bins})
        # Get short filename
        string(REGEX MATCH "([^/]+)$" filename ${bin})
        # Replace filename spaces & extension separator for C compatibility
        string(MAKE_C_IDENTIFIER ${filename} filename)
        # Read hex data from file
        file(READ ${bin} filedata HEX)
        # Convert hex data for C compatibility
        string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata ${filedata})
        # Append data to output file
        file(APPEND ${coutput} "const unsigned char ${filename}[] = {${filedata}}; \nconst unsigned long long ${filename}_size = sizeof(${filename});\n")
        file(APPEND ${houtput} "extern \"C\" const unsigned char ${filename}[];\nextern \"C\" const unsigned long long ${filename}_size;\n")
    endforeach ()

    file(APPEND ${houtput} "#endif\n")
endfunction()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(../../../../VulkanSDK/1.0.65.0/Include)
include_directories(../../../../Users/chakm/Desktop/DevIL/DevIL/include)
include_directories(../../../../Users/chakm/Desktop/assimp/include)

set(ASSIMP_LIB ${PROJECT_SOURCE_DIR}/../../Desktop/assimp/build/code/Release)

#create_resources("Shader Resources" resources.c resources.h)

set(SOURCE_FILES main.cpp "Vulkan Engine.cpp" "Vulkan Engine Exception.cpp")

add_executable(Vulkan_Test ${SOURCE_FILES})

target_link_libraries(Vulkan_Test ${PROJECT_SOURCE_DIR}/../../../../VulkanSDK/1.0.65.0/Lib/vulkan-1.lib ${PROJECT_SOURCE_DIR}/../../Desktop/DevIL/DevIL/build/lib/x64/Release/DevIL.lib)
#        ${PROJECT_SOURCE_DIR}/linkables/0_png.o
#        ${PROJECT_SOURCE_DIR}/linkables/0n_png.o
#        ${PROJECT_SOURCE_DIR}/linkables/0s_png.o
#        ${PROJECT_SOURCE_DIR}/linkables/1_png.o
#        ${PROJECT_SOURCE_DIR}/linkables/1n_png.o
#        ${PROJECT_SOURCE_DIR}/linkables/1s_png.o
#        ${PROJECT_SOURCE_DIR}/linkables/2_png.o
#        ${PROJECT_SOURCE_DIR}/linkables/2n_png.o
#        ${PROJECT_SOURCE_DIR}/linkables/2s_png.o
#        ${PROJECT_SOURCE_DIR}/linkables/3_png.o
#        ${PROJECT_SOURCE_DIR}/linkables/3n_png.o
#        ${PROJECT_SOURCE_DIR}/linkables/3s_png.o
#        ${PROJECT_SOURCE_DIR}/linkables/cygwin1_dll.o
#        ${PROJECT_SOURCE_DIR}/linkables/cygstdc++-6_dll.o
#        ${PROJECT_SOURCE_DIR}/linkables/cyggcc_s-seh-1_dll.o
#        ${PROJECT_SOURCE_DIR}/linkables/nyra_obj.o)

#target_link_libraries (Vulkan_Test ${PROJECT_SOURCE_DIR}/../../../../VulkanSDK/1.0.65.0/Lib/vulkan-1.lib ${PROJECT_SOURCE_DIR}/../../Desktop/DevIL/DevIL/build/lib/x64/Release/DevIL.lib
#        ${PROJECT_SOURCE_DIR}/../Vulkan_Test ${PROJECT_SOURCE_DIR}/linkables/0_png.o ${PROJECT_SOURCE_DIR}/linkables/0n_png.o ${PROJECT_SOURCE_DIR}/linkables/0s_png.o ${PROJECT_SOURCE_DIR}/linkables/1_png.o ${PROJECT_SOURCE_DIR}/linkables/1n_png.o ${PROJECT_SOURCE_DIR}/linkables/1s_png.o ${PROJECT_SOURCE_DIR}/linkables/2_png.o ${PROJECT_SOURCE_DIR}/linkables/2n_png.o ${PROJECT_SOURCE_DIR}/linkables/2s_png.o ${PROJECT_SOURCE_DIR}/linkables/3_png.o ${PROJECT_SOURCE_DIR}/linkables/3n_png.o ${PROJECT_SOURCE_DIR}/linkables/3s_png.o ${PROJECT_SOURCE_DIR}/linkables/nyra_obj.o ${PROJECT_SOURCE_DIR}/linkables/cyggcc_s-seh-1_dll.o ${PROJECT_SOURCE_DIR}/linkables/cygstdc++-6_dll.o ${PROJECT_SOURCE_DIR}/linkables/cygwin1_dll.o)