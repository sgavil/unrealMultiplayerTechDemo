#include "Transporter.h"
#include "CollectableKey.h"
#include "PressurePlate.h"

UTransporter::UTransporter() : moveTime(3.F), activatedTriggerCount(0), arePointsSet(false), 
	startPoint(FVector::Zero()), endPoint(FVector::Zero())
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	

}


void UTransporter::SetPoints(const FVector& point1, const FVector& point2)
{
	if (point1.Equals(point2)) return;

	startPoint = point1;
	endPoint = point2;
	arePointsSet = true;
}

void UTransporter::OnTriggerActorActivated()
{
	activatedTriggerCount++;
	GEngine->AddOnScreenDebugMessage(-1, 3.F, FColor::Cyan, TEXT("Activated"));
}

void UTransporter::OnTriggerActorDeactivated()
{
	activatedTriggerCount--;
}

// Called when the game starts
void UTransporter::BeginPlay()
{
	Super::BeginPlay();

	if (ownerIsTriggerActor) {
		triggerActors.Add(GetOwner());
	}
	for (AActor* TA : triggerActors) {
		APressurePlate* pressurePlateActor = Cast<APressurePlate>(TA);
		if (pressurePlateActor) {
			pressurePlateActor->OnActivated.AddDynamic(this, &UTransporter::OnTriggerActorActivated);
			pressurePlateActor->OnDeactivated.AddDynamic(this, &UTransporter::OnTriggerActorDeactivated);
			continue;
		}

		ACollectableKey* collectableKeyActor = Cast<ACollectableKey>(TA);
		if (collectableKeyActor) {
			collectableKeyActor->OnKeyCollected.AddDynamic(this, &UTransporter::OnTriggerActorActivated);
		}
	}
	
}


void UTransporter::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (triggerActors.Num() > 0) {
		allTriggerActorsTriggered = activatedTriggerCount >= triggerActors.Num();
		if (allTriggerActorsTriggered) {
			//GEngine->AddOnScreenDebugMessage(-1, 3.F, FColor::Cyan, FString("All trigger activated"));
		}
	}
	
	AActor* transporterOwner = GetOwner();
	if (transporterOwner && transporterOwner->HasAuthority() && arePointsSet) {
		FVector currentLocation = transporterOwner->GetActorLocation();
		float speed = FVector::Distance(startPoint, endPoint) / moveTime;

		FVector targetLocation = allTriggerActorsTriggered ? endPoint : startPoint;

		if (!currentLocation.Equals(targetLocation)) {
			FVector location = FMath::VInterpConstantTo(currentLocation, targetLocation, DeltaTime, speed);
			transporterOwner->SetActorLocation(location);
		}
	}
}

