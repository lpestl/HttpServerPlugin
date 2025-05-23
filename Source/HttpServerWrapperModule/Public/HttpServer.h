#pragma once

#include "CoreMinimal.h"
#include "HttpRouteHandle.h"
#include "Config/HttpServerSettings.h"
#include "UObject/Object.h"
#include "HttpServer.generated.h"

class IHttpRouter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnReceivedRequest, const FString&, RequestBody, FEndpointData, EndpointData);
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class HTTPSERVERWRAPPERMODULE_API UHttpServer : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "HttpServer")
	void StartApiServer();
	
	UFUNCTION(BlueprintCallable, Category = "HttpServer")
	void StopApiServer();

	virtual bool HandleRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete, FEndpointData EndpointData);

	FOnReceivedRequest& OnReceivedRequest()
	{
		return Delegate_OnReceivedRequest;
	}
protected:	
	bool ProcessResponse(const FHttpResultCallback& OnComplete, const FString& Key, const FString& MessageText, EHttpServerResponseCodes ResponseCode);
	
private:	
	/** Http router handle */
	TSharedPtr<IHttpRouter> HttpRouter;
	
	/** Mapping of routes to delegate handles */
	TMap<FString, FHttpRouteHandle> ActiveRouteHandles;

	UPROPERTY(BlueprintAssignable)
	FOnReceivedRequest Delegate_OnReceivedRequest;	
};
