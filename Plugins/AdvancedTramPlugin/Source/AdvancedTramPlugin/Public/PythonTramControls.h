// OTIV & Howest DAE © 2023

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Sockets.h" //#include "Networking/Public/Sockets.h"
#include "SocketSubsystem.h" //#include "Networking/Public/SocketSubsystem.h"
#include "PythonTramControls.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVANCEDTRAMPLUGIN_API UATP_PythonTramControls : public UActorComponent
{
	GENERATED_BODY()

public:	
	UATP_PythonTramControls();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Sends a message via UDP
	void SendUDPMessage(const FString& Message, const FString& IP);

	// Checks for input from Python and returns a char representing the pressed key
	char GetInputOverUDP();

protected:
	virtual void BeginPlay() override;

private:
	const uint16 m_UDPListenPort{ 9999 };
	const int32 m_UDPSendPort{ 9988 };

	// The listening socket
	FSocket* m_pListenSocket{ nullptr };
};