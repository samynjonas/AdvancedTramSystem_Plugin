// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/TramCartBehavior.h"

#include "EngineUtils.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SplineComponent.h"

#include "../Public/AdvancedSplineTracks.h"

#include "Components/BoxComponent.h"

//------------------------------------------
//------------------Unreal-Functions--------
//------------------------------------------

UATP_TramCartBehavior::UATP_TramCartBehavior()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UATP_TramCartBehavior::BeginPlay()
{
	Super::BeginPlay();

	m_MaxSpeed_UUs = KMpHToUnrealUnits(m_MaxSpeed_kmph) * 10.f;
}

void UATP_TramCartBehavior::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (m_IsSplineSet == false)
	{
		return;
	}

	//This will make it so it only ticks on the first one and this one will update the rest
	if (m_pCartToFollow == nullptr)
	{
		UpdatePosition(DeltaTime);
		if (m_pFollowingCart)
		{
			m_pFollowingCart->UpdatePosition(DeltaTime);
		}
	}
}

//------------------------------------------
//------------------Initialization----------
//------------------------------------------

void UATP_TramCartBehavior::InitializeCart(const FTramCartStruct_& cart)
{
	m_pThisCart.pCart = cart.pCart;
	m_pThisCart.pFrontWheel = cart.pFrontWheel;
	m_pThisCart.pBackWheel = cart.pBackWheel;
	m_pThisCart.pFrontConnnection = cart.pFrontConnnection;
	m_pThisCart.pBackConnection = cart.pBackConnection;

	//Wheel Distance - the distance between the front and back wheel
	m_pThisCart.m_WheelDistance = FVector::Distance(m_pThisCart.pFrontWheel->GetRelativeLocation(), m_pThisCart.pBackWheel->GetRelativeLocation());

	//Rotation Offset - the rotation offset off the cart and the world
	m_pThisCart.m_TramRotationOffset = cart.pCart->GetRelativeRotation();
}

void UATP_TramCartBehavior::AddCartToFollow(UATP_TramCartBehavior* cartToFollow)
{
	if (cartToFollow != this)
	{
		m_pCartToFollow = cartToFollow;
		cartToFollow->SetFollowingCart(this);
	}
}

void UATP_TramCartBehavior::InitializeTram()
{
	UE_LOG(LogTemp, Warning, TEXT("Initializing Tram"));

	//If m_pCartToFollow == nullptr -> This cart is front cart
	//If m_pCartToFollow != nullptr -> Take front cart in account and spawn after the cart

	//Spline Initializing
	if (m_pCartToFollow) //TODO Make it so it searches for it if it is set for the owner
	{
		//Take spline from cart infront
		m_pSpline = m_pCartToFollow->GetSpline();
		if (m_pSpline == nullptr)
		{
			m_pCartToFollow->InitializeTram();
			m_pSpline = m_pCartToFollow->GetSpline();
		}		
		m_pTramController = m_pCartToFollow->GetTramController();
	}
	else
	{
		if (RetrieveTramController() && RetrieveTramTracks())
		{
			return;
		}
	}

	//Finding and initializing the collision box
	//InitializeCollisionBox();
}

//------------------------------------------
//------------------Splines-----------------
//------------------------------------------

USplineComponent* UATP_TramCartBehavior::FindSplineWithString(const FString& name)
{
	TArray<USplineComponent*> arrTramTracks{};

	// Iterate over all actors in the current level
	for (TActorIterator<AActor> pActorItr(GetWorld()); pActorItr; ++pActorItr)
	{
		AActor* pCurrentActor = *pActorItr;
		
		// Check if the actor has a USplineComponent
		USplineComponent* pSplineComponent = pCurrentActor->FindComponentByClass<USplineComponent>();
		if (pSplineComponent)
		{
			if (pSplineComponent->GetOwner()->Tags.Contains("TramTrack"))
			{
				//Closes loop so tram can drive indefinetly around the track
				if (pSplineComponent->IsClosedLoop() == false)
				{
					pSplineComponent->SetClosedLoop(true);
				}

				arrTramTracks.Add(pSplineComponent);
			}
		}
	}

	if (arrTramTracks.IsEmpty())
	{
		return nullptr;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%d valid tram tracks found"), arrTramTracks.Num());
	}

	if (arrTramTracks.Num() == 1)
	{
		return arrTramTracks[0];
	}
	else
	{
		const int randomTramTrack = rand() % arrTramTracks.Num();
		if (arrTramTracks.Num() > randomTramTrack)
		{
			return arrTramTracks[randomTramTrack];
		}
	}

	return nullptr;
}

