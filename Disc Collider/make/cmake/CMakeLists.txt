# \copyright Zachary Wartell, 2009
#
# This cmake file aims to conform to the definitions and build organization
# discussed in:
# 
#      svn://linux01.uncc.edu:3690/zwartell-public/ZJW Library/trunk/Programming Standards.doc
#
# CITATIONS:
# [1] svn://linux01.uncc.edu:3690/zwartell-public/ZJW Library/trunk/Programming Standards.doc
#
cmake_minimum_required (VERSION 2.6)

include("zjw library.cmake")
add_subdirectory("../.." "../${MAKE_CONFIG_PATH_SUFFIX}/")
