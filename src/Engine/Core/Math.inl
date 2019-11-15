/*
 * neuromore Engine
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */


inline bool Math::IsNaN(float x)
{
#if (defined NEUROMORE_PLATFORM_ANDROID || defined NEUROMORE_PLATFORM_LINUX)
	return x == NAN;
#else
	return isnan<float>(x);
#endif
}


inline bool Math::IsNaND(double x)
{
#if (defined NEUROMORE_PLATFORM_ANDROID || defined NEUROMORE_PLATFORM_LINUX)
	return x == NAN;
#else
	return isnan<double>(x);
#endif
}

			
inline bool Math::IsInf(float x)
{
#if (defined NEUROMORE_PLATFORM_ANDROID || defined NEUROMORE_PLATFORM_LINUX)
	return x == INFINITY;
#else
	return isinf<float>(x);
#endif
}


inline bool Math::IsInfD(double x)
{
#if (defined NEUROMORE_PLATFORM_ANDROID || defined NEUROMORE_PLATFORM_LINUX)
	return x == INFINITY;
#else
	return isinf<double>(x);
#endif
}
			
inline bool Math::IsValidNumber(float x)
{
	return !IsInf(x) && !IsNaN(x);
}


inline bool Math::IsValidNumberD(double x)
{
	return !IsInfD(x) && !IsNaND(x);
}




inline float Math::Log2(float x)
{
#ifdef NEUROMORE_PLATFORM_ANDROID
	CORE_ASSERT(false); // NOT SUPPORTED
	return 0.0f;
#else
	return log2f(x);
#endif
}


inline double Math::Log2D(double x)
{
#ifdef NEUROMORE_PLATFORM_ANDROID
	CORE_ASSERT(false); // NOT SUPPORTED
	return 0.0f;
#else
	return log2(x);
#endif
}


inline uint32 Math::NextPowerOfTwo(uint32 x)
{
	uint32 nextPowerOfTwo = x;

	nextPowerOfTwo--;
	nextPowerOfTwo= (nextPowerOfTwo>> 1) | nextPowerOfTwo;
	nextPowerOfTwo= (nextPowerOfTwo>> 2) | nextPowerOfTwo;
	nextPowerOfTwo= (nextPowerOfTwo>> 4) | nextPowerOfTwo;
	nextPowerOfTwo= (nextPowerOfTwo>> 8) | nextPowerOfTwo;
	nextPowerOfTwo= (nextPowerOfTwo>> 16) | nextPowerOfTwo;
	nextPowerOfTwo++;

	return nextPowerOfTwo;
}


// round up to the next "nice" number in the 1-2-5 sequence: 1,2,5,10,20,50,...
inline double Math::NiceNumberCeil(double value)
{
	// scale to a single digit
	const double magnitude = PowD(10.0, CeilD(Log10D(AbsD(value))) - 1.0);		// TODO performance?
	if (magnitude == 0)
		return 0.0;

	double normedValue = value / magnitude;

	// round up to 2/5/10
	if (normedValue < 2)
		normedValue = 2;
	else if (normedValue < 5)
		normedValue = 5;
	else
		normedValue = 10;

	// unscale value
	return normedValue * magnitude;
}


// round down to the next "nice" number in the 1-2-5 sequence: 1,2,5,10,20,50,...
inline double Math::NiceNumberFloor(double value)
{
	// scale to a single digit
	const double magnitude = PowD(10.0, CeilD(Log10D(AbsD(value))) - 1.0);		// TODO performance?
	if (magnitude == 0)
		return 0.0;

	double normedValue = value / magnitude;

	// round down to 1/2/5
	if (normedValue < 2)
		normedValue = 1;
	else if (normedValue < 5)
		normedValue = 2;
	else
		normedValue = 5;

	return normedValue * magnitude;
}