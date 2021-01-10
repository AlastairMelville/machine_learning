// Fill out your copyright notice in the Description page of Project Settings.

#include "ImageRecognition.h"
#include "Kismet/KismetDataFormatting.h"

#include "Engine/Texture2D.h"

#include "Paths.h"
#include "FileHelper.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/TextureRenderTarget2D.h"
#include "VictoryBPFunctionLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
//#include "Runtime/MediaAssets/Public/MediaTexture.h"
#include "Runtime/MediaAssets/Private/Misc/MediaTextureResource.h"

#include "ImageUtils.h"
#include "Engine/StaticMeshActor.h"

FPredictionCallback UImageRecognition::Predict(const TArray<double> imagePixelValues, const TArray<FDoubleArray> weights, const TArray<double> biases,
	const TArray<int32> labels /* = TArray<int32>() */)
{
	// TEMP Start
	//TArray<FDoubleArray> tempWeights = TArray<FDoubleArray>{ FDoubleArray(TArray<double>{0.25,1.0}), FDoubleArray(TArray<double>{0.5,0.75}), FDoubleArray(TArray<double>{0.75,0.5}), FDoubleArray(TArray<double>{1.0,0.25}), };
	
	//TArray<FDoubleArray> tempBiases = TArray<FDoubleArray>{ FDoubleArray(TArray<double>{0.5,1.0}) };
	
	//TArray<double> tempTestPixels = TArray<double>{0.25, 0.5, 0.75, 1.0};
	// TEMP End


	FPredictionCallback predictionCallback = FPredictionCallback();

	TArray<FDoubleArray> newWeights = weights;
	TransposeMatrix(newWeights);

	/*TArray<FDoubleArray> tempNewMatrix{};
	for (int32 rowIdx = 0; rowIdx < newWeights.Num(); ++rowIdx)
	{
		tempNewMatrix.Add(FDoubleArray());
		for (int32 colIdx = 0; colIdx < newWeights[0].Doubles.Num(); ++colIdx)
		{
			double oldValue = newWeights[rowIdx].Doubles[colIdx];
			tempNewMatrix[rowIdx].Doubles.Add(1.0f - oldValue);
		}
	}*/

	TArray<FDoubleArray> z{};
	TArray<FDoubleArray> pixelMatrix{};
	pixelMatrix.Add(FDoubleArray(imagePixelValues));
	MatrixMultiplication_Doubles(z, newWeights, pixelMatrix);
	TransposeMatrix(z);
	
	TArray<FDoubleArray> z2{};
	TArray<FDoubleArray> biasesMatrix{};
	biasesMatrix.Add(FDoubleArray(biases));
	if (!MatrixAddition_Doubles(z2, z, biasesMatrix))
	{
		//return FPredictionCallback();
	}

	TArray<double> softmaxUnitVector{};
	int32 highestIndex = INDEX_NONE;
	if (FTextRecognitionMath::Softmax(z[0].Doubles, softmaxUnitVector)) // switched z2 with z
	{
		float maxValue = FMath::Max(softmaxUnitVector, &highestIndex);
	}
	else
	{

	}

	double TEMP_minValueForDecisionToBeBasedOnThis = 0.4f; // TODO: Make this a suitable member variable
	if (highestIndex > INDEX_NONE && softmaxUnitVector[highestIndex] > TEMP_minValueForDecisionToBeBasedOnThis)
	{
		// TODO: Select a 
	}

	predictionCallback.Prediction = highestIndex;
	return predictionCallback;
}

