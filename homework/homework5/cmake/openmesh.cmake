if(TARGET OpenMeshCore)
    return()
endif()

include(FetchContent)
FetchContent_Declare(
    openmesh
    GIT_REPOSITORY https://gitlab.vci.rwth-aachen.de:9000/OpenMesh/OpenMesh.git
    GIT_TAG OpenMesh-9.0
)

FetchContent_MakeAvailable(openmesh)
