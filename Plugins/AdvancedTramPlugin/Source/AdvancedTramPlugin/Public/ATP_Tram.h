#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ATP_Tram.generated.h"

/*
	This is the tram itself, it is the actor that will be moving along the spline tracks

	Needed items:
	- Body
	- Wheels(Front and back)
	- TramCarBehavior
*/

class UATP_TramCartController;
class UATP_TramCartBehavior;
class UArrowComponent;

UCLASS()
class ADVANCEDTRAMPLUGIN_API AATP_Tram : public AActor
{
	GENERATED_BODY()

public:
	AATP_Tram();

protected:
	virtual void BeginPlay() override;
	
	bool InitializeController();
	bool SetupTramCart();
	bool AttachCarts();

public:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tram")
	USceneComponent* m_pRootComponent{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tram")
	UStaticMeshComponent* m_pBody{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tram")
	USceneComponent* m_pFrontWheels{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tram")
	USceneComponent* m_pBackWheels{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tram")
	UArrowComponent* m_pDirectionArrow{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tram")
	UATP_TramCartBehavior* m_pTramCartBehavior{ nullptr };

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tram")
	UATP_TramCartController* m_pTramCartController{ nullptr };

protected:

	// Number of carts that will follow this cart
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	int m_CartCount{ 0 };

	//Tram Controller
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tram")
	TSubclassOf<UATP_TramCartController> m_TramControllerClass{ nullptr }; //Class selector

};
