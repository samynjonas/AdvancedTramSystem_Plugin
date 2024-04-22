// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/TramCartController.h"
#include "../Public/PythonTramControls.h"


// Sets default values for this component's properties
UATP_TramCartController::UATP_TramCartController()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// --------------------------------------------------------
// BASE CLASS TRAM CONTROLLER
// --------------------------------------------------------

void UATP_TramCartController::BeginPlay()
{
	Super::BeginPlay();
}

void UATP_TramCartController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	Control(DeltaTime);
}

// --------------------------------------------------------
// PYTHON TRAM CONTROLLER
// --------------------------------------------------------

void UATP_TramCartControllerPython::BeginPlay()
{
    UE_LOG(LogTemp, Warning, TEXT("Python Tram Controller Initialized"));

    m_pPythonTramControls = GetOwner()->GetComponentByClass<UATP_PythonTramControls>();
    if (!m_pPythonTramControls)
    {
        m_pPythonTramControls = Cast<UATP_PythonTramControls>(GetOwner()->AddComponentByClass(UATP_PythonTramControls::StaticClass(), false, GetOwner()->GetActorTransform(), false));
    }

    if (!m_pPythonTramControls)
    {
        UE_LOG(LogTemp, Error, TEXT("Python Tram Controller could not be initialized"));
    }
}

void UATP_TramCartControllerPython::Control(float DeltaTime)
{
    m_Output = 0.f;
    if (m_pPythonTramControls == nullptr)
    {
        return;
    }

    char input = m_pPythonTramControls->GetInputOverUDP();
    if (input == '0')
        return;
    
    if (input == m_AccelerationKey)
    {
        m_Output = 1.f;
    }
    else if (input == m_BrakingKey)
    {
        m_Output = -1.f;
    }
}


// --------------------------------------------------------
// AI TRAM CONTROLLER
// --------------------------------------------------------

void UATP_TramCartControllerAI::BeginPlay()
{
    UE_LOG(LogTemp, Warning, TEXT("AI Tram Controller Initialized"));
}

void UATP_TramCartControllerAI::Control(float DeltaTime)
{
    m_Output = 1.f;
}