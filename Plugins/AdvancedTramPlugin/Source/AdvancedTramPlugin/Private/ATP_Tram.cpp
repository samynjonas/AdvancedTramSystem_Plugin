#include "../Public/ATP_Tram.h"
#include "../Public/TramCartBehavior.h"

#include "Components/ArrowComponent.h"

AATP_Tram::AATP_Tram()
{
 	PrimaryActorTick.bCanEverTick = true;

	// Create the default components and attach them to the root component
	m_pRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = m_pRootComponent;

	m_pBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));

	m_pFrontWheels = CreateDefaultSubobject<USceneComponent>(TEXT("FrontWheels"));
	m_pBackWheels = CreateDefaultSubobject<USceneComponent>(TEXT("BackWheels"));

	m_pDirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("DirectionArrow"));

	m_pTramCartBehavior = CreateDefaultSubobject<UATP_TramCartBehavior>(TEXT("TramCartBehavior"));

	// Attach the components to the root component
	m_pBody->SetupAttachment(m_pRootComponent);
	
	m_pFrontWheels->SetupAttachment(m_pRootComponent);
	m_pBackWheels->SetupAttachment(m_pRootComponent);

	m_pDirectionArrow->SetupAttachment(m_pRootComponent);	
}

void AATP_Tram::BeginPlay()
{
	Super::BeginPlay();
	InitializeController();
	SetupTramCart();
	m_pTramCartBehavior->InitializeTram();
}

void AATP_Tram::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AATP_Tram::SetupTramCart()
{
	FTramCartStruct_ tramCart;
	tramCart.pCart			= m_pBody;
	tramCart.pBackWheel		= m_pBackWheels;
	tramCart.pFrontWheel	= m_pFrontWheels;

	m_pTramCartBehavior->InitializeCart(tramCart);
	return false;
}

bool AATP_Tram::InitializeController()
{
	//Initialize the controller for the tram

	//Create the for the tram based on the tramCartBehavior controller class
	m_pTramCartController = NewObject<UATP_TramCartControllerAI>(this, TEXT("TramCartController"));
	if (!m_pTramCartController)
	{
		UE_LOG(LogTemp, Error, TEXT("AATP_Tram::InitializeController() -- Tram Controller could not be initialized"));
		return false;
	}
	
	return true;
}

bool AATP_Tram::AttachCarts()
{
	//Depending on the number of carts, attach them to the tram

	//Spawn in the carts


	//Attach the carts to the tram


	return true;
}

