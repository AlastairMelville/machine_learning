// Fill out your copyright notice in the Description page of Project Settings.

#include "MyHUD.h"
#include "Engine/Canvas.h"


UCanvas* AMyHUD::GetCanvas() const
{
	return Canvas;
}

UCanvas* AMyHUD::GetDebugCanvas() const
{
	return DebugCanvas;
}

UTexture2D* AMyHUD::GetCanvasDefaultTexture() const
{
	if (UCanvas* canvas = GetCanvas())
	{
		return canvas->DefaultTexture;
	}
	return nullptr;
}

UTexture2D* AMyHUD::GetDebugCanvasDefaultTexture() const
{
	if (UCanvas* canvas = GetDebugCanvas())
	{
		return canvas->DefaultTexture;
	}
	return nullptr;
}
