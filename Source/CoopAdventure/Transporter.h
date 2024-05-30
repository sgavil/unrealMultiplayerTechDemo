// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Transporter.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COOPADVENTURE_API UTransporter : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTransporter();

	FVector startPoint;
	FVector endPoint;
	bool arePointsSet{ false };

	UPROPERTY(EditAnywhere)
	float moveTime;

	UPROPERTY(EditAnywhere)
	bool ownerIsTriggerActor{false};

	UPROPERTY(EditAnywhere)
	TArray<AActor*> triggerActors;

	UPROPERTY(VisibleAnywhere)
	int activatedTriggerCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool allTriggerActorsTriggered;

	UFUNCTION(BlueprintCallable)
	void SetPoints(const FVector& point1, const FVector& point2);

	UFUNCTION()
	void OnTriggerActorActivated();

	UFUNCTION()
	void OnTriggerActorDeactivated();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
