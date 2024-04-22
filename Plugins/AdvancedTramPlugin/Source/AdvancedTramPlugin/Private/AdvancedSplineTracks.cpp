// Fill out your copyright notice in the Description page of Project Settings.
#include "AdvancedSplineTracks.h"
#include "Components/SplineComponent.h"

AATP_AdvancedSplineTracks::AATP_AdvancedSplineTracks()
{
 	PrimaryActorTick.bCanEverTick = false;

	m_pSplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
}

void AATP_AdvancedSplineTracks::BeginPlay()
{
	Super::BeginPlay();
}

void AATP_AdvancedSplineTracks::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

