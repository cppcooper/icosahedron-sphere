#include "debug.h"
#include <locale>
#include <codecvt>
#include <string>

DColor DColor::Black    {FColor::Black,  COLOR_BLACK};
DColor DColor::Red      {FColor::Red,    COLOR_RED};
DColor DColor::Green    {FColor::Green,  COLOR_GREEN};
DColor DColor::Blue     {FColor::Blue,   COLOR_BLUE};
DColor DColor::Yellow   {FColor::Yellow, COLOR_YELLOW};
DColor DColor::Cyan     {FColor::Cyan,   COLOR_CYAN};
DColor DColor::Purple   {FColor::Purple, COLOR_PURPLE};
DColor DColor::Gray     {FColor::Silver, COLOR_GRAY};
DColor DColor::White    {FColor::White,  COLOR_WHITE};

//todo: try to optimize this function. manually convert characters perhaps.
/*std::wstring Debug::text(const char* msg){
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wide = converter.from_bytes(msg);
    return wide;
}*/

void Debug::printOnScreen(const FString &msg, const DColor &color, float duration, uint64 key){
    if (GEngine){
        GEngine->AddOnScreenDebugMessage(key,duration,color.ScreenColor,msg );
    }
}
void Debug::logLine(const FName &logName, const FString &msg, const DColor &color, ELogVerbosity::Type level, FileName file, int32 line){
    SET_WARN_COLOR(color.LogColor);
    logLine(logName,msg,level,file,line);
    CLEAR_WARN_COLOR();
}
void Debug::logLine(const FName &logName, const FString &msg, ELogVerbosity::Type level, FileName file, int32 line){
    FMsg::Logf_Internal(file,line,logName,level,msg.GetCharArray().GetData());
}

void Debug::log(const FName &logName, const FString &msg, ELogVerbosity::Type level, FileName file, int32 line, bool printToScreen, float duration, uint64 key){
    DColor& color = getSeverityColor(level);
    if(printToScreen){
        printOnScreen(msg,color,duration,key);
    }
    logLine(logName,msg,color,level,file,line);
}
void Debug::log(const FName &logName, const FString &msg, const DColor &color, ELogVerbosity::Type level, FileName file, int32 line, bool printToScreen, float duration, uint64 key){
    if(printToScreen){
        printOnScreen(msg,color,duration,key);
    }
    logLine(logName,msg,color,level,file,line);
}