bool UImageRecognition::GetPixelsArrayFromT2D(UTexture2D* texture, int32& textureWidth, int32& textureHeight, TArray<FLinearColor>& pixelArray, 
	TArray<float> greyscaleArray, bool bConvertToGreyscale /* = true */)
{
	if (!texture)
	{
		return false;
	}

	//To prevent overflow in BP if used in a loop
	pixelArray.Empty();

	texture->SRGB = false;
	texture->CompressionSettings = TC_VectorDisplacementmap;

	//Update settings
	texture->UpdateResource();

	// Get the first Mip in the texture's Mips that has a valid BulkData ptr
	int32 mipIndex = 0;
	for (int32 index = 0; index < texture->PlatformData->Mips.Num(); ++index)
	{
		if (texture->PlatformData->Mips[index].BulkData.CanGetPixelInfo())
		{
			mipIndex = index;
			break;
		}
	}

	FTexture2DMipMap& MyMipMap = texture->PlatformData->Mips[mipIndex];
	textureWidth = MyMipMap.SizeX;
	textureHeight = MyMipMap.SizeY;

	FUntypedBulkData* RawImageData = &MyMipMap.BulkData;

	if (!RawImageData)
	{
		return false;
	}

	FColor* RawColorArray = static_cast<FColor*>(RawImageData->Lock(LOCK_READ_ONLY));

	if (RawColorArray)
	{
		for (int32 x = 0; x < textureWidth; x++)
		{
			for (int32 y = 0; y < textureHeight; y++)
			{
				FLinearColor pixelColour = RawColorArray[x * textureWidth + y];
				
				if (bConvertToGreyscale)
				{
					greyscaleArray.Add(ConvertPixelToGreyscale(pixelColour, FColor::Transparent));
				}
				else
				{
					pixelArray.Add(pixelColour);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogScript, Warning, TEXT("RawColorArray is nullptr"));
		return false;
	}

	RawImageData->Unlock();

	FString errorString;
	//if (UVictoryBPFunctionLibrary::Victory_SavePixels(FPaths::GameSavedDir() + "myimage.png", 256, 256, pixelArray, false, true, errorString))
	{
		UE_LOG(LogScript, Warning, TEXT("It worked"));
	}

	return true;
}

float UImageRecognition::ConvertPixelToGreyscale(const FLinearColor& linearPixel, const FColor& colorPixel)
{
	// Using the luminosity method. https://www.johndcook.com/blog/2009/08/24/algorithms-convert-color-grayscale/
	const float redWeight = 0.21f;
	const float greenWeight = 0.72f;
	const float blueWeight = 0.07f;

	float red = 0.0f;
	float green = 0.0f;
	float blue = 0.0f;

	if (linearPixel != FLinearColor::Transparent)
	{
		red = linearPixel.R;
		green = linearPixel.G;
		blue = linearPixel.B;
	}
	else
	{
		red = colorPixel.R;
		green = colorPixel.G;
		blue = colorPixel.B;
	}

	// Return weighted average of color (weights above)
	return (red * redWeight) + (green * greenWeight) + (blue * blueWeight);
}

TArray<float> UImageRecognition::ConvertPixelArrayToGreyscale(const TArray<FLinearColor>& linearPixelArray, const TArray<FColor>& colorPixelArray)
{
	TArray<float> greyscalePixels{};

	if (linearPixelArray.Num())
	{
		for (const FLinearColor& pixel : linearPixelArray)
		{
			greyscalePixels.Add(ConvertPixelToGreyscale(pixel, FColor::Transparent));
		}
	}
	else if (colorPixelArray.Num())
	{
		for (const FColor& pixel : colorPixelArray)
		{
			greyscalePixels.Add(ConvertPixelToGreyscale(FLinearColor::Transparent, pixel));
		}
	}

	return greyscalePixels;
}

TArray<float> UImageRecognition::Evaluate(TArray<float>& pixelValueArray, TArray<FLabelWeight>& pixelWeightArray)
{
	if (pixelValueArray.Num() && pixelWeightArray.Num())
	{
		// If 
		if (pixelValueArray.Num() == pixelWeightArray.Num())
		{
			TArray<float> chancesOfBeingLabels{};
			chancesOfBeingLabels.SetNum(int32(ELabel::COUNT));

			/* For each label, calculate the chance of this image being that label by stepping through each pixel and multiplying 
			   its value by the pixel weight. */
			for (int32 labelIdx = 0; labelIdx < int32(ELabel::COUNT); ++labelIdx)
			{
				//if (pixelBiasArray.IsValidIndex(labelIdx))
				{
					float chanceOfBeingLabel = 0.0f;

					int32 pixelValueArrayCount = pixelValueArray.Num();
					for (int32 pixelIdx = 0; pixelIdx < pixelValueArrayCount; ++pixelIdx)
					{
						float pixelWeight = pixelWeightArray[pixelIdx].Weights[labelIdx];
						float chanceValue = pixelValueArray[pixelIdx] * pixelWeight;						
						chanceOfBeingLabel += chanceValue;
					}

					// Store the chance of this image being of this label
					chancesOfBeingLabels[labelIdx] = chanceOfBeingLabel;
				}
				//else
				{
					UE_LOG(LogScript, Warning, TEXT("pixelBiasArray element was too short. It didn't contain Label %i or above."), labelIdx);
				}
			}

			chancesOfBeingLabels.Sort();
			return chancesOfBeingLabels;
		}
	}
	return TArray<float>();
}

bool UImageRecognition::FindFirstPixelInImage(const TArray<double>& pixels, const FVector2D& pixelsDimensions, const TArray<FDoubleArray>& weightsMatrix, 
	uint32& OUT_pixelLocation)
{
	/* Reshape pixel array into pixel matrix to know it's dimensions/borders, so as to better match the image against the possible word image */
	TArray<FDoubleArray> pixelMatrix = ReshapePixelArray(pixels, pixelsDimensions);
	// 
	TArray<FDoubleArray> tempWeightsMatrix = weightsMatrix;
	TransposeMatrix(tempWeightsMatrix);


	uint32 numRows = pixelMatrix.Num();
	uint32 numCols = pixelMatrix[0].Doubles.Num();
	for (uint32 rowIdx = 0; rowIdx < numRows; ++rowIdx)
	{
		for (uint32 colIdx = 0; colIdx < numCols; ++colIdx)
		{
			// Try to find the first pixel of each possible text image/ feature
			//uint8 numFeatures = tempWeightsMatrix.Num();
			//for (uint8 featureIdx = 0; featureIdx < numFeatures; ++featureIdx)
			{				
				/* To get a better idea of how well it fits the text image, reshape the text image's pixels into the text image's pixel dimensions (32 x 8) */
				//FVector2D newShape = FVector2D(32, 8);
				//TArray<FDoubleArray> reshapedWeightsMatrix = ReshapePixelArray(tempWeightsMatrix[featureIdx].Doubles, newShape);

				// 
				FVector2D dimensions = FVector2D(6, 6);
				uint32 pixelIdx = (rowIdx * colIdx) + colIdx;
				if (colIdx >= 0 && (colIdx + dimensions.X) < numCols
					&& rowIdx >= 0 && (rowIdx + dimensions.Y) < numRows)
				{					
					TArray<FDoubleArray> matrix{};/* = TArray<FDoubleArray>{ 
						FDoubleArray(TArray<double>{ 0.75, 0.75, 0.75, 0.75, 0.75, 0.75 }),
						FDoubleArray(TArray<double>{ 0.75, 0.75, 0.75, 0.75, 0.75, 0.75 }),
						FDoubleArray(TArray<double>{ 0.7, 0.2, 0.2, 0.2, 0.2, 0.7 }),
						FDoubleArray(TArray<double>{ 0.7, 0.2, 0.2, 0.2, 0.2, 0.7 }),
						FDoubleArray(TArray<double>{ 0.7, 0.2, 0.2, 0.2, 0.2, 0.7 }),
						FDoubleArray(TArray<double>{ 0.7, 0.2, 0.2, 0.2, 0.2, 0.7 }),
						FDoubleArray(TArray<double>{ 0.7, 0.7, 0.7, 0.7, 0.7, 0.7 }),
						FDoubleArray(TArray<double>{ 0.7, 0.7, 0.7, 0.7, 0.7, 0.7 }),
					};*/
					TArray<double> tempArray{ 0.765804,0.227177,0.754039,0.756863,0.760784,0.196627,0.76498,0.702235,0.098588,0.094667,0.756863,0.757961,0.757961,0.09102,0.054627,0.055451,0.102784,0.764706,0.760784,0.079255,0.051804,0.051804,0.071137,0.764706,0.756863,0.079255,0.051804,0.046784,0.06749,0.764706,0.757137,0.075333,0.044235,0.043961,0.063294,0.764706,0.757961,0.071686,0.040314,0.040039,0.055725,0.761882 };
					matrix = ReshapePixelArray(tempArray, FVector2D(6, 7));

					// Compare each surrounding pixels within the above shape to the shapes values
					double diff = 0.0;
					for (uint8 xIdx = 0; xIdx < dimensions.X; ++xIdx)
					{
						for (uint8 yIdx = 0; yIdx < dimensions.Y; ++yIdx)
						{
							double pixelMatrixValue = pixelMatrix[rowIdx + xIdx].Doubles[colIdx + yIdx];
							double testMatrixValue = matrix[xIdx].Doubles[yIdx];
							diff += (FMath::Abs(pixelMatrixValue - testMatrixValue) * (1 / (dimensions.X * dimensions.Y)));
						}
					}

					//if (pixelMatrix[rowIdx].Doubles[colIdx - 1] < 0.2 && pixelMatrix[rowIdx].Doubles[colIdx] > 0.7
						//&& pixelMatrix[rowIdx].Doubles[colIdx + 5] < 0.2)//PixelsMatch(pixelMatrix[rowIdx].Doubles[colIdx], reshapedWeightsMatrix[featureIdx].Doubles[0]))
					if (diff < 0.05)
					{
						uint32 originalPixelLoc = (rowIdx * numCols) + colIdx;
						OUT_pixelLocation = ((rowIdx + (dimensions.X * 0.5)) * numCols) + colIdx;
						UE_LOG(LogTemp, Warning, TEXT("Matching pixel found at row index %i, column index %i"), rowIdx, colIdx);

						// Find the closest white space that could be a space in a sentence
						uint32 baseMinimumSpaceLength = 3, baseMaximumSpaceLength = 5;
						uint32 fontSize = 10; // the font size that the model found a match with - eg if it cannot find any macthes at size 8, it will try 9, and then 10 etc.
						uint32 minimumSpaceLength = (fontSize * baseMinimumSpaceLength);
						uint32 maximumSpaceLength = (fontSize * baseMaximumSpaceLength);
						uint32 numSpacesDetectedToDate = 10;
						uint32 estimatedSpaceLength = 7; // estimated and modified at runtime
						uint32 numPixelsTogether = 0;
						for (uint32 yIdx = 0; yIdx < (numCols - colIdx); ++yIdx)
						{
							uint8 currentIndex = colIdx + yIdx;
							if (pixelMatrix[rowIdx].Doubles.IsValidIndex(currentIndex))
							{
								if (pixelMatrix[rowIdx].Doubles[currentIndex] < 0.1)
								{
									++numPixelsTogether;
								}
								else
								{
									if (numPixelsTogether >= (estimatedSpaceLength - 2)) // the -2 is to allow tolerence
									{
										if (FMath::IsWithinInclusive(numPixelsTogether, minimumSpaceLength, maximumSpaceLength))
										{
											// Can confirm that it is a space by searching the rectangle between the two tracked non-blank pixels
											bool bConfirmedAsSpace = true;
											if (true)
											{												
												uint32 letterHeight = matrix.Num();
												for (uint32 xIdx = 0; xIdx < numPixelsTogether; ++xIdx)
												{
													for (uint32 yIdx = 0; yIdx < letterHeight; ++yIdx)
													{
														if (pixelMatrix[rowIdx + xIdx].Doubles[(currentIndex - numPixelsTogether) + yIdx] > 0.2)
														{
															bConfirmedAsSpace = false;
														}
													}
												}
											}

											// Update what the software believes the standard space length to be if it differs from uour curret estimate
											if (numPixelsTogether != estimatedSpaceLength)
											{
												uint32 unaveragedTotal = estimatedSpaceLength * numSpacesDetectedToDate;
												uint32 newEstimatedAverage = (unaveragedTotal + numPixelsTogether) / (numSpacesDetectedToDate + 1);
												estimatedSpaceLength = newEstimatedAverage;
											}
											++numSpacesDetectedToDate;

											// Get word length to filter out which words to test the pixels against (the words which the pixels more likely represent)
											uint32 endOfWord = currentIndex - numPixelsTogether;
											UE_LOG(LogTemp, Log, TEXT("numPixelsTogether > estimatedSpaceLength. Starting at pixel index %i"), endOfWord);
											uint16 bufferLength = numCols * 0.05; // add 5%
											uint32 wordLength = (endOfWord - colIdx) + bufferLength;

											//if (wordLength < )
											{

											}
											//else if (wordLength < )
											{

											}
										}

										break; // you have found a space, or a line end (no more words in line)
									}

									numPixelsTogether = 0;
								}
							}
						}



						if (diff < ClosestMatch)
						{
							ClosestMatch = diff;
							ClosestMatchingPixel = originalPixelLoc;
						}

						// Test One Start - print the found pixels to file
						TArray<float> importantPixels{};
						for (uint8 xIdx = 0; xIdx < 7; ++xIdx)
						{
							for (uint8 yIdx = 0; yIdx < 6; ++yIdx)
							{
								importantPixels.Add(pixelMatrix[14 + xIdx].Doubles[(41 + yIdx)]);
							}
						}
					
						FString fullStringOne;
						for (int32 index = 0; index < importantPixels.Num(); ++index)
						{
							FString newConv = UKismetStringLibrary::Conv_FloatToString(importantPixels[index]) + ",";
							fullStringOne += newConv;
						}

						FString filenameOne = FPaths::ProjectSavedDir() + "TextImageData\\ImportantPixels.txt";
						if (FPaths::FileExists(filenameOne) && fullStringOne.Len())
						{
							//fullString = "\n" + fullString;
							FFileHelper::SaveStringToFile(fullStringOne, *filenameOne);// , FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
						}



						// Test 2 Start - 
						TArray<float> testPixels{};
						testPixels.Init(0.0, (pixelMatrix.Num() * pixelMatrix[0].Doubles.Num()));
						for (uint8 xIdx = 0; xIdx < dimensions.X; ++xIdx)
						{
							for (uint8 yIdx = 0; yIdx < dimensions.Y; ++yIdx)
							{
								if (((rowIdx + xIdx) * numCols) + (colIdx + yIdx) == OUT_pixelLocation)
								{
									continue;
								}

								testPixels[((rowIdx + xIdx) * numCols) + (colIdx + yIdx)] = 1.0f;
							}
						}

						FString fullString;
						for (int32 index = 0; index < testPixels.Num(); ++index)
						{							
							FString newConv = UKismetStringLibrary::Conv_FloatToString(testPixels[index]) + ",";
							fullString += newConv;
						}

						FString filename = FPaths::ProjectSavedDir() + "TextImageData\\SentenceSearchTest.txt";
						if (FPaths::FileExists(filename) && fullString.Len())
						{							
							fullString = "\n" + fullString;
							FFileHelper::SaveStringToFile(fullString, *filename, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
						}
						// Test(s) End

						//return true;
						continue;

						// 
						FVector2D pixelStartCoords = FVector2D(rowIdx, colIdx - 1);
						FVector2D featureImageDims = FVector2D(32, 8);
						TArray<double> extractedPixels = ExtractPixels(pixelMatrix, pixelStartCoords, featureImageDims);
						
						
						// TEMP Start
						TArray<FStringArray> dummyStringArray{};

						TArray<FDoubleArray> imagePixels{};
						UKismetDataFormatting::GetFormattedWordVectorData(ImagePixelsFilename, imagePixels, dummyStringArray);


						TArray<FDoubleArray> weights{};
						UKismetDataFormatting::GetFormattedWordVectorData(WeightsFilename, weights, dummyStringArray);


						TArray<FDoubleArray> biases{};
						UKismetDataFormatting::GetFormattedWordVectorData(BiasesFilename, biases, dummyStringArray);


						Predict(extractedPixels, weights, biases[0].Doubles, TArray<int32>());
						// TEMP End
					}
				}
			}
		}
	}

	return false;
}

bool UImageRecognition::PixelsMatch(const double& imagePixel, const double& pixelWeight)
{
	//double diff = FMath::Abs(imagePixel - pixelWeight);
	double result = imagePixel * pixelWeight;
	if (result > 0.3)
	{
		return true;
	}

	if (result > ClosestMatch)
	{
		ClosestMatch = result;
	}

	return false;
}

TArray<double> UImageRecognition::ExtractPixels(const TArray<FDoubleArray>& pixelMatrix, const FVector2D& startPixelCoordinates, 
	const FVector2D& featureImageDimensions)
{
	// 
	uint32 numPixelsToExtract = featureImageDimensions.X * featureImageDimensions.Y;

	// 
	TArray<double> extractedPixels{};
	extractedPixels.Init(0.0, numPixelsToExtract);
	uint32 xIdx = 0;
	uint32 yIdx = 0;

	// 
	uint32 rowNum = pixelMatrix.Num();
	uint32 colNum = pixelMatrix[0].Doubles.Num();
	for (uint32 pixelIdx = 0; pixelIdx < numPixelsToExtract; ++pixelIdx)
	{
		extractedPixels[pixelIdx] = pixelMatrix[startPixelCoordinates.X + xIdx].Doubles[startPixelCoordinates.Y + yIdx];

		// 
		if (yIdx == (rowNum - 1))
		{
			// Move to next row
			yIdx = 0;
			++xIdx;
		}
		else
		{
			++yIdx;
		}
	}


	return extractedPixels;
}

FVector2D UImageRecognition::FindEndOfWord(const TArray<double>& pixels, const FVector2D& startLocation, const FVector2D& dimensions)
{
	FVector2D location2D = FVector2D();
	TArray<FDoubleArray> pixelMatrix = ReshapePixelArray(pixels, dimensions);
	uint16 numCols = pixelMatrix[0].Doubles.Num();

	bool couldBeWordsEnd = false;
	uint8 middleOfImage = dimensions.X * 0.5f;
	uint8 numPixelsCheck = 4;
	for (uint16 colIdx = 0; colIdx < numCols; ++colIdx)
	{
		if (colIdx < numCols - numPixelsCheck)
		{
			if (pixelMatrix[middleOfImage].Doubles[colIdx] > 0.7 && pixelMatrix[middleOfImage].Doubles[colIdx + 1] < 0.2)
			{
				// If this pixel is white and the next is black, check how many black pixels follow it (including the one pixel already checked) - is it the words end?
				for (uint8 checkIdx = 0; checkIdx < numPixelsCheck; ++checkIdx)
				{
					if (pixelMatrix[middleOfImage].Doubles[colIdx + checkIdx] > 0.2)
					{
						// Pixel is NOT black
						break;
					}

					if (checkIdx == (numPixelsCheck - 1))
					{
						// If made it to last, it could be the words end
						couldBeWordsEnd = true;
					}
				}
			}
		}
	}

	return startLocation + location2D;
}

#if WITH_EDITOR
void UImageRecognition::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property)
	{
		if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UImageRecognition, bPredict))
		{
			if (bPredict)
			{
				TArray<FStringArray> dummyStringArray{};

				TArray<FDoubleArray> imagePixels{};
				UKismetDataFormatting::GetFormattedWordVectorData(ImagePixelsFilename, imagePixels, dummyStringArray);


				TArray<FDoubleArray> weights{};
				UKismetDataFormatting::GetFormattedWordVectorData(WeightsFilename, weights, dummyStringArray);


				TArray<FDoubleArray> biases{};
				UKismetDataFormatting::GetFormattedWordVectorData(BiasesFilename, biases, dummyStringArray);
				

				Predict(imagePixels[0].Doubles, weights, biases[0].Doubles, TArray<int32>());

				bPredict = false;
			}
		}
		else if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UImageRecognition, bFindFirstPixel))
		{
			if (bFindFirstPixel)
			{
				TArray<double> pixels = TArray<double>{ 0.542196,0.54302,0.550588,0.554235,0.558157,0.562078,0.566,0.566824,0.573569,0.574667,0.573843,0.570471,0.577216,0.581961,0.585608,0.593177,0.600471,0.604392,0.604667,0.60102,0.596549,0.596824,0.592353,0.595451,0.596275,0.596275,0.599373,0.595726,0.592353,0.594902,0.594353,0.594078,0.595451,0.595451,0.602196,0.602196,0.601373,0.605569,0.605569,0.605569,0.605569,0.605569,0.605569,0.605569,0.605569,0.608667,0.60949,0.60949,0.608667,0.604745,0.605569,0.605569,0.608392,0.605294,0.609216,0.609216,0.608941,0.613686,0.613412,0.612863,0.613412,0.62098,0.625451,0.626275,0.626275,0.626275,0.626275,0.629922,0.633569,0.634392,0.635216,0.638314,0.634392,0.629922,0.622353,0.626275,0.626275,0.629922,0.629647,0.641686,0.666588,0.682275,0.698235,0.682275,0.638863,0.638863,0.637765,0.64251,0.64251,0.649804,0.643333,0.639412,0.642235,0.639412,0.645882,0.654275,0.662392,0.670784,0.670784,0.674431,0.674157,0.678078,0.67498,0.677804,0.674157,0.674157,0.671059,0.671059,0.66349,0.658745,0.655647,0.663216,0.662941,0.63102,0.628196,0.627098,0.616706,0.60749,0.600745,0.599922,0.596824,0.592902,0.592627,0.592627,0.589529,0.589529,0.589255,0.585333,0.546392,0.550039,0.554784,0.558431,0.562353,0.569922,0.573843,0.573843,0.577765,0.581686,0.581961,0.581137,0.585059,0.58898,0.58898,0.596549,0.597373,0.608314,0.609412,0.608588,0.608863,0.608314,0.60749,0.603843,0.607765,0.608039,0.607765,0.611961,0.60749,0.606941,0.606392,0.606392,0.606667,0.610588,0.610588,0.610588,0.609765,0.613686,0.613961,0.613961,0.616784,0.617059,0.617882,0.617059,0.62098,0.617059,0.621804,0.62098,0.618157,0.617882,0.617059,0.62098,0.617882,0.621529,0.620431,0.617608,0.621529,0.622353,0.622353,0.621529,0.625176,0.629647,0.634118,0.637765,0.637765,0.633843,0.637765,0.642235,0.650627,0.651177,0.657922,0.657922,0.650627,0.645882,0.637765,0.633843,0.637765,0.642235,0.649529,0.666039,0.693765,0.714471,0.729608,0.717294,0.675255,0.662392,0.662118,0.666039,0.666588,0.674157,0.681726,0.686471,0.677804,0.67498,0.681451,0.682275,0.689843,0.690667,0.694314,0.697961,0.694039,0.690392,0.689294,0.682549,0.675255,0.674157,0.674157,0.671059,0.671059,0.674706,0.674706,0.678353,0.667686,0.644157,0.639686,0.631843,0.619804,0.611412,0.604667,0.603569,0.600745,0.600745,0.604118,0.604118,0.603843,0.600745,0.596824,0.592902,0.553961,0.558431,0.562353,0.566,0.569647,0.574118,0.578039,0.581686,0.581961,0.586157,0.58898,0.590078,0.593177,0.592902,0.596824,0.600745,0.604392,0.608863,0.612784,0.620353,0.624,0.627922,0.624275,0.620079,0.619529,0.619529,0.623177,0.623451,0.622627,0.619255,0.618706,0.622627,0.621804,0.625726,0.625726,0.626824,0.625726,0.626,0.626824,0.629922,0.629098,0.629098,0.629922,0.629098,0.630196,0.63302,0.633294,0.634118,0.634118,0.634118,0.633569,0.638039,0.638863,0.638863,0.637216,0.633843,0.636667,0.633843,0.633843,0.63749,0.638314,0.645608,0.653726,0.658196,0.653726,0.645608,0.645608,0.650078,0.657373,0.662118,0.670235,0.670235,0.665765,0.661843,0.658196,0.657373,0.657373,0.661843,0.666588,0.685922,0.705529,0.725961,0.737176,0.729608,0.722039,0.693216,0.685647,0.685647,0.690392,0.702431,0.72851,0.756784,0.744471,0.713647,0.705804,0.706078,0.705804,0.709451,0.709451,0.709451,0.701882,0.701882,0.686745,0.678353,0.671608,0.67051,0.67051,0.674157,0.67498,0.678902,0.682275,0.674706,0.662392,0.647529,0.639686,0.635216,0.627647,0.624,0.619804,0.616157,0.61698,0.615608,0.615608,0.616431,0.61251,0.609412,0.601843,0.597098,0.566824,0.569647,0.574392,0.577216,0.578039,0.581686,0.585608,0.586157,0.593177,0.593451,0.596275,0.597098,0.60102,0.604941,0.608588,0.61251,0.61251,0.616157,0.620079,0.624549,0.631843,0.635765,0.636588,0.636039,0.635216,0.639137,0.639137,0.640235,0.639686,0.63549,0.634941,0.635216,0.642235,0.638314,0.642235,0.645333,0.646157,0.645333,0.642235,0.63749,0.633843,0.634667,0.63749,0.637765,0.638314,0.638588,0.641686,0.644784,0.641686,0.641412,0.642784,0.647255,0.650627,0.650078,0.645882,0.641412,0.641686,0.645333,0.645333,0.646157,0.645882,0.649255,0.657647,0.662941,0.657647,0.650353,0.649529,0.650353,0.653726,0.658471,0.671059,0.685098,0.685922,0.686745,0.686745,0.685922,0.678353,0.677804,0.669412,0.678627,0.682824,0.702431,0.717294,0.733529,0.733529,0.718118,0.698784,0.718392,0.729333,0.740824,0.745569,0.763529,0.756784,0.722039,0.713647,0.713647,0.717294,0.717294,0.713647,0.705804,0.701882,0.702157,0.694314,0.69349,0.690667,0.682,0.678353,0.681726,0.682824,0.681726,0.682549,0.678627,0.667412,0.655373,0.647529,0.639961,0.639961,0.639686,0.636039,0.635765,0.631843,0.628196,0.624275,0.616706,0.616431,0.61251,0.608863,0.604118,0.578314,0.585059,0.586157,0.58898,0.592628,0.593726,0.596549,0.600196,0.60102,0.604118,0.608039,0.611686,0.615608,0.616157,0.620079,0.624824,0.627647,0.632392,0.636039,0.64051,0.644431,0.643608,0.643882,0.644157,0.647804,0.647804,0.651451,0.655373,0.658471,0.654275,0.651177,0.650627,0.657647,0.658471,0.661569,0.662392,0.66549,0.661569,0.657647,0.124039,0.076706,0.068863,0.07251,0.068588,0.068588,0.068588,0.064667,0.064667,0.064667,0.06749,0.064392,0.068314,0.064392,0.068314,0.064667,0.063569,0.064667,0.064667,0.063569,0.064392,0.064667,0.064392,0.064392,0.065216,0.064392,0.064392,0.064392,0.064392,0.068039,0.068039,0.068039,0.071686,0.068863,0.068863,0.071686,0.071686,0.071686,0.068863,0.072784,0.072784,0.075608,0.080353,0.084,0.087922,0.091843,0.088196,0.088196,0.099961,0.158784,0.757059,0.763529,0.771098,0.764353,0.741922,0.733529,0.725961,0.725137,0.717569,0.71,0.701333,0.702431,0.702431,0.702157,0.702157,0.698235,0.689843,0.694314,0.697961,0.697961,0.690392,0.697686,0.694588,0.68702,0.67498,0.666588,0.659843,0.655922,0.655922,0.655647,0.651726,0.647804,0.64051,0.631843,0.628196,0.624275,0.620353,0.616431,0.616431,0.593177,0.594,0.596824,0.597922,0.60102,0.604667,0.608863,0.608863,0.616431,0.617255,0.623726,0.624549,0.62902,0.636588,0.639412,0.641059,0.64498,0.652549,0.65902,0.660667,0.667686,0.667686,0.667137,0.659843,0.659294,0.659569,0.659569,0.663216,0.666314,0.662941,0.662392,0.666588,0.669686,0.669961,0.673882,0.677804,0.676706,0.673882,0.669137,0.108627,0.044235,0.033294,0.036118,0.029098,0.028275,0.031922,0.028,0.028824,0.024902,0.024902,0.028,0.028,0.024902,0.024078,0.027725,0.024078,0.024078,0.024078,0.024902,0.024078,0.024902,0.024902,0.024902,0.024627,0.027725,0.024902,0.024902,0.024902,0.027725,0.027725,0.024627,0.028549,0.024627,0.028549,0.027451,0.027451,0.028549,0.028549,0.028824,0.032471,0.031647,0.032471,0.036392,0.036392,0.040314,0.044235,0.04451,0.060196,0.135255,0.748392,0.759882,0.767451,0.771373,0.768549,0.763529,0.741098,0.729882,0.725137,0.717569,0.71,0.706353,0.705255,0.706078,0.706078,0.69851,0.702157,0.705804,0.702706,0.699059,0.694314,0.702431,0.709725,0.706078,0.69851,0.679451,0.671059,0.671059,0.659843,0.659569,0.659569,0.651726,0.644431,0.636863,0.632118,0.62902,0.627922,0.624275,0.624824,0.601843,0.604941,0.605765,0.609686,0.613608,0.61753,0.620353,0.624275,0.628196,0.632667,0.637137,0.648079,0.652824,0.656745,0.664039,0.671608,0.675529,0.68,0.680275,0.676078,0.672431,0.675529,0.675529,0.67498,0.671608,0.671608,0.671882,0.671882,0.675255,0.675529,0.67498,0.677804,0.682,0.684824,0.685098,0.685098,0.685647,0.685922,0.681176,0.104431,0.040039,0.028,0.024902,0.023804,0.020706,0.020706,0.016784,0.020706,0.020706,0.016784,0.016784,0.01651,0.020431,0.01651,0.01651,0.016784,0.016784,0.016784,0.016784,0.020431,0.016784,0.016784,0.01651,0.019608,0.01651,0.01651,0.016784,0.01651,0.01651,0.01651,0.01651,0.01651,0.01651,0.01651,0.01651,0.020431,0.01651,0.020431,0.020431,0.020431,0.020706,0.020431,0.024353,0.028275,0.028275,0.032471,0.032471,0.051255,0.127412,0.740274,0.752588,0.760157,0.760157,0.763804,0.763804,0.756235,0.736627,0.722588,0.721765,0.713922,0.710275,0.713098,0.71,0.705529,0.706078,0.713647,0.717294,0.714471,0.713373,0.710275,0.713922,0.714745,0.713922,0.702431,0.691216,0.678902,0.667686,0.666863,0.662941,0.656196,0.652275,0.647529,0.643608,0.639686,0.636039,0.632118,0.632118,0.62902,0.613059,0.613333,0.617255,0.622,0.628745,0.629569,0.63349,0.637412,0.64051,0.641059,0.652275,0.659843,0.664314,0.668784,0.676353,0.68,0.687569,0.691216,0.69149,0.683647,0.676628,0.679726,0.683647,0.683647,0.686745,0.683922,0.687843,0.690667,0.690118,0.690118,0.690667,0.689843,0.694039,0.694039,0.697137,0.694039,0.696863,0.697137,0.693216,0.104431,0.036941,0.027725,0.020706,0.020706,0.019608,0.01651,0.01651,0.015412,0.01651,0.012588,0.012588,0.012588,0.012588,0.012588,0.012588,0.016235,0.016235,0.016235,0.012588,0.016235,0.01651,0.016235,0.016235,0.016235,0.016235,0.012314,0.012314,0.015412,0.012314,0.012314,0.012314,0.012314,0.012314,0.015137,0.016235,0.012314,0.012314,0.016235,0.016235,0.016235,0.016235,0.016235,0.020157,0.017333,0.024078,0.024353,0.028275,0.04451,0.124588,0.737451,0.748941,0.749216,0.752863,0.760157,0.752588,0.748941,0.737451,0.73298,0.725686,0.72149,0.722314,0.725961,0.72149,0.713922,0.717569,0.722039,0.725686,0.722039,0.721765,0.717843,0.717843,0.71502,0.710275,0.701882,0.690392,0.68,0.671608,0.671608,0.664039,0.662941,0.660118,0.652275,0.651451,0.648353,0.644431,0.644431,0.64051,0.636588,0.628745,0.626196,0.632941,0.636863,0.640784,0.641608,0.645529,0.648353,0.648628,0.652549,0.656471,0.660118,0.664863,0.671608,0.676078,0.68702,0.687843,0.690941,0.692039,0.687843,0.688118,0.687294,0.691216,0.695137,0.695137,0.695137,0.698235,0.702157,0.702157,0.703255,0.702431,0.701882,0.702157,0.705255,0.705255,0.706078,0.709176,0.709176,0.701882,0.109176,0.040588,0.028549,0.024627,0.020431,0.020431,0.019333,0.01651,0.016235,0.016235,0.016235,0.012588,0.015412,0.016235,0.015412,0.012314,0.012314,0.015412,0.01651,0.020157,0.020157,0.01651,0.020157,0.020157,0.016235,0.015961,0.012314,0.012314,0.012314,0.012314,0.012314,0.011216,0.011216,0.008392,0.012039,0.012314,0.012314,0.012039,0.012314,0.012314,0.012314,0.015137,0.016235,0.016235,0.01651,0.020431,0.024078,0.028275,0.044235,0.127412,0.748941,0.75651,0.752863,0.752863,0.752039,0.744471,0.741647,0.744471,0.734078,0.733255,0.734078,0.733804,0.737451,0.729059,0.72149,0.729882,0.733529,0.730431,0.725686,0.72651,0.722588,0.718118,0.714196,0.710275,0.702157,0.695137,0.690667,0.686745,0.679177,0.675529,0.672431,0.671333,0.671333,0.667412,0.663216,0.663216,0.660392,0.656471,0.652275,0.641608,0.641882,0.644706,0.648627,0.649726,0.652824,0.656745,0.660667,0.660667,0.664863,0.667686,0.671608,0.671882,0.676353,0.683922,0.687569,0.69149,0.692314,0.694588,0.691765,0.694863,0.695137,0.699882,0.699059,0.702706,0.706902,0.706902,0.709725,0.710549,0.718941,0.718118,0.713922,0.717294,0.714471,0.718118,0.718118,0.722039,0.718118,0.713922,0.115647,0.04451,0.032471,0.032196,0.024353,0.020431,0.01651,0.015412,0.015412,0.012588,0.012314,0.015137,0.015137,0.012314,0.012314,0.012314,0.016235,0.01651,0.021255,0.024078,0.024078,0.024078,0.028,0.028,0.020157,0.016235,0.015137,0.012314,0.011216,0.012039,0.012039,0.008392,0.012039,0.012039,0.011216,0.012039,0.008392,0.008392,0.012314,0.012039,0.012039,0.012314,0.016235,0.016235,0.020157,0.020157,0.020431,0.024353,0.044235,0.127412,0.760431,0.768,0.760431,0.753137,0.748667,0.741922,0.741098,0.741098,0.738,0.737177,0.740824,0.738,0.737725,0.733804,0.733804,0.737451,0.737176,0.734353,0.734353,0.734078,0.729333,0.72651,0.721765,0.715294,0.714196,0.710275,0.706353,0.702706,0.694863,0.694863,0.690941,0.690941,0.690667,0.686745,0.682824,0.676078,0.678902,0.675804,0.675529,0.653373,0.653647,0.656745,0.660667,0.664588,0.664588,0.671608,0.672706,0.67298,0.675804,0.680549,0.683373,0.684471,0.688118,0.695137,0.695961,0.699882,0.703529,0.703804,0.706902,0.706902,0.706902,0.710824,0.713922,0.714196,0.715294,0.718118,0.718392,0.722039,0.72651,0.72651,0.726784,0.729882,0.729059,0.730157,0.730157,0.73298,0.733804,0.726784,0.124039,0.18149,0.729412,0.043961,0.032196,0.020431,0.01651,0.01651,0.016235,0.012314,0.015137,0.012314,0.012314,0.012314,0.012314,0.015137,0.019882,0.024078,0.289647,0.745098,0.036118,0.035843,0.687373,0.745098,0.039765,0.016235,0.015961,0.015137,0.012314,0.011216,0.008392,0.012039,0.012039,0.012039,0.011216,0.008392,0.012039,0.008392,0.012039,0.012039,0.008392,0.012039,0.015137,0.012314,0.016235,0.020157,0.020431,0.028,0.041137,0.128235,0.768824,0.771647,0.768,0.760706,0.760706,0.759882,0.753137,0.756784,0.752863,0.75651,0.753686,0.748941,0.748941,0.748941,0.748941,0.745843,0.745843,0.745569,0.748392,0.741647,0.740824,0.737725,0.734078,0.730157,0.726235,0.726235,0.722314,0.722314,0.718392,0.714471,0.714196,0.709451,0.706353,0.702706,0.702706,0.69851,0.694588,0.690667,0.690667,0.660667,0.664588,0.668784,0.668784,0.671882,0.676628,0.680549,0.680549,0.684471,0.691216,0.695137,0.695961,0.699059,0.70298,0.707726,0.707726,0.714471,0.714471,0.718392,0.719765,0.722588,0.722863,0.722863,0.726784,0.726784,0.729882,0.726235,0.730157,0.730981,0.734078,0.738,0.737725,0.738,0.741647,0.745569,0.741922,0.745569,0.745569,0.738,0.127961,0.228275,0.756863,0.071412,0.036118,0.028,0.020431,0.020157,0.016235,0.016235,0.016235,0.016235,0.016235,0.016235,0.016235,0.015961,0.020157,0.031922,0.337529,0.768628,0.063294,0.043686,0.721569,0.772549,0.066941,0.020157,0.015961,0.015137,0.015137,0.012314,0.015137,0.015961,0.012039,0.012039,0.008118,0.008392,0.008392,0.008392,0.012039,0.012039,0.012039,0.012039,0.015961,0.012314,0.016235,0.017333,0.017333,0.025176,0.044235,0.131059,0.775569,0.776392,0.776392,0.776392,0.772745,0.772745,0.771922,0.768,0.771647,0.771647,0.771647,0.764628,0.764078,0.760157,0.753412,0.756235,0.753137,0.753137,0.749216,0.749216,0.745569,0.741647,0.741647,0.737725,0.733804,0.733804,0.733529,0.730431,0.726784,0.72651,0.725686,0.721765,0.717843,0.71502,0.71,0.707177,0.706078,0.699333,0.699059,0.672157,0.672157,0.676078,0.677176,0.683922,0.684196,0.690941,0.691765,0.694863,0.698784,0.699608,0.706627,0.710275,0.710549,0.714471,0.715294,0.722039,0.723137,0.723137,0.727059,0.727059,0.727333,0.730431,0.734353,0.734353,0.734353,0.738275,0.738549,0.742196,0.741373,0.741647,0.745294,0.745294,0.749216,0.753137,0.753137,0.753137,0.753137,0.745569,0.132706,0.224353,0.757961,0.079255,0.047882,0.036118,0.031922,0.020431,0.020157,0.01651,0.01651,0.020157,0.024078,0.028,0.028,0.024078,0.027725,0.035843,0.333333,0.772549,0.071137,0.051529,0.721843,0.772549,0.071137,0.027725,0.020157,0.023804,0.024078,0.024078,0.024078,0.020157,0.015961,0.008392,0.012039,0.008118,0.008118,0.008118,0.008392,0.008118,0.008392,0.012039,0.012039,0.012314,0.016235,0.016235,0.020431,0.024353,0.044235,0.136078,0.776392,0.783961,0.784235,0.784235,0.783961,0.780039,0.776392,0.775569,0.776392,0.776118,0.776118,0.772471,0.771373,0.767451,0.763804,0.760706,0.76098,0.757059,0.753137,0.749216,0.745569,0.741647,0.741647,0.737725,0.737725,0.737725,0.733804,0.733529,0.734353,0.730431,0.729333,0.72651,0.722588,0.718667,0.713647,0.713647,0.709726,0.706902,0.70298,0.672431,0.676902,0.677176,0.684196,0.687843,0.690941,0.692039,0.698784,0.698784,0.703529,0.706627,0.707451,0.715294,0.714196,0.715569,0.71949,0.71949,0.726235,0.727059,0.730157,0.731255,0.734353,0.738275,0.738275,0.742196,0.742196,0.745843,0.746118,0.749216,0.750314,0.74949,0.74949,0.753412,0.754235,0.757059,0.757333,0.76098,0.757333,0.753412,0.132706,0.232196,0.756863,0.086824,0.071412,0.087098,0.059647,0.031922,0.024353,0.020431,0.024078,0.028,0.055451,0.086549,0.063294,0.031647,0.035569,0.043686,0.337529,0.768628,0.071137,0.059098,0.721843,0.776471,0.075059,0.031922,0.028,0.031922,0.055176,0.082627,0.059098,0.031647,0.020157,0.015961,0.012039,0.012039,0.008118,0.008392,0.008118,0.012039,0.012039,0.012039,0.012314,0.015961,0.016235,0.01651,0.020431,0.028275,0.04451,0.136078,0.780588,0.784235,0.783412,0.784235,0.780314,0.784235,0.780314,0.780314,0.779216,0.779216,0.776392,0.772471,0.772471,0.768549,0.764628,0.764902,0.76098,0.757333,0.753137,0.74949,0.74949,0.744745,0.738,0.741922,0.738,0.737725,0.734902,0.730706,0.730706,0.730706,0.726784,0.722863,0.722863,0.718941,0.713922,0.710824,0.707177,0.706078,0.699333,0.676902,0.68,0.684745,0.687843,0.691765,0.694863,0.695686,0.699608,0.707451,0.710275,0.711373,0.711373,0.715294,0.719216,0.723137,0.723412,0.726235,0.730157,0.73098,0.734078,0.735177,0.738,0.738275,0.742196,0.746118,0.748941,0.746118,0.753961,0.753137,0.754235,0.754235,0.757333,0.758157,0.758157,0.76098,0.764902,0.764902,0.765177,0.757333,0.136627,0.224627,0.765804,0.227177,0.754039,0.756863,0.760784,0.196627,0.036118,0.031922,0.03502,0.40502,0.760784,0.764706,0.764706,0.729412,0.047608,0.051529,0.33251,0.768902,0.074784,0.059098,0.72549,0.772549,0.07898,0.039765,0.063294,0.741176,0.764706,0.768628,0.768628,0.741176,0.03949,0.019882,0.012039,0.012039,0.008118,0.008392,0.008392,0.012039,0.008392,0.012314,0.012039,0.012314,0.016235,0.016235,0.020431,0.025451,0.04451,0.136353,0.779765,0.780863,0.780588,0.780588,0.780588,0.783412,0.780314,0.783412,0.780314,0.780314,0.776392,0.776392,0.772471,0.772471,0.767726,0.764902,0.76098,0.76098,0.753686,0.753686,0.749765,0.74502,0.745843,0.741922,0.741922,0.737725,0.734078,0.733804,0.733804,0.727059,0.725961,0.722863,0.719216,0.715294,0.71502,0.714196,0.707177,0.706078,0.706078,0.680824,0.687843,0.687843,0.691765,0.69851,0.699333,0.702431,0.707177,0.707451,0.711098,0.716118,0.716118,0.718941,0.723961,0.727882,0.73098,0.73098,0.734902,0.738,0.738824,0.741922,0.742196,0.74302,0.746118,0.750039,0.750039,0.753686,0.753961,0.757882,0.757882,0.758157,0.764628,0.765726,0.768549,0.768549,0.769373,0.769647,0.772471,0.764902,0.139725,0.228275,0.76498,0.702235,0.098588,0.094667,0.756863,0.757961,0.067216,0.042863,0.145373,0.764706,0.149294,0.094392,0.133608,0.768628,0.12102,0.055451,0.333608,0.772549,0.074784,0.059098,0.72549,0.776471,0.082627,0.047608,0.596078,0.768628,0.106157,0.078706,0.126039,0.765804,0.611765,0.031647,0.015961,0.012039,0.008118,0.008118,0.008392,0.008392,0.008118,0.012039,0.012314,0.012314,0.016235,0.020157,0.020431,0.025451,0.04451,0.132706,0.780039,0.783961,0.780863,0.78451,0.78451,0.78451,0.780588,0.780588,0.784235,0.780314,0.780314,0.776667,0.776392,0.772745,0.768824,0.768,0.764078,0.757608,0.753961,0.752863,0.752863,0.750039,0.746118,0.742196,0.742196,0.738275,0.738,0.734078,0.73098,0.730157,0.727059,0.723412,0.719216,0.718118,0.718118,0.714196,0.711098,0.707177,0.707177,0.680275,0.684745,0.69149,0.695412,0.695412,0.699608,0.700431,0.704353,0.711098,0.711098,0.715843,0.718941,0.719765,0.722863,0.726784,0.730706,0.733804,0.734902,0.738824,0.741922,0.745843,0.746941,0.749765,0.750039,0.753961,0.757608,0.757608,0.760706,0.764628,0.768549,0.768824,0.768549,0.769373,0.772471,0.773294,0.776392,0.776392,0.777216,0.772471,0.140275,0.228549,0.757961,0.09102,0.054627,0.055451,0.102784,0.764706,0.098588,0.060471,0.760784,0.760784,0.094392,0.075059,0.075059,0.761882,0.760784,0.074784,0.333608,0.772549,0.074784,0.059373,0.725765,0.772824,0.081804,0.071137,0.768628,0.733333,0.039765,0.035569,0.035569,0.733333,0.76498,0.055176,0.020157,0.015961,0.008118,0.012039,0.008392,0.012039,0.008392,0.008392,0.008392,0.012314,0.012314,0.01651,0.017608,0.028275,0.04451,0.136078,0.780863,0.780863,0.780863,0.784235,0.780314,0.77749,0.776392,0.777216,0.776118,0.776118,0.773294,0.772196,0.772196,0.768275,0.764628,0.76098,0.761255,0.757333,0.753686,0.753686,0.750039,0.746118,0.746118,0.741922,0.738,0.734078,0.734902,0.730157,0.729882,0.725961,0.722314,0.722039,0.715294,0.71502,0.711373,0.707177,0.703255,0.70298,0.69851,0.684196,0.687843,0.691765,0.695686,0.699608,0.703804,0.707451,0.711373,0.711373,0.715294,0.715294,0.722863,0.726784,0.727059,0.730706,0.733804,0.737726,0.741647,0.742745,0.74949,0.749765,0.753686,0.757608,0.760706,0.761529,0.765451,0.765177,0.768275,0.772196,0.776118,0.780039,0.783961,0.780039,0.783961,0.783961,0.787882,0.787882,0.787882,0.783961,0.144471,0.231373,0.760784,0.079255,0.051804,0.051804,0.071137,0.764706,0.110353,0.075059,0.764706,0.772549,0.768628,0.768628,0.768628,0.772549,0.764706,0.082627,0.333608,0.768902,0.074784,0.059373,0.72549,0.772549,0.07898,0.071137,0.772549,0.110078,0.042588,0.031647,0.031647,0.161059,0.768628,0.086824,0.024078,0.012039,0.012039,0.008118,0.008118,0.008392,0.012039,0.012039,0.012314,0.015137,0.016235,0.01651,0.020706,0.024353,0.045608,0.133529,0.780588,0.783686,0.783412,0.780588,0.780314,0.780314,0.776392,0.776392,0.776118,0.772196,0.768549,0.769373,0.765451,0.764628,0.761804,0.76098,0.758157,0.757333,0.753412,0.753686,0.749765,0.749765,0.745843,0.741922,0.738,0.737726,0.733804,0.730706,0.729882,0.722863,0.722863,0.718118,0.711373,0.711373,0.707451,0.707177,0.703255,0.699333,0.691765,0.691765,0.695412,0.699608,0.703255,0.707177,0.714196,0.71502,0.718941,0.722863,0.726784,0.729608,0.733529,0.734353,0.734353,0.738275,0.745294,0.746118,0.749216,0.753137,0.753961,0.76098,0.764628,0.768549,0.771647,0.775569,0.776392,0.776392,0.780314,0.780314,0.788157,0.791804,0.79098,0.794902,0.795725,0.798824,0.796,0.798824,0.799647,0.796,0.148667,0.236392,0.756863,0.079255,0.051804,0.046784,0.06749,0.764706,0.110353,0.075059,0.764706,0.694392,0.133608,0.122118,0.118196,0.117922,0.117098,0.059098,0.340627,0.769726,0.071961,0.059098,0.721843,0.772549,0.082627,0.070863,0.768628,0.110078,0.039765,0.031647,0.031647,0.141451,0.772549,0.075059,0.024078,0.012039,0.008118,0.008392,0.008118,0.008118,0.008392,0.008392,0.012314,0.012039,0.016235,0.01651,0.020431,0.024627,0.048431,0.136627,0.783961,0.783961,0.783961,0.784784,0.783686,0.780039,0.780039,0.776941,0.776118,0.775843,0.775843,0.769098,0.768275,0.768275,0.765451,0.768275,0.764353,0.760706,0.757882,0.760706,0.757059,0.753137,0.753137,0.749216,0.745294,0.738275,0.737451,0.734353,0.72651,0.72651,0.722588,0.718667,0.714745,0.710824,0.706902,0.70298,0.70298,0.695137,0.695412,0.70298,0.706627,0.707451,0.714471,0.718392,0.722314,0.729882,0.730157,0.733804,0.737725,0.738824,0.741647,0.745569,0.74949,0.753137,0.757059,0.76098,0.76098,0.764902,0.768,0.768824,0.776667,0.776667,0.780314,0.783412,0.784235,0.787333,0.791255,0.792078,0.796,0.796,0.799922,0.802745,0.802745,0.806667,0.806667,0.80749,0.806667,0.799922,0.148941,0.232471,0.757137,0.075333,0.044235,0.043961,0.063294,0.764706,0.106431,0.055451,0.760784,0.760784,0.078706,0.043686,0.035843,0.035843,0.035569,0.043686,0.333608,0.768628,0.071137,0.051529,0.72549,0.769726,0.078706,0.067216,0.769726,0.615961,0.043412,0.031647,0.035569,0.612039,0.764706,0.060196,0.020157,0.015137,0.008118,0.008392,0.008392,0.012039,0.008392,0.008392,0.012039,0.015961,0.016235,0.01651,0.020706,0.024627,0.044784,0.136902,0.788157,0.791804,0.79098,0.79098,0.791804,0.788157,0.788157,0.788157,0.784235,0.783137,0.780314,0.776392,0.77949,0.775569,0.772745,0.775569,0.771647,0.767726,0.767726,0.763804,0.76098,0.760157,0.757333,0.753412,0.750314,0.745569,0.745294,0.738549,0.737451,0.730706,0.726784,0.722863,0.722039,0.718118,0.711373,0.710275,0.707451,0.703529,0.699882,0.714745,0.718392,0.722588,0.726235,0.730157,0.734078,0.737725,0.741922,0.741922,0.745843,0.753412,0.753412,0.756235,0.760157,0.764902,0.768,0.768,0.771922,0.775569,0.776667,0.77949,0.780314,0.783412,0.787333,0.788157,0.79098,0.792078,0.796,0.798824,0.798824,0.802745,0.799922,0.803843,0.806667,0.80749,0.80749,0.810588,0.810588,0.803843,0.155686,0.240314,0.757961,0.071686,0.040314,0.040039,0.055725,0.761882,0.10251,0.042863,0.384588,0.765804,0.121843,0.06302,0.055451,0.094392,0.035843,0.039765,0.333608,0.768628,0.064118,0.047333,0.721569,0.772549,0.074784,0.047333,0.761882,0.76498,0.117922,0.06302,0.110078,0.764706,0.746196,0.027725,0.019882,0.012039,0.008392,0.008118,0.008118,0.008392,0.008392,0.008392,0.012039,0.015961,0.016235,0.01651,0.020431,0.024627,0.048706,0.141098,0.798824,0.802745,0.799922,0.798824,0.798824,0.796,0.794902,0.791255,0.791255,0.788157,0.787333,0.787333,0.787333,0.784235,0.783412,0.77949,0.77949,0.776667,0.775569,0.771922,0.768,0.764078,0.761255,0.760157,0.753412,0.74949,0.74949,0.745569,0.741647,0.737725,0.734078,0.730157,0.726235,0.722314,0.722039,0.718392,0.714471,0.710549,0.706902,0.722588,0.72651,0.730157,0.734078,0.738,0.741922,0.741922,0.745843,0.74949,0.753412,0.757333,0.761255,0.764078,0.764902,0.768824,0.771922,0.772745,0.775569,0.776667,0.780588,0.783412,0.783412,0.787333,0.788157,0.791255,0.795177,0.794902,0.796,0.796,0.799922,0.803843,0.802745,0.806667,0.806667,0.80749,0.810588,0.810588,0.80749,0.803843,0.152863,0.220706,0.753216,0.068588,0.036118,0.036118,0.051804,0.754039,0.094667,0.031922,0.035843,0.74902,0.761608,0.764706,0.760784,0.761882,0.18851,0.031647,0.317922,0.764706,0.059098,0.03949,0.710078,0.768628,0.063294,0.035569,0.055176,0.757961,0.764706,0.768628,0.764706,0.760784,0.031647,0.019882,0.015961,0.012039,0.008118,0.008392,0.008118,0.008118,0.008392,0.008392,0.012039,0.012314,0.01651,0.01651,0.020706,0.028549,0.04898,0.144196,0.803843,0.806667,0.806667,0.807765,0.802745,0.803843,0.799922,0.799922,0.799098,0.796,0.795177,0.795177,0.792078,0.791255,0.788157,0.787333,0.783686,0.779765,0.77949,0.775843,0.772745,0.771922,0.771922,0.768,0.764078,0.760157,0.756235,0.752588,0.752588,0.745843,0.741922,0.741922,0.738,0.730157,0.730157,0.726235,0.722314,0.718667,0.714745,0.730431,0.730431,0.734078,0.738,0.741922,0.745843,0.745843,0.752588,0.75651,0.757333,0.761255,0.764078,0.765177,0.768,0.771922,0.772745,0.775569,0.776667,0.776667,0.780588,0.78451,0.787333,0.788157,0.788157,0.792078,0.794902,0.794902,0.796,0.799922,0.798824,0.802745,0.803843,0.803569,0.806667,0.806667,0.807765,0.810588,0.80749,0.806667,0.148941,0.084,0.078706,0.032471,0.032196,0.028,0.024353,0.059647,0.024078,0.024078,0.024078,0.028,0.071137,0.106431,0.105333,0.071137,0.020157,0.020157,0.039765,0.059373,0.019882,0.023804,0.043412,0.064118,0.023804,0.019882,0.023804,0.043686,0.098314,0.109255,0.075059,0.024078,0.020157,0.012039,0.008118,0.008118,0.008392,0.008392,0.008118,0.008392,0.012039,0.008392,0.012314,0.012314,0.016235,0.017608,0.020706,0.028549,0.04898,0.145294,0.807765,0.811412,0.811412,0.810588,0.806667,0.803843,0.806667,0.802745,0.802745,0.799098,0.799098,0.798824,0.796,0.795177,0.791255,0.787333,0.787333,0.787333,0.783412,0.77949,0.776667,0.775569,0.775569,0.771922,0.771922,0.764353,0.764078,0.760431,0.75651,0.753412,0.749765,0.745843,0.74502,0.741098,0.737176,0.734078,0.730431,0.726235,0.72651,0.730157,0.734078,0.738,0.741922,0.741922,0.745843,0.749765,0.752588,0.75651,0.756235,0.761255,0.764078,0.765177,0.768,0.771922,0.771922,0.775569,0.776667,0.77949,0.780588,0.78451,0.78451,0.788157,0.792078,0.791255,0.792078,0.796,0.798824,0.796,0.799922,0.799922,0.802745,0.803569,0.803843,0.807765,0.807765,0.80749,0.810588,0.806667,0.148118,0.056824,0.036667,0.028549,0.028275,0.020431,0.020431,0.016235,0.020157,0.016235,0.020157,0.020157,0.024078,0.024078,0.024078,0.020157,0.019059,0.015961,0.015961,0.012039,0.015961,0.014863,0.015961,0.015961,0.015961,0.015961,0.014863,0.019882,0.020157,0.023804,0.019882,0.015961,0.012039,0.008118,0.008118,0.008118,0.004471,0.008118,0.008392,0.008392,0.012039,0.012039,0.012039,0.012314,0.01651,0.01651,0.020706,0.028549,0.04898,0.145294,0.810588,0.810588,0.811412,0.807765,0.810588,0.806667,0.803843,0.802745,0.799922,0.802745,0.799922,0.796,0.796,0.795177,0.791255,0.788157,0.791255,0.787333,0.78451,0.780588,0.780588,0.775843,0.775843,0.772745,0.771922,0.768,0.764353,0.764353,0.760431,0.75651,0.753686,0.752588,0.749765,0.745843,0.74502,0.738275,0.734353,0.730431,0.730431,0.730157,0.734078,0.734078,0.738,0.741922,0.745843,0.749765,0.753412,0.753412,0.757333,0.760157,0.764078,0.764078,0.768,0.768824,0.772745,0.775843,0.77949,0.77949,0.780314,0.78451,0.787333,0.788157,0.788157,0.792078,0.792078,0.796,0.796,0.798824,0.799922,0.802745,0.803843,0.803843,0.806667,0.806667,0.80749,0.810588,0.80749,0.802745,0.145294,0.052078,0.032745,0.024627,0.020431,0.01651,0.016235,0.015412,0.012314,0.015137,0.012314,0.012039,0.015961,0.015961,0.012039,0.015137,0.011216,0.008118,0.008118,0.008118,0.008118,0.008118,0.012039,0.012039,0.012039,0.012039,0.008118,0.008118,0.012039,0.012039,0.012039,0.008118,0.008118,0.008118,0.004196,0.008118,0.007294,0.008118,0.008118,0.008118,0.012039,0.008392,0.012314,0.012314,0.016235,0.016784,0.020706,0.024902,0.04898,0.145294,0.810588,0.811412,0.811412,0.807765,0.810588,0.80749,0.803843,0.803843,0.799922,0.799922,0.798824,0.799098,0.795177,0.795177,0.792078,0.791255,0.787333,0.787333,0.783412,0.783412,0.779765,0.776667,0.77949,0.771922,0.771922,0.768,0.768,0.764078,0.764078,0.75651,0.75651,0.752588,0.748667,0.745843,0.741922,0.741922,0.738,0.734078,0.730431,0.730431,0.734078,0.734078,0.738,0.741922,0.745843,0.74949,0.74949,0.75651,0.753412,0.760157,0.761255,0.764902,0.768824,0.771922,0.771647,0.772745,0.775569,0.77949,0.77949,0.784235,0.787333,0.788157,0.788157,0.791255,0.795177,0.796,0.795177,0.798824,0.798824,0.802745,0.799922,0.806667,0.803569,0.80749,0.80749,0.810588,0.80749,0.803843,0.145294,0.04898,0.028824,0.021529,0.020431,0.01651,0.016235,0.015137,0.012314,0.012039,0.012039,0.008392,0.008392,0.008118,0.008118,0.008118,0.008118,0.008118,0.008118,0.008118,0.008118,0.008118,0.008118,0.008118,0.004196,0.008118,0.008118,0.004196,0.008118,0.008118,0.00702,0.008118,0.008118,0.008118,0.008118,0.008118,0.004196,0.008118,0.004471,0.008118,0.008392,0.012039,0.012039,0.012314,0.01651,0.01651,0.020706,0.027725,0.04898,0.145294,0.810588,0.811412,0.810588,0.807765,0.806667,0.803843,0.806667,0.802745,0.802745,0.799922,0.799922,0.796,0.794902,0.792078,0.795177,0.791255,0.791255,0.787333,0.783686,0.783412,0.77949,0.776667,0.775843,0.771922,0.771922,0.768,0.768,0.764353,0.760157,0.760157,0.75651,0.749765,0.749765,0.745843,0.745843,0.741922,0.738,0.734353,0.730431,0.730157,0.730431,0.734078,0.738,0.741647,0.745843,0.745843,0.749765,0.753412,0.757333,0.757333,0.76098,0.765177,0.765177,0.771922,0.771922,0.772745,0.776667,0.776667,0.783412,0.783412,0.784235,0.788157,0.788157,0.792078,0.794902,0.792078,0.796,0.799922,0.799922,0.798824,0.802745,0.803843,0.803843,0.806667,0.80749,0.810588,0.807765,0.803843,0.145294,0.04898,0.028549,0.020706,0.016784,0.016235,0.016235,0.012314,0.012039,0.008392,0.008392,0.008118,0.008392,0.008118,0.008118,0.004471,0.008118,0.008118,0.008118,0.008118,0.004196,0.008118,0.008118,0.008118,0.004196,0.008118,0.004196,0.008118,0.008118,0.008118,0.008118,0.004196,0.007843,0.008118,0.004196,0.008118,0.008118,0.008118,0.004471,0.008118,0.008118,0.008392,0.012039,0.015137,0.016235,0.01651,0.020706,0.027725,0.052078,0.145294,0.80749,0.807765,0.810588,0.806667,0.807765,0.806667,0.806667,0.802745,0.802745,0.799922,0.798824,0.796,0.796,0.792078,0.791255,0.788157,0.787333,0.787333,0.783412,0.780588,0.77949,0.776667,0.772745,0.772745,0.771922,0.768,0.768,0.761255,0.761255,0.757333,0.75651,0.749765,0.749765,0.745843,0.741922,0.738,0.737176,0.734353,0.730431,0.730157,0.734078,0.734078,0.738,0.741922,0.745843,0.745843,0.752588,0.753412,0.75651,0.757333,0.760157,0.764078,0.764078,0.768,0.768824,0.772745,0.775569,0.77949,0.780588,0.784235,0.78451,0.788157,0.788157,0.788157,0.794902,0.796,0.795177,0.798824,0.796,0.799922,0.802745,0.803843,0.803843,0.803569,0.80749,0.807765,0.80749,0.803569,0.145294,0.04898,0.028824,0.020706,0.020431,0.01651,0.016235,0.012039,0.008392,0.012039,0.008392,0.008392,0.008118,0.008118,0.008118,0.004196,0.008118,0.008118,0.008118,0.004196,0.004196,0.008118,0.008118,0.008118,0.004196,0.008118,0.004196,0.004196,0.004196,0.008118,0.008118,0.004196,0.008118,0.008118,0.008118,0.008118,0.004196,0.008118,0.008118,0.008392,0.008118,0.008392,0.012039,0.012314,0.016235,0.01651,0.020431,0.024902,0.04898,0.145294,0.80749,0.806667,0.80749,0.806667,0.806667,0.806667,0.803843,0.802745,0.802745,0.799922,0.796,0.796,0.796,0.792078,0.791255,0.788157,0.787333,0.78451,0.78451,0.780588,0.776667,0.776667,0.772745,0.771922,0.771647,0.768,0.765177,0.764078,0.760157,0.75651,0.753686,0.749765,0.749765,0.744745,0.741922,0.738,0.737176,0.733255,0.729333,0.72651,0.730431,0.734078,0.738,0.741922,0.745569,0.745843,0.74949,0.753412,0.753412,0.757333,0.760157,0.761255,0.768,0.768824,0.771647,0.775569,0.772745,0.77949,0.77949,0.783412,0.784235,0.784235,0.788157,0.791255,0.791255,0.794902,0.796,0.796,0.799922,0.798824,0.802745,0.802745,0.803843,0.803843,0.803569,0.806667,0.806667,0.803843,0.145294,0.04898,0.028824,0.020706,0.016784,0.016235,0.015412,0.012314,0.012039,0.008392,0.008392,0.008118,0.004471,0.008118,0.004196,0.008118,0.004196,0.004196,0.008118,0.004196,0.007843,0.004196,0.007843,0.008118,0.007843,0.004196,0.004196,0.004196,0.008118,0.004196,0.004196,0.008118,0.004196,0.004196,0.008118,0.004196,0.004196,0.004196,0.008118,0.008118,0.008118,0.012039,0.012314,0.015137,0.016235,0.017333,0.020706,0.024627,0.04898,0.145294,0.807765,0.806667,0.806667,0.806667,0.803843,0.803843,0.802745,0.802745,0.799922,0.799098,0.799098,0.796,0.795177,0.791255,0.791255,0.787333,0.787333,0.783412,0.780588,0.779765,0.779765,0.77949,0.775843,0.771922,0.771922,0.768,0.764078,0.764078,0.760431,0.753686,0.753412,0.752588,0.745843,0.745843,0.741922,0.738275,0.738,0.734353,0.730431,0.72651,0.730157,0.734078,0.734078,0.741922,0.745569,0.745843,0.748667,0.753412,0.753686,0.757333,0.760157,0.764078,0.765177,0.768,0.768824,0.771922,0.772745,0.776667,0.77949,0.780588,0.784235,0.787333,0.788157,0.788157,0.792078,0.794902,0.795177,0.794902,0.796,0.799922,0.799922,0.803569,0.803843,0.803843,0.803569,0.803843,0.807765,0.802745,0.145294,0.04898,0.028549,0.024353,0.01651,0.01651,0.012314,0.012314,0.012039,0.008392,0.008392,0.008392,0.008118,0.004196,0.008118,0.004196,0.008118,0.004196,0.004196,0.004196,0.007843,0.008118,0.008118,0.007843,0.008118,0.004196,0.008118,0.004196,0.004196,0.004196,0.004196,0.007843,0.007843,0.004196,0.004196,0.004196,0.004196,0.008118,0.008118,0.008118,0.008118,0.012039,0.008392,0.012314,0.016235,0.01651,0.020706,0.024902,0.04898,0.14502,0.803843,0.807765,0.806667,0.806667,0.803843,0.806667,0.799922,0.799922,0.798824,0.799098,0.796,0.795177,0.792078,0.792078,0.791255,0.787333,0.78451,0.783412,0.780588,0.783412,0.77949,0.775843,0.771922,0.771922,0.768,0.768,0.765177,0.760431,0.760431,0.757333,0.752588,0.749765,0.748667,0.745843,0.741922,0.738,0.734078,0.730431,0.729333,0.729333,0.730157,0.730431,0.734353,0.739098,0.745569,0.745843,0.74949,0.753412,0.752588,0.757333,0.760157,0.761255,0.764078,0.768824,0.771922,0.772745,0.775569,0.776667,0.77949,0.77949,0.784235,0.783412,0.787333,0.787333,0.791255,0.792078,0.792078,0.796,0.796,0.796,0.799922,0.803569,0.799922,0.806667,0.803569,0.806667,0.80749,0.799922,0.145294,0.04898,0.028824,0.020706,0.016784,0.01651,0.012588,0.012314,0.012314,0.012039,0.008392,0.008392,0.008118,0.008118,0.004471,0.008118,0.008118,0.008118,0.008118,0.008118,0.004196,0.008118,0.007843,0.008118,0.004196,0.008118,0.004196,0.004196,0.008118,0.004196,0.008118,0.008118,0.008118,0.004196,0.008118,0.008118,0.008118,0.008392,0.008118,0.008118,0.008392,0.012039,0.012314,0.012588,0.01651,0.020431,0.020706,0.028824,0.052902,0.149216,0.803843,0.806667,0.80749,0.806667,0.806667,0.802745,0.802745,0.799922,0.799098,0.799098,0.798824,0.796,0.795177,0.791255,0.787333,0.787333,0.78451,0.783412,0.780588,0.77949,0.77949,0.775843,0.771922,0.768,0.768,0.764078,0.761255,0.757333,0.757333,0.753412,0.753686,0.749765,0.745843,0.745843,0.741922,0.738,0.737176,0.730431,0.72651,0.72651,0.730157,0.733255,0.734353,0.738,0.741922,0.745843,0.745843,0.749765,0.752588,0.753412,0.757333,0.764078,0.764078,0.768,0.768824,0.771647,0.771922,0.772745,0.776667,0.780588,0.783412,0.784235,0.787333,0.787333,0.788157,0.791255,0.792078,0.792078,0.798824,0.799922,0.798824,0.799922,0.802745,0.803569,0.806667,0.803843,0.803843,0.803569,0.181137,0.105255,0.08902,0.085098,0.081176,0.077255,0.07698,0.073333,0.073333,0.073059,0.069412,0.072235,0.073059,0.069137,0.069137,0.069137,0.069137,0.068863,0.072784,0.069137,0.069137,0.069137,0.069137,0.069137,0.069137,0.069137,0.069137,0.069137,0.071961,0.072235,0.072235,0.069137,0.069137,0.069137,0.069137,0.069137,0.069137,0.069137,0.069137,0.073059,0.073059,0.073059,0.07698,0.076157,0.081176,0.081176,0.085098,0.08902,0.109176,0.17749,0.803843,0.803843,0.806667,0.803843,0.803843,0.802745,0.799922,0.799922,0.796,0.798824,0.796,0.792078,0.792078,0.791255,0.788157,0.78451,0.787333,0.783412,0.780588,0.77949,0.775569,0.775843,0.771647,0.771922,0.768,0.764353,0.764078,0.760431,0.757333,0.753412,0.752588,0.749765,0.745843,0.741922,0.738,0.734353,0.733255,0.730431,0.729333 };

				FVector2D dimensions = FVector2D(128, 32);

				TArray<FDoubleArray> pixelWeightsMatrix{};
				TArray<FStringArray> pixelMatrixAsStrings{};
				UKismetDataFormatting::GetFormattedWordVectorData(WeightsFilename, pixelWeightsMatrix, pixelMatrixAsStrings);

				uint32 index = 0;
				FindFirstPixelInImage(pixels, dimensions, pixelWeightsMatrix, index);

				bFindFirstPixel = false;
			}
		}
		else if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UImageRecognition, bGetPercentRelevantPixelsInWordImage))
		{
			if (bGetPercentRelevantPixelsInWordImage)
			{
				struct ImageImportResults
				{
					bool bIsValid = false;
					int32 width = 0;
					int32 height = 0;
					TArray<FLinearColor> pixels;
				};
				ImageImportResults results;

				UTexture2D* texture = UVictoryBPFunctionLibrary::Victory_LoadTexture2D_FromFile_Pixels(FullFilepath, EJoyImageFormats::PNG, 
					results.bIsValid, results.width, results.height, results.pixels);

				if (texture && results.pixels.Num() > 0)
				{
					TArray<float> greyscalePixels = UImageRecognition::ConvertPixelArrayToGreyscale(results.pixels, TArray<FColor>());
					for (int32 index = 0; index < greyscalePixels.Num(); ++index)
					{
						greyscalePixels[index] = greyscalePixels[index] * (1.0f / 255.0f);
					}
					
					uint32 wordPixelLength = 0;
					float relativeWordLength = 0.0f; // percentage of max word length

					uint32 numPixelsToRecogniseWordEnd = 10;
					uint32 numBlankPixelsTogether = 0;
					for (int32 index = 0; index < results.width; ++index)
					{
						uint32 pixelIndex = ((results.height * 0.5) * results.width) + index;
						if (greyscalePixels[pixelIndex] < 0.1)
						{
							++numBlankPixelsTogether;
							if (numBlankPixelsTogether >= numPixelsToRecogniseWordEnd)
							{
								wordPixelLength = index - numBlankPixelsTogether; // Calculate approximate word length in pixels
								relativeWordLength = float(wordPixelLength) / float(results.width);


								FString fullString;
								for (int32 index = 0; index < greyscalePixels.Num(); ++index)
								{
									FString newConv = UKismetStringLibrary::Conv_FloatToString(greyscalePixels[index]);

									if (index < greyscalePixels.Num())
									{
										// If not the last element
										newConv += ",";
									}

									fullString += newConv;
								}

								// Add extra data about the pixel array
								fullString += ("\n" + UKismetStringLibrary::Conv_FloatToString(relativeWordLength));

								fullString += "\n"; // go to next line for the next word's data to be added
								FString filename = FPaths::ProjectSavedDir() + "TextImageData\\WordData.txt";
								if (FPaths::FileExists(filename) && fullString.Len())
								{
									FFileHelper::SaveStringToFile(fullString, *filename, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
								}

								break;
							}
						}
						else
						{
							if (numBlankPixelsTogether > 0)
							{
								numBlankPixelsTogether = 0;
							}
						}
					}
				}


				bGetPercentRelevantPixelsInWordImage = false;
			}
		}
	}
}
#endif // WITH_EDITOR

