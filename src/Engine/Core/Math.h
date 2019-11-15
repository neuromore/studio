/****************************************************************************
**
** Copyright 2019 neuromore co
** Contact: https://neuromore.com/contact
**
** Commercial License Usage
** Licensees holding valid commercial neuromore licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and neuromore. For licensing terms
** and conditions see https://neuromore.com/licensing. For further
** information use the contact form at https://neuromore.com/contact.
**
** neuromore Public License Usage
** Alternatively, this file may be used under the terms of the neuromore
** Public License version 1 as published by neuromore co with exceptions as 
** appearing in the file neuromore-class-exception.md included in the 
** packaging of this file. Please review the following information to 
** ensure the neuromore Public License requirements will be met: 
** https://neuromore.com/npl
**
****************************************************************************/

#ifndef __CORE_MATH_H
#define __CORE_MATH_H

//  include required headers
#include "StandardHeaders.h"


namespace Core
{

class ENGINE_API Math
{
	public:
		static inline float RadiansToDegrees(float rad)			{ return (rad * 57.2957795130823208767); }
		static inline float DegreesToRadians(float deg)			{ return (deg * 0.01745329251994329576); }
//		static inline double RadiansToDegreesD(double rad);
//		static inline double DegreesToRadiansD(double deg);

		static inline float Abs(float x)						{ return fabs(x); }
		static inline double AbsD(double x)						{ return fabs(x); }
		static inline float Ceil(float x)						{ return ceil(x); }
		static inline double CeilD(double x)					{ return ceil(x); }
		static inline float Floor(float x)						{ return floor(x); }
		static inline double FloorD(double x)					{ return floor(x); }

		static inline float FMod(float x, float y)				{ return fmodf(x, y); }
		static inline double FModD(double x, double y)			{ return fmod(x, y); }

		static inline float Sin(float x)						{ return sinf(x); }
		static inline double SinD(double x)						{ return sin(x); }
		static inline float Cos(float x)						{ return cosf(x); }
		static inline double CosD(double x)						{ return cos(x); }
		static inline float Tan(float x)						{ return tanf(x); }
		static inline double TanD(double x)						{ return tan(x); }
		static inline float ASin(float x)						{ return asinf(x); }
		static inline double ASinD(double x)					{ return asin(x); }
		static inline float ACos(float x)						{ return acosf(x); }
		static inline double ACosD(double x)					{ return acos(x); }
		static inline float ATan(float x)						{ return atanf(x); }
		static inline double ATanD(double x)					{ return atan(x); }
		static inline float ATan2(float y, float x)				{ return atan2f(y, x); }
		static inline double ATan2D(double y, double x)			{ return atan2(y, x); }
		static inline float  Sinh(float x)						{ return sinhf(x); }
		static inline double SinhD(double x)					{ return sinh(x); }
		static inline float  Cosh(float x)						{ return coshf(x); }
		static inline double CoshD(double x)					{ return cosh(x); }
		static inline float  ASinh(float x)						{ return asinhf(x); }
		static inline double ASinhD(double x)					{ return asinh(x); }
		static inline float  ACosh(float x)						{ return acoshf(x); }
		static inline double ACoshD(double x)					{ return acosh(x); }

		static inline float Exp(float x)						{ return expf(x); }
		static inline double ExpD(double x)						{ return exp(x); }

		static inline float Log(float x)						{ return logf(x); }
		static inline double LogD(double x)						{ return log(x); }
		static inline float Log10(float x)						{ return log10f(x); }
		static inline double Log10D(double x)					{ return log10(x); }


		/**
		* Calculates the base-2 logarithm.
		* The log functions return the base-2 logarithm of x. If x is negative,
		* these functions return an indefinite, by default. If x is 0, they
		* return INF (infinite).
		* @param x The value.
		* @return The logarithm of x.
		*/
		static inline float  Log2(float x);
		static inline double Log2D(double x);

		static inline float Pow(float base, float exponent)		{ return powf(base, exponent); }
		static inline double PowD(double base, double exponent)	{ return pow(base, exponent); }

		static inline float Sqrt(float x)						{ return sqrtf(x); }
		static inline double SqrtD(double x)					{ return sqrt(x); }

		static inline float SafeSqrt(float x)					{ return (x > epsilon) ? sqrtf(x) : 0.0f; }
		static inline double SafeSqrtD(double x)				{ return (x > epsilon) ? sqrt(x) : 0.0f; }


		static inline float InvSqrt(float x)					{ return 1.0f / Sqrt(x); }
		static inline double InvSqrtD(double x)					{ return 1.0f / SqrtD(x); }


		/**
		 * Calculates the next power of two.
		 * @param x The value.
		 * @return The next power of two of the parameter.
		 */
		static inline uint32 NextPowerOfTwo(uint32 x);

