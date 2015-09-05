# \brief This is not yet tested

# GOAL:  Ultimate goal is to make this and all dependent libraries compliant
# with the CMake build organization described in ZJW Library

#
# https://cci-subv.uncc.edu/svn/zwartell-public/ZJW Library/trunk
#

# Initially generated via vcproj2cmake.rb from
# https://cci-subv.uncc.edu/svn/zwartell-public/ITCS 6120-8120/OpenGLTrainer/trunk/make/VS08 
#
# See svn log for progressive update history....
#
#

# TODOS:
# [T2] - how do I add non-source files to the IDE's build scripts?
#
cmake_minimum_required(VERSION 2.6)

#
# Handle incorrect zjw library cmake execution 
#
if(NOT DEFINED ZJW_LIBRARY_CMAKE)
    message(FATAL_ERROR	
	    "This file:\n"
	    "   ${CMAKE_CURRENT_LIST_FILE}\n" 
	    "should not be directly executed by CMake.\n"
	    "You should execute CMake on lists file within the sub-directory:\n"
	    "   make\\cmake\\"
	    )
endif(NOT DEFINED ZJW_LIBRARY_CMAKE)

#
# set project
#
project( OpenGLTrainer )

##
## find libraries 
##

# find libraries available through standard Cmake mechanisms...
find_package(OpenGL REQUIRED)
if (NOT CMAKE_HOST_WIN32)
    # zjw_find_package not ready for none Win32 compilation, so use standard. 
    find_package(GLUT 3.7.6 REQUIRED) 
endif (NOT CMAKE_HOST_WIN32)

