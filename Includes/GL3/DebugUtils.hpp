#ifndef DEBUG_UTILS_HPP
#define DEBUG_UTILS_HPP

#include <GL3/GLTypes.hpp>

namespace GL3 {

    //!
    //! \brief      Collection of useful debugging util functions.
    //!
    class DebugUtils
    {
    public:
        //! Print stack trace from the current code line.
    #if defined(_WIN32) || defined(__linux__)
    	static void PrintStack();
    #else
        static void PrintStack() {};
    #endif
        //! Debug logging for opengl context.
        //! This function will be passed into glDebugMessageCallback call
        static void DebugLog(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);
    };

};

#endif //! end of DebugUtils.hpp
