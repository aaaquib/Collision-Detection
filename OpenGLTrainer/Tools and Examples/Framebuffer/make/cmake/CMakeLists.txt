# \copyright Zachary Wartell, 2009
#
# This cmake file aims to conform to the definitions and build organization
# discussed in:
# 
#      https://cci-subv.uncc.edu/svn/zwartell-public/ZJW Library/trunk/Programming Standards.doc
#
# CITATIONS:
# [1] https://cci-subv.uncc.edu/svn/zwartell-public/ZJW Library/trunk/Programming Standards.doc
#
cmake_minimum_required (VERSION 2.6)

include("zjw library.cmake")
add_subdirectory("../.." "../${MAKE_CONFIG_PATH_SUFFIX}/")
