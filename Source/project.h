// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "core.h"

/**
* Here we create our logs
* Each log comes with:
* Name                       - Identifier, creates a class by the same name
* Default Verbosity          - Not sure to be honest, the debug.h macros certainly don't allow for defaults
* Max Compile Time Verbosity - Just like it sounds. Anything with a higher value than the one provided here will be discarded
* 
* todo: ensure all log messages are put into the log, hide only from console -> then test speed of execution
*/

//Logging during game startup
DECLARE_LOG_CATEGORY_EXTERN(Init, Display, All);
 
//Logging for a that troublesome system
DECLARE_LOG_CATEGORY_EXTERN(Geometry, Display, Log);
DECLARE_LOG_CATEGORY_EXTERN(Materials, Display, All);
 
//Logging for Critical Errors that must always be addressed
DECLARE_LOG_CATEGORY_EXTERN(CriticalErrors, Error, All);

