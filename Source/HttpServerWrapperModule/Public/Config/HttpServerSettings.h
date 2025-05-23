#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HttpServerSettings.generated.h"

UENUM(BlueprintType, Blueprintable)
enum class EHttpServerWrapperRequestVerbs : uint8
{
	VERB_NONE   = 0,
	VERB_GET    = 1 << 0,
	VERB_POST   = 1 << 1,
	VERB_PUT    = 1 << 2,
	VERB_PATCH  = 1 << 3,
	VERB_DELETE = 1 << 4,
	VERB_OPTIONS = 1 << 5
};

USTRUCT(BlueprintType)
struct HTTPSERVERWRAPPERMODULE_API FEndpointData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Endpoint {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHttpServerWrapperRequestVerbs Verbs {};
}; 

UCLASS(DisplayName="REST Api Server", Config = "HttpServer", DefaultConfig)
class HTTPSERVERWRAPPERMODULE_API UHttpServerSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	static UHttpServerSettings* Get()
	{
		return GetMutableDefault<UHttpServerSettings>();
	}
	
	const TArray<FEndpointData>& GetEndpoints() const
	{
		return Endpoints;
	}

	int32 GetPort() const
	{
		return Port;
	}

protected:
	virtual FName GetCategoryName() const override
	{
		return FName("Plugins");
	}
	
private:
	UPROPERTY(meta = (AllowPrivateAccess = "true"), Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	int32 Port { 8080 };
	
	UPROPERTY(meta = (AllowPrivateAccess = "true"), Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	TArray<FEndpointData> Endpoints {};
};
