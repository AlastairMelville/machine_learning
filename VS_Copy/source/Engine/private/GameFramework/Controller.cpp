
#include "source/Engine/classes/GameFramework/Controller.h"

//static const char INVALID_KEY = '_'; // Represents an unused element in the HeldKeys array.
//static const uint8_t HELD_KEYS_LENGTH = 3;
//static uint8_t NUM_HELD_KEYS = 0; // The number of keys which are currently being held

AController::AController(const std::string& str)
	: MouseDeltaToCameraRotation(1.0f)
{
	//HeldKeys = new char[HELD_KEYS_LENGTH]{INVALID_KEY};
	HeldKeys.reserve(5);
}

void AController::Cpp_OnMouseMove(float InMouseX, float InMouseY)
{
	LastMouseMovement = Vector2D(InMouseX - MousePosition.X, InMouseY - MousePosition.Y);
	LastMoveDistance = LastMouseMovement.X + LastMouseMovement.Y;

	CameraRotation += Rotator(LastMouseMovement.X * MouseDeltaToCameraRotation.X,
		LastMouseMovement.Y * MouseDeltaToCameraRotation.Y,
		0.0);

	// Update the mouse position
	MousePosition = Vector2D(InMouseX, InMouseY);
}

void AController::Cpp_OnKeyPresssed(const char InKey)
{
	HeldKeys.insert(InKey);

	if (InKey == 'w')
	{

	}

	/*if (NUM_HELD_KEYS >= HELD_KEYS_LENGTH)
	{
		std::cout << "Warning: HeldKeys array is full" << std::endl;
		return;
	}

	for (uint8_t Index = 0; Index < HELD_KEYS_LENGTH; ++Index)
	{
		if (HeldKeys[Index] == INVALID_KEY)
		{
			HeldKeys[Index] = InKey;
			++NUM_HELD_KEYS;

			printf("Pressed %c, and placed at index %d in HeldKeys array.", InKey, Index);
			break;
		}
	}*/
}

void AController::Cpp_OnKeyReleased(const char InKey)
{
	HeldKeys.erase(InKey);

	/*if (NUM_HELD_KEYS < 1)
	{
		std::cout << "Error: No held keys have been cached." << std::endl;
		return;
	}

	for (uint8_t Index = 0; Index < HELD_KEYS_LENGTH; ++Index)
	{
		if (HeldKeys[Index] == InKey)
		{
			HeldKeys[Index] = INVALID_KEY;
			--NUM_HELD_KEYS;

			printf("Released %c, and removed from index %d in HeldKeys array.", InKey, Index);
			break;
		}
	}*/
}
