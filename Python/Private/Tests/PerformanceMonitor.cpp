// Fill out your copyright notice in the Description page of Project Settings.

#include "Tests/PerformanceMonitor.h"
#include "Tests/PerformanceCommon.h"
//#include "Tests/FPSTestLocation.h"
//#include "GameInstances/FishingGameInstance.h"
//#include "Inventory/GameInventory.h"
//#include "Inventory/LakeInfo.h"
//#include "Peg/FishingPeg.h"
//#include "Peg/BoatPeg.h"
//#include "Tests/FPSTestLocation.h"
//#include "Level/TeleportPoint.h"
//#include "Characters/AnglingCharacter.h"

#include "AutomationTest.h"
#include "ChartCreation.h"

//
// Path to the FPS logs
//
//FString GetFPSDataPath(const FString& fileName, const FString& extension)
//{
//	if (FFishingTestGlobals::Get()->GetTestDataPath().IsEmpty())
//	{
//		return FPaths::ProfilingDir() + TEXT("FPS/") + fileName + extension;
//	}
//	else
//	{
//		return FPaths::ProjectSavedDir() + FFishingTestGlobals::Get()->GetTestDataPath() + TEXT("/") + fileName + extension;
//	}
//}
//
//
//// 
//// Command for starting the FPS chart
////
//class FFPSBaseCommand : public IAutomationLatentCommand
//{
//public:
//	FFPSBaseCommand(TSharedPtr<FPerformanceTrackingChart>& performanceCounter, FDateTime& captureStartTime)
//		: PerformanceCounter(performanceCounter)
//		, CaptureStartTime(captureStartTime)
//	{
//	}
//
//	virtual bool Update() override
//	{
//		CaptureStartTime = FDateTime::Now();
//		PerformanceCounter = MakeShareable(new FPerformanceTrackingChart(CaptureStartTime, FString(TEXT("FPSTest"))));
//		GEngine->AddPerformanceDataConsumer(PerformanceCounter);
//
//		return true;
//	}
//
//	TSharedPtr<FPerformanceTrackingChart>& PerformanceCounter;
//	FDateTime& CaptureStartTime;
//};
//
//// 
//// Command for starting the FPS chart
////
//class FStartFPSChartCommand : public FFPSBaseCommand
//{
//public:
//	FStartFPSChartCommand(TSharedPtr<FPerformanceTrackingChart>& performanceCounter, FDateTime& captureStartTime)
//		: FFPSBaseCommand(performanceCounter, captureStartTime)
//	{
//	}
//};
//
//
//// 
//// Command for stopping the FPS chart
////
//class FStopFPSChartCommand : public FFPSBaseCommand
//{
//public:
//	FStopFPSChartCommand(TSharedPtr<FPerformanceTrackingChart>& performanceCounter, FDateTime& captureStartTime, const FLocationInfo& location)
//		: FFPSBaseCommand(performanceCounter, captureStartTime)
//		, TestLocation(location)
//	{
//	}
//
//	virtual bool Update() override
//	{
//		if (PerformanceCounter.IsValid())
//		{
//			FPerformanceTrackingChart* chart = PerformanceCounter.Get();
//
//			int32 numFrames = chart->FramerateHistogram.GetNumMeasurements();
//			double totalTime = chart->FramerateHistogram.GetSumOfAllMeasures();
//
//			float averageFPS = (double)numFrames / totalTime;
//
//			float avgGPUFrameTime = float((chart->TotalFrameTime_GPU / numFrames) * 1000.0);
//			float boundGameThreadPct = (float(chart->NumFramesBound_GameThread) / float(numFrames)) * 100.0f;
//			float boundRenderThreadPct = (float(chart->NumFramesBound_RenderThread) / float(numFrames)) * 100.0f;
//			float boundGPUPct = (float(chart->NumFramesBound_GPU) / float(numFrames)) * 100.0f;
//
//			FString path = GetFPSDataPath(TestLocation.Name, TEXT(".log"));
//
//			FString fpsString = FString::Printf(TEXT("%s;fps=%f;gpu=%f;gamepct=%f;renderpct=%f;gpupct=%f;\n"), *FFishingTestGlobals::Get()->GetMetaDataString(&CaptureStartTime), averageFPS, avgGPUFrameTime, boundGameThreadPct, boundRenderThreadPct, boundGPUPct);
//
//			FFileHelper::SaveStringToFile(fpsString, *path);
//
//			GEngine->RemovePerformanceDataConsumer(PerformanceCounter);
//			PerformanceCounter.Reset();
//		}
//
//		return true;
//	}
//
//private:
//	FLocationInfo TestLocation;
//
//};