bool UATP_TramCartBehavior::PlaceCartOnSpline()
{
	if (m_pThisCart.pFrontWheel == nullptr || m_pThisCart.pBackWheel == nullptr)
	{
		return false;
	}

	if (m_pThisCart.pCart == nullptr)
	{
		return false;
	}

	if (m_pSpline == nullptr)
	{
		return false;
	}

	if (m_pCartToFollow)
	{
		if (m_pCollisionBox == nullptr)
		{
			InitializeCollisionBox();
		}
	
		if (m_pCollisionBox)
		{
			FBoxSphereBounds Bounds = m_pCollisionBox->GetLocalBounds();
			FVector Extents = Bounds.BoxExtent; // Half-size in each dimension
			m_pThisCart.m_PreviousCartDistance = Extents.Y * 2;
		}
	}

	FVector frontWheelPosition = m_pThisCart.pFrontWheel->GetComponentLocation();
	if (m_IsDirectionReversed)
	{
		m_pThisCart.m_FrontWheelDistance = GetDistanceAlongSpline(frontWheelPosition, m_pSpline) - m_pThisCart.m_PreviousCartDistance;
	}
	else
	{
		m_pThisCart.m_FrontWheelDistance = GetDistanceAlongSpline(frontWheelPosition, m_pSpline) + m_pThisCart.m_PreviousCartDistance;
	}

	if (m_pThisCart.m_FrontWheelDistance < 0)
	{
		m_pThisCart.m_FrontWheelDistance += m_pSpline->GetSplineLength();
	}

	FTransform frontWheelOnSplineTransform = m_pSpline->GetTransformAtDistanceAlongSpline(m_pThisCart.m_FrontWheelDistance, ESplineCoordinateSpace::World);
	frontWheelOnSplineTransform.SetScale3D(m_pThisCart.pFrontWheel->GetRelativeTransform().GetScale3D());
	m_pThisCart.pFrontWheel->SetWorldTransform(frontWheelOnSplineTransform);

	m_pThisCart.m_BackWheelDistance = m_pThisCart.m_FrontWheelDistance + m_pThisCart.m_WheelDistance;
	FTransform backWheelOnSplineTransform = m_pSpline->GetTransformAtDistanceAlongSpline(m_pThisCart.m_BackWheelDistance, ESplineCoordinateSpace::World);
	backWheelOnSplineTransform.SetScale3D(m_pThisCart.pBackWheel->GetRelativeTransform().GetScale3D());
	m_pThisCart.pBackWheel->SetWorldTransform(backWheelOnSplineTransform);
	
	MoveCartBasedOnWheels(1.f);

	return true;
}

float UATP_TramCartBehavior::GetDistanceAlongSpline(const FVector& worldLocation, USplineComponent* pSpline) const
{
	if (pSpline == nullptr)
	{
		return 0.f;
	}

	auto key = pSpline->FindInputKeyClosestToWorldLocation(worldLocation);
	float distance = pSpline->GetDistanceAlongSplineAtSplineInputKey(key);

	return distance;
}

FVector UATP_TramCartBehavior::FindClosestPointOnSplineAtDistance(USplineComponent* pSpline, const FVector& point, float distance) const
{
	float distanceAtSpline = GetDistanceAlongSpline(point, pSpline);

	FVector bestLocation{};
	float bestDistance{ FLT_MAX };

	float stepSize{ 0.05f };

	for (float alpha = 0.0f; alpha <= distance * 2; alpha += stepSize)
	{
		const FVector sampleLocation = pSpline->GetTransformAtDistanceAlongSpline(distanceAtSpline - alpha, ESplineCoordinateSpace::World).GetLocation();
		const float sampleDistance = FVector::Dist(point, sampleLocation);

		if (sampleDistance < bestDistance)
		{
			bestDistance = sampleDistance;
			bestLocation = sampleLocation;
		}
		else if (sampleDistance > bestDistance)
		{
			return bestLocation;
		}
	}
	return bestLocation;
}

