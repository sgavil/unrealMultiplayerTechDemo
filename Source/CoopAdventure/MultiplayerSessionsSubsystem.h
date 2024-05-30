// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "MultiplayerSessionsSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerCreateDel, bool, WasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerJoinDel, bool, WasSuccessful);
/**
 * 
 */
UCLASS()
class COOPADVENTURE_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMultiplayerSessionsSubsystem();

	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	void CreateServer(const FString& ServerName);

	UFUNCTION(BlueprintCallable)
	void FindServer(const FString& ServerName);

	UPROPERTY(BlueprintReadWrite)
	FString GameLevelPath;

private:
	IOnlineSessionPtr SessionInterface;
	IOnlineSubsystem* OnlineSubsystem;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	bool IsLan{ false };

	bool CreateServerOnDestroy{ false };
	FString DestroyServerName{};
	FString ServerNameToFind{};
	FName MySessionName{ "Co-op Adventure Session Name" };

	void OnCreateSessionDelegate(FName SessionName, bool WasSuccessful);
	void OnDestroySessionDelegate(FName SessionName, bool WasSuccessful);
	void OnFindSessionsCompleteDelegates(bool WasSuccessful);
	void OnJoinSessionCompleteDelegate(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	
	UPROPERTY(BlueprintAssignable)
	FServerCreateDel ServerCreateDel;

	UPROPERTY(BlueprintAssignable)
	FServerJoinDel ServerJoinDel;

	
};

void PrintString(const FString& message);
