// Fill out your copyright notice in the Description page of Project Settings.

#include "Tests/PerformanceCommon.h"
#include "AutomationTest.h"
#include "ChartCreation.h"

bool FWaitCommand::Update()
{
	double currentTime = FPlatformTime::Seconds();
	if (currentTime - StartTime >= Duration)
	{
		return true;
	}

	return false;
}

// 
// Command for starting the FPS chart
//
class FFPSBaseCommand : public IAutomationLatentCommand
{
public:
	FFPSBaseCommand(TSharedPtr<FPerformanceTrackingChart>& performanceCounter, FDateTime& captureStartTime)
		: PerformanceCounter(performanceCounter)
		, CaptureStartTime(captureStartTime)
	{
	}

	virtual bool Update() override
	{
		CaptureStartTime = FDateTime::Now();
		PerformanceCounter = MakeShareable(new FPerformanceTrackingChart(CaptureStartTime, FString(TEXT("FPSTest"))));
		GEngine->AddPerformanceDataConsumer(PerformanceCounter);

		return true;
	}

	TSharedPtr<FPerformanceTrackingChart>& PerformanceCounter;
	FDateTime& CaptureStartTime;
};

// 
// Command for starting the FPS chart
//
class FStartFPSChartCommand : public FFPSBaseCommand
{
public:
	FStartFPSChartCommand(TSharedPtr<FPerformanceTrackingChart>& performanceCounter, FDateTime& captureStartTime)
		: FFPSBaseCommand(performanceCounter, captureStartTime)
	{
	}
};


// 
// Command for stopping the FPS chart
//
class FStopFPSChartCommand : public FFPSBaseCommand
{
public:
	FStopFPSChartCommand(TSharedPtr<FPerformanceTrackingChart>& performanceCounter, FDateTime& captureStartTime)
		: FFPSBaseCommand(performanceCounter, captureStartTime)
	{
	}

	virtual bool Update() override
	{
		if (PerformanceCounter.IsValid())
		{
			FPerformanceTrackingChart* chart = PerformanceCounter.Get();

			int32 numFrames = chart->FramerateHistogram.GetNumMeasurements();
			double totalTime = chart->FramerateHistogram.GetSumOfAllMeasures();

			float averageFPS = (double)numFrames / totalTime;

			float avgGPUFrameTime = float((chart->TotalFrameTime_GPU / numFrames) * 1000.0);
			float boundGameThreadPct = (float(chart->NumFramesBound_GameThread) / float(numFrames)) * 100.0f;
			float boundRenderThreadPct = (float(chart->NumFramesBound_RenderThread) / float(numFrames)) * 100.0f;
			float boundGPUPct = (float(chart->NumFramesBound_GPU) / float(numFrames)) * 100.0f;

			//FString path = |"";//GetFPSDataPath(TestLocation.Name, TEXT(".log"));

			//FString fpsString = FString::Printf(TEXT("%s;fps=%f;gpu=%f;gamepct=%f;renderpct=%f;gpupct=%f;\n"), *FFishingTestGlobals::Get()->GetMetaDataString(&CaptureStartTime), averageFPS, avgGPUFrameTime, boundGameThreadPct, boundRenderThreadPct, boundGPUPct);

			//FFileHelper::SaveStringToFile(fpsString, *path);

			GEngine->RemovePerformanceDataConsumer(PerformanceCounter);
			PerformanceCounter.Reset();
		}

		return true;
	}

private:
	//FLocationInfo TestLocation;

};

//
// Command to initialize an FPS test. Enqueues further late commands.
//
class FInitFPSTestCommand : public FFPSBaseCommand
{
public:
	FInitFPSTestCommand(TSharedPtr<FPerformanceTrackingChart>& performanceCounter, FDateTime& captureStartTime, float measureTime, const FString& mapName)
		: FFPSBaseCommand(performanceCounter, captureStartTime)
		, MeasureTime(measureTime)
		, MapName(mapName)
	{
	}

	//
	// Set up the commands for one single test point
	//
	void SetupSingleTestPoint()
	{
		// wait for a short time before starting to measure to avoid any artifacts from teleporting
		ADD_LATENT_AUTOMATION_COMMAND(FWaitCommand(1.0f));
		ADD_LATENT_AUTOMATION_COMMAND(FStartFPSChartCommand(PerformanceCounter, CaptureStartTime));

		// measure for N seconds
		ADD_LATENT_AUTOMATION_COMMAND(FWaitCommand(MeasureTime));

		ADD_LATENT_AUTOMATION_COMMAND(FStopFPSChartCommand(PerformanceCounter, CaptureStartTime));

		// wait a short time before moving on to avoid getting a screenshot mid-teleport

		//ADD_LATENT_AUTOMATION_COMMAND(FScreenshotCommand(CaptureStartTime, location));
		ADD_LATENT_AUTOMATION_COMMAND(FWaitCommand(1.0f));
	}

	virtual bool Update() override
	{
		SetupSingleTestPoint();

		// set the max fps back to default
		//ADD_LATENT_AUTOMATION_COMMAND(FExecConsoleCommand(TEXT("t.MaxFPS 30")));

		return true;
	}

	float MeasureTime;
	FString MapName;

	//FTestLocationHelper LocationHelper;
};

IMPLEMENT_COMPLEX_AUTOMATION_TEST(FMenuSoakTest, "Yummy.BearTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ServerContext | EAutomationTestFlags::ProductFilter)

void FMenuSoakTest::GetTests(TArray<FString>& outBeautifiedNames, TArray<FString>& outTestCommands) const
{
	outBeautifiedNames.Add(TEXT("Main Menu"));
	outTestCommands.Add(TEXT("Main Menu"));
}


bool FMenuSoakTest::RunTest(const FString& parameters)
{
	FDateTime dateTime = FDateTime::Now();
	TSharedPtr<FPerformanceTrackingChart> performanceChart = MakeShareable( new FPerformanceTrackingChart(dateTime, "LabelStuff"));
	ADD_LATENT_AUTOMATION_COMMAND(FInitFPSTestCommand(performanceChart, dateTime, 10.0f, "MyMapName"));   //FWaitCommand(60.0f));

	return true;
}