void UATP_TramCartBehavior::SetTramTrackSpline(USplineComponent* pSpline)
{
	UE_LOG(LogTemp, Warning, TEXT("Setting TramTrack Spline"));

	m_pSpline = pSpline;
	PlaceCartOnSpline();

	if (m_pFollowingCart)
	{
		m_pFollowingCart->SetTramTrackSpline(pSpline);
	}
	m_IsSplineSet = true;
}

bool UATP_TramCartBehavior::RetrieveTramTracks()
{
	//Search for all actors of type advanced spline tracks
	for (TActorIterator<AATP_AdvancedSplineTracks> pActorItr(GetWorld()); pActorItr; ++pActorItr)
	{
		AATP_AdvancedSplineTracks* pTramTrack = *pActorItr;
		if (pTramTrack)
		{
			m_pSpline = pTramTrack->GetSplineComponent();
			if (m_pSpline)
			{
				if (m_Debug)
				{
					UE_LOG(LogTemp, Warning, TEXT("UATP_TramCartBehavior::RetrieveTramTracks() -- TramTrack found!"));
				}
				m_IsSplineSet = true;
				PlaceCartOnSpline();
				return true;
			}
		}
	}
	return false;
}

//------------------------------------------
//------------------Physics-----------------
//------------------------------------------

float UATP_TramCartBehavior::CalculateSpeedDifference(float Force, bool skipInput) const
{
	float normalForce{ m_Mass * m_Gravity };
	float frictionForce{ normalForce * m_Friction };

	float netForce{ frictionForce };
	if (skipInput == false)
	{
		netForce = Force - frictionForce;
	}

	float difference{ netForce / m_Mass };
	difference *= -1.f;

	return difference;
}

void UATP_TramCartBehavior::CalculateCurrentSpeed(float deltaTime)
{
	if (m_pTramController == nullptr)
	{
		if (RetrieveTramController() == false)
		{
			return;
		}
	}

	if (m_pCartToFollow == nullptr)
	{
		if (IsDelayDone(deltaTime))
		{
			//Reading output from controller
			float output{ m_pTramController->ReadOutput() };
			output = 1.f;
			if (m_Debug)
			{
				UE_LOG(LogTemp, Warning, TEXT("UATP_TramCartBehavior::CalculateCurrentSpeed() -- Output: %f"), output);
			}
			if (output > 0)
			{
				m_CurrentSpeed += CalculateSpeedDifference(m_EnginePower);
			
			}
			else if(output < 0)
			{
				m_CurrentSpeed -= CalculateSpeedDifference(m_BrakeForce);	
			}
			
			GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, FString::Printf(TEXT("CurrentSpeed: %f km/h"), UnrealUnitsToKMpH(m_CurrentSpeed)));
		}
		else
		{
			m_CurrentSpeed = 0.01f;
			if (m_Debug)
			{
				UE_LOG(LogTemp, Warning, TEXT("UATP_TramCartBehavior::CalculateCurrentSpeed() -- Waiting for delaying"));
			}
		}		
	}
	else
	{
		m_CurrentSpeed = m_pCartToFollow->GetCurrentSpeed();
	}

	//Clamping speed
	m_CurrentSpeed = FMath::Clamp(m_CurrentSpeed, 0, m_MaxSpeed_UUs);
}

void UATP_TramCartBehavior::UpdateWeatherFriction(float wetness, float snowCoverage)
{
	float friction = (1.f - wetness - snowCoverage) * m_DryFriction + wetness * m_RainFriction + snowCoverage * m_SnowFriction;
	m_Friction = friction;
}

float UATP_TramCartBehavior::UnrealUnitsToKMpH(float unrealSpeed) const
{
	//Conversion from Unreal Units to km/h - 1uu = 1cm
	return unrealSpeed * 0.036f;
}

float UATP_TramCartBehavior::KMpHToUnrealUnits(float kmphSpeed) const
{
	return (kmphSpeed / 36.f) * 100.f;
}

//------------------------------------------
//------------------Movement----------------
//------------------------------------------

