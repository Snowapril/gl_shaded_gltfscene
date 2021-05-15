#ifndef DEBUG_UTILS_HPP
#define DEBUG_UTILS_HPP

#include <GL3/GLTypes.hpp>
#include <string>

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

        //! Enable object & scope labeling for debug output
        static void EnabelDebugLabel(bool enable);

        //! Default constructor
        DebugUtils() = default;
        //! Default destructor
        ~DebugUtils() = default;

        void SetObjectName(GLenum identifier, GLuint name, const std::string& label) const;

        struct ScopedLabel
        {
            //! Manually delete defualt constructor
            ScopedLabel() = delete;
            //! Constructor with pushing scoped label
            ScopedLabel(const std::string& message);
            //! Destructor with popping scoped label
            ~ScopedLabel();
        };

        [[nodiscard]] ScopedLabel ScopeLabel(const std::string& message) const
        {
            return ScopedLabel(message);
        }
    private:
        static GLuint _scopeID;
        static bool _labelEnabled;
    };

};

#endif //! end of DebugUtils.hpp