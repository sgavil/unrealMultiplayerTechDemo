#include "CollectableKey.h"

#include "Net/UnrealNetwork.h"
#include "CoopAdventureCharacter.h"


// Sets default values
ACollectableKey::ACollectableKey()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(RootComp);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(RootComp);
	Capsule->SetIsReplicated(true);
	Capsule->SetCollisionProfileName(FName("OverlapAllDynamic"));
	Capsule->SetCapsuleHalfHeight(150.0f);
	Capsule->SetCapsuleRadius(100.0f);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComp);
	Mesh->SetIsReplicated(true);
	Mesh->SetCollisionProfileName(FName("OverlapAllDynamic"));

	RotationSpeed = 100.F;

	CollectAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("CollectAudio"));
	CollectAudio->SetupAttachment(RootComp);
	CollectAudio->SetAutoActivate(false);

}


void ACollectableKey::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACollectableKey, IsCollected);
}

// Called when the game starts or when spawned
void ACollectableKey::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACollectableKey::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority()) {

		Mesh->AddRelativeRotation(FRotator(0.F, RotationSpeed * DeltaTime, 0.F));

		TArray<AActor*> triggerActors;
		Capsule->GetOverlappingActors(triggerActors, ACoopAdventureCharacter::StaticClass());
		if (!triggerActors.IsEmpty() && !IsCollected) {
			IsCollected = true;
			OnRep_IsCollected();
		}
	}


}

void ACollectableKey::OnRep_IsCollected()
{
	if (HasAuthority()) {
		UE_LOG(LogTemp, Display, TEXT("OnRep_IsCollected called from the server!"));
		
		if (IsCollected) {
			OnKeyCollected.Broadcast();
		}
	}
	else {
		UE_LOG(LogTemp, Display, TEXT("OnRep_IsCollected called from the client!"));
	}

	Mesh->SetVisibility(!IsCollected);
	CollectAudio->Play();
	if (IsCollected && KeyHolder) {
		KeyHolder->ActivateKeyMesh();
	}
}
