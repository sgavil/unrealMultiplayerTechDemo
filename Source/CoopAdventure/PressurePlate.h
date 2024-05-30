// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Transporter.h"
#include "PressurePlate.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPressurePlateOnActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPressurePlateOnDeactivated);

UCLASS()
class COOPADVENTURE_API APressurePlate : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APressurePlate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	USceneComponent* rootComp;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UStaticMeshComponent* triggerShape;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UStaticMeshComponent* mesh;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool activated{ false };

	UPROPERTY(BlueprintAssignable)
	FPressurePlateOnActivated OnActivated;

	UPROPERTY(BlueprintAssignable)
	FPressurePlateOnDeactivated OnDeactivated;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UTransporter* Transporter;

};
