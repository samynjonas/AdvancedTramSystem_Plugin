// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/TramController.h"
#include "../Public/PythonTramControls.h"

// --------------------------------------------------------
// BASE CLASS TRAM CONTROLLER
// --------------------------------------------------------

UATP_TramController::UATP_TramController()
{
}

UATP_TramController::~UATP_TramController()
{
}

void UATP_TramController::BeginPlay()
{

}

void UATP_TramController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Control(DeltaTime);
}

// --------------------------------------------------------
// PYTHON TRAM CONTROLLER
// --------------------------------------------------------

UATP_TramControllerPython::UATP_TramControllerPython()
{

}

UATP_TramControllerPython::~UATP_TramControllerPython()
{

}

void UATP_TramControllerPython::BeginPlay()
{
    m_pPythonTramControls = GetOwner()->GetComponentByClass<UATP_PythonTramControls>();
    if (!m_pPythonTramControls)
    {
        m_pPythonTramControls = CreateDefaultSubobject<UATP_PythonTramControls>(TEXT("ATP_PythonTramControls"));
        GetOwner()->AddInstanceComponent(m_pPythonTramControls);
    }
}

void UATP_TramControllerPython::Control(float DeltaTime)
{
    m_Output = 0.f;
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