# find libraries available through zjw_find_package mechanisms... 
get_filename_component(TPL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../Third Party Libraries" ABSOLUTE) 
#message(STATUS "TPL_DIR: ${TPL_DIR}")

zjw_find_package(glut 3.7.6 
    "https://cci-subv.uncc.edu/svn/zwartell-public/vendor/opengl.org/glut/" 
    ${TARGET_CONFIG_PATH_SUFFIX} 
    STATIC 
    ${TPL_DIR})

zjw_find_package(glew 1.4
    "https://cci-subv.uncc.edu/svn/zwartell-public/vendor/glew/local/precompiled"
    ${TARGET_CONFIG_PATH_SUFFIX} 
    STATIC 
    ${TPL_DIR})

# hacks for libraries not yet fully supported by zjw_find_package
if (ZJW_glew_FOUND AND NOT glew_FOUND) 
    # SVN checkout successful, but library needs manual compilation...
    set(GLEW_INCLUDE_DIR "${TPL_DIR}/glew/include")
    set(GLEW_LIBRARY_DIR "${TPL_DIR}/glew/lib")
    if (CMAKE_HOST_WIN32)
	# \todo this assuming linking to Static Release library...
	set(GLEW_LIBRARIES "glew32sd")
	set(GLEW_DEFINITIONS "-DGLEW_STATIC")
    else (CMAKE_HOST_WIN32)
	messsage(FATAL_ERROR "CMake GLEW_LIBRARIES variable must be manually set for this operating system") 
    endif (CMAKE_HOST_WIN32)
endif (ZJW_glew_FOUND AND NOT glew_FOUND) 

# "find" libraries that are not yet supported by standard CMake find_package, etc.

##
## set source code files for this project
##


set(SOURCES 
  Framebuffer.cpp
  OpenGLTrainer.cpp
  ZoomWindow.cpp
  PanZoomWindow.cpp
  Experimental/Framebuffer1.cpp
  Experimental/Framebuffer2.cpp
  Experimental/Framebuffer3.cpp
  Experimental/Testing.cpp

  ../README.html # \todo see [T2]
)

##
## set include directories
##
include_directories( 
  "${OPENGL_INCLUDE_DIR}/."
  "${GLUT_INCLUDE_DIR}/."
  "${GLEW_INCLUDE_DIR}/."
  "../"
)


##
## set compiler's pre-set macro definitions
##
add_definitions( 
    ${GLUT_DEFINITIONS}
    ${GLEW_DEFINITIONS}
    )

if(CMAKE_HOST_WIN32)
    add_definitions( 
      -DWIN32
      -D_CRT_SECURE_NO_DEPRECATE
      -D_CRT_SECURE_NO_WARNINGS
      -D_DEBUG
      -D_LIB
    )
else(CMAKE_HOST_WIN32)
    # \todo add requirements for other OS'
endif(CMAKE_HOST_WIN32)

##
## add library target to project and dependent libraries
##

#
# Common settings
#

# \todo much of this is boilerplate for zjw_find_package eventually it should
# be made into functions from "zjw library.cmake"

set(LIBRARY_NAME OpenGLTrainer)

add_library( ${LIBRARY_NAME} ${SOURCES} )


# OpenGLTrainer_DIR points to this source distribution that contains this
# target's source distribution
get_filename_component(OpenGLTrainer_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." 
	ABSOLUTE)
get_filename_component(OpenGLTrainer_INCLUDE_DIR 
	"${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
set(OpenGLTrainer_DEPENDENCY_TARGET ${LIBRARY_NAME})


add_dependencies(${LIBRARY_NAME}
    	${GLUT_DEPENDENCY_TARGET} 
        ${GLEW_DEPENDENCY_TARGET}
	)

link_directories( ${GLUT_LIBRARY_DIR} ${GLEW_LIBRARY_DIR} )

set(STD_LINK_LIBRARIES ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBARIES})


# distinguish shared and static library output
if(BUILD_SHARED_LIBS)
    set(OpenGLTrainer_LIBRARY_DIR "${OpenGLTrainer_DIR}/lib/${TARGET_CONFIG_PATH_SUFFIX}/Shared") # see [F1]

    set_property(
	    TARGET ${LIBRARY_NAME}
	    PROPERTY ARCHIVE_OUTPUT_DIRECTORY "${OpenGLTrainer_DIR}/lib/${TARGET_CONFIG_PATH_SUFFIX}/Shared")
    set_property(
	    TARGET ${LIBRARY_NAME}
	    PROPERTY RUNTIME_OUTPUT_DIRECTORY "${OpenGLTrainer_DIR}/bin/${TARGET_CONFIG_PATH_SUFFIX}/Shared")
else(BUILD_SHARED_LIBS)
    set(OpenGLTrainer_LIBRARY_DIR "${OpenGLTrainer_DIR}/lib/${TARGET_CONFIG_PATH_SUFFIX}/Static") # see [F1]

    set_property( TARGET ${LIBRARY_NAME}
	    # static linked library
	    PROPERTY ARCHIVE_OUTPUT_DIRECTORY "${OpenGLTrainer_DIR}/lib/${TARGET_CONFIG_PATH_SUFFIX}/Static")
    set_property(
	    TARGET ${LIBRARY_NAME}
	    PROPERTY RUNTIME_OUTPUT_DIRECTORY "${OpenGLTrainer_DIR}/bin/${TARGET_CONFIG_PATH_SUFFIX}/Static")
endif (BUILD_SHARED_LIBS)

#
# OS Specific settings
#
if (CMAKE_HOST_WIN32)

    set(OpenGLTrainer_LIBRARIES OpenGLTrainer.lib) # see [F1]

    add_definitions( 
      -D_CRT_SECURE_NO_DEPRECATE
      -D_CRT_SECURE_NO_WARNINGS
      )
    
    target_link_libraries(${LIBRARY_NAME} winmm.lib ${STD_LINK_LIBRARIES}) 

    if(BUILD_SHARED_LIBS)
	# build shared libraries 

	# ... nothing special for now...
    else(BUILD_SHARED_LIBS)
	# build static libraries 

	# ... nothing special for now...
	#set(OpenGLTrainer_DEFINITIONS "-DOpenGLTrainer_FORCE_BUILD_STATIC")
    endif (BUILD_SHARED_LIBS)

    set(OpenGLTrainer_FOUND TRUE) # see [F1]
else (CMAKE_HOST_WIN32)
    # \todo figure all this stuff out.... ARGGG!!!!!!!!!!!

    target_link_libraries(${LIBRARY_NAME} ${STD_LINK_LIBRARIES}) 

    set(OpenGLTrainer_DEFINITIONS "")
    set(OpenGLTrainer_FOUND FALSE) # see [F1]
endif (CMAKE_HOST_WIN32)

set(VAR_OUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../make/${MAKE_CONFIG_PATH_SUFFIX}")

# make these variables available to zjw_find_package via an include command
# (direct sets even with PARENT_SCOPE proved way to difficult)
file (WRITE  "${VAR_OUT_DIR}/VARS.txt" "set(OpenGLTrainer_LIBRARIES \"${OpenGLTrainer_LIBRARIES}\" PARENT_SCOPE)\n")
file (APPEND "${VAR_OUT_DIR}/VARS.txt" "set(OpenGLTrainer_INCLUDE_DIR \"${OpenGLTrainer_INCLUDE_DIR}\" PARENT_SCOPE)\n")
file (APPEND "${VAR_OUT_DIR}/VARS.txt" "set(OpenGLTrainer_LIBRARY_DIR \"${OpenGLTrainer_LIBRARY_DIR}\" PARENT_SCOPE)\n")
file (APPEND "${VAR_OUT_DIR}/VARS.txt" "set(OpenGLTrainer_DEFINITIONS \"${OpenGLTrainer_DEFINITIONS}\" PARENT_SCOPE)\n")
file (APPEND "${VAR_OUT_DIR}/VARS.txt" "set(OpenGLTrainer_DEPENDENCY_TARGET \"${OpenGLTrainer_DEPENDENCY_TARGET}\" PARENT_SCOPE)\n")

# SCRAP CODE
#execute_process (COMMAND "cmd" "/c" "dir" "..\..\..\..")
