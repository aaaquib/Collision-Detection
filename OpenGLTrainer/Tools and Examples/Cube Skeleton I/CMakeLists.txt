# \author Zachary Wartell
#
# \brief This is not yet tested

# Mostly, students will need to only adjust the "set(SOURCES ..."
# command.

# IMPLEMENTATION DETAILS:
# Initially generated via vcproj2cmake.rb from
#
# https://cci-subv.uncc.edu/svn/zwartell-public/ITCS 6120-8120/OpenGLTrainer/trunk/make/VS08 
#
# See svn log for more details on progress in getting this file working.

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
project( "OpenGL and GLUT - 101" )

##
## find libraries 
##

# find libraries available through standard Cmake mechanisms...
find_package(OpenGL REQUIRED)
if (NOT CMAKE_HOST_WIN32)
    # zjw_find_package not ready for none Win32 compilation, so use standard. 
    find_package(GLUT 3.7.6 REQUIRED) 
endif (NOT CMAKE_HOST_WIN32)

# find libraries available through zjw_find_package mechanism... 
get_filename_component(TPL_DIR_FULL_PATH 
	"${CMAKE_CURRENT_SOURCE_DIR}/../../../" ABSOLUTE)
message(STATUS "${TPL_DIR_FULL_PATH}")

if (CMAKE_HOST_WIN32)
    zjw_find_package(glut 3.7.6 
	"https://cci-subv.uncc.edu/svn/zwartell-public/vendor/opengl.org/glut/" 
	${TARGET_CONFIG_PATH_SUFFIX} 
	STATIC 
	${TPL_DIR_FULL_PATH})
endif (CMAKE_HOST_WIN32)# AND FALSE)

zjw_find_package(OpenGLTrainer Latest 
    "https://cci-subv.uncc.edu/svn/zwartell-public/ITCS 6120-8120/OpenGLTrainer/trunk"
    ${TARGET_CONFIG_PATH_SUFFIX} 
    STATIC 
    ${TPL_DIR_FULL_PATH})

## hacks for libraries that are not yet supported by standard CMake find_package
## (nor or have I had time to go the zjw_find_package route.)

# GLEW
set(GLEW_INCLUDE_DIR "${TPL_DIR_FULL_PATH}/glew/include")
set(GLEW_LIBRARY_DIR "${TPL_DIR_FULL_PATH}/glew/lib")
if (CMAKE_HOST_WIN32)
    # \todo this assuming linking to Static Debug library...
    set(GLEW_LIBRARIES "glew32sd")
    set(GLEW_DEFINITIONS "-DGLEW_STATIC")
else (CMAKE_HOST_WIN32)
    messsage(FATAL_ERROR "CMake GLEW_LIBRARIES variable must be manually set for this operating system") 
endif (CMAKE_HOST_WIN32)


##
## set source code files for this project
##
set(SOURCES 
  Main.cpp

  #ITCS4120.vssettings  # \todo see [T2]
)

##
## set include directories, link directories, etc.
##
include_directories( 
   "${OpenGLTrainer_INCLUDE_DIR}/."
   "${OPENGL_INCLUDE_DIR}/."
   "${GLUT_INCLUDE_DIR}/."
   "${GLEW_INCLUDE_DIR}/."

   # For now, I list every TPL I've every used for any class project.
   # These have not been converted into the zjw_find_package convention
   # so must be installed and listed independently....
  "$ENV{UNCC_ITCS_GRAPHICS}/Third Party Libraries/glew/include"
)

link_directories( 
	"${GLUT_LIBRARY_DIR}/." 
	"${GLEW_LIBRARY_DIR}/." 
	"${OpenGLTrainer_LIBRARY_DIR}/." 
	)  

##
## set compiler's pre-set macro definitions
##
add_definitions( 
	"${GLUT_DEFINITIONS}"
	"${GLEW_DEFINITIONS}"
	"${OpenGLTrainer_DEFINITIONS}" 
	)
if (CMAKE_HOST_WIN32)
    add_definitions( 
      -DWIN32
      -D_CONSOLE
      -D_DEBUG	    # \todo I'm suspicious of this. Its from vcproj2cmake.rb
    )
else(CMAKE_HOST_WIN32)
    # \todo add requirements for other OS's
endif(CMAKE_HOST_WIN32)

##
## add executable target to project
##
if (CMAKE_HOST_WIN32)
   # target .exe with spaces breaks the manifest file arguement
   # to the MSVS linker, so...
   set(TARGET_NAME "OpenGL_and_GLUT_-_101")
   add_executable(${TARGET_NAME} ${SOURCES} )

   # but this is ok...
   set_target_properties (${TARGET_NAME}
	PROPERTIES PROJECT_LABEL "OpenGL and GLUT - 101"
	)
else(CMAKE_HOST_WIN32)
   set(TARGET_NAME "OpenGL and GLUT - 101")
   add_executable(${TARGET_NAME} ${SOURCES} )
endif(CMAKE_HOST_WIN32)


##
## add dependent libraries
##

# for IDE/native build scripts
add_dependencies(${TARGET_NAME}
    	${GLUT_DEPENDENCY_TARGET} 
        ${OpenGLTrainer_DEPENDENCY_TARGET} 
        ${GLEW_DEPENDENCY_TARGET}
	)

# for linker
target_link_libraries( ${TARGET_NAME}
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARIES} 
    ${GLEW_LIBRARIES} 
    ${OpenGLTrainer_LIBRARIES} 
    )
