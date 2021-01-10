
#include "source/Core/classes/Math/PositionalMath.h"
#include "source/Core/classes/Math/MathUtilities.h"
#include "source/Core/classes/Math/Rotator.h"
#include "source/Core/classes/Math/Vector.h"

Vector Rotator::ToVector() const
{
	float CP, SP, CY, SY;
	Math::SinCos(&SP, &CP, Math::DegreesToRadians(Pitch));
	Math::SinCos(&SY, &CY, Math::DegreesToRadians(Yaw));
	
	Vector V(CP * CY, CP * SY, SP);
	return V;
}