		/**
		* Create a 'nice', readable number of the form {1,2,5}*10^n.
		* This readable number sequence is often used as a readable logarithmic scale with nice round numbers.
		* The input number will be rounded down to the next highest number in this sequence:
		*    ... 0.01, 0.02, 0.05, 0.1, 0.2, 0.5, 1, 2, 5, 10, 20, 50, 100 ...
		**/
		static inline double NiceNumberCeil(double x);
		static inline double NiceNumberFloor(double x);

		static inline bool IsNaN(float x);
		static inline bool IsNaND(double x);

		static inline bool IsInf(float x);
		static inline bool IsInfD(double x);

		static inline bool IsValidNumber(float x);
		static inline bool IsValidNumberD(double x);

		static inline int32 Rand()																{ return rand(); }
		static inline int32 RandBigRange(const int32 minVal, const int32 maxVal)				{ return minVal + (maxVal - minVal) * rand(); }
		static inline int32 RandSmallRange(const int32 minVal, const int32 maxVal)				{ if (maxVal - minVal + 1 == 0) return (rand()>0?maxVal:minVal); return minVal + (rand() % (int)(maxVal - minVal + 1)); }
		static inline uint32 RandIndex(uint32 numElements)										{ if (numElements == 0) return CORE_INVALIDINDEX32; return rand() % (int)(numElements); }
		static inline float RandF()																{ return rand() / (float)RAND_MAX; }
		static inline float RandF(const float minVal, const float maxVal)						{ return minVal + (maxVal - minVal) * rand() / (float)RAND_MAX; }
		static inline double RandD()															{ return rand() / (double)RAND_MAX; }
		static inline double RandD(const double minVal, const double maxVal)					{ return minVal + (maxVal - minVal) * rand() / (double)RAND_MAX; }

	public:
		static const float pi;				/**< The value of PI, which is 3.1415926 */
		static const float twoPi;			/**< PI multiplied by two */
		static const float halfPi;			/**< PI divided by two */
		static const float invPi;			/**< 1.0 divided by PI */
		static const float epsilon;		/**< A very small number, almost equal to zero (0.000001). */
		static const float sqrtHalf;		/**< The square root of 0.5. */
		static const float sqrt2;			/**< The square root of 2. */
		static const float sqrt3;			/**< The square root of 3. */
		static const float halfSqrt2;		/**< Half the square root of two. */
		static const float halfSqrt3;		/**< Half the square root of three. */

		static const double piD;			/**< The value of PI, which is 3.1415926 */
		static const double twoPiD;		/**< PI multiplied by two */
		
};

// include the inline code
#include "Math.inl"

// minimum & maximum
template<class T> T Min(T a, T b)																							{ return (a < b) ? a : b; }
template<class T> T Min3(T a, T b, T c)																						{ return Min(Min(a, b), c); }
template<class T> T Max(T a, T b)																							{ return (a > b) ? a : b; }
template<class T> T Max3(T a, T b, T c)																						{ return Max(Max(a, b), c); }

// value comparison
template<class T> bool IsClose(const T& a, const T& b, const float threshold);
template<class T> bool InRange(T x, T low, T high)																			{ return ((x>=low) && (x<=high)); }


// interval helpers
template<class T> T Clamp(T x, T min, T max)																				{ if (x < min) x = min; if (x > max) x = max; return x; }
double ENGINE_API RemapRange(double value, double fromMin, double fromMax, double toMin, double toMax);
double ENGINE_API ClampedRemapRange(double value, double fromMin, double fromMax, double toMin, double toMax);

// interpolation
float ENGINE_API CalcCosineInterpolationWeight(float linearValue);
template <class T> T LinearInterpolate(const T& source, const T& target, float timeValue)									{ return static_cast<T>( source*(1.0f - timeValue) + (timeValue*target) ); }
template <class T> T CosineInterpolate(const T& source, const T& target, float timeValue)									{ const float weight = CalcCosineInterpolationWeight( timeValue ); return source*(1.0f - weight) + (weight*target); }
float ENGINE_API SampleEaseInOutCurve(float t, float k1, float k2);
template <class T> T EaseInOutInterpolate(const T& source, const T& target, float timeValue, float k1=0.5f, float k2=0.5f)	{ const float t = SampleEaseInOutCurve(timeValue, k1, k2); return source + t*(target-source); }


// others
template<class T> inline void Swap(T& a, T& b)																				{ if (&a == &b) return; T tempObj = a; a = b; b = tempObj; }


// cartesian to spherical coordidate translation and vice versa
//Vector2 ENGINE_API ToSpherical(const Vector3& normalizedVector);
//Vector3 ENGINE_API FromSpherical(const Vector2& spherical);

}	// namespace Core


#endif