void UATP_TramCartBehavior::MoveCartBasedOnWheels(float DeltaTime)
{
	if (m_pThisCart.pFrontWheel == nullptr || m_pThisCart.pBackWheel == nullptr || m_pThisCart.pCart == nullptr || m_pSpline == nullptr)
	{
		return;
	}

	FTransform frontWheelOnSplineTransform = m_pSpline->GetTransformAtDistanceAlongSpline(m_pThisCart.m_FrontWheelDistance, ESplineCoordinateSpace::World);
	FTransform backWheelOnSplineTransform = m_pSpline->GetTransformAtDistanceAlongSpline(m_pThisCart.m_BackWheelDistance, ESplineCoordinateSpace::World);

	FVector averagePosition = (frontWheelOnSplineTransform.GetLocation() + backWheelOnSplineTransform.GetLocation()) * 0.5;

	//Setting the actors position to the averagePosition of the wheels
	m_pThisCart.pCart->SetWorldLocation(averagePosition);

	//Calculate and set the rotation to the LookAtRotation
	auto lookAtRotation = UKismetMathLibrary::FindLookAtRotation(averagePosition, frontWheelOnSplineTransform.GetLocation());
	m_pThisCart.pCart->SetWorldRotation(lookAtRotation + m_pThisCart.m_TramRotationOffset);
}

void UATP_TramCartBehavior::MoveWheels(float deltaTime)
{
	if (m_pSpline == nullptr)
	{
		//InitializeTram();
		return;
	}

	//No speed - no need to calculate
	if (FMath::IsNearlyZero(m_CurrentSpeed))
	{
		return;
	}

	//No speed - no need to calculate
	if (m_CurrentSpeed == 0.f)
	{
		return;
	}

	//Calculate new distance to add with deltaTime and add to wheelDistances
	float distanceToAdd = m_CurrentSpeed * deltaTime;
	if (m_IsDirectionReversed)
	{
		distanceToAdd *= -1.f;
	}
	float splineLength = m_pSpline->GetSplineLength();

	if (m_pCartToFollow)
	{
		if (m_IsDirectionReversed)
		{
			m_pThisCart.m_FrontWheelDistance = m_pCartToFollow->GetTramCart().m_FrontWheelDistance + m_pThisCart.m_PreviousCartDistance;
		}
		else
		{
			m_pThisCart.m_FrontWheelDistance = m_pCartToFollow->GetTramCart().m_FrontWheelDistance - m_pThisCart.m_PreviousCartDistance;
		}
	}
	else
	{
		m_pThisCart.m_FrontWheelDistance += distanceToAdd;
	}

	if (m_pSpline->IsClosedLoop())
	{
		if (m_pThisCart.m_FrontWheelDistance > splineLength)
		{
			m_pThisCart.m_FrontWheelDistance -= splineLength;
		}
		if (m_pThisCart.m_FrontWheelDistance < 0)
		{
			m_pThisCart.m_FrontWheelDistance += splineLength;
		}
	}
	//Calculate their new transform
	auto frontWheelSplineTransform = m_pSpline->GetTransformAtDistanceAlongSpline(m_pThisCart.m_FrontWheelDistance, ESplineCoordinateSpace::World);
	m_pThisCart.pFrontWheel->SetWorldTransform(frontWheelSplineTransform);

	if (m_IsDirectionReversed)
	{
		m_pThisCart.m_BackWheelDistance = m_pThisCart.m_FrontWheelDistance - m_pThisCart.m_WheelDistance;
	}
	else
	{
		m_pThisCart.m_BackWheelDistance = m_pThisCart.m_FrontWheelDistance + m_pThisCart.m_WheelDistance;
	}

	if (m_pSpline->IsClosedLoop())
	{
		if (m_pThisCart.m_BackWheelDistance > splineLength)
		{
			m_pThisCart.m_BackWheelDistance -= splineLength;
		}
		if (m_pThisCart.m_BackWheelDistance < 0)
		{
			m_pThisCart.m_BackWheelDistance += splineLength;
		}
	}

	//Calculate their new transform
	auto backWheelSplineTransform = m_pSpline->GetTransformAtDistanceAlongSpline(m_pThisCart.m_BackWheelDistance, ESplineCoordinateSpace::World);
	m_pThisCart.pBackWheel->SetWorldTransform(backWheelSplineTransform);
}

