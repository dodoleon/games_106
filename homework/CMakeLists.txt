# Function for building single homework
function(buildHomework HOMEWORK_NAME)
	SET(HOMEWORK_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/${HOMEWORK_NAME})
	message(STATUS "Generating project file for homework in ${HOMEWORK_FOLDER}")
	# Main
	file(GLOB SOURCE *.cpp ${BASE_HEADERS} ${HOMEWORK_FOLDER}/*.cpp)
	SET(MAIN_CPP ${HOMEWORK_FOLDER}/${HOMEWORK_NAME}.cpp)
	if(EXISTS ${HOMEWORK_FOLDER}/main.cpp)
		SET(MAIN_CPP ${HOMEWORK_FOLDER}/main.cpp)
	ENDIF()
	if(EXISTS ${HOMEWORK_FOLDER}/${HOMEWORK_NAME}.h)
		SET(MAIN_HEADER ${HOMEWORK_FOLDER}/${HOMEWORK_NAME}.h)
	ENDIF()
	# imgui homework requires additional source files
	IF(${HOMEWORK_NAME} STREQUAL "imgui")
		file(GLOB ADD_SOURCE "../external/imgui/*.cpp")
		SET(SOURCE ${SOURCE} ${ADD_SOURCE})
	ENDIF()
	# wayland requires additional source files
	IF(USE_WAYLAND_WSI)
		SET(SOURCE ${SOURCE} ${CMAKE_BINARY_DIR}/xdg-shell-client-protocol.h ${CMAKE_BINARY_DIR}/xdg-shell-protocol.c)
	ENDIF()
	# Add shaders
	set(SHADER_DIR_GLSL "../data/homework/shaders/glsl/${HOMEWORK_NAME}")
	file(GLOB SHADERS_GLSL "${SHADER_DIR_GLSL}/*.vert" "${SHADER_DIR_GLSL}/*.frag" "${SHADER_DIR_GLSL}/*.comp" "${SHADER_DIR_GLSL}/*.geom" "${SHADER_DIR_GLSL}/*.tesc" "${SHADER_DIR_GLSL}/*.tese" "${SHADER_DIR_GLSL}/*.mesh" "${SHADER_DIR_GLSL}/*.task" "${SHADER_DIR_GLSL}/*.rgen" "${SHADER_DIR_GLSL}/*.rchit" "${SHADER_DIR_GLSL}/*.rmiss" "${SHADER_DIR_GLSL}/*.rcall")
	set(SHADER_DIR_HLSL "../data/homework/shaders/hlsl/${HOMEWORK_NAME}")
	file(GLOB SHADERS_HLSL "${SHADER_DIR_HLSL}/*.vert" "${SHADER_DIR_HLSL}/*.frag" "${SHADER_DIR_HLSL}/*.comp" "${SHADER_DIR_HLSL}/*.geom" "${SHADER_DIR_HLSL}/*.tesc" "${SHADER_DIR_HLSL}/*.tese" "${SHADER_DIR_HLSL}/*.mesh" "${SHADER_DIR_HLSL}/*.task" "${SHADER_DIR_HLSL}/*.rgen" "${SHADER_DIR_HLSL}/*.rchit" "${SHADER_DIR_HLSL}/*.rmiss" "${SHADER_DIR_HLSL}/*.rcall")
	source_group("Shaders\\GLSL" FILES ${SHADERS_GLSL})
	source_group("Shaders\\HLSL" FILES ${SHADERS_HLSL})
	# Add optional readme / tutorial
	file(GLOB README_FILES "${HOMEWORK_FOLDER}/*.md")
	if(WIN32)
		add_executable(${HOMEWORK_NAME} WIN32 ${MAIN_CPP} ${SOURCE} ${MAIN_HEADER} ${SHADERS_GLSL} ${SHADERS_HLSL} ${README_FILES})
		target_link_libraries(${HOMEWORK_NAME} base ${Vulkan_LIBRARY} ${WINLIBS})
	else(WIN32)
		add_executable(${HOMEWORK_NAME} ${MAIN_CPP} ${SOURCE} ${MAIN_HEADER} ${SHADERS_GLSL} ${SHADERS_HLSL} ${README_FILES})
		target_link_libraries(${HOMEWORK_NAME} base )
	endif(WIN32)

	set_target_properties(${HOMEWORK_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
	if(${HOMEWORK_NAME} STREQUAL "texture3d")
		if(APPLE)
			# SRS - Use MacPorts paths as default since the same on x86 and Apple Silicon, can override for homebrew on cmake command line
			if(NOT OpenMP_omp_LIBRARY AND EXISTS /opt/local/lib/libomp/libomp.dylib)
				set(OpenMP_omp_LIBRARY /opt/local/lib/libomp/libomp.dylib)
			endif()
			if(CMAKE_C_COMPILER_ID MATCHES "Clang\$")
				set(OpenMP_C_FLAGS "-Xclang -fopenmp")
				set(OpenMP_C_LIB_NAMES "omp")
				if(NOT OpenMP_C_INCLUDE_DIR AND EXISTS /opt/local/include/libomp)
					set(OpenMP_C_INCLUDE_DIR /opt/local/include/libomp)
				endif()
			endif()
			if(CMAKE_CXX_COMPILER_ID MATCHES "Clang\$")
				set(OpenMP_CXX_FLAGS "-Xclang -fopenmp")
				set(OpenMP_CXX_LIB_NAMES "omp")
				if(NOT OpenMP_CXX_INCLUDE_DIR AND EXISTS /opt/local/include/libomp)
					set(OpenMP_CXX_INCLUDE_DIR /opt/local/include/libomp)
				endif()
			endif()
		endif()
		find_package(OpenMP)
		if(OpenMP_CXX_FOUND)
   			target_link_libraries(${HOMEWORK_NAME} OpenMP::OpenMP_CXX)
		endif()
	endif()

	if(RESOURCE_INSTALL_DIR)
		install(TARGETS ${HOMEWORK_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
	endif()
endfunction(buildHomework)

# Build all homework
function(buildHomeworks)
	foreach(HOMEWORK ${HOMEWORKS})
		buildHomework(${HOMEWORK})
	endforeach(HOMEWORK)
endfunction(buildHomeworks)

set(HOMEWORKS
	homework0
	homework1
)

buildHomeworks()

add_subdirectory(homework5)