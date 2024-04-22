// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class UATP_PythonTramControls;

/**
	TRAM CONTROLLER WILL BE USED TO DETERMINE HOW THE TRAM WILL BE CONTROLLED - YOU WILL BE ABLE TO READ OUT THE INPUT AS AN OUTPUT
 
 **/
class ADVANCEDTRAMPLUGIN_API UATP_TramController : public UActorComponent
{
public:
	UATP_TramController();
	virtual ~UATP_TramController();

	float ReadOutput() const
	{ 
		return m_Output; 
	}

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void Control(float DeltaTime) = 0;


protected:
	float m_Output{}; // Output could be analog so float is used

};


// Python Tram Controller -- read out inputs through python
class ADVANCEDTRAMPLUGIN_API UATP_TramControllerPython final : public UATP_TramController
{
public:
	UATP_TramControllerPython();
	~UATP_TramControllerPython();


	virtual void BeginPlay() override;

protected:
	void Control(float DeltaTime) override;


protected:
	//Python Input
	const char m_AccelerationKey{ 'w' };
	const char m_BrakingKey{ 's' };
	UATP_PythonTramControls* m_pPythonTramControls{ nullptr };

};