void UATP_TramCartBehavior::UpdatePosition(float deltaTime)
{
	if (m_Debug)
	{
		UE_LOG(LogTemp, Warning, TEXT("UATP_TramCartBehavior::UpdatePosition() -- Updating position"));
	}

	CalculateCurrentSpeed(deltaTime);
	MoveWheels(deltaTime);
	MoveCartBasedOnWheels(deltaTime);

	if (m_pFollowingCart != nullptr)
	{
		m_pFollowingCart->UpdatePosition(deltaTime);
	}
}

bool UATP_TramCartBehavior::IsDelayDone(float deltaTime)
{
	if (m_DelayBeforeStart <= 0)
	{
		return true;
	}
	m_DelayBeforeStart -= deltaTime;
	
	return false;
}

bool UATP_TramCartBehavior::RetrieveTramController()
{
	if (m_pTramController != nullptr)
	{
		return true;
	}

	if (m_pCartToFollow)
	{
		m_pTramController = m_pCartToFollow->GetTramController();
	}
	else
	{
		m_pTramController = GetOwner()->GetComponentByClass<UATP_TramCartController>();
	}

	if (m_pTramController)
	{
		if (m_pFollowingCart)
		{
			m_pFollowingCart->RetrieveTramController();
		}
		return true;
	}

	if (m_Debug)
	{
		UE_LOG(LogTemp, Error, TEXT("UATP_TramCartBehavior::RetrieveTramController() -- No TramController found!"));
	}
	return false;
}

//------------------------------------------
////----------------Collisions--------------
//------------------------------------------

void UATP_TramCartBehavior::InitializeCollisionBox()
{
	if (m_pCollisionBox != nullptr)
	{
		return;
	}

	m_pCollisionBox = GetOwner()->GetComponentByClass<UBoxComponent>();
	if (m_pCollisionBox == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No collision box found!"));
		return;
	}
	UE_LOG(LogTemp, Display, TEXT("-ATTACHED COLLISIONBOX SUCCESFULLY-"));
}

void UATP_TramCartBehavior::EnableCollision(bool enable)
{
	if (m_pCollisionBox == nullptr)
	{
		return;
	}

	if (enable == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("- Collisions enabled -"));

		m_pCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("- Collisions dissabled -"));

		m_pCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void UATP_TramCartBehavior::OnCollisionEnter(AActor* other)
{
	/*
	UE_LOG(LogTemp, Warning, TEXT("- Collision with TRAM -"));
	if (other == nullptr || other == GetOwner())
	{
		return;
	}

	// Assuming 'VehicleActor' is a pointer to your vehicle actor
	UChaosVehicleMovementComponent* pVehicleComponent = other->FindComponentByClass<UChaosVehicleMovementComponent>();
	if (pVehicleComponent)
	{
		if (m_IsDataCollectionMode)
		{
			UMassAgentComponent* pMassAgentComponent = other->FindComponentByClass<UMassAgentComponent>();
			if (pMassAgentComponent)
			{
				other->Tags.AddUnique("Otiv_ForceDelete"); //Mark for later deletion in MassTrafficDataDamageRepairProcessor
				return;
			}
		}
		// Get the mass of the other actor involved in the collision        
		float otherMass = pVehicleComponent->Mass * m_CollisionMassMultiplier;
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::White, FString::Printf(TEXT("Collision's actors Mass: %f kg"), otherMass));

		// Calculate the impulse, which is proportional to the mass of the other object and the relative velocity
		// For simplicity, we assume a perfectly inelastic collision and ignore the other object's velocity
		float collisionImpulse = otherMass * m_CurrentSpeed; // This is a simplification

		// Calculate the change in speed of the tram using the impulse-momentum theorem
		float deltaV = collisionImpulse / m_Mass;

		// Apply the change in speed to the tram's current speed
		m_CurrentSpeed -= deltaV;

		// Ensure the tram's speed doesn't go negative
		m_CurrentSpeed = FMath::Max(m_CurrentSpeed, 0.0f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("- UChaosVehicleMovementComponent not found on the other actor -"));
	}
	*/
}

void UATP_TramCartBehavior::OnCollisionExit(AActor* other)
{

}