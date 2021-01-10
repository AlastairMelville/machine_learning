// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCommandPrompt.h"
#include "Misc/MonitoredProcess.h"

// Called when the subprocess receives data
void AMyCommandPrompt::OnProcessOutput(FString msg)
{
	UE_LOG(LogTemp, Log, TEXT("OnProcessOutput() %s"), *msg);
}

// Called when the subprocess completes
void AMyCommandPrompt::OnProcessCompleted(int32 exitCode)
{
	UE_LOG(LogTemp, Log, TEXT("OnProcessCompleted() %i"), exitCode);
}

// Called when the game starts or when spawned
int32 AMyCommandPrompt::RunQuery()
{
	// build the query string we'll send to our separate query processor
	const FString query = FString::Printf(TEXT("\"SELECT id FROM data WHERE id BETWEEN %f and %f\""));// , qOffset, qLimit);

	// create a monitored process instance using our query
	const FString fullUrl = FPaths::ProjectContentDir() + URL;
	FMonitoredProcess *testProc = new FMonitoredProcess(fullUrl, CommandQuery /*query*/, bShowCmdWindow);

	// bind listeners to output and completion events
	testProc->OnOutput().BindUObject(this, &AMyCommandPrompt::OnProcessOutput);
	testProc->OnCompleted().BindUObject(this, &AMyCommandPrompt::OnProcessCompleted);

	// launch the process
	testProc->Launch();

	return (int32)testProc->Update();
}

bool AMyCommandPrompt::RunPythonScript()
{
	GEngine->Exec(NULL, TEXT("C:/Users/alime/Anaconda3/FULL-TENSORFLOW-NOTES-AND-DATA/Tensorflow-Bootcamp-master/darkflow/yolov2_od_webcam.py"));

	return true;
}