void ATester::BeginPlay()
{
	Super::BeginPlay();

	PrimaryActorTick.bCanEverTick = true;

	if (RenderTexture)
	{
		TArray<FColor> pixels{};
		check(IsInGameThread());
		FTextureRenderTargetResource* res = static_cast<FTextureRenderTargetResource*>(RenderTexture->Resource);
		if (res)
		{
			res->ReadPixels(pixels);
		}

		TArray<float> greyscalePixels = UImageRecognition::ConvertPixelArrayToGreyscale(TArray<FLinearColor>(), pixels);

		TArray<double> tempPixelArray{};
		for (int32 index = 0; index < greyscalePixels.Num(); ++index)
		{
			tempPixelArray.Add(greyscalePixels[index]);
		}		
		TArray<FDoubleArray> pixelMatrix = UImageRecognition::ReshapePixelArray(tempPixelArray, FVector2D(32, 32));

		FString fullStringOne;

		// Remove the pixels that are irrelevant
		int32 rowNum = pixelMatrix.Num();
		int32 colNum = pixelMatrix[0].Doubles.Num();
		int32 pixelToRemoveFrom = 22;
		for (int32 rowIdx = 0; rowIdx < rowNum; ++rowIdx)
		{
			pixelMatrix[rowIdx].Doubles.RemoveAt(pixelToRemoveFrom, colNum - pixelToRemoveFrom);
		
			for (int32 colIdx = 0; colIdx < pixelToRemoveFrom; ++colIdx)
			{
				FString newConv = UKismetStringLibrary::Conv_FloatToString(pixelMatrix[rowIdx].Doubles[colIdx] * (1.0f / 255.0f)) + ",";
				fullStringOne += newConv;
			}
		}

		FString filenameOne = FPaths::ProjectSavedDir() + "TextImageData\\GreyscalePixels.txt";
		if (FPaths::FileExists(filenameOne) && fullStringOne.Len())
		{
			//fullString = "\n" + fullString;
			FFileHelper::SaveStringToFile(fullStringOne, *filenameOne);// , FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
		}
	}

#if WITH_EDITOR
	if (UImageRecognition* obj = NewObject<UImageRecognition>(this, UImageRecognition::StaticClass()))
	{
		obj->FormatData();


		//obj->GetPixelsArrayFromT2D(Texture, Texture->GetSizeX, Texture->GetSizeY, );


		//int32 indexOfMax = -1;
		//float valueOfMax = 0.0f;
		//UKismetMathLibrary::MaxOfFloatArray();
		//if ()
		//{
		//	ELabel(indexOfMax); // The most likely label
		//}
	}
#endif // WITH_EDITOR}
}

