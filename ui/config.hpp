/*!
* THUOSU Library Defination
*
* \brief
*
*/

#ifndef THUOSU_CONFIG_HPP
#define THUOSU_CONFIG_HPP

#if defined(_WIN32)
#define THUOSU_WIN32 1
//!\brief operating system

#define THUOSU_WINDOWS 1
#elif defined(_WIN64)
#define THUOSU_WIN64 1
#define THUOSU_WINDOWS 1
#elif defined(linux) || defined(__linux) || defined(__linux__)
#define THUOSU_LINUX 1
#elif defined(__MACH__)
#define THUOSU_MACOSX 1
#endif

//!\brief architecture

#if defined(i386) || defined(__i386) || defined(__i386__) || defined(_M_I86) || defined(_M_IX86) || defined(_X86_)
#define THUOSU_X86 1 
#elif defined(__ia64__) || defined(_IA64) || defined(__IA64__) || defined(_M_IA64)
#define THUOSU_IA64 1
#elif defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_640) || defined(_M_X64) || defined(_M_AMD64)
#define THUOSU_X64 1
#endif

#endif // THUOSU_CONFIG_HPP