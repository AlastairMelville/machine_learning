#pragma once


struct Vector2D
{
	/** Vector's X component. */
	float X;

	/** Vector's Y component. */
	float Y;

public:

	/** Global 2D zero vector constant (0,0) */
	static const Vector2D ZeroVector;

	/**
	* Global 2D one vector (poorly named) constant (1,1).
	*
	* @note Incorrectly named "unit" vector though its magnitude/length/size is not one. Would fix, though likely used all over the world. Use `Unit45Deg` below for an actual unit vector.
	*/
	static const Vector2D UnitVector;

	/**
	* Global 2D unit vector constant along the 45 degree angle or symmetrical positive axes (sqrt(.5),sqrt(.5)) or (.707,.707). https://en.wikipedia.org/wiki/Unit_vector
	*
	* @note The `UnitVector` above is actually a value with axes of 1 rather than a magnitude of one.
	*/
	static const Vector2D Unit45Deg;

public:

	/** Default constructor (no initialization). */
	__forceinline Vector2D()
		: X(0.0f)
		, Y(0.0f)
	{}

	/**
	 * Constructor using initial values for each component.
	 *
	 * @param InX X coordinate.
	 * @param InY Y coordinate.
	 */
	__forceinline Vector2D(float InX, float InY);
};

__forceinline Vector2D::Vector2D(float InX, float InY)
	: X(InX)
	, Y(InY)
{

}