void ATester::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*UTextureRenderTarget2D* renderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this);
	if (renderTarget)
	{
		renderTarget->SizeX = 256;
	}*/
}

void ATester::Stuff()
{
	if (UImageRecognition* obj = NewObject<UImageRecognition>(this, UImageRecognition::StaticClass()))
	{
		TArray<FLinearColor> pixelColours{};
		TArray<float> greyscale{};

		if (RenderTargetTexture)
		{
			TArray<FColor> pixels{};
			check(IsInGameThread());

			FTextureRenderTargetResource* res = static_cast< FTextureRenderTargetResource*>(RenderTargetTexture->Resource);
			if (res)
			{
				res->ReadPixels(pixels);
				if (pixels.Num())
				{
					TArray<float> greyscalePixels = obj->ConvertPixelArrayToGreyscale(TArray<FLinearColor>(), pixels);
					if (greyscalePixels.Num())
					{

					}
				}
			}



			if (UTextureRenderTarget2D* rendTar = Cast<UTextureRenderTarget2D>(RenderTargetTexture))
			{
				if (!NewTexture2D)
				{
					NewTexture2D = rendTar->ConstructTexture2D(this, "NewRenderTargetTexture", EObjectFlags::RF_Transient);
				}

				/*if (!new2DTexture)
				{
					FCreateTexture2DParameters params = FCreateTexture2DParameters();
					new2DTexture = FImageUtils::CreateTexture2D(rendTar->SizeX, rendTar->SizeY, pixels, this, "new2DTexture", EObjectFlags::RF_Transient, params);
				}*/

				/*rendTar->InitAutoFormat(28, 28);
				rendTar->UpdateResourceImmediate(false);*/
			}
		}

				
		if (MediaTexture)
		{
			TArray<FColor> pixels{};
			check(IsInGameThread());
			FMediaTextureResource* res = static_cast< FMediaTextureResource*>(MediaTexture->Resource);
			if (res)
			{
				res->ReadPixels(pixels);
			}

			//if (!new2DTexture)
			//{
			//	FCreateTexture2DParameters params = FCreateTexture2DParameters();
			//	new2DTexture = FImageUtils::CreateTexture2D(res->GetSizeXY().X /*1280*/, res->GetSizeXY().Y /*720*/, pixels, this, "new2DTexture", EObjectFlags::RF_Transient, params);
			//}
		}

		if (/*false && */RenderTexture)
		{
			TArray<FColor> pixels{};
			check(IsInGameThread());
			FTextureRenderTargetResource* res = static_cast< FTextureRenderTargetResource*>(RenderTexture->Resource);
			if (res)
			{
				res->ReadPixels(pixels);
			}

			/* This is commented out, as not enough time inbetween it deleting the current texture and creates a new one with the desired resolution,
			   and it refreshing the pixels. */
			/*RenderTexture->InitAutoFormat(256, 256);
			RenderTexture->UpdateResourceImmediate();*/

			if (!new2DTexture)
			{
				FCreateTexture2DParameters params = FCreateTexture2DParameters();
				new2DTexture = FImageUtils::CreateTexture2D(res->GetSizeX() /*1280*/, res->GetSizeY() /*720*/, pixels, this, "new2DTexture", EObjectFlags::RF_Dynamic, params);
			}
			if (!NewTexture2D)
			{
				NewTexture2D = RenderTexture->ConstructTexture2D(this, "NewTexture2D", EObjectFlags::RF_Transient);
			}
			if (NewTexture2D)
			{
				int32 sizeX = NewTexture2D->GetSizeX(),
					sizeY = NewTexture2D->GetSizeY();
				obj->GetPixelsArrayFromT2D(NewTexture2D, sizeX, sizeY, pixelColours, greyscale, false);
			}
		}
	}
}


