// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Templates/UniquePtr.h"
#include "TramCartController.h"
#include "TramCartBehavior.generated.h"

class USplineComponent;
class UBoxComponent;

USTRUCT(BlueprintType)
struct FTramCartStruct_
{
	GENERATED_BODY()

	// Constructor for initialization
	FTramCartStruct_()
		: pCart(nullptr), pFrontWheel(nullptr), pBackWheel(nullptr), pFrontConnnection(nullptr), pBackConnection(nullptr)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TramCart")
	UStaticMeshComponent* pCart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TramCart")
	USceneComponent* pFrontWheel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TramCart")
	USceneComponent* pBackWheel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TramCart")
	UStaticMeshComponent* pFrontConnnection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TramCart")
	UStaticMeshComponent* pBackConnection;

	float m_WheelDistance{ 0.f };
	float m_PreviousCartDistance{ 0.f };

	float m_FrontWheelDistance{ 0.f };
	float m_BackWheelDistance{ 0.f };

	FRotator m_TramRotationOffset{};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVANCEDTRAMPLUGIN_API UATP_TramCartBehavior : public UActorComponent
{
	GENERATED_BODY()

//---------------------------------------------------------------------------------\\
//------------------------------------Functions------------------------------------\\
//---------------------------------------------------------------------------------\\

//----------------------------------------------------\\
//------------------Unreal-Functions------------------\\
//----------------------------------------------------\\

public:
	UATP_TramCartBehavior();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

//--------------------------------------------------\\
//------------------Initialization------------------\\
//--------------------------------------------------\\

public:
	UFUNCTION(BlueprintCallable)
	void InitializeCart(const FTramCartStruct_& carts);
	
	UFUNCTION(BlueprintCallable)
	void AddCartToFollow(UATP_TramCartBehavior* cartToFollow);

	UFUNCTION(BlueprintCallable)
	void InitializeTram();

	void SetFollowingCart(UATP_TramCartBehavior* cartToFollow) { m_pFollowingCart = cartToFollow; }

	const FTramCartStruct_& GetTramCart() const { return m_pThisCart; }

	UFUNCTION(BlueprintCallable)
	void IsEngineCart(bool& isEngine)
	{
		if (m_pCartToFollow == nullptr)
		{
			isEngine = true;
			return;
		}
		isEngine = false;
	}

protected:
private:
	
//--------------------------------------------------\\
//------------------Splines-------------------------\\
//--------------------------------------------------\\

public:
	USplineComponent* GetSpline() const { return m_pSpline; }

	UFUNCTION(BlueprintCallable)
	void SetTramTrackSpline(USplineComponent* pSpline);

protected:

private:
	UFUNCTION(BlueprintCallable)
	USplineComponent* FindSplineWithString(const FString& name);

	UFUNCTION(BlueprintCallable)
	bool RetrieveTramTracks();

	bool PlaceCartOnSpline();

	//Get Distance on spline depending on a world Location point
	float GetDistanceAlongSpline(const FVector& worldLocation, USplineComponent* pSpline) const;
	
	FVector FindClosestPointOnSplineAtDistance(USplineComponent* pSpline, const FVector& point, float distance) const;

//--------------------------------------------------\\
//------------------Physics-------------------------\\
//--------------------------------------------------\\

public:
	UFUNCTION(BlueprintCallable)
	void UpdateWeatherFriction(float wetness, float snowCoverage);

protected:

	float GetCurrentSpeed() const
	{
		return m_CurrentSpeed;
	}

private:
	//This will calculate the new speed of the tram according to the current input and physics(mass, friction)
	float CalculateSpeedDifference(float Force, bool skipInput = false) const;
	
	void CalculateCurrentSpeed(float deltaTime);

	float UnrealUnitsToKMpH(float unrealSpeed) const;

	float KMpHToUnrealUnits(float kmphSpeed) const;

//--------------------------------------------------\\
//------------------Movement------------------------\\
//--------------------------------------------------\\

public:
	void UpdatePosition(float DeltaTime);

	UATP_TramCartController* GetTramController() { return m_pTramController; }

protected:
	
private:
	void MoveCartBasedOnWheels(float DeltaTime);

	void MoveWheels(float deltaTime);

	bool IsDelayDone(float deltaTime);

	bool RetrieveTramController();

//--------------------------------------------------\\
//------------------Collisions----------------------\\
//--------------------------------------------------\\

public:
	UFUNCTION(BlueprintCallable)
	void OnCollisionEnter(AActor* other);

	UFUNCTION(BlueprintCallable)
	void OnCollisionExit(AActor* other);

protected:

private:
	void InitializeCollisionBox();

	void EnableCollision(bool enable);
	
//---------------------------------------------------------------------------------\\
//------------------------------------Variables------------------------------------\\
//---------------------------------------------------------------------------------\\

protected:

//--------------------------------------------------\\
//------------------GeneralTram---------------------\\
//--------------------------------------------------\\

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tram|General")
	FTramCartStruct_ m_pThisCart{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tram|General")
	UATP_TramCartBehavior* m_pCartToFollow{ nullptr };
	UATP_TramCartBehavior* m_pFollowingCart{ nullptr };

//--------------------------------------------------\\
//------------------Spline--------------------------\\
//--------------------------------------------------\\

	USplineComponent* m_pSpline{ nullptr };
	bool m_IsSplineSet{ false };

//--------------------------------------------------\\
//------------------Physics-------------------------\\
//--------------------------------------------------\\

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tram|Physics")
	float m_Gravity{ 9.81f };
	float m_Friction{ 0.5f };
	float m_CurrentSpeed{ 0.1f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tram|Physics|Friction")
	float m_RainFriction{ 0.2f };	//0.1  to 0.2
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tram|Physics|Friction")
	float m_SnowFriction{ 0.1f };	//0.05 to 0.1
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tram|Physics|Friction")
	float m_DryFriction{ 0.4f };	//0.2  to 0.5

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tram|Settings")
	float m_Mass{ 39500.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tram|Settings")
	float m_EnginePower{ 590.f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tram|Settings")
	float m_BrakeForce{ 800.f };

//--------------------------------------------------\\
//------------------Movement------------------------\\
//--------------------------------------------------\\

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tram|Physics")
	float m_MaxSpeed_kmph{ 80.f };
	
	float m_MaxSpeed_UUs{};

	//Tram Controller
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tram")
	TSubclassOf<UATP_TramCartController> m_TramControllerClass{ nullptr }; //Class selector

	UATP_TramCartController* m_pTramController{ nullptr }; //Actual controller

	bool m_IsDirectionReversed{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tram|Settings")
	float m_DelayBeforeStart{ 10.f };

//--------------------------------------------------\\
//------------------Collisions----------------------\\
//--------------------------------------------------\\

	UBoxComponent* m_pCollisionBox{ nullptr };
	
	//Collision mass multiplier for more intenst collisions reaction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tram|Collisions")
	float m_CollisionMassMultiplier{ 1.f };

//--------------------------------------------------\\
//--------------------DEBUG-------------------------\\
//--------------------------------------------------\\

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tram|Settings")
	bool m_Debug{ true };
};
