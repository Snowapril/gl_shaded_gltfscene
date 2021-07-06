#include <GL3/DebugUtils.hpp>
#include <Core/Macros.hpp>
#include <iostream>
#include <cstdio>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define NOMINMAX
	#include <windows.h>
	#include <DbgHelp.h>
	#pragma comment(lib,"Dbghelp")
	#define STDCALL __stdcall
#else
	#define STDCALL
#endif

#if defined(__linux__)
	#include <unistd.h>
	#include <execinfo.h>
#endif

#include <glad/glad.h>

namespace GL3
{
    GLuint DebugUtils::_scopeID = 0;
    bool DebugUtils::_labelEnabled = true;

    void DebugUtils::EnabelDebugLabel(bool enable)
    {
        DebugUtils::_labelEnabled = enable;
    }

    void DebugUtils::SetObjectName(GLenum identifier, GLuint name, const std::string& label) const
    {
        if (DebugUtils::_labelEnabled)
            glObjectLabel(identifier, name, -1, label.c_str());
    }

    DebugUtils::ScopedLabel::ScopedLabel(const std::string& message)
    {
        if (DebugUtils::_labelEnabled)
            glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, DebugUtils::_scopeID++, -1, message.c_str());
    }

    DebugUtils::ScopedLabel::~ScopedLabel()
    {
        if (DebugUtils::_labelEnabled)
        {
            glPopDebugGroup();
            --DebugUtils::_scopeID;
        }
    }


	// output the call stack
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    void DebugUtils::PrintStack() 
	{
        unsigned int   i;
        void         * stack[ 100 ];
        unsigned short frames;
        SYMBOL_INFO  * symbol;
        HANDLE         process;

        process = GetCurrentProcess();

        SymSetOptions(SYMOPT_LOAD_LINES);

        SymInitialize( process, NULL, TRUE );

        frames               = CaptureStackBackTrace( 0, 200, stack, NULL );
        symbol               = ( SYMBOL_INFO * )calloc( sizeof( SYMBOL_INFO ) + 256 * sizeof( char ), 1 );
        symbol->MaxNameLen   = 255;
        symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

		printf("---------------------Stack Trace---------------------\n");
        for( i = 0; i < frames; i++ )
        {
        	SymFromAddr( process, ( DWORD64 )( stack[ i ] ), 0, symbol );
        	DWORD  dwDisplacement;
        	IMAGEHLP_LINE64 line;

        	line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        	if (!strstr(symbol->Name,"VSDebugLib::") && SymGetLineFromAddr64(process, ( DWORD64 )( stack[ i ] ), &dwDisplacement, &line)) 
			{
				printf("Function : %s - line : %lu\n", symbol->Name, line.LineNumber);
        	}

        	if (0 == strcmp(symbol->Name,"main"))
        		break;
        }
		printf("-----------------------------------------------------\n");
        free( symbol );
    }
#elif __linux__
    void DebugUtils::PrintStack() 
	{
        constexpr size_t kTraceDepth = 10;
        void* arr[kTraceDepth];
        size_t size;

        size = backtrace(arr, kTraceDepth);

		printf("---------------------Stack Trace---------------------\n");
        backtrace_symbols_fd(arr, size, STDERR_FILENO);
		printf("-----------------------------------------------------\n");
    }
#endif

	namespace Detail
    {
        // aux function to translate source to string
        std::string GetStringForSource(GLenum source) {
        
        	switch(source) {
        		case GL_DEBUG_SOURCE_API: 
        			return("API");
        		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        			return("Window System");
        		case GL_DEBUG_SOURCE_SHADER_COMPILER:
        			return("Shader Compiler");
        		case GL_DEBUG_SOURCE_THIRD_PARTY:
        			return("Third Party");
        		case GL_DEBUG_SOURCE_APPLICATION:
        			return("Application");
        		case GL_DEBUG_SOURCE_OTHER:
        			return("Other");
        		default:
        			return("");
        	}
        }

        // aux function to translate severity to string
        std::string GetStringForSeverity(GLenum severity) {
        
        	switch(severity) {
        		case GL_DEBUG_SEVERITY_HIGH: 
        			return("High");
        		case GL_DEBUG_SEVERITY_MEDIUM:
        			return("Medium");
        		case GL_DEBUG_SEVERITY_LOW:
        			return("Low");
        		default:
        			return("");
        	}
        }

        // aux function to translate type to string
        std::string GetStringForType(GLenum type) {
        
        	switch(type) {
        		case GL_DEBUG_TYPE_ERROR: 
        			return("Error");
        		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        			return("Deprecated Behaviour");
        		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        			return("Undefined Behaviour");
        		case GL_DEBUG_TYPE_PORTABILITY:
        			return("Portability Issue");
        		case GL_DEBUG_TYPE_PERFORMANCE:
        			return("Performance Issue");
        		case GL_DEBUG_TYPE_OTHER:
        			return("Other");
        		default:
        			return("");
        	}
        }
    }

    void DebugUtils::DebugLog(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const GLvoid* userParam)
    {
        UNUSED_VARIABLE(userParam);
        UNUSED_VARIABLE(length);

        std::cerr << "[Type] : "       << Detail::GetStringForType(type)           << 
                	 "[Source] : "     << Detail::GetStringForSource(source)       <<
                	 "[ID] : "         <<              id                          <<
                	 "[Serverity] : "  << Detail::GetStringForSeverity(severity)   << std::endl;
                          
        std::cerr << "[Message] : "    << message << std::endl;
        
        DebugUtils::PrintStack();
    }
};