#ifndef DEBUG_UTILS_HPP
#define DEBUG_UTILS_HPP

#include <GL3/GLTypes.hpp>

#if defined(__linux__)
    extern const char *__progname;
#endif

namespace GL3 {
    //! The function collection for printing the call stack
    class StackTrace
    {
    public:
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(__linux__)
    	static void PrintStack();
    #else
        static void PrintStack() {};
    #endif
    };

    //! The function collection related to opengl.
    class GL3Debug
    {
    public:
        //! Debug logging for opengl context with GL_ARB_debug_output extension.
        static void DebugLog(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const GLvoid* userParam);
    };
};

#endif //! end of DebugUtils.hpp