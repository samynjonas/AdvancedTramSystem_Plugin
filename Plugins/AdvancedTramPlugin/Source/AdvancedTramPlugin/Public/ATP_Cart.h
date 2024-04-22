// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ATP_Cart.generated.h"

class UATP_TramCartController;

UCLASS()
class ADVANCEDTRAMPLUGIN_API AATP_Cart : public AActor
{
	GENERATED_BODY()
	
public:
	AATP_Cart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cart")
	UStaticMeshComponent* m_pBody{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cart")
	USceneComponent* m_pFrontWheels{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cart")
	USceneComponent* m_pBackWheels{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cart")
	UATP_TramCartController* m_pTramCarBehavior{ nullptr };

protected:


};
