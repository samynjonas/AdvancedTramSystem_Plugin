// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TramCartController.generated.h"

class UATP_PythonTramControls;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVANCEDTRAMPLUGIN_API UATP_TramCartController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UATP_TramCartController();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void Control(float DeltaTime) {}


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	float ReadOutput() const
	{
		return m_Output;
	}

protected:
	float m_Output{}; // Output could be analog so float is used
		
};

UCLASS()
class UATP_TramCartControllerPython final : public UATP_TramCartController
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void Control(float DeltaTime) override;

protected:
	//Python Input
	const char m_AccelerationKey{ 'w' };
	const char m_BrakingKey{ 's' };
	UATP_PythonTramControls* m_pPythonTramControls{ nullptr };
};

UCLASS()
class ADVANCEDTRAMPLUGIN_API UATP_TramCartControllerAI final : public UATP_TramCartController
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void Control(float DeltaTime) override;

protected:

};