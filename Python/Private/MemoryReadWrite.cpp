// Fill out your copyright notice in the Description page of Project Settings.

#include "MemoryReadWrite.h"
#include "FileHelper.h"
#include "WorldStateComponent.h"

bool UMemoryReadWrite::SaveToFilePath(const FString& FullFilePath, TArray<FTEST_Action>& actions, FObservedActor& observedActor)
{
	FBufferArchive ToBinary;

	return SaveGameDataToFile(FullFilePath, ToBinary, actions, observedActor);
}

bool UMemoryReadWrite::LoadFromFilePath(const FString& FullFilePath, TArray<FTEST_Action>& actions, FObservedActor& observedActor)
{	
	return LoadGameDataFromFile(FullFilePath, actions, observedActor);
}

//SaveLoadData
void UMemoryReadWrite::SaveLoadData(FArchive& Ar, TArray<FTEST_Action>& actions)
{
	// TODO: Could have a check to ensure you are not saving junk data to the file - could put a function for this on the class (e.g. check certain (or all) properties)

	Ar << actions;
}

void UMemoryReadWrite::SaveLoadData(FArchive& Ar, FObservedActor& observedActor)
{
	Ar << observedActor;
}

bool UMemoryReadWrite::SaveLoadWorldState(FArchive& Ar, const UWorldStateComponent* worldState)
{
	if (worldState)
	{
		TArray<FString> relatedWorldStates{};
		for (uint8 index = 0; index < worldState->RelatedStates.Num(); ++index)
		{
			relatedWorldStates.Add(worldState->RelatedStates[index].ToString());
		}
		Ar << relatedWorldStates;
	
		uint8 socialFeel = (uint8)worldState->SocialFeel;
		Ar << socialFeel;

		uint8 relationshipState = (uint8)worldState->RelationshipState;
		Ar << relationshipState;

		uint8 personPhysicalStrength = (uint8)worldState->PersonPhysicalStrength;
		Ar << personPhysicalStrength;



		return true;
	}

	return false;
}

bool UMemoryReadWrite::SaveGameDataToFile(const FString& FullFilePath, FBufferArchive& ToBinary, TArray<FTEST_Action>& actions, FObservedActor& observedActor)
{
	//note that the supplied FString must be the entire Filepath
	// 	if writing it out yourself in C++ make sure to use the \\
			// 	for example:

// 	FString SavePath = "C:\\MyProject\\MySaveDir\\mysavefile.save";

//Step 1: Variable Data -> Binary

	if (!actions.Num() && !observedActor.Actor)
	{
		return false;
	}

	if (observedActor.Actor)
	{
		SaveLoadData(ToBinary, observedActor);
	}
	else
	{
		SaveLoadData(ToBinary, actions);
	}

	//presumed to be global var data, 
	//could pass in the data too if you preferred

	//No Data
	if (ToBinary.Num() <= 0)
	{
		return false;
	}
	//~

	//Step 2: Binary to Hard Disk
	if (FFileHelper::SaveArrayToFile(ToBinary, *FullFilePath))
	{
		// Free Binary Array 	
		ToBinary.FlushCache();
		ToBinary.Empty();

		//ClientMessage("Save Success!");
		return true;
	}

	// Free Binary Array 	
	ToBinary.FlushCache();
	ToBinary.Empty();

	//ClientMessage("File Could Not Be Saved!");

	return false;
}

//I am using the sample save data from above examples as the data being loaded
bool UMemoryReadWrite::LoadGameDataFromFile(const FString& FullFilePath, TArray<FTEST_Action>& actions, FObservedActor& observedActor)
{
	//Load the data array,
	// 	you do not need to pre-initialize this array,
	//		UE4 C++ is awesome and fills it 
	//		with whatever contents of file are, 
	//		and however many bytes that is
	TArray<uint8> TheBinaryArray;
	if (!FFileHelper::LoadFileToArray(TheBinaryArray, *FullFilePath))
	{
		//ClientMessage("FFILEHELPER:>> Invalid File");
		return false;
		//~~
	}
		
	//File Load Error
	if (TheBinaryArray.Num() <= 0)
	{
		return false;
	}

	//~
	//		  Read the Data Retrieved by GFileManager
	//~

	FMemoryReader FromBinary = FMemoryReader(TheBinaryArray, true); //true, free data after done
	FromBinary.Seek(0);

	if (observedActor.Actor && !actions.Num())
	{
		SaveLoadData(FromBinary, observedActor);
	}
	else
	{
		actions.Empty();
		SaveLoadData(FromBinary, actions);
	}

	//~
	//								Clean up 
	//~
	FromBinary.FlushCache();

	// Empty & Close Buffer 
	TheBinaryArray.Empty();
	FromBinary.Close();

	return true;
}

