// Fill out your copyright notice in the Description page of Project Settings.

#include "PixelCapture.h"


APixelCapture::APixelCapture()
{
	PrimaryActorTick.bCanEverTick = true;

}

void APixelCapture::BeginPlay()
{
	Super::BeginPlay();
	
}

void APixelCapture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool APixelCapture::CapturePixels()
{


	return false;
}

bool APixelCapture::TrimPixels(TArray<FDoubleArray>& OUT_pixelMatrix)
{


	return false;
}

bool APixelCapture::ExportPixels(const TArray<FDoubleArray>& pixelMatrix)
{


	return false;
}
