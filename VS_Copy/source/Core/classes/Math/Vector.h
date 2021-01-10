#pragma once

/**
 * A vector in 3-D space composed of components (X, Y, Z) with floating point precision.
 */
struct Vector
{
public:

	/** Vector's X component. */
	float X;

	/** Vector's Y component. */
	float Y;

	/** Vector's Z component. */
	float Z;

public:

	/** A zero vector (0,0,0) */
	static const Vector ZeroVector;

	/** One vector (1,1,1) */
	static const Vector OneVector;

	/** Unreal up vector (0,0,1) */
	static const Vector UpVector;

	/** Unreal down vector (0,0,-1) */
	static const Vector DownVector;

	/** Unreal forward vector (1,0,0) */
	static const Vector ForwardVector;

	/** Unreal backward vector (-1,0,0) */
	static const Vector BackwardVector;

	/** Unreal right vector (0,1,0) */
	static const Vector RightVector;

	/** Unreal left vector (0,-1,0) */
	static const Vector LeftVector;

public:
	__forceinline Vector()
		: X(0.0f)
		, Y(0.0f)
		, Z(0.0f)
	{}

	explicit __forceinline Vector(float InXYZ)
		: X(InXYZ)
		, Y(InXYZ)
		, Z(InXYZ)
	{}

	__forceinline Vector(float InX, float InY, float InZ)
		: X(InX)
		, Y(InY)
		, Z(InZ)
	{}

	/**
	 * Check against another vector for equality.
	 *
	 * @param V The vector to check against.
	 * @return true if the vectors are equal, false otherwise.
	 */
	__forceinline bool operator==(const Vector& InVector) const;

	/**
	 * Adds another vector to this.
	 * Uses component-wise addition.
	 *
	 * @param V Vector to add to this.
	 * @return Copy of the vector after addition.
	 */
	__forceinline Vector operator+=(const Vector& InVector);

	/**
	 * Subtracts another vector from this.
	 * Uses component-wise subtraction.
	 *
	 * @param V Vector to subtract from this.
	 * @return Copy of the vector after subtraction.
	 */
	__forceinline Vector operator-=(const Vector& InVector);
};

__forceinline bool Vector::operator==(const Vector& InVector) const
{
	return X == InVector.X && Y == InVector.Y && Z == InVector.Z;
}

__forceinline Vector Vector::operator+=(const Vector& InVector)
{
	X += InVector.X;
	Y += InVector.Y;
	Z += InVector.Z;
	return *this;
}

__forceinline Vector Vector::operator-=(const Vector& InVector)
{
	X -= InVector.X;
	Y -= InVector.Y;
	Z -= InVector.Z;
	return *this;
}