bool UMemoryReadWrite::SaveGameDataToFileCompressed(const FString& FullFilePath,
	int32& SaveDataInt32,
	FVector& SaveDataVector,
	TArray<FRotator>& SaveDataRotatorArray
) {
	FBufferArchive ToBinary;
	//SaveLoadData(ToBinary, NumGemsCollected, PlayerLocation, ArrayOfRotationsOfTheStars);
	 
	// Compress File 
	//tmp compressed data array
	TArray<uint8> CompressedData;
	FArchiveSaveCompressedProxy Compressor = FArchiveSaveCompressedProxy(CompressedData, ECompressionFlags::COMPRESS_ZLIB);

	//Send entire binary array/archive to compressor
	Compressor << ToBinary;

	//send archive serialized data to binary array
	Compressor.Flush();

	//vibes to file, return successful or not
	if (FFileHelper::SaveArrayToFile(CompressedData, *FullFilePath))
	{
		// Free Binary Arrays 
		Compressor.FlushCache();
		CompressedData.Empty();

		ToBinary.FlushCache();
		ToBinary.Empty();

		// Close Buffer 
		ToBinary.Close();

		//ClientMessage("File Save Success!");

		return true;
		//
	}
	else
	{
		// Free Binary Arrays 
		Compressor.FlushCache();
		CompressedData.Empty();

		ToBinary.FlushCache();
		ToBinary.Empty();

		// Close Buffer 
		ToBinary.Close();

		//ClientMessage("File Could Not Be Saved!");

		return false;
	}
}

//I am using the sample save data from above examples as the data being loaded
bool UMemoryReadWrite::LoadGameDataFromFileCompressed(
	const FString& FullFilePath,
	int32& SaveDataInt32,
	FVector& SaveDataVector,
	TArray<FRotator>& SaveDataRotatorArray
) {
	//Load the Compressed data array
	TArray<uint8> CompressedData;
	if (!FFileHelper::LoadFileToArray(CompressedData, *FullFilePath))
	{
		//Optimize("FFILEHELPER:>> Invalid File");
		return false;
		//~~
	}

	// Decompress File 
	FArchiveLoadCompressedProxy Decompressor = FArchiveLoadCompressedProxy(CompressedData, ECompressionFlags::COMPRESS_ZLIB);

	//Decompression Error?
	if (Decompressor.GetError())
	{
		//Optimize("FArchiveLoadCompressedProxy>> ERROR : File Was Not Compressed ");
		return false;
		//
	}

	//Decompress
	FBufferArchive DecompressedBinaryArray;
	Decompressor << DecompressedBinaryArray;

	//~
	// Read the Data Retrieved by GFileManager
	//~

	FMemoryReader FromBinary = FMemoryReader(DecompressedBinaryArray, true); //true, free data after done
	FromBinary.Seek(0);
	//SaveLoadData(FromBinary, NumGemsCollected, PlayerLocation, ArrayOfRotationsOfTheStars);

	//~
	// Clean up 
	//~
	CompressedData.Empty();
	Decompressor.FlushCache();
	FromBinary.FlushCache();

	// Empty & Close Buffer 
	DecompressedBinaryArray.Empty();
	DecompressedBinaryArray.Close();

	return true;
}

AMemoryReadWriteTester::AMemoryReadWriteTester()
{
	MemoryReaderWriter = CreateDefaultSubobject<UMemoryReadWrite>(TEXT("MemoryReaderWriter"));
}
