#ifndef OPENGLTRAINER_EXPORTS_H
#define OPENGLTRAINER_EXPORTS_H
/**
\file MyLibrary_exports.h
\author John Doe \n
   Copyright 2006.  Zachary Wartell, University of North Carolina at \n
   Charlotte. All rights reserved.  No one except the author has any \n
   rights to copy, duplicate, or even read this document for any \n
   purpose.   If you've been given permission to read it, you still should \n
   not distribute it further.

\brief This file defines macros that generate compiler and operating specific
storage class specifiers that indicate that the declared function or global
variable is dynamically linked and exported outside the library in which
it is defined.
*/

/**
\def OPENGLTRAINER_EXPORT
\brief Use this storage class specifier for exported functions and global 
variables.

Example:
    OPENGLTRAINER_EXPORT int aGlobalIn_MyLibrary;
    OPENGLTRAINER_EXPORT int a_function_in_MyLibrary(int a);
*/
/**
\def OPENGLTRAINER_CLASS
\brief Use this storage class specifier for C++ classes whose data members
or member functions are exported outside the library.
variables.

Example:
    class OPENGLTRAINER_CLASS MyClass 
	{
	};
*/
/**
\def OPENGLTRAINER_CLASS_STL_MEMBER
\brief Use this macro if a class has an STL template member and the class is exported.

Example:
    OPENGLTRAINER_CLASS_STL_MEMBER(std::vector<int>);

    class OPENGLTRAINER_CLASS MyClass 
	{
	std::vector<int> array;
	};

\internal
For Win32 explanation see: "HOWTO: Exporting STL Components Inside & Outside of a Class"
MSDN Article ID: Q168958.
*/
#ifdef _WIN32
#ifdef OPENGLTRAINER_EXPORTS

#ifdef __cplusplus
#define OPENGLTRAINER_EXPORT extern "C" __declspec(dllexport)
#define OPENGLTRAINER_CLASS  __declspec(dllexport)
#define OPENGLTRAINER_CLASS_STL_MEMBER(STL_TYPE_) template class __declspec(dllexport) STL_TYPE_
#else
#define OPENGLTRAINER_EXPORT extern __declspec(dllexport)
#endif

#else

#ifdef __cplusplus
#define OPENGLTRAINER_EXPORT extern "C" __declspec(dllimport)
#define OPENGLTRAINER_CLASS  __declspec(dllimport)
#define OPENGLTRAINER_CLASS_STL_MEMBER(STL_TYPE_) extern template class __declspec(dllimport) STL_TYPE_
#else
#define OPENGLTRAINER_EXPORT extern __declspec(dllimport)
#endif

#endif
#else
#ifdef __cplusplus
#define OPENGLTRAINER_EXPORT extern "C"
#define OPENGLTRAINER_CLASS  
#define OPENGLTRAINER_CLASS_STL_MEMBER(STL_TYPE_) extern int dummy_to_allow_semicolon_after_macro
#else 
#define OPENGLTRAINER_EXPORT extern
#define OPENGLTRAINER_CLASS  
#define OPENGLTRAINER_CLASS_STL_MEMBER(STL_TYPE_) extern int dummy_to_allow_semicolon_after_macro
#endif
#endif

#endif
