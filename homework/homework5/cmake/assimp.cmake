if(TARGET assimp::assimp)
    return()
endif()

include(FetchContent)
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v5.2.5
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL_PDB OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(assimp)

set_target_properties(assimp PROPERTIES FOLDER "3rdparty/assimp")
set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER "3rdparty/assimp")
set_target_properties(zlibstatic PROPERTIES FOLDER "3rdparty/assimp")