#pragma once
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

using FileName = const ANSICHAR*;

struct DColor{
    FColor ScreenColor;
    const TCHAR* LogColor;
    
    static DColor Black;
    static DColor Red;
    static DColor Green;
    static DColor Blue;
    static DColor Yellow;
    static DColor Cyan;
    static DColor Purple;
    static DColor Gray; //Silver
    static DColor White;
};

class Debug
{
private:
    static DColor& getSeverityColor( ELogVerbosity::Type level )
    {
        switch( level )
        {
            case ELogVerbosity::Type::VeryVerbose:
            case ELogVerbosity::Type::Verbose:
            case ELogVerbosity::Type::Display:
                return DColor::Gray;
            case ELogVerbosity::Type::Log:
                return DColor::White;
            case ELogVerbosity::Type::Warning:
                return DColor::Yellow;
            case ELogVerbosity::Type::Error:
                return DColor::Red;
            case ELogVerbosity::Type::Fatal:
                return DColor::Purple;
        }
        return DColor::Gray;
    }
public:
    template<class T>
    static auto ToString(T &obj){
        return obj.ToString().GetCharArray().GetData();
    }
    template<typename... Args>
    static FString sprintf(const wchar_t* format, Args... args){
        return FString::Printf( format, args... );
    }
    static void printOnScreen(const FString &msg, const DColor &color = DColor::White, float duration = 5, uint64 key = INDEX_NONE);
    static void logLine(const FName &logName, const FString &msg, const DColor &color, ELogVerbosity::Type level, FileName file, int32 line);
    static void logLine(const FName &logName, const FString &msg, ELogVerbosity::Type level, FileName file, int32 line);
    static void log(const FName &logName, const FString &msg, ELogVerbosity::Type level, FileName file, int32 line, bool printToScreen = false, float duration = 5, uint64 key = INDEX_NONE);
    static void log(const FName &logName, const FString &msg, const DColor &color, ELogVerbosity::Type level, FileName file, int32 line, bool printToScreen = false, float duration = 5, uint64 key = INDEX_NONE);
};


/**
* Logging Macros
*****************
*
* These macros were specifically made to improve quality of life for anybody writing or viewing debug log lines for this project.
* Features:
*   -Colored console output
*   -Printing to Screen
*   -Logging class::function[line]
*   -Auto widening of literal strings
*   -Compile time disabling of logs
* 
* todo list:
*   1) Add all verbosity levels
*   2) Add feature for only printing logs to file
*/

#define INVOCATIONLINE ("[" + FString(__FUNCTION__) + ":" + FString::FromInt(__LINE__) + "] ")
#define LOGLINETEXT(Format,...) (INVOCATIONLINE + Debug::sprintf( L##Format, ##__VA_ARGS__ ))

/**
* BASE Logging Macro
*/
#define IN_LOG(CategoryName,Verbosity,PrintToScreen,Format,...) \
{ \
	static_assert((ELogVerbosity::Verbosity & ELogVerbosity::VerbosityMask) < ELogVerbosity::NumVerbosity && ELogVerbosity::Verbosity > 0, "Verbosity must be constant and in range."); \
    if(ELogVerbosity::Verbosity <= FLogCategory##CategoryName::CompileTimeVerbosity){ \
        Debug::log(CategoryName.GetCategoryName(),LOGLINETEXT( Format, ##__VA_ARGS__ ),ELogVerbosity::Type::Verbosity,__FILE__,__LINE__,PrintToScreen); \
	} \
}
/**
* BASE Colored Logging Macro
*/
#define IN_LOGC(CategoryName,Verbosity,Color,PrintToScreen,Format,...) \
{ \
	static_assert((ELogVerbosity::Verbosity & ELogVerbosity::VerbosityMask) < ELogVerbosity::NumVerbosity && ELogVerbosity::Verbosity > 0, "Verbosity must be constant and in range."); \
    if(ELogVerbosity::Verbosity <= FLogCategory##CategoryName::CompileTimeVerbosity){ \
        Debug::log(CategoryName.GetCategoryName(),LOGLINETEXT( Format, ##__VA_ARGS__ ),Color,ELogVerbosity::Type::Verbosity,__FILE__,__LINE__,PrintToScreen); \
	} \
}

#define logInfo(CategoryName,Format,...) IN_LOG(CategoryName,Log,false,Format,##__VA_ARGS__);
#define logError(CategoryName,Format,...) IN_LOG(CategoryName,Error,true,Format,##__VA_ARGS__);
#define logFatal(CategoryName,Format,...) IN_LOG(CategoryName,Fatal,true,Format,##__VA_ARGS__);
#define logWarning(CategoryName,Format,...) IN_LOG(CategoryName,Warning,true,Format,##__VA_ARGS__);
#define logVerbose(CategoryName,Format,...) IN_LOG(CategoryName,Verbose,false,Format,##__VA_ARGS__);
#define logVeryVerbose(CategoryName,Format,...) IN_LOG(CategoryName,VeryVerbose,false,Format,##__VA_ARGS__);

#define logInfoC(CategoryName,Color,PrintToScreen,Format,...) IN_LOGC(CategoryName,Log,Color,PrintToScreen,Format,##__VA_ARGS__);
#define logErrorC(CategoryName,Color,PrintToScreen,Format,...) IN_LOGC(CategoryName,Error,Color,PrintToScreen,Format,##__VA_ARGS__);
#define logFatalC(CategoryName,Color,PrintToScreen,Format,...) IN_LOGC(CategoryName,Fatal,Color,PrintToScreen,Format,##__VA_ARGS__);
#define logWarningC(CategoryName,Color,PrintToScreen,Format,...) IN_LOGC(CategoryName,Warning,Color,PrintToScreen,Format,##__VA_ARGS__);
#define logVerboseC(CategoryName,Color,PrintToScreen,Format,...) IN_LOGC(CategoryName,Verbose,Color,PrintToScreen,Format,##__VA_ARGS__);
#define logVeryVerboseC(CategoryName,Color,PrintToScreen,Format,...) IN_LOGC(CategoryName,VeryVerbose,Color,PrintToScreen,Format,##__VA_ARGS__);

#define LOGINIT(color) logInfoC(Init,color,true," ");
#define LOGCALL(log,color) logInfoC(log,color,false," ");

/** logLevelMsg
* Sometimes you really just need to log a ToString(), and when you do you're gonna need these.
* Unless of course I can figure out how to get the above macros to accept FStrings as part of the sprintf
* Which I did.. gonna keep this for posterity's sake. Despite having deleted the definitions themselves
*/
/*
#define logInfoMsg(CategoryName,Msg) 
#define logErrorMsg(CategoryName,Msg) 
#define logFatalMsg(CategoryName,Msg) 
#define logWarningMsg(CategoryName,Msg) 
#define logVerboseMsg(CategoryName,Msg) 

#define logInfoMsgC(CategoryName,Color,PrintToScreen,Msg) 
#define logErrorMsgC(CategoryName,Color,PrintToScreen,Msg) 
#define logFatalMsgC(CategoryName,Color,PrintToScreen,Msg) 
#define logWarningMsgC(CategoryName,Color,PrintToScreen,Msg) 
*/