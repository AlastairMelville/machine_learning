#pragma once


struct Rotator
{
public:
	/** Rotation around the right axis (around Y axis), Looking up and down (0=Straight Ahead, +Up, -Down) */
	float Pitch;

	/** Rotation around the up axis (around Z axis), Running in circles 0=East, +North, -South. */
	float Yaw;

	/** Rotation around the forward axis (around X axis), Tilting your head, 0=Straight, +Clockwise, -CCW. */
	float Roll;

public:

	/** A rotator of zero degrees on each axis. */
	static const Rotator ZeroRotator;

public:
	/**
	 * Default constructor (no initialization).
	 */
	__forceinline Rotator()
		: Pitch(0.0f)
		, Yaw(0.0f)
		, Roll(0.0f)
	{}

	/**
	 * Constructor
	 *
	 * @param InF Value to set all components to.
	 */
	explicit __forceinline Rotator(float InF);

	/**
	 * Constructor.
	 *
	 * @param InPitch Pitch in degrees.
	 * @param InYaw Yaw in degrees.
	 * @param InRoll Roll in degrees.
	 */
	__forceinline Rotator(float InPitch, float InYaw, float InRoll);

	__forceinline Rotator operator+=(const Rotator& InRotator);

	/**
	 * Convert a rotation into a unit vector facing in its direction.
	 *
	 * @return Rotation as a unit direction vector.
	 */
	struct Vector ToVector() const;
};

__forceinline Rotator::Rotator(float InF)
	: Pitch(InF)
	, Yaw(InF)
	, Roll(InF)
{

}

__forceinline Rotator::Rotator(float InPitch, float InYaw, float InRoll)
	: Pitch(InPitch)
	, Yaw(InYaw)
	, Roll(InRoll)
{

}

__forceinline Rotator Rotator::operator+=(const Rotator& InRotator)
{
	Pitch += InRotator.Pitch;
	Yaw += InRotator.Yaw;
	Roll += InRotator.Roll;
	return *this;
}
