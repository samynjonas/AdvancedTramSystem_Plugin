// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AdvancedSplineTracks.generated.h"

class USplineComponent;

UCLASS()
class ADVANCEDTRAMPLUGIN_API AATP_AdvancedSplineTracks : public AActor
{
	GENERATED_BODY()
	
public:	
	AATP_AdvancedSplineTracks();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	USplineComponent* GetSplineComponent() const { return m_pSplineComponent; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SplineTrack")
	USplineComponent* m_pSplineComponent{ nullptr };

};
