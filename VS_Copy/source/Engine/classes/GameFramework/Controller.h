#pragma once

#include <memory>
#include <unordered_set>

#include "Vector2D.h"
#include "source/Core/classes/Math/Vector.h"
#include "source/Core/classes/Math/Rotator.h"


class APawn;

class AController
{
public:
	AController(const std::string& str);

	~AController()
	{
		//delete [] HeldKeys;
	}

	void Cpp_OnMouseMove(float InMouseX, float InMouseY);

	void Cpp_OnKeyPresssed(const char InKey);

	void Cpp_OnKeyReleased(const char InKey);

	std::shared_ptr<APawn> GetPawn() const { return Pawn; }

private:
	Vector2D MousePosition;
	Vector2D LastMouseMovement;
	float LastMoveDistance;

	Vector MouseDeltaToCameraRotation;

	Vector CharacterPosition;
	Rotator CameraRotation;

	// Array of the keys that are currently being held down.
	// Set to specified width. If an element is equal to '_', then the element is unused.
	//char* HeldKeys;
	std::unordered_set<char> HeldKeys;

	std::shared_ptr<APawn> Pawn;
};
