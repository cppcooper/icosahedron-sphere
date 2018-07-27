// Fill out your copyright notice in the Description page of Project Settings.

#include "project.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, project, "project" );

//Logging during game startup
DEFINE_LOG_CATEGORY(Init);
 
//Logging for some system
DEFINE_LOG_CATEGORY(Geometry);
DEFINE_LOG_CATEGORY(Materials);
 
//Logging for Critical Errors that must always be addressed
DEFINE_LOG_CATEGORY(CriticalErrors);