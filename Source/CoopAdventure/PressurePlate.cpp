// Fill out your copyright notice in the Description page of Project Settings.


#include "PressurePlate.h"

// Sets default values
APressurePlate::APressurePlate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	SetRootComponent(rootComp);

	SetReplicates(true);
	SetReplicateMovement(true);

	triggerShape = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TriggerMesh"));
	triggerShape->SetupAttachment(rootComp);
	triggerShape->SetIsReplicated(true);

	auto triggerMeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Game/StarterContent/Shapes/Shape_Cylinder.Shape_Cylinder"));
	if (triggerMeshAsset.Succeeded()) {
		triggerShape->SetStaticMesh(triggerMeshAsset.Object);
		triggerShape->SetRelativeScale3D(FVector(3.3F, 3.3F, 0.2F));
		triggerShape->SetRelativeLocation(FVector(0.F, 0.F, 10.F));
	}

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	mesh->SetupAttachment(rootComp);
	mesh->SetIsReplicated(true);

	auto meshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("/Game/Stylized_Egypt/Meshes/building/SM_building_part_08.SM_building_part_08"));
	if (meshAsset.Succeeded()) {
		mesh->SetStaticMesh(meshAsset.Object);
		mesh->SetRelativeScale3D(FVector(4.F,4.F,0.5F));
		mesh->SetRelativeLocation(FVector(0.F, 0.F, 7.2F));
	}

	Transporter = CreateDefaultSubobject<UTransporter>(TEXT("Transporter"));
	Transporter->ownerIsTriggerActor = true;
}

// Called when the game starts or when spawned
void APressurePlate::BeginPlay()
{
	Super::BeginPlay();

	triggerShape->SetVisibility(false);
	triggerShape->SetCollisionProfileName(FName("OverlapAll"));

	Transporter->moveTime = 0.25F;
	FVector point1 = GetActorLocation();
	FVector point2 = point1 + FVector(0.F,0.F,-10.F);

	Transporter->SetPoints(point1, point2);
	
}

// Called every frame
void APressurePlate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority()) {
		TArray<AActor*> overlappedActors;
		AActor* triggerActor = 0;
		triggerShape->GetOverlappingActors(overlappedActors);

		for (auto* a : overlappedActors) {
			if (a->ActorHasTag("TriggerActor")) {
				triggerActor = a;
				break;
			}
		}

		if (triggerActor) {
			if (!activated) {
				activated = true;
				OnActivated.Broadcast();
				//GEngine->AddOnScreenDebugMessage(-1, 1.F, FColor::White, FString::Printf(TEXT("Activated")));
			}
		}
		else {
			if (activated) {
				activated = false;
				OnDeactivated.Broadcast();
				//GEngine->AddOnScreenDebugMessage(-1, 1.F, FColor::White, FString::Printf(TEXT("Deactivated")));
			}
		}
	}

}

