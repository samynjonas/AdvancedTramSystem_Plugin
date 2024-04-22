// OTIV & Howest DAE © 2023

#include "PythonTramControls.h"
#include "Networking/Public/Networking.h"
#include "Engine/World.h"
#include "Engine.h"

//	*******************************************************
//  Helper function to convert FString to an array of bytes
void StringToBytes(const FString& InString, TArray<uint8>& OutBytes, int32 Count)
{
	OutBytes.Empty(Count);
	const ANSICHAR* AnsiData = StringCast<ANSICHAR>(*InString).Get();

	// Append the ANSICHAR data to the output array
	for (int32 i = 0; i < Count; ++i)
	{
		OutBytes.Add(static_cast<uint8>(AnsiData[i]));
	}
}
//	*******************************************************

UATP_PythonTramControls::UATP_PythonTramControls()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UATP_PythonTramControls::BeginPlay()
{
	Super::BeginPlay();

	FIPv4Endpoint Endpoint(FIPv4Address(127, 0, 0, 1), m_UDPListenPort);

	// Create a UDP Socket
	m_pListenSocket = FUdpSocketBuilder(TEXT("PyListenSocket"))
		.AsReusable()
		.AsNonBlocking()
		.BoundToEndpoint(Endpoint);

	m_pListenSocket->SetRecvErr();
}

void UATP_PythonTramControls::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (m_pListenSocket)
	{
		m_pListenSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(m_pListenSocket);
	}
}

void UATP_PythonTramControls::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// Returns the first character of the received UDP message, '0' is returned if no message was received
char UATP_PythonTramControls::GetInputOverUDP()
{
    if (!m_pListenSocket) {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error: m_ListenSocket not set up"));
        return '0';
    }

    uint8 ReceiveBuffer[1];  // Buffer size of 1 byte
    int32 BytesRead;
    TSharedRef<FInternetAddr> SenderAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

    // Handle received UDP packets
    while (m_pListenSocket->RecvFrom(ReceiveBuffer, sizeof(ReceiveBuffer), BytesRead, *SenderAddress))
    {
        if (BytesRead > 0)
        {
            return ReceiveBuffer[0]; // Directly return the received character
        }
    }

    return '0';
}


void UATP_PythonTramControls::SendUDPMessage(const FString& Message, const FString& IP)
{
	if (!m_pListenSocket) return;

	int32 BytesSent;
	TSharedRef<FInternetAddr> RemoteAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

	bool bIsValid;
	RemoteAddr->SetIp(*IP, bIsValid);
	RemoteAddr->SetPort(m_UDPSendPort);

	if (bIsValid)
	{
		TArray<uint8> SendBuffer;
		StringToBytes(Message, SendBuffer, StringCast<ANSICHAR>(*Message).Length());
		m_pListenSocket->SendTo(SendBuffer.GetData(), SendBuffer.Num(), BytesSent, *RemoteAddr);
	}
}