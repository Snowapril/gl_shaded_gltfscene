#ifndef MACROS_HPP
#define MACROS_HPP

#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS
#elif defined(__APPLE__)
#define APPLE
#ifndef IOS
#define MACOSX
#endif
#elif defined(linux) || defined(__linux__)
#define LINUX
#endif

#if defined(WINDOWS) && defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#else
#include <sys/types.h>
#endif

#ifndef UNUSED_VARIABLE
#define UNUSED_VARIABLE(x) ((void)x)
#endif

#endif //! end of Macros.hpp