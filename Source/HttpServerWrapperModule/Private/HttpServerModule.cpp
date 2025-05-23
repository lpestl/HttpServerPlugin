#include "HttpServerModule.h"

DEFINE_LOG_CATEGORY(LogHttpServerPlugin);

#define LOCTEXT_NAMESPACE "FHttpServerWrapperModule"

void FHttpServerWrapperModule::StartupModule()
{
	UE_LOG(LogHttpServerPlugin, Log, TEXT("~~~ FHttpServerWrapperModule::StartupModule"));
}

void FHttpServerWrapperModule::ShutdownModule()
{
	UE_LOG(LogHttpServerPlugin, Log, TEXT("~~~ FHttpServerWrapperModule::ShutdownModule"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FHttpServerWrapperModule, HttpServerWrapperModule)