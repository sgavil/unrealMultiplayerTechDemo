// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "Engine/Engine.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionDelegates.h"

void PrintString(const FString& message)
{
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 15.F, FColor::Cyan, message);
	}
}


UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
	//PrintString("MSS Constructor");
}

void UMultiplayerSessionsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem) {
		IsLan = OnlineSubsystem->GetSubsystemName() == "NULL";
		FString SubsystemName = OnlineSubsystem->GetSubsystemName().ToString();
		PrintString(SubsystemName);
		SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid()) {
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, 
				&UMultiplayerSessionsSubsystem::OnCreateSessionDelegate);

			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, 
				&UMultiplayerSessionsSubsystem::OnDestroySessionDelegate);

			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this,
				&UMultiplayerSessionsSubsystem::OnFindSessionsCompleteDelegates);
			
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this,
				&UMultiplayerSessionsSubsystem::OnJoinSessionCompleteDelegate);
		}
	}
}

void UMultiplayerSessionsSubsystem::Deinitialize()
{
	//UE_LOG(LogTemp, Warning, TEXT("MSS Deinitialize"));
}

void UMultiplayerSessionsSubsystem::CreateServer(const FString& ServerName)
{
	PrintString("Create Server");

	if (ServerName.IsEmpty()) {
		PrintString("Server name cannot be empty");
		ServerCreateDel.Broadcast(false);
		return;
	}


	FNamedOnlineSession * ExistingSession = SessionInterface->GetNamedSession(MySessionName);
	if (ExistingSession) {
		PrintString(FString::Printf(TEXT("Session %s already exists, destroying it"), *MySessionName.ToString()));
		CreateServerOnDestroy = true;
		DestroyServerName = ServerName;
		SessionInterface->DestroySession(MySessionName);
		return;
	}

	FOnlineSessionSettings SessionSettings;

	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.NumPublicConnections = 2;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bAllowJoinViaPresence = true;
	SessionSettings.bIsLANMatch = IsLan;

	SessionSettings.Set(FName("SERVER_NAME"), ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	SessionInterface->CreateSession(0, MySessionName, SessionSettings);
}

void UMultiplayerSessionsSubsystem::FindServer(const FString& ServerName)
{
	PrintString("Finding Server");
	if (ServerName.IsEmpty()) {
		PrintString("Server name cannot be empty");
		ServerJoinDel.Broadcast(false);
		return;
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = IsLan;
	SessionSearch->MaxSearchResults = 999;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	ServerNameToFind = ServerName;

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	
}

void UMultiplayerSessionsSubsystem::OnCreateSessionDelegate(FName SessionName, bool WasSuccessful)
{
	PrintString(FString::Printf(TEXT("On Create Session Delegate %d"), WasSuccessful));

	ServerCreateDel.Broadcast(WasSuccessful);
	
	FString MapPath = GameLevelPath.IsEmpty() ? "/Game/ThirdPerson/Maps/ThirdPersonMap?listen" : FString::Printf(TEXT("%s?listen"), *GameLevelPath);
	GetWorld()->ServerTravel(MapPath);
}

void UMultiplayerSessionsSubsystem::OnDestroySessionDelegate(FName SessionName, bool WasSuccessful)
{
	PrintString(FString::Printf(TEXT("On Destroy Session Delegate %d"), WasSuccessful));
	if (CreateServerOnDestroy) {
		CreateServerOnDestroy = false;
		CreateServer(DestroyServerName);
	}
}

void UMultiplayerSessionsSubsystem::OnFindSessionsCompleteDelegates(bool WasSuccessful)
{
	if (!WasSuccessful) return;
	if (ServerNameToFind.IsEmpty()) return;

	TArray<FOnlineSessionSearchResult> Results = SessionSearch->SearchResults;
	if (Results.Num() > 0) {
		FString Msg = FString::Printf(TEXT("%d sessions found."), Results.Num());
		PrintString(Msg);
		FOnlineSessionSearchResult *FoundedSessionResult = 0;

		for (auto& resultSession : Results) {
			if (resultSession.IsValid()) {
				FString ServerName{ "No-name" };
				resultSession.Session.SessionSettings.Get(FName("SERVER_NAME"), ServerName);

				if (ServerName.Equals(ServerNameToFind)) {
					FoundedSessionResult = &resultSession;
					FString Msg2 = FString::Printf(TEXT("Founded server: %s "), *ServerName);
					PrintString(Msg2);
					break;
				}

			}
		}

		if (FoundedSessionResult) {
			PrintString(FString::Printf(TEXT("Joining to Session: %s "), *MySessionName.ToString()));
			SessionInterface->JoinSession(0, MySessionName, *FoundedSessionResult);
		}
		else {
			ServerJoinDel.Broadcast(false);
		}
	}
	else {
		PrintString("Zero sessions found");
		ServerJoinDel.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::OnJoinSessionCompleteDelegate(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result != EOnJoinSessionCompleteResult::Success) {
		PrintString("OnJoinSessionComplete Failed");
		ServerJoinDel.Broadcast(false);
		return;
	}

	FString Address{};
	if (SessionInterface->GetResolvedConnectString(MySessionName, Address)) {
		PrintString(FString::Printf(TEXT("Session Address: %s "), *Address));
		if (auto* PlayerController = GetGameInstance()->GetFirstLocalPlayerController(); PlayerController) {
			PrintString("Travelling");
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
		else {
			PrintString("Player controller none");
		}

	}
	else {
		PrintString("Cant resolve adress");
	}


}

