#include "HttpServer.h"

#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "HttpServerModule.h"
#include "Config/HttpServerSettings.h"
#include "Runtime/Online/HTTPServer/Public/HttpServerModule.h"

void UHttpServer::StartApiServer()
{
	FHttpServerModule& HttpServerModule = FHttpServerModule::Get();

	UHttpServerSettings* Settings = UHttpServerSettings::Get();
	if (IsValid(Settings))
	{
		if (HttpRouter)
		{
			HttpRouter.Reset();
		}
		HttpRouter = FHttpServerModule::Get().GetHttpRouter(Settings->GetPort(), /* bFailOnBindFailure = */ true);
		UE_LOG(LogHttpServerPlugin, Log, TEXT("Starting HttpServer on %d port:"), Settings->GetPort());
		
		TArray<FEndpointData> Endpoints = Settings->GetEndpoints();
		for (FEndpointData& EndpointData : Endpoints)
		{
			FHttpPath EndpointPath = EndpointData.Endpoint;
			EHttpServerRequestVerbs Verbs = static_cast<EHttpServerRequestVerbs>(EndpointData.Verbs);
			ActiveRouteHandles.Add(
				EndpointData.Endpoint,
				HttpRouter->BindRoute(
					EndpointPath,
					Verbs,
					FHttpRequestHandler::CreateUObject(this, &UHttpServer::HandleRequest, EndpointData)
				)
			);
			UE_LOG(LogHttpServerPlugin, Log, TEXT("\t%s: %s "), *StaticEnum<EHttpServerWrapperRequestVerbs>()->GetNameStringByValue(static_cast<int64>(EndpointData.Verbs)), *EndpointData.Endpoint);
		}

		HttpServerModule.StartAllListeners();
	}
}

void UHttpServer::StopApiServer()
{
	if (FHttpServerModule::IsAvailable())
	{
		FHttpServerModule& HttpServerModule = FHttpServerModule::Get();
		HttpServerModule.StopAllListeners();
		
		UHttpServerSettings* Settings = UHttpServerSettings::Get();
		if (IsValid(Settings))
		{
			TArray<FEndpointData> Endpoints = Settings->GetEndpoints();
			UE_LOG(LogHttpServerPlugin, Log, TEXT("Unbound endpoints:"));
			
			for (FEndpointData& EndpointData : Endpoints)
			{
				if (ActiveRouteHandles.Find(EndpointData.Endpoint) != nullptr)
				{
					if (ActiveRouteHandles[EndpointData.Endpoint].IsValid())
					{
						UE_LOG(LogHttpServerPlugin, Log, TEXT("\t* %s"), *EndpointData.Endpoint);
						HttpServerModule.GetHttpRouter(Settings->GetPort())->UnbindRoute(ActiveRouteHandles[EndpointData.Endpoint]);
					}
				}
			}
		}
		ActiveRouteHandles.Reset();
	}

	if (HttpRouter)
	{
		HttpRouter.Reset();
	}
}

bool UHttpServer::HandleRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete,
	FEndpointData EndpointData)
{
	FString RequestBody = FString(Request.Body.Num(), UTF8_TO_TCHAR(Request.Body.GetData()));
	UE_LOG(LogHttpServerPlugin, Log, TEXT("Received request: \n%s"), *RequestBody);

	// Parse JSON from body request
	TSharedPtr<FJsonObject> JsonRequest;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(RequestBody);

	if (!FJsonSerializer::Deserialize(Reader, JsonRequest) || !JsonRequest.IsValid())
	{
		return ProcessResponse(OnComplete, TEXT("Error"), TEXT("Invalid JSON format"), EHttpServerResponseCodes::BadRequest);
	}

	if (Delegate_OnReceivedRequest.IsBound())
	{
		Delegate_OnReceivedRequest.Broadcast(RequestBody, EndpointData);
	}
	
	return ProcessResponse(OnComplete, TEXT("Status"), TEXT("Request Handled. Everything is OK"), EHttpServerResponseCodes::Accepted);
}

bool UHttpServer::ProcessResponse(const FHttpResultCallback& OnComplete, const FString& Key, const FString& MessageText, EHttpServerResponseCodes ResponseCode)
{
	const bool bIsError = static_cast<uint16>(ResponseCode) >= 400;
	if (bIsError)
	{
		UE_LOG(LogHttpServerPlugin, Error, TEXT("%s: %s"), *Key, *MessageText);
	}
	else
	{
		UE_LOG(LogHttpServerPlugin, Log, TEXT("%s: %s"), *Key, *MessageText);
	}
	
	TSharedPtr<FJsonObject> MessageResponse = MakeShareable(new FJsonObject());
	MessageResponse->SetStringField(Key, MessageText);

	FString MessageResponseString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&MessageResponseString);
	FJsonSerializer::Serialize(MessageResponse.ToSharedRef(), Writer);

	TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(MessageResponseString, TEXT("application/json"));
	Response->Code = ResponseCode;
	OnComplete(MoveTemp(Response));

	return !bIsError;
}