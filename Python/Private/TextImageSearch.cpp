// Fill out your copyright notice in the Description page of Project Settings.

#include "TextImageSearch.h"
#include "ImageRecognition.h"
#include "Kismet/KismetDataFormatting.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet//KismetStringLibrary.h"
#include "FileHelper.h"

#include "VictoryBPFunctionLibrary.h"


// Array is in order of alphabet
static const TArray<LetterDim> LETTER_DIM_DIFFS = TArray<LetterDim>{ LetterDim(4,-3), LetterDim(0,-3), LetterDim(4,-3) };

static const TArray<FString> CAPITAL_WORDS = TArray<FString>{ "", "", "", "", "", "", "", "", "", "", "WAR", "", "" };

static const TArray<FString> CAPITAL_LETTERS = TArray<FString>{"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};

static const int32 MAX_SPACE_SIZE = 30;

static const FString PLACEHOLDER_WORD = "PLACEHOLDER"; // Used when the model fails to find a word that matches the text image

//const static TArray<double> TempPixels = TArray<double>{ 0.521569,0.913726,0.988235,0.701961,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.945098,0.831373,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.956863,0.905882,0.576471,0.388235,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.945098,0.831373,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.627451,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.945098,0.831373,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.580392,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.109804,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.945098,0.831373,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,1.0,0.921569,0.0,0.0,0.647059,0.929412,0.988235,0.937255,0.694118,0.0,0.0,0.0,0.945098,0.792157,0.815686,0.984314,0.521569,0.0,0.0,0.0,0.0,0.803922,0.996078,0.337255,0.0,0.0,0.294118,0.992157,0.815686,0.0,0.0,0.647059,0.929412,0.988235,0.937255,0.694118,0.0,0.0,0.0,0.960784,0.815686,0.0,0.0,0.0,0.776471,0.988235,0.0,0.0,0.0,0.0,0.0,0.0,0.945098,0.831373,0.705882,0.952941,0.968628,0.82353,0.270588,0.0,0.0,0.0,0.670588,0.92549,0.984314,0.92549,0.662745,0.0,0.0,0.713726,0.996078,0.345098,0.0,0.0,0.109804,0.964706,0.803922,0.0,0.0,0.647059,0.933333,0.988235,0.921569,0.588235,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.717647,0.494118,0.0,0.643137,1.0,0.803922,0.592157,0.776471,1.0,0.713726,0.0,0.0,0.945098,0.992157,0.741176,0.545098,0.207843,0.0,0.0,0.0,0.0,0.521569,1.0,0.678431,0.0,0.0,0.647059,1.0,0.556863,0.0,0.643137,1.0,0.803922,0.592157,0.776471,1.0,0.713726,0.0,0.0,0.960784,0.815686,0.0,0.0,0.0,0.776471,0.988235,0.0,0.0,0.0,0.0,0.0,0.0,0.945098,0.980392,0.8,0.572549,0.717647,0.996078,0.788235,0.0,0.0,0.67451,0.996078,0.729412,0.576471,0.815686,1.0,0.584314,0.0,0.384314,0.996078,0.670588,0.0,0.0,0.529412,1.0,0.54902,0.0,0.647059,1.0,0.772549,0.584314,0.831373,0.996078,0.415686,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.580392,0.0,0.05098,0.952941,0.866667,0.0,0.0,0.0,0.82353,0.976471,0.231373,0.0,0.945098,0.862745,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.933333,0.882353,0.0,0.0,0.854902,0.956863,0.086275,0.05098,0.952941,0.866667,0.0,0.0,0.0,0.82353,0.976471,0.231373,0.0,0.960784,0.815686,0.0,0.0,0.0,0.776471,0.988235,0.0,0.0,0.0,0.0,0.0,0.0,0.945098,0.866667,0.0,0.0,0.0,0.843137,0.945098,0.0,0.0,0.643137,0.686275,0.0,0.0,0.086275,0.968628,0.803922,0.0,0.0,0.890196,0.866667,0.0,0.0,0.768628,0.956863,0.109804,0.086275,0.956863,0.854902,0.0,0.0,0.14902,0.960784,0.768628,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.580392,0.0,0.423529,1.0,0.666667,0.0,0.0,0.0,0.596078,1.0,0.529412,0.0,0.945098,0.831373,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.721569,0.996078,0.337255,0.25098,0.988235,0.768628,0.0,0.423529,1.0,0.666667,0.0,0.0,0.0,0.596078,1.0,0.529412,0.0,0.960784,0.815686,0.0,0.0,0.0,0.776471,0.988235,0.0,0.0,0.0,0.0,0.0,0.0,0.945098,0.831373,0.0,0.0,0.0,0.760784,0.996078,0.0,0.0,0.0,0.086275,0.4,0.521569,0.537255,0.952941,0.847059,0.0,0.0,0.678431,0.992157,0.270588,0.0,0.933333,0.780392,0.0,0.439216,1.0,0.870588,0.737255,0.737255,0.737255,0.952941,0.858824,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.580392,0.0,0.509804,1.0,0.596078,0.0,0.0,0.0,0.509804,1.0,0.596078,0.0,0.945098,0.831373,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.372549,0.996078,0.678431,0.627451,1.0,0.47451,0.0,0.509804,1.0,0.596078,0.0,0.0,0.0,0.509804,1.0,0.596078,0.0,0.960784,0.815686,0.0,0.0,0.0,0.776471,0.988235,0.0,0.0,0.0,0.0,0.0,0.0,0.945098,0.831373,0.0,0.0,0.0,0.756863,1.0,0.0,0.0,0.529412,0.929412,1.0,0.984314,0.980392,0.996078,0.847059,0.0,0.0,0.309804,0.992157,0.627451,0.443137,1.0,0.509804,0.0,0.513726,1.0,0.933333,0.898039,0.898039,0.898039,0.898039,0.796079,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.580392,0.0,0.427451,1.0,0.666667,0.0,0.0,0.0,0.592157,1.0,0.529412,0.0,0.945098,0.831373,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.870588,0.87451,0.839216,0.92549,0.0,0.0,0.427451,1.0,0.666667,0.0,0.0,0.0,0.592157,1.0,0.529412,0.0,0.952941,0.82353,0.0,0.0,0.0,0.776471,0.988235,0.0,0.0,0.0,0.0,0.0,0.0,0.945098,0.831373,0.0,0.0,0.0,0.756863,1.0,0.0,0.133333,0.968628,0.917647,0.415686,0.0,0.0,0.937255,0.847059,0.0,0.0,0.0,0.862745,0.835294,0.717647,0.945098,0.0,0.0,0.443137,1.0,0.682353,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.580392,0.0,0.086275,0.956863,0.862745,0.0,0.0,0.0,0.819608,0.980392,0.231373,0.0,0.945098,0.831373,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.627451,0.988235,0.980392,0.717647,0.0,0.0,0.086275,0.956863,0.862745,0.0,0.0,0.0,0.819608,0.980392,0.231373,0.0,0.901961,0.890196,0.0,0.0,0.0,0.835294,0.988235,0.0,0.0,0.0,0.0,0.0,0.0,0.945098,0.831373,0.0,0.0,0.0,0.756863,1.0,0.0,0.388235,1.0,0.682353,0.0,0.0,0.278431,0.972549,0.847059,0.0,0.0,0.0,0.639216,0.976471,0.894118,0.756863,0.0,0.0,0.109804,0.964706,0.858824,0.0,0.0,0.0,0.05098,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.580392,0.0,0.0,0.654902,1.0,0.8,0.588235,0.768628,1.0,0.721569,0.0,0.0,0.945098,0.831373,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.207843,0.972549,0.996078,0.376471,0.0,0.0,0.0,0.654902,1.0,0.8,0.588235,0.768628,1.0,0.721569,0.0,0.0,0.713726,0.996078,0.713726,0.572549,0.780392,0.968628,0.988235,0.0,0.0,0.0,0.0,0.0,0.0,0.945098,0.831373,0.0,0.0,0.0,0.756863,1.0,0.0,0.133333,0.960784,0.905882,0.580392,0.647059,0.921569,0.988235,0.854902,0.0,0.0,0.0,0.239216,0.984314,1.0,0.470588,0.0,0.0,0.0,0.682353,1.0,0.796079,0.580392,0.654902,0.894118,0.623529,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,1.0,0.580392,0.0,0.0,0.0,0.654902,0.929412,0.992157,0.941177,0.698039,0.0,0.0,0.0,0.945098,0.831373,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.890196,0.886275,0.0,0.0,0.0,0.0,0.0,0.654902,0.929412,0.992157,0.941177,0.698039,0.0,0.0,0.0,0.14902,0.760784,0.960784,0.972549,0.780392,0.713726,0.988235,0.0,0.0,0.0,0.0,0.0,0.0,0.945098,0.831373,0.0,0.0,0.0,0.756863,1.0,0.0,0.0,0.537255,0.913726,0.984314,0.92549,0.619608,0.894118,0.909804,0.0,0.0,0.0,0.0,0.835294,0.92549,0.0,0.0,0.0,0.0,0.0,0.666667,0.929412,0.992157,0.960784,0.807843,0.352941,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.447059,0.996078,0.658824,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.14902,0.639216,0.905882,0.952941,0.219608,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0 };

bool UTextImageSearch::FindNextWordStart(bool bIsCapitalLetter, const float& greyscaleSearchColour, const FVector2D& startLocation, const FVector2D& endLocation, const TArray<FDoubleArray>& pixelMatrix,
	const TArray<FDoubleArray>& weightsMatrix, FVector2D& OUT_pixelLocation, int32& OUT_pixelHeight, int8& bNewSentence)
{
	uint32 numRows = pixelMatrix.Num();
	uint32 numCols = pixelMatrix[0].Doubles.Num();
	//uint32 startingIndex = bIsCapitalLetter ? 0 : 7;
	int32 startingColumn = startLocation.Y;
	uint32 maxRow = MAX_int32;
	if (endLocation != FVector2D::ZeroVector)
	{
		maxRow = endLocation.X;
	}

	for (uint32 rowIdx = startLocation.X; rowIdx < numRows; ++rowIdx)
	{
		if (bNewSentence < 1 && rowIdx > maxRow)
		{
			bNewSentence = 1;
		}

		for (uint32 colIdx = startingColumn; colIdx < numCols; ++colIdx)
		{
			//uint32 pixelIdx = (rowIdx * colIdx) + colIdx;
			// Ensure that a letter will fit at the pixel location you are currently at
			/*if (colIdx >= 0 && (colIdx + letterDimensions.X) < numCols
				&& rowIdx >= 0 && (rowIdx + letterDimensions.Y) < numRows)*/
			{
				// Compare each surrounding pixels within the above shape to the shapes values
				//double diff = 0.0;
				double pixelValue = pixelMatrix[rowIdx].Doubles[colIdx];
				if (FMath::IsWithinInclusive<float>(pixelValue, (greyscaleSearchColour - 0.5f), (greyscaleSearchColour + 0.5f)) /*pixelValue > 0.45f*/ ) //diff < 0.05)
				{
					// Check if we missed any pixels (e.g. the parts sticking out from an f, y, etc.)
					int32 newColIndex = colIdx;
					FVector2D foundPixelLoc = FVector2D::ZeroVector;
					while ((newColIndex > startingColumn) &&
						CheckIfMissedPixels(greyscaleSearchColour, FVector2D(startLocation.X, startingColumn) + FVector2D(rowIdx, newColIndex - 1), 15, pixelMatrix, foundPixelLoc))
					{
						newColIndex = foundPixelLoc.Y;
					}

					// Check the font size  // TODO: Should this be below CheckIfMissedPixels() ?!
					if (GetLetterPixelHeight(greyscaleSearchColour, bIsCapitalLetter, FVector2D(rowIdx, newColIndex), pixelMatrix, OUT_pixelHeight))
					{

					}

					double resultValue = pixelMatrix[rowIdx].Doubles[newColIndex];
					uint32 originalPixelLoc = (rowIdx * numCols) + newColIndex;
					OUT_pixelLocation = /*startLocation + */FVector2D(rowIdx, newColIndex);

					return true;
				}
			}
		}

		// Ensure that only the first checked column starts at the startLocation.Y
		if (rowIdx > (startLocation.X + OUT_pixelHeight))
		{
			startingColumn = 0;
		}
	}

	return false;
}

bool UTextImageSearch::CheckIfMissedPixels(const float& greyscaleSearchColour, const FVector2D& startLocation, const int32& numRowsToCheck, const TArray<FDoubleArray>& pixelMatrix, 
	FVector2D& OUT_pixelLocation)
{
	for (int32 rowIdx = 0; rowIdx < numRowsToCheck; ++rowIdx)
	{
		double pixelValue = pixelMatrix[startLocation.X + rowIdx].Doubles[startLocation.Y];
		if ((FMath::IsWithinInclusive<float>(pixelValue, (greyscaleSearchColour - 0.5f), (greyscaleSearchColour + 0.5f))) /*pixelValue > 0.45*/)
		{
			OUT_pixelLocation = FVector2D(rowIdx, startLocation.Y);
			return true;
		}
	}

	return false;
}


bool UTextImageSearch::GetLetterPixelHeight(const float& greyscaleSearchColour, bool bIsCapitalLetter, const FVector2D& startLocation, const TArray<FDoubleArray>& pixelMatrix, int32& OUT_height)
{
	// TODO: Utilise the bIsCapitalLetter bool param

	uint8 maxNumPixels = 48; // TODO: Get the height of largest font (currenly using 36pt font size)
	uint8 width = 10;

	for (uint8 heightIdx = 0; heightIdx < maxNumPixels; ++heightIdx)
	{
		int8 numNonBlankPixels = width;
		for (uint8 widthIdx = 0; widthIdx < width; ++widthIdx)
		{
			double pixelValue = pixelMatrix[startLocation.X + heightIdx].Doubles[startLocation.Y + widthIdx];
			if (FMath::IsWithinInclusive<float>(pixelValue, ((1.0f - greyscaleSearchColour) - 0.1f), ((1.0f - greyscaleSearchColour) + 0.1f)) /*pixelValue < 0.05*/)
			{
				--numNonBlankPixels;
			}
		}

		if (numNonBlankPixels < 1)
		{
			// Return how many pixels we had to travel downwards to get the height (TODO: This won't work for e.g. p, q, etc.)
			OUT_height = heightIdx;
			return true;
		}
	}

	return false;
}

bool UTextImageSearch::DetermineFirstLetter(const FVector2D& wordStartLoc, const int32& fontSize, const TArray<FDoubleArray>& pixelMatrix, 
	FString& OUT_letter, bool& OUT_bIsCapital)
{
	bool bSearchingForCapital = OUT_bIsCapital; // Determines whether we look for a lower or upper case letter first.

	TArray<double> relevantPixels{};
	if (UTextImageSearch::GetPixelsForWordPrediction(pixelMatrix, wordStartLoc, wordStartLoc + FVector2D(/*fontSize*/ 13, 15), relevantPixels))
	{
		if (ATextImageSearchActor::ExportPixels("TextImageData/TestPixels.txt", relevantPixels, 0, 16)) // Here for debugging
		{
		}
		for (int8 index = 0; index < 2; ++index) // go through each state of the bool
		{
			FString fullFilepath = (bSearchingForCapital) ? FPaths::ProjectSavedDir() + "TextImageData/Words/Searching/SearchingForCapitalLetter.txt" : 
				FPaths::ProjectSavedDir() + "TextImageData/Words/Searching/SearchingForLowerCaseLetter.txt";

			TArray<FDoubleArray> pixelWeights{};
			if (UTextImageSearch::ExtractPixelWeightsFromFile(fullFilepath, pixelWeights))
			{
				TArray<double> pixelBiases = (bSearchingForCapital) ? 
					TArray<double>{ 0.02491502, -0.20708296,  0.0318209 , -0.06084771, -0.07906058,
					0.07150341, -0.05531294, -0.00117204,  0.30824038,  0.12453611,
					-0.05693191,  0.16464549, -0.05825716, -0.06528224, -0.01892825,
					-0.01788369, -0.05361281, -0.10757   , -0.0403848 ,  0.1295585 ,
					-0.04010379,  0.01567343, -0.07809288,  0.00058107,  0.07302094,
					-0.00397172 } // For CAPITAL LETTERS
				: TArray<double>{ -0.08792884, -0.10550601,  0.02928731, -0.03246943, -0.10050617,
					-0.03726508, -0.14807785, -0.05681005,  0.27337125,  0.45962498,
					-0.10069345,  0.14578685, -0.07586065, -0.0180496 , -0.03319618,
					-0.20779581, -0.10289611,  0.19489726, -0.02473318,  0.0892993 ,
					-0.02067364,  0.07983749, -0.07833716, -0.00708841, -0.01487055,
					-0.0193461 }; // For lower-case letters

				FPredictionCallback firstLetterPredictionCallback = UImageRecognition::Predict(relevantPixels, pixelWeights, pixelBiases, TArray<int32>());
				int8 predictIndex = firstLetterPredictionCallback.Prediction;
				if (predictIndex > INDEX_NONE)
				{
					OUT_bIsCapital = bSearchingForCapital;
					OUT_letter = (bSearchingForCapital) ? CAPITAL_LETTERS[predictIndex] : CAPITAL_LETTERS[predictIndex].ToLower();
					return true;
				}
			
				// If we fail to find a match, check the oppossing letter case
				bSearchingForCapital = !bSearchingForCapital;
			}
		}
	}

	return false;
}

bool UTextImageSearch::FindNextSpace(const float& greyscaleSearchColour, bool bIsCapitalLetter, const FVector2D& startLocation, const int32& desiredMinSpaceSize, const int32& maxSpaceSize, const TArray<FDoubleArray>& pixelMatrix, FVector2D& OUT_pixelLocation)
{
	int32 baseMinimumSpaceLength = 3, baseMaximumSpaceLength = 5;
	int32 fontSize = 10; // the font size that the model found a match with - eg if it cannot find any macthes at size 8, it will try 9, and then 10 etc.
	int32 minimumSpaceLength = desiredMinSpaceSize;// (fontSize * baseMinimumSpaceLength);
	//uint32 maximumSpaceLength = 10;// (fontSize * baseMaximumSpaceLength);
	int32 estimatedSpaceLength = 6; // estimated and modified at runtime
	int32 numPixelsTogether = 0;

	int32 numRemainingCols = pixelMatrix[0].Doubles.Num() - startLocation.Y;
	for (int32 yIdx = 0; yIdx < /*110*/ numRemainingCols; ++yIdx)
	{
		int32 currentRow = bIsCapitalLetter ? startLocation.X : startLocation.X + 6; // 6 is half of the text's height
		int32 currentIndex = startLocation.Y + yIdx;
		if (pixelMatrix[currentRow].Doubles.IsValidIndex(currentIndex))
		{
			double pixelVal = pixelMatrix[currentRow].Doubles[currentIndex];
			if (FMath::IsWithinInclusive<float>(pixelVal, ((1.0f - greyscaleSearchColour) - 0.1f), ((1.0f - greyscaleSearchColour) + 0.1f)) /*pixelVal < 0.1*/
				&& numPixelsTogether < maxSpaceSize) // will this break it - should it be -1 (not - 2)
			{
				++numPixelsTogether;
			}
			else
			{
				if (numPixelsTogether >= minimumSpaceLength) // the -2 is to allow tolerence
				{
					// Can confirm that it is a space by searching the rectangle between the two tracked non-blank pixels
					int32 numNeeded = minimumSpaceLength; // how many 
					int32 currentNumErrors = 0;
					bool bFoundSpace = false;
					//if (numPixelsTogether >= 6)//FMath::IsWithinInclusive(numPixelsTogether, minimumSpaceLength, maximumSpaceLength))
					{
						int32 potentialSpaceStart = currentIndex - numPixelsTogether;
						int32 actualSpaceStart = potentialSpaceStart;
						int32 lastInvalidPixel = potentialSpaceStart - 1;
						int32 currentGap = potentialSpaceStart;
						for (int32 colIdx = 0; colIdx < numPixelsTogether; ++colIdx)
						{
							currentGap = ((potentialSpaceStart + colIdx) - lastInvalidPixel) - 1;
							if (currentGap == minimumSpaceLength)
							{
								// You have found your space
								actualSpaceStart = (potentialSpaceStart + colIdx) - minimumSpaceLength;
								bFoundSpace = true;
								OUT_pixelLocation = FVector2D(startLocation.X, actualSpaceStart);
								return true;
							}

							for (int32 xIdx = 0; xIdx < 12; ++xIdx)
							{
								double pixelVal = pixelMatrix[startLocation.X + xIdx].Doubles[potentialSpaceStart + colIdx];
								if (FMath::IsWithinInclusive<float>(pixelVal, greyscaleSearchColour - 0.8f, greyscaleSearchColour + 0.8f) /*pixelVal > 0.2*/) // using 0.2 so as to avoid tripping up on single weak pixels around the edges of letters
								{
									++currentNumErrors;
																		
									//else
									{
										lastInvalidPixel = potentialSpaceStart + colIdx;
									}
									break;
								}
							}

							if (currentNumErrors > (numPixelsTogether - numNeeded))
							{
								break;
							}
						}
					}
				}

				numPixelsTogether = 0;
			}
		}
	}

	ensure(true);
	return false;
}

bool UTextImageSearch::FindFileLocationsForPotentialWords(const FString& startingLetter, const bool& bIsCapital, const uint8& wordLength, FString& OUT_file)
{
	FString letterCaseExtension = (bIsCapital) ? "WithCapitals/" : "LowerCase/";
	FString filepathStart = FPaths::ProjectSavedDir() + "TextImageData/Words/" + letterCaseExtension;
	FString wordLengthString = UKismetStringLibrary::Conv_IntToString(wordLength);
	FString filepathEnd = startingLetter + "/" + wordLengthString + "/";
	FString filename = startingLetter + wordLengthString + ".txt";

	FString fullFilepath = filepathStart + filepathEnd + filename;
	if (FPaths::FileExists(fullFilepath))
	{
		OUT_file = fullFilepath;
		return true;
	}

	return false;
}

bool UTextImageSearch::ExtractPixelWeightsFromFile(const FString& fullFilepath, TArray<FDoubleArray>& OUT_pixelWeights)
{
	TArray<FStringArray> dummyStringArray{};
	if (UKismetDataFormatting::GetFormattedWordVectorData(fullFilepath, OUT_pixelWeights, dummyStringArray))
	{
		return true;
	}

	return false;
}

float UTextImageSearch::Lerp(float s, float e, float t)
{
	return s + (e - s) * t;
}

float UTextImageSearch::Blerp(float c00, float c10, float c01, float c11, float tx, float ty)
{
	return Lerp(Lerp(c00, c10, tx), Lerp(c01, c11, tx), ty);
}

void UTextImageSearch::ResizePixelMatrix(TArray<FDoubleArray>& pixelWeights, float scaleX, float scaleY)
{
	// Create new pixel matrix dimensions
	int32 oldWidth = pixelWeights[0].Doubles.Num();
	int32 oldHeight = pixelWeights.Num();
	int32 newWidth = (int32)(oldWidth * scaleX);
	int32 newHeight = (int32)(oldHeight * scaleY);

	TArray<FDoubleArray> newPixelWeightMatrix{};
	TArray<double> stdPixels{};
	stdPixels.Init(0.0, newWidth);
	newPixelWeightMatrix.Init(FDoubleArray(stdPixels), newHeight);

	// 
	for (int32 x = 0; x < newWidth; ++x)
	{
		for (int32 y = 0; y < newHeight; ++y)
		{
			float gx = ((float)x) / newWidth * (oldWidth - 1);
			float gy = ((float)y) / newHeight * (oldHeight - 1);
			int32 ordIdxX = (int32)gx;
			int32 ordIdxY = (int32)gy;
			float c00 = pixelWeights[ordIdxY].Doubles[ordIdxX];
			float c10 = pixelWeights[ordIdxY + 1].Doubles[ordIdxX];
			float c01 = pixelWeights[ordIdxY].Doubles[ordIdxX + 1];
			float c11 = pixelWeights[ordIdxY + 1].Doubles[ordIdxX + 1];

			//float greyscale = FMath::Lerp<float, float>(FMath::Lerp<float, float>(), FMath::Lerp<float, float>(), );
			float greyscale = Blerp(c00, c10, c01, c11, gx - ordIdxX, gy - ordIdxY);
			newPixelWeightMatrix[y].Doubles[x] = greyscale;
		}
	}

	pixelWeights = newPixelWeightMatrix;


	/// https://stackoverflow.com/questions/15176972/bi-cubic-interpolation-algorithm-for-image-scaling

	// Relocate existing pixels



	// Assign values to new pixels

}

void UTextImageSearch::ResizePixelArray(TArray<float>& pixelWeights, int32 originalSizeX, int32 originalSizeY, float scaleX, float scaleY)
{
	// Create new pixel array dimensions
	int32 newWidth = (int32)(originalSizeX * scaleX);
	int32 newHeight = (int32)(originalSizeY * scaleY);

	TArray<float> newPixelWeightArray{};
	int32 numPixels = newHeight * newWidth;
	newPixelWeightArray.Init(0.0, numPixels);

	// 
	int32 x = 0;
	int32 y = 0;
	for (int32 pixelIdx = 0; pixelIdx < numPixels; ++pixelIdx)
	{
		x = pixelIdx % newWidth;
		if (x == (newWidth - 1) && pixelIdx < (numPixels - 10))
		{
			++y;
		}
		float gx = ((float)x) / newWidth * (originalSizeX - 1);
		float gy = ((float)y) / newHeight * (originalSizeY - 1);
		int32 ordIdxX = (int32)gx;
		int32 ordIdxY = (int32)gy;

		// Get values from the original image/pixel array
		float c00 = pixelWeights[((ordIdxY * originalSizeX) + ordIdxX)];
		float c01 = pixelWeights[((ordIdxY * originalSizeX) + (ordIdxX + 1))];
		float c10 = pixelWeights[(((ordIdxY + 1) * originalSizeX) + ordIdxX)];
		float c11 = pixelWeights[(((ordIdxY + 1) * originalSizeX) + (ordIdxX + 1))];

		//float greyscale = FMath::Lerp<float, float>(FMath::Lerp<float, float>(c00, c01, (gx - ordIdxX)), FMath::Lerp<float, float>(), );
		float greyscale = Lerp(Lerp(c00, c01, (gx - ordIdxX)), Lerp(c10, c11, (gx - ordIdxX)), (gy - ordIdxY));
		//float greyscale = Blerp(c00, c10, c01, c11, (gx - ordIdxX), (gy - ordIdxY));
		newPixelWeightArray[((y * newWidth) + x)] = greyscale;
	}

	pixelWeights = newPixelWeightArray;
}

void UTextImageSearch::ResizePixelArray(TArray<double>& pixelWeights, int32 originalSizeX, int32 originalSizeY, float scaleX, float scaleY)
{
	// Create new pixel array dimensions
	int32 newWidth = (int32)(originalSizeX * scaleX);
	int32 newHeight = (int32)(originalSizeY * scaleY);

	TArray<double> newPixelWeightArray{};
	int32 numPixels = newHeight * newWidth;
	newPixelWeightArray.Init(0.0, numPixels);

	// 
	int32 x = 0;
	int32 y = 0;
	for (int32 pixelIdx = 0; pixelIdx < numPixels; ++pixelIdx)
	{
		x = pixelIdx % newWidth;
		if (x == (newWidth - 1) && pixelIdx < (numPixels - 10))
		{
			++y;
		}
		float gx = ((float)x) / newWidth * (originalSizeX - 1);
		float gy = ((float)y) / newHeight * (originalSizeY - 1);
		int32 ordIdxX = (int32)gx;
		int32 ordIdxY = (int32)gy;
		
		// Get values from the original image/pixel array
		float c00 = pixelWeights[((ordIdxY * originalSizeX) + ordIdxX)];
		float c01 = pixelWeights[((ordIdxY * originalSizeX) + (ordIdxX + 1))];
		float c10 = pixelWeights[(((ordIdxY + 1) * originalSizeX) + ordIdxX)];
		float c11 = pixelWeights[(((ordIdxY + 1) * originalSizeX) + (ordIdxX + 1))];

		//float greyscale = FMath::Lerp<float, float>(FMath::Lerp<float, float>(c00, c01, (gx - ordIdxX)), FMath::Lerp<float, float>(), );
		float greyscale = Lerp(Lerp(c00, c01, (gx - ordIdxX)), Lerp(c10, c11, (gx - ordIdxX)), (gy - ordIdxY));
		//float greyscale = Blerp(c00, c10, c01, c11, (gx - ordIdxX), (gy - ordIdxY));
		newPixelWeightArray[((y * newWidth) + x)] = greyscale;
	}

	pixelWeights = newPixelWeightArray;
}

void UTextImageSearch::ResizePixelArray(TArray<FColor>& pixelWeights, int32 originalSizeX, int32 originalSizeY, int32 newWidth, int32 newHeight)
{
	TArray<FColor> newPixelWeightArray{};
	int32 numPixels = newHeight * newWidth;
	newPixelWeightArray.Init(FColor(), numPixels);

	// 
	int32 x = 0;
	int32 y = 0;
	for (int32 pixelIdx = 0; pixelIdx < numPixels; ++pixelIdx)
	{
		x = pixelIdx % newWidth;
		if (x == (newWidth - 1) && pixelIdx < (numPixels - 10))
		{
			++y;
		}
		float gx = ((float)x) / newWidth * (originalSizeX - 1);
		float gy = ((float)y) / newHeight * (originalSizeY - 1);
		int32 ordIdxX = (int32)gx;
		int32 ordIdxY = (int32)gy;

		// Get values from the original image/pixel array
		FColor c00 = pixelWeights[((ordIdxY * originalSizeX) + ordIdxX)];
		FColor c01 = pixelWeights[((ordIdxY * originalSizeX) + (ordIdxX + 1))];
		FColor c10 = pixelWeights[(((ordIdxY + 1) * originalSizeX) + ordIdxX)];
		FColor c11 = pixelWeights[(((ordIdxY + 1) * originalSizeX) + (ordIdxX + 1))];

		float r = Lerp(Lerp(c00.R, c01.R, (gx - ordIdxX)), Lerp(c10.R, c11.R, (gx - ordIdxX)), (gy - ordIdxY));
		float g = Lerp(Lerp(c00.G, c01.G, (gx - ordIdxX)), Lerp(c10.G, c11.G, (gx - ordIdxX)), (gy - ordIdxY));
		float b = Lerp(Lerp(c00.B, c01.B, (gx - ordIdxX)), Lerp(c10.B, c11.B, (gx - ordIdxX)), (gy - ordIdxY));

		newPixelWeightArray[((y * newWidth) + x)] = FColor(r, g, b);
	}

	pixelWeights = newPixelWeightArray;
}

bool UTextImageSearch::GetPixelsForWordPrediction(const TArray<FDoubleArray>& pixelMatrix, const FVector2D& startLocation, const FVector2D& endLocation,
	TArray<double>& OUT_pixelArray, const int32& fixedColLen)
{
	int32 tempExtraCols = 0;
	if (fixedColLen > 0)
	{
		tempExtraCols = fixedColLen /*89*/ /*15*/ - (endLocation.Y - startLocation.Y); // Used when capturing pixels, to ensure that dimensions are the same for all features
	}

	int32 amountRows = (endLocation.X /*24*/) - startLocation.X; // + 12 is temp
	int32 amountCols = (endLocation.Y - startLocation.Y) + 1;
	int32 outArrayLen = amountRows * amountCols;
	if (outArrayLen <= 0)
	{
		return false;
	}
	OUT_pixelArray.Init(0.0, outArrayLen + (amountRows * tempExtraCols));
	int32 index = 0;

	for (int32 xIdx = 0; xIdx < amountRows; ++xIdx)
	{
		for (int32 yIdx = 0; yIdx < amountCols; ++yIdx)
		{
			double pixelValue = pixelMatrix[(startLocation.X + xIdx)].Doubles[(FMath::Max(0.0f, startLocation.Y + yIdx - 1))];
			OUT_pixelArray[/*((xIdx * amountCols) + yIdx)*/ index] = pixelValue;
			
			++index;
		}
		index += tempExtraCols;
	}

	if (OUT_pixelArray.Num() > 0)
	{
		return true;
	}

	return false;
}

bool UTextImageSearch::DetectPunctuation(const float& greyscaleSearchColour, const TArray<FDoubleArray>& pixelMatrix, const FVector2D& startLocation, const FVector2D& endLocation,
	EPunctuation& OUT_punctuationFound, FVector2D& OUT_punctionStartLocation)
{
	int32 minimumFilledLength = 3;
	int32 minimumBlankLength = 1;
	int32 numFilledPixelsTogether = 0;
	int32 numBlankPixelsTogether = 0;

	for (int32 colIdx = startLocation.Y; colIdx > endLocation.Y; --colIdx)
	{
		double pixelVal = pixelMatrix[endLocation.X - 1].Doubles[colIdx];
		if (FMath::IsWithinInclusive<float>(pixelVal, greyscaleSearchColour - 0.5f, greyscaleSearchColour + 0.01f) /*pixelVal > 0.5*/
			&& numFilledPixelsTogether < minimumFilledLength)
		{
			++numFilledPixelsTogether;
		}
		else if (FMath::IsWithinInclusive<float>(pixelVal, ((1.0f - greyscaleSearchColour) - 0.2f), ((1.0f - greyscaleSearchColour) + 0.2f)) /*pixelVal < 0.2*/ 
			&& numFilledPixelsTogether >= minimumFilledLength && numBlankPixelsTogether < minimumBlankLength)
		{
			++numBlankPixelsTogether;
		}
		else if (numFilledPixelsTogether >= minimumFilledLength && numBlankPixelsTogether >= minimumBlankLength)
		{
			if (FMath::IsWithinInclusive<float>(pixelVal, (greyscaleSearchColour - 0.3f), (greyscaleSearchColour + 0.01f)) /*pixelVal > 0.7*/)
			{
				OUT_punctuationFound = EPunctuation::FullStop;
				OUT_punctionStartLocation = FVector2D(startLocation.X, colIdx + 1);
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	return false;

	const int32 numY = endLocation.Y - startLocation.Y;
	const int32 numX = endLocation.X - startLocation.X;
	int32 currentIndexY = 0;
	int32 currentIndexX = 0;

	for (int32 colIdx = 0; currentIndexY < numY; colIdx += (numY / FMath::Abs(numY)))
	{
		for (int32 rowIdx = 0; currentIndexX < numX; rowIdx += (numX / FMath::Abs(numX)))
		{			

			++currentIndexX;
		}
		++currentIndexY;
		currentIndexX = 0;
	}

	return false;
}

void UTextImageSearch::RemovePixelWeights(const FString& shortFilepath, TArray<FString>& pixelWeights, const FVector2D& originalPixelDimensions, 
	const int32& colsToTrim, const int32& rowsToTrim)
{
	if (!pixelWeights.Num())
	{
		const FString fullFilePath = FPaths::ProjectSavedDir() + shortFilepath;
		if (FPaths::FileExists(fullFilePath))
		{
			FFileHelper::LoadFileToStringArray(pixelWeights, *fullFilePath);
		}
	}

	const int32 numPixels = pixelWeights.Num();

	// Remove rows //
	if (rowsToTrim > 0)
	{
		const int32 pixelsToTrim = rowsToTrim * originalPixelDimensions.Y;
		const int32 indexToRemoveFrom = numPixels - pixelsToTrim;
		pixelWeights.RemoveAt(indexToRemoveFrom, pixelsToTrim);
	}

	// Remove columns //
	if (colsToTrim > 0)
	{
		const int32 newRowLen = originalPixelDimensions.Y - colsToTrim;

		// Start from the end of the array
		const int32 lastIndex = numPixels - 1;

		// For each pixel in a column
		for (int32 pixelIdx = lastIndex; pixelIdx < originalPixelDimensions.X; pixelIdx -= originalPixelDimensions.Y)
		{
			const int32 indexToRemoveFrom = pixelIdx;
			pixelWeights.RemoveAt(indexToRemoveFrom, colsToTrim);
		}
	}
}

void UTextImageSearch::SavePixelsToFile(const TArray<FDoubleArray>& pixelMatrix)
{
	FString fullString;
	uint32 numRows = pixelMatrix.Num();
	uint32 numCols = pixelMatrix[0].Doubles.Num();
	for (uint32 rowIdx = 0; rowIdx < numRows; ++rowIdx)
	{
		for (uint32 colIdx = 0; colIdx < numCols; ++colIdx)
		{
			double normalisedPixel = pixelMatrix[rowIdx].Doubles[colIdx];
			FString newConv = UKismetStringLibrary::Conv_FloatToString(normalisedPixel);

			// Don't add a comma on the last array element
			//if (colIdx < (numCols - 1))
			{
				newConv += ",";
			}

			fullString += newConv;
		}
	}

	FString filenameOne = FPaths::ProjectSavedDir() + "TextImageData\\GreyscalePixels.txt";
	if (FPaths::FileExists(filenameOne) && fullString.Len())
	{
		FFileHelper::SaveStringToFile(fullString, *filenameOne);// , FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
	}
}

TArray<FString> ATextImageSearchActor::BuildSentenceFromImages(TArray<FColor>& pixels, const FVector2D& imageDims, UTextureRenderTarget2D* renderTexture, bool bIsCapitalLetter, const FColor searchColour, const FBox2D bounds)
{
	///if (renderTexture)
	{
		///TArray<FColor> pixels{};
		///check(IsInGameThread());
		///FTextureRenderTargetResource* res = static_cast<FTextureRenderTargetResource*>(renderTexture->Resource);
		///if (res)
		{
			float greyscaleSearchColour = ((UImageRecognition::ConvertPixelToGreyscale(FLinearColor::Transparent, searchColour)) * (1.0f / 255.0f));
			// Load weights in white or black pixels depending on what shade we are looking for
			///res->ReadPixels(pixels);
					

			
			/*for (int32 index = 0; index < pixels.Num(); ++index)
			{
				FColor oldColor = pixels[index];
				pixels[index] = FColor(255 - oldColor.R, 255 - oldColor.G, 255 - oldColor.B);
			}*/

			// Convert to greyscale and normalise
			uint32 numPixels = pixels.Num();
			TArray<float> greyscalePixels = UImageRecognition::ConvertPixelArrayToGreyscale(TArray<FLinearColor>(), pixels);
			TArray<double> normalisedGreyscalePixels{};
			normalisedGreyscalePixels.Init(0.0f, numPixels);

			for (uint32 pixelIdx = 0; pixelIdx < numPixels; ++pixelIdx)
			{
				float normalisedPixel = /*FMath::Clamp(1.0f - (*/greyscalePixels[pixelIdx] * (1.0f / 255.0f)/*), 0.0f, 1.0f)*/;
				normalisedGreyscalePixels[pixelIdx] = normalisedPixel;
			}

			TArray<FDoubleArray> pixelMatrix = UObjectRecognitionBase::ReshapePixelArray(normalisedGreyscalePixels, FVector2D(imageDims.X, imageDims.Y));// FVector2D(res->GetSizeX(), res->GetSizeY()));

			TArray<FString> sentences{};
			FString wordToCarryToNextSentence;
			int8 bFoundSentenceEnd = INDEX_NONE;
			bool bCarryWordOver = false;
			//bool bFoundTextSectionEnd = false;
			bool bReachedTextEnd = false;
			
			FVector2D startLocation = FVector2D(0, 0);
			int32 previousWordHeight = INT_MAX; // TODO: Should this be 0?
			while (!bReachedTextEnd || !wordToCarryToNextSentence.IsEmpty())
			{
				// Until you find a full stop, continue to add to the current sentence
				FString sentence;

				// Add carried over word, if there is one
				if (!wordToCarryToNextSentence.IsEmpty())
				{
					sentence += wordToCarryToNextSentence;
					wordToCarryToNextSentence = "";

					if (bReachedTextEnd)
					{
						// If we only came back into the loop to do this, then exit the loop now.
						sentences.Add(sentence);
						break;
					}
				}

				TArray<FPredictionCallback> predictionCallbacks{};

				while (bFoundSentenceEnd == INDEX_NONE)
				{					
					FPredictionCallback predictionCallback = FetchWordFromImage(pixelMatrix, greyscaleSearchColour, startLocation, previousWordHeight, bounds,
						sentence.IsEmpty(), bFoundSentenceEnd, bReachedTextEnd);
					predictionCallbacks.Add(predictionCallback);

					const int32 predIndex = predictionCallback.Prediction;
					if (bFoundSentenceEnd != 1)
					{
						if (predIndex > INDEX_NONE)
						{
							sentence += " " + CAPITAL_WORDS[predIndex];
						}
						else
						{
							sentence += " " + PLACEHOLDER_WORD;
						}
					}
					else
					{						
						if (predIndex > INDEX_NONE)
						{
							wordToCarryToNextSentence = CAPITAL_WORDS[predIndex];
						}
						else
						{
							wordToCarryToNextSentence = PLACEHOLDER_WORD;
						}
					}
				}

				sentences.Add(sentence);

				if (!bReachedTextEnd)
				{
					// Start a new sentence
					bFoundSentenceEnd = INDEX_NONE;
				}
			}
			
			/*for (int32 index = 0; index < predictionCallbacks.Num(); ++index)
			{
				int32 predictedIndex = predictionCallbacks[index].Prediction;
				if (CAPITAL_WORDS.IsValidIndex(predictedIndex))
				{
					sentence += (CAPITAL_WORDS[predictedIndex] + " ");
				}
				else
				{

				}
			}*/

			for (const FString& word : sentences)
			{
				UE_LOG(LogScript, Warning, TEXT(""), *word);
			}

			return sentences;
		}
	}

	return TArray<FString>{};
}

FPredictionCallback ATextImageSearchActor::FetchWordFromImage(const TArray<FDoubleArray>& pixelMatrix, const float& greyscaleSearchColour, FVector2D& startLocation, int32& previousWordHeight,
	const FBox2D& bounds, bool bIsCapitalLetter, int8& OUT_foundSentenceEnd, bool& OUT_reachedTextEnd)
{
	FPredictionCallback predictionCallback;

	int8 fontSize = 12;
	int32 pixelHeight = previousWordHeight;
	FVector2D wordStartLoc = FVector2D::ZeroVector;
	float wordHeightPlusGap = previousWordHeight * 1.75f;
	if (UTextImageSearch::FindNextWordStart(bIsCapitalLetter, greyscaleSearchColour, startLocation, FVector2D((startLocation.X + wordHeightPlusGap), MAX_int32), pixelMatrix,
		TArray<FDoubleArray>(), wordStartLoc, pixelHeight, OUT_foundSentenceEnd))
	{
		// If a valid bounds is present, check that the word is within the bounds (speech bubble, close-caption section etc.)
		if (bounds.bIsValid && !bounds.IsInside(wordStartLoc))
		{
			return predictionCallback;
		}


		previousWordHeight = pixelHeight;
		pixelHeight = 12; // TEMP (for capital letter recognition)

		FString foundLetter;
		if (UTextImageSearch::DetermineFirstLetter(wordStartLoc, 13, pixelMatrix, foundLetter, bIsCapitalLetter))
		{
		}		

		// Predict first letter using the start location and pixel height
		// TODO: Go through the weights of each possible letter, starting with lower or upper case depending on bIsCapitalLetter
		/* TODO: Once you identify what letter and case it is, calculate the font size using LETTER_DIM_DIFFS for the offsets e.g. if letter's 
		   StartDiff is -3, then make the start location 3 pixels higher than it is not. 
		   Note: only do this part for lower case letters, as all capitals are the same height*/
		if (!bIsCapitalLetter)
		{
			// All capital letters will be the same size, so you can skip over the extra computations
			fontSize = pixelHeight;


		}
		
		FVector2D wordEndLoc = FVector2D::ZeroVector;
		if (UTextImageSearch::FindNextSpace(greyscaleSearchColour, bIsCapitalLetter, wordStartLoc, 4, MAX_SPACE_SIZE, pixelMatrix, wordEndLoc))
		{
			// Set it so that on the next loop we start looking at the pixels after this word
			startLocation = FVector2D(wordStartLoc.X, wordEndLoc.Y);

			wordEndLoc = FVector2D(wordEndLoc.X + pixelHeight, wordEndLoc.Y); // Factor in the calculated pixel height

			// If a valid bounds is present, check that the word is within the bounds (speech bubble, close-caption section etc.)
			if (bounds.bIsValid && !bounds.IsInside(wordEndLoc))
			{
				return predictionCallback;
			}

			// Attempt to find punctuation
			FVector2D tempPunctuationStartLoc = wordEndLoc; // Used to adjust the words end location, so it can be correctly predicted
			EPunctuation punctuationFound;
			if (UTextImageSearch::DetectPunctuation(greyscaleSearchColour, pixelMatrix, wordEndLoc, wordEndLoc - FVector2D(0, 10), punctuationFound, tempPunctuationStartLoc))
			{
				OUT_foundSentenceEnd = 0;
				wordEndLoc = tempPunctuationStartLoc; // Remove the full stop
			}
			
			//float tempLastCol = /*59.0f*/ 89.0f /*119.0f*/ + wordStartLoc.Y; // TEMP
			//wordEndLoc = FVector2D(wordEndLoc.X, tempLastCol); // TEMP
			uint32 wordLength = wordEndLoc.Y - wordStartLoc.Y; // wordEndLoc.Y will be calculated using the font size
			TArray<double> relevantPixels{};
			if (UTextImageSearch::GetPixelsForWordPrediction(pixelMatrix, wordStartLoc, wordEndLoc, relevantPixels, 89))
			{
				if (ExportPixels("TextImageData/TestPixels.txt", relevantPixels, 0, wordLength)) // Here for debugging
				{}

				FString fullFilepath;
				if (UTextImageSearch::FindFileLocationsForPotentialWords(foundLetter, bIsCapitalLetter, /*89*/ wordLength, fullFilepath))
				{
					TArray<FDoubleArray> pixelWeights{};
					if (UTextImageSearch::ExtractPixelWeightsFromFile(fullFilepath, pixelWeights))
					{
						/*UTextImageSearch::ResizePixelArray(relevantPixels, 120, 24, 0.75f, 0.5f);
						if (ExportPixels("TextImageData/ExportedPixels.txt", relevantPixels, 0))
						{}*/

						/*TArray<double> pixelBiases{ 0.0321309 , -0.00217578,  0.01309562,  0.0009527 , -0.00885963,
							0.02076684,  0.00251284, -0.00359073, -0.00745964, -0.00726352,
							-0.00316341, -0.00270933, -0.00323592, -0.00930695,  0.00606052,
							-0.00357082,  0.00034515, -0.00757281, -0.00783241, -0.00912345 };*/

						TArray<double> pixelBiases{ 0.02491502, -0.20708296,  0.0318209 , -0.06084771, -0.07906058,
							0.07150341, -0.05531294, -0.00117204,  0.30824038,  0.12453611,
							-0.05693191,  0.16464549, -0.05825716, -0.06528224, -0.01892825,
							-0.01788369, -0.05361281, -0.10757   , -0.0403848 ,  0.1295585 ,
							-0.04010379,  0.01567343, -0.07809288,  0.00058107,  0.07302094,
							-0.00397172 }; // For CAPITAL LETTERS 

						predictionCallback = UImageRecognition::Predict(relevantPixels, pixelWeights, pixelBiases, TArray<int32>());
						return predictionCallback;
					}
				}
			}
		}
	}
	else
	{
		OUT_reachedTextEnd = true;
	}
		
	return FPredictionCallback();
}

bool ATextImageSearchActor::CapturePixels(const FString& filepath, bool bIsCapital, const int32& wordIndex)
{
	if (RenderTexture)
	{
		TArray<FColor> pixels{};
		check(IsInGameThread());
		FTextureRenderTargetResource* res = static_cast<FTextureRenderTargetResource*>(RenderTexture->Resource);
		if (res)
		{
			res->ReadPixels(pixels);
			
			// Convert to greyscale and normalise
			uint32 numPixels = pixels.Num();
			TArray<float> greyscalePixels = UImageRecognition::ConvertPixelArrayToGreyscale(TArray<FLinearColor>(), pixels);
			TArray<double> normalisedGreyscalePixels{};
			normalisedGreyscalePixels.Init(0.0f, numPixels);

			for (uint32 pixelIdx = 0; pixelIdx < numPixels; ++pixelIdx)
			{
				float normalisedPixel = greyscalePixels[pixelIdx] * (1.0f / 255.0f);
				normalisedGreyscalePixels[pixelIdx] = normalisedPixel;
			}

			TArray<FDoubleArray> pixelMatrix = UObjectRecognitionBase::ReshapePixelArray(normalisedGreyscalePixels, FVector2D(res->GetSizeX(), res->GetSizeY()));
			FVector2D wordStartLoc = FVector2D::ZeroVector;
			int32 pixelHeight = 0;
			int8 foundSentenceEnd = INDEX_NONE;
			if (UTextImageSearch::FindNextWordStart(bIsCapital, 1.0f, FVector2D(0, 0), FVector2D(MAX_int32, MAX_int32), pixelMatrix, TArray<FDoubleArray>(), 
				wordStartLoc, pixelHeight, foundSentenceEnd))
			{
				ensure(pixelHeight <= 13);
				pixelHeight = 13; // TEMP
				
				//wordStartLoc = FVector2D(2, 0); // TEMP
				FVector2D wordEndLoc = FVector2D::ZeroVector;
				if (UTextImageSearch::FindNextSpace(1.0f, bIsCapital, wordStartLoc, 4, MAX_SPACE_SIZE, pixelMatrix, wordEndLoc))
				{
					wordEndLoc = FVector2D(wordEndLoc.X + pixelHeight, wordEndLoc.Y); // Factor in the calculated pixel height
					uint32 cachedWordLength = wordEndLoc.Y - wordStartLoc.Y;

					//float tempLastCol = /*59.0f*/ 89.0f + wordStartLoc.Y;
					//wordEndLoc = FVector2D(wordEndLoc.X, tempLastCol); // TEMP
					TArray<double> relevantPixels{};
					if (UTextImageSearch::GetPixelsForWordPrediction(pixelMatrix, wordStartLoc, wordEndLoc, relevantPixels, 89))
					{
						if (ExportPixels(filepath, relevantPixels, wordIndex, cachedWordLength))
						{

						}
					}
				}
			}
		}
	}

	return false;
}

bool ATextImageSearchActor::ExportPixels(const FString& filepath, const TArray<double>& pixelArray, const int32& wordIndex, const int32& wordLength)
{
	FString fullString;
	uint32 numPixels = pixelArray.Num();
	for (uint32 index = 0; index < numPixels; ++index)
	{
		double normalisedPixel = pixelArray[index];
		FString newConv = UKismetStringLibrary::Conv_FloatToString(normalisedPixel);
		fullString += newConv;

		// Don't add a comma on the last array element
		if (index < (numPixels - 1))
		{
			fullString += ",";
		}
		else
		{
			fullString += "\n" + UKismetStringLibrary::Conv_IntToString(wordIndex) + "\n";
			//fullString += UKismetStringLibrary::Conv_IntToString(wordLength) + "\n";
		}
	}

	FString filenameOne = FPaths::ProjectSavedDir() + filepath;
	if (FPaths::FileExists(filenameOne) && fullString.Len())
	{
		FFileHelper::SaveStringToFile(fullString, *filenameOne, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
		return true;
	}

	return false;
}

int32 ATextImageSearchActor::SplitWordWeightsIntoSeperateFiles(const FString& wordPixelFilepath, const FString& weightFilepath)
{
	FString wordPixelFullFilepath = FPaths::ProjectSavedDir() + wordPixelFilepath;
	FString weightFullFilepath = FPaths::ProjectSavedDir() + weightFilepath;
	if (FPaths::FileExists(wordPixelFullFilepath) && FPaths::FileExists(weightFullFilepath))
	{
		// 
		TArray<FString> wordPixelInfo{};
		uint8 wordIndexLineIndex = 1;
		uint8 wordLenLineIndex = 2;
		if (FFileHelper::LoadFileToStringArray(wordPixelInfo, *wordPixelFullFilepath))
		{
			// 
			TArray<int32> wordLengths{};
			wordLengths.Init(0, (wordPixelInfo.Num() / 3));

			// 
			TArray<FString> wordWeights{};
			if (FFileHelper::LoadFileToStringArray(wordWeights, *weightFullFilepath))
			{
				int32 wordPixelFileLen = wordPixelInfo.Num();
				for (int32 index = 0; index < wordPixelFileLen; ++index)
				{
					uint8 remainder = index % wordLenLineIndex;
					if (remainder == 0) // if (a multiple of the relevantLineIndex)
					{
						// 
						int32 wordIndex = UKismetStringLibrary::Conv_StringToInt(wordPixelInfo[index - 1]);
						int32 wordLen = UKismetStringLibrary::Conv_StringToInt(wordPixelInfo[index]);
						wordLengths[wordIndex] = wordLen;						
					}
				}

				// Use wordLenghts array to group all word of the same length and add them to their file at the same time
				int32 maxLengthIndex;
				int32 maxLengthValue; 
				UKismetMathLibrary::MaxOfIntArray(wordLengths, maxLengthValue, maxLengthIndex);
				for (int32 wordLen = 0; wordLen < maxLengthValue; ++wordLen)
				{
					TArray<int32> indicesWithCurrentLen{};
					// TODO: Find all array elements that have this wordLength, if any
					int32 numElements = indicesWithCurrentLen.Num();

					TArray<FStringArray> weightsMatrix{}; // final contents to go into the new file
					for (int32 elemIdx = 0; elemIdx < numElements; ++elemIdx)
					{
						// Put the word's weights in the correct file
						FString filepath;
						if (UTextImageSearch::FindFileLocationsForPotentialWords("D", true, wordLen, filepath))
						{
							// Extract the weights to carry over
							TArray<FString> filteredWordWeights{};
							ExtractWordWeights(weightFullFilepath, wordWeights, elemIdx, wordLen, filteredWordWeights);
							weightsMatrix.Add(filteredWordWeights); // Actually, may need to merge into a single string rather than making a matrix...?
						}
					}

					//// When you have collected all of the word weights group them together in their respective files
					//FString filenameOne = FPaths::ProjectSavedDir() + filepath;
					//if (FPaths::FileExists(filenameOne))
					//{
					//	FFileHelper::SaveStringToFile(fullString, *filenameOne, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_Append);
					//}
				}
			}
		}
	}

	return INDEX_NONE;
}

bool ATextImageSearchActor::ExtractWordWeights(const FString& fullFilepath, const TArray<FString>& fileContents, const int32& colIndex,
	const int32& wordLength, TArray<FString>& OUT_wordWeights)
{
	if (!fileContents.Num())
	{
		// Extract content from file path into empty fileContents

	}
	
	OUT_wordWeights.Init("0.0", wordLength); // you don't want to carry over pixel weights over row wordLength
	for (int32 pixelIdx = 0; pixelIdx < wordLength; ++pixelIdx)
	{
		TArray<FString> columns = UKismetStringLibrary::ParseIntoArray(fileContents[pixelIdx], TEXT(","));
		if (columns.IsValidIndex(colIndex))
		{			
			OUT_wordWeights[pixelIdx] = columns[colIndex];
		}
	}

	return true;
}

void ATextImageSearchActor::ImageResizeTest(float scaleX, float scaleY)
{
	TArray<double> pixelArray{ 0.0,0.0,0.0,0.596078,0.917647,0.984314,0.866667,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.45098,0.992157,1.0,0.960784,0.807843,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.701961,1.0,0.941177,0.14902,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.752941,1.0,0.882353,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.109804,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.898039,0.898039,1.0,1.0,1.0,1.0,0.647059,0.0,0.109804,0.729412,0.941177,0.992157,0.941177,0.733333,0.109804,0.0,0.388235,1.0,1.0,0.627451,0.960784,0.729412,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.82353,0.82353,0.964706,1.0,0.980392,0.921569,0.592157,0.0,0.796079,1.0,0.996078,0.94902,0.996078,1.0,0.803922,0.0,0.388235,1.0,1.0,0.992157,0.929412,0.596078,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.756863,1.0,0.882353,0.0,0.0,0.407843,0.996078,1.0,0.603922,0.0,0.592157,1.0,1.0,0.431373,0.388235,1.0,1.0,0.639216,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.756863,1.0,0.882353,0.0,0.0,0.654902,1.0,0.956863,0.0,0.0,0.0,0.952941,1.0,0.67451,0.388235,1.0,1.0,0.431373,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.756863,1.0,0.882353,0.0,0.0,0.705882,1.0,0.913726,0.0,0.0,0.0,0.905882,1.0,0.72549,0.388235,1.0,1.0,0.431373,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.756863,1.0,0.882353,0.0,0.0,0.654902,1.0,0.956863,0.0,0.0,0.0,0.94902,1.0,0.678431,0.388235,1.0,1.0,0.431373,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.756863,1.0,0.882353,0.0,0.0,0.411765,0.996078,1.0,0.588235,0.0,0.576471,1.0,1.0,0.439216,0.388235,1.0,1.0,0.431373,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.756863,1.0,0.882353,0.0,0.0,0.0,0.803922,1.0,0.996078,0.94902,0.996078,1.0,0.811765,0.0,0.388235,1.0,1.0,0.431373,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.756863,1.0,0.882353,0.0,0.0,0.0,0.133333,0.733333,0.941177,0.992157,0.945098,0.741176,0.133333,0.0,0.388235,1.0,1.0,0.431373,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0 };
	
	/*TArray<FDoubleArray> pixelValues = UObjectRecognitionBase::ReshapePixelArray(pixelArray, FVector2D(60, 15));
	UTextImageSearch::ResizePixelMatrix(pixelValues, scaleX, scaleY);
	UTextImageSearch::SavePixelsToFile(pixelValues);*/

	UTextImageSearch::ResizePixelArray(pixelArray, 60, 15, scaleX, scaleY);
	if (ExportPixels("TextImageData/ExportedPixels.txt", pixelArray, 0, 0))
	{

	}
}

void ATextImageSearchActor::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property)
	{
		if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ATextImageSearchActor, bTrimPixelWeightsInFile))
		{
			if (bTrimPixelWeightsInFile)
			{
				TArray<FString> pixelWeights{};
				UTextImageSearch::RemovePixelWeights(ShortFilePath, pixelWeights, FVector2D(12, 90), (int32)DimensionTrim.X, (int32)DimensionTrim.Y);

				FFileHelper::SaveStringArrayToFile(pixelWeights, TEXT("C:/Users/alime/Documents/UnrealProjects/EpicGamesCharacters/Saved/TextImageData/Words/D/34/D34.txt"));

				bTrimPixelWeightsInFile = false;
			}
		}
	}
}
