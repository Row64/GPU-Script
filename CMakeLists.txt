
cmake_minimum_required (VERSION 3.0)
project( "ExampleApp" )

message(STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR})
message(STATUS "CMAKE_SOURCE_DIR: " ${CMAKE_SOURCE_DIR})


######################
# User Set Variables #
######################

# If you are using the system installed Vulkan, leave this blank.
# If you are using a local install of Vulkan, set this to your VulkanSDK path.
set( VULKAN_SDK )


#################################################
# CMAKE flags and platform specific definitions #
#################################################

if ( WIN32 )
	set(CMAKE_CXX_STANDARD 17)
elseif ( APPLE )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++1z -Wno-c++11-narrowing" )
elseif ( UNIX )
	set(CMAKE_CXX_STANDARD 17)
endif()

#####################
# Set Tutorial Flag #
#####################

set(TUTORIAL ON)
set(TUTORIAL_LIST
	"Tutorials/T01_Particles"
	"Tutorials/T02_Ocean"
	"Tutorials/T03_Ice"
)

#############################
# Project specific includes #
#############################

include_directories( "Include" )
include_directories( "Include/COM_Solvers" )
include_directories( "Include/Compute" )
include_directories( "Include/Core" )
include_directories( "Include/ProjectSetup" )
include_directories( "Include/Layouts" )
include_directories( "Include/SUI_Panes" )
include_directories( "Include/TUI_Panes" )
include_directories( "Include/User" )
include_directories( "UILib" )

include_directories( "External" )
include_directories( "External/imgui" )
include_directories( "External/glm" )

file( GLOB_RECURSE SHARED_SOURCE_FILES
	"Include/*.cpp" 
	"UILib/*.cpp"
)

file( GLOB EXTERNAL_SOURCE_FILES
	"External/imgui/*.cpp"
	"External/*.cpp"
)

set( ALL_SHARED_FILES
	${SHARED_SOURCE_FILES}
	${EXTERNAL_SOURCE_FILES} )

if ( TUTORIAL )
	foreach( tutorial ${TUTORIAL_LIST} )
		include_directories( ${tutorial} )
		file( GLOB TUTORIAL_SRC "${tutorial}/*.cpp")
		set( ALL_SHARED_FILES ${ALL_SHARED_FILES} ${TUTORIAL_SRC} )
	endforeach()
endif()
	
##################
# Add Executable #
##################

# Executable
add_executable( ${PROJECT_NAME} ${ALL_SHARED_FILES}
	main.cpp
)
target_compile_definitions( ${PROJECT_NAME} PRIVATE USE_SWAPCHAIN_EXTENSIONS )


######################
# External Libraries #
######################

# Statically link to GLFW library
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(External/glfw)
target_link_libraries (${PROJECT_NAME} glfw)

if( EXISTS "${VULKAN_SDK}" )

	message( STATUS "Attempting to link with VulkanSDK located: " ${VULKAN_SDK} )

	if ( WIN32 )

		find_path(VULKAN_INCLUDE_DIR
			NAMES vulkan/vulkan.h
			PATHS 
				"${VULKAN_SDK}/Include"
				NO_DEFAULT_PATH 
		)

		find_library(VULKAN_LIBRARY
			NAMES vulkan-1
			PATHS
				"${VULKAN_SDK}/Lib"
				"${VULKAN_SDK}/Bin"
				NO_DEFAULT_PATH 
		)
	
	else()

		find_path(VULKAN_INCLUDE_DIR
			NAMES vulkan/vulkan.h
			PATHS 
				"${VULKAN_SDK}/include"
				NO_DEFAULT_PATH 
		)

		find_library(VULKAN_LIBRARY
			NAMES vulkan
			PATHS
				"${VULKAN_SDK}/lib"
				"${VULKAN_SDK}/bBin"
				NO_DEFAULT_PATH 
		)
	endif()

	message(STATUS "Vulkan include directories: " ${VULKAN_INCLUDE_DIR})
	message(STATUS "Vulkan library: " ${VULKAN_LIBRARY} )
	include_directories( ${VULKAN_INCLUDE_DIR} )
	target_link_libraries( ${PROJECT_NAME} ${VULKAN_LIBRARY})

else()

	message( STATUS "Attempting to link with system installed Vulkan")

	# Statically link with Vulkan SDK loader
	if ( UNIX AND NOT APPLE )
	else ()
		# Environment path needed for Windows and Mac
		if( NOT DEFINED ENV{VULKAN_SDK} )
			message( FATAL_ERROR "VULKAN_SDK not found!" )
		endif()
		set( VULKAN_SDK "$ENV{VULKAN_SDK}" )
	endif()

	find_package(Vulkan REQUIRED)
	if (VULKAN_FOUND)
		message(STATUS "Found Vulkan, Including and Linking now")
		message(STATUS "Including Vulkan directories: " ${Vulkan_INCLUDE_DIRS})
		message(STATUS "Vulkan library: " ${Vulkan_LIBRARIES})
		include_directories(${Vulkan_INCLUDE_DIRS})
		target_link_libraries (${PROJECT_NAME} ${Vulkan_LIBRARIES})
	endif (VULKAN_FOUND)

