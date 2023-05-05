set(TARGET_NAME homework5)

option(QEM_USE_OPENMESH "Link in OpenMesh to be used by QEM algorithm" OFF)
option(QEM_USE_UHEMESH "Include OpenMesh to be used by QEM algorithm" ON)

if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(libigl)
include(assimp)
if (QEM_USE_OPENMESH)
include(openmesh)
endif()

set(CMAKE_CXX_STANDARD 17)

# Add your project files
file(GLOB SRC_FILES src/*.cpp src/*.h)
add_executable(${TARGET_NAME} ${SRC_FILES})
target_include_directories(${TARGET_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(${TARGET_NAME} PUBLIC igl::glfw igl::imgui assimp::assimp)

if (QEM_USE_OPENMESH)
    target_link_libraries(${TARGET_NAME} PUBLIC OpenMeshCore OpenMeshTools)
    target_compile_definitions(${TARGET_NAME} PUBLIC _USE_MATH_DEFINES)
endif()

if (QEM_USE_UHEMESH)
    target_compile_definitions(${TARGET_NAME} PUBLIC QEM_USE_UHEMESH)
endif()

if (QEM_USE_OPENMESH)
    target_compile_definitions(${TARGET_NAME} PUBLIC QEM_USE_OPENMESH)
endif()