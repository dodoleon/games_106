file(GLOB BASE_SRC "*.cpp" "*.hpp" "*.h" "../external/imgui/*.cpp")
file(GLOB BASE_HEADERS "*.hpp" "*.h")

set(KTX_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../external/ktx)
set(KTX_SOURCES
    ${KTX_DIR}/lib/texture.c
    ${KTX_DIR}/lib/hashlist.c
    ${KTX_DIR}/lib/checkheader.c
    ${KTX_DIR}/lib/swap.c
    ${KTX_DIR}/lib/memstream.c
    ${KTX_DIR}/lib/filestream.c)

add_library(base STATIC ${BASE_SRC} ${KTX_SOURCES})
if(WIN32)
    target_link_libraries(base ${Vulkan_LIBRARY} ${WINLIBS})
 else(WIN32)
    target_link_libraries(base ${Vulkan_LIBRARY} ${XCB_LIBRARIES} ${WAYLAND_CLIENT_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
endif(WIN32)

target_include_directories(
    base PUBLIC 
    ${PROJECT_SOURCE_DIR}/external
    ${PROJECT_SOURCE_DIR}/external/glm
    ${PROJECT_SOURCE_DIR}/external/gli
    ${PROJECT_SOURCE_DIR}/external/imgui
    ${PROJECT_SOURCE_DIR}/external/tinygltf
    ${PROJECT_SOURCE_DIR}/external/ktx/include
    ${PROJECT_SOURCE_DIR}/external/ktx/other_include
    ${PROJECT_SOURCE_DIR}/base
)

target_compile_definitions(base PUBLIC "-DNOMINMAX -D_USE_MATH_DEFINES")