endif()



# Boost

if ( WIN32 )
	set(boost_min_ver 1.70.0)
	set(BOOST_ROOT "C:/local/boost_1_70_0")
	set(BOOST_INCLUDEDIR "${BOOST_ROOT}/boost")
	set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib64-msvc-14.2")
	set(Boost_USE_STATIC_LIBS	ON)
	set(Boost_ARCHITECTURE 		"-x64")
elseif ( APPLE )
	set(boost_min_ver 1.69.0)
endif()

set(boost_libs system filesystem)
find_package(Boost ${boost_min_ver})
 
if(Boost_FOUND)
	find_package(Boost ${boost_min_ver} COMPONENTS ${boost_libs})
	include_directories(${Boost_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()


# Native File Dialog
add_subdirectory(External/nativefiledialog)
target_link_libraries(${PROJECT_NAME} nativefiledialog)


############################
# Copying Data and Shaders #
############################

# Specify GLSL compiler
if ( UNIX AND NOT APPLE )
	set( GLSL_COMPILER glslangValidator -V )
else ()
	set( GLSL_COMPILER ${VULKAN_SDK}/bin/glslangValidator -V )
endif()
message(STATUS "GLSL_COMPILER: " ${GLSL_COMPILER})

# Macro to copy data files to build/Data
macro( DRC_COPY_DATA _folder _destfolder )
	file( GLOB DATA_FILES "${CMAKE_SOURCE_DIR}/${_folder}/*.*" )
	file( COPY ${DATA_FILES} DESTINATION "${CMAKE_SOURCE_DIR}/build/${_destfolder}/" )
endmacro()

# Macro to cycle through each shader file and compile with GLSL

macro( DRC_COMPILE_SHADERS _folder )

	file( GLOB_RECURSE FRAG_SHADER_FILES "${CMAKE_SOURCE_DIR}/${_folder}/*.frag")
	file( GLOB_RECURSE VERT_SHADER_FILES "${CMAKE_SOURCE_DIR}/${_folder}/*.vert")
	file( GLOB_RECURSE COMP_SHADER_FILES "${CMAKE_SOURCE_DIR}/${_folder}/*.comp")

	file( COPY ${FRAG_SHADER_FILES} DESTINATION "${CMAKE_SOURCE_DIR}/build/Shaders/" )
	file( COPY ${VERT_SHADER_FILES} DESTINATION "${CMAKE_SOURCE_DIR}/build/Shaders/" )
	file( COPY ${COMP_SHADER_FILES} DESTINATION "${CMAKE_SOURCE_DIR}/build/Shaders/" )

	foreach (shaderfile ${FRAG_SHADER_FILES} ${VERT_SHADER_FILES} ${COMP_SHADER_FILES} )
		message (STATUS "Compiling Shader: " ${shaderfile})
		set(VERTFRAG ${shaderfile})
		get_filename_component(VERTFRAG_NAME ${shaderfile} NAME)
		set(VERTFRAG_INCLUDE ${CMAKE_SOURCE_DIR}/build/Shaders/${VERTFRAG_NAME}.spv)
		message(STATUS "SPV File: " ${VERTFRAG_INCLUDE})
		add_custom_command(
			MAIN_DEPENDENCY ${VERTFRAG}
			OUTPUT ${VERTFRAG_INCLUDE}
			COMMAND ${GLSL_COMPILER} -o ${VERTFRAG_INCLUDE} ${VERTFRAG}
		)
		set( SHADER_DEPENDENCIES ${SHADER_DEPENDENCIES} ${VERTFRAG_INCLUDE})
	endforeach()

endmacro()

set( SHADER_DEPENDENCIES )
DRC_COPY_DATA( "Data" "Data" )
DRC_COPY_DATA( "Data/Icons" "Data/Icons" )
DRC_COMPILE_SHADERS( "Shaders" )

if ( TUTORIAL )
	foreach ( tutorial ${TUTORIAL_LIST} )
		message( STATUS "Adding Tutorial: " ${tutorial})
		if( EXISTS "${CMAKE_SOURCE_DIR}/${tutorial}/Data" )
			message( STATUS "Copying Data: " "${tutorial}/Data")
			DRC_COPY_DATA( "${tutorial}/Data" "Data" )
		endif()
		DRC_COMPILE_SHADERS( ${tutorial} )
	endforeach()
endif()

add_custom_target(
	${PROJECT_NAME}_shaders
	COMMENT "Adding shader dependencies"
	DEPENDS ${SHADER_DEPENDENCIES}
)
add_dependencies( ${PROJECT_NAME} ${PROJECT_NAME}_shaders )