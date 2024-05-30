#include "WinArea.h"
#include "CoopAdventureCharacter.h"

AWinArea::AWinArea()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	WinCondition = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	SetRootComponent(BoxComponent);
}

void AWinArea::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void AWinArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() && !WinCondition) {
		TArray<AActor*> overlapActors;
		BoxComponent->GetOverlappingActors(overlapActors, ACoopAdventureCharacter::StaticClass());
		WinCondition = overlapActors.Num() == 2;
		if (WinCondition) {
			UE_LOG(LogTemp, Display, TEXT("Winnnnn"));
			MulticastRPCWin();
		}
	}

}

void AWinArea::MulticastRPCWin_Implementation()
{
	OnWinCondition.Broadcast();
}