#if WITH_EDITOR
bool UImageRecognition::FormatData()
{
	FString filePath = FPaths::ProjectSavedDir() + "file.txt";
	FString formattedFilePath = FPaths::ProjectSavedDir() + "formattedfile.txt";
	if (FPaths::FileExists(filePath) && FPaths::FileExists(formattedFilePath))
	{
		FString fileString;
		TArray<FString> pixelStrings{};
		TArray<FStringArray> pixelLabelInfo{};
		if (FFileHelper::LoadFileToString(fileString, *filePath))
		{
			fileString = fileString.Replace(TEXT(" "), TEXT(""));
			fileString = fileString.Replace(TEXT("["), TEXT(""));
			fileString = fileString.Replace(TEXT("\r"), TEXT(""));
			fileString = fileString.Replace(TEXT("\n"), TEXT(""));

			int32 index = -1;
			int32 numPixels = 784 * int32(ELabel::COUNT);
			int32 currentStep = 0;

			FString strToFind = "e";
			FString followingFormat = "+00";
			while (currentStep < numPixels)
			{
				index = fileString.Find(strToFind, ESearchCase::IgnoreCase, ESearchDir::FromStart, index + 1); // Start the new search from after the last element found
				if (index == INDEX_NONE)
				{
					break;
				}

				fileString.InsertAt((index + strToFind.Len() + followingFormat.Len()), ",");

				++currentStep;
			}

			TCHAR* zeroValue = TEXT("0.00");
			fileString = fileString.Replace(TEXT("0.00000000e+00"), zeroValue);

			pixelStrings = UKismetStringLibrary::ParseIntoArray(fileString, "]");

			for (FString& elem : pixelStrings)
			{
				pixelLabelInfo.Add(UKismetStringLibrary::ParseIntoArray(elem, ","));
			}

			for (const FStringArray& elem : pixelLabelInfo)
			{
				int32 elemLen = elem.Strings.Num();
				for (int32 index = 0; index < elemLen; ++index)
				{
					if (elem.Strings[index] != zeroValue)
					{
						FString& elemString = const_cast<FString&>(elem.Strings[index]);
						UKismetDataFormatting::ConvertMathNotation(elemString);
					}
				}
			}

			// Fill formatted file with this modified data
			TArray<FString> stringArrayToExport{};
			for (const FStringArray& l : pixelLabelInfo)
			{
				FString newString = FString();
				for (int32 j = 0; j < l.Strings.Num(); ++j)
				{
					newString.Append(l.Strings[j]);
					newString.Append(",");
				}
				stringArrayToExport.Add(newString);
			}

			if (FFileHelper::SaveStringArrayToFile(stringArrayToExport, *formattedFilePath))
			{
				return true;
			}
		}
	}

	return false;
}
#endif // WITH_EDITOR
