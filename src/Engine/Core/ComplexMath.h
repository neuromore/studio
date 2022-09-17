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

#ifndef __NEUROMORE_COMPLEXMATH_H
#define __NEUROMORE_COMPLEXMATH_H

#include <complex>
#include "Math.h"
#include <math.h>

namespace Core
{

// the complex number class
class ENGINE_API Complex
{
	
	public:
		// constructors & destructor
		Complex() : mReal(0.0), mImag(0.0)												{}
        Complex(double real, double imaginary) : mReal(real), mImag(imaginary)			{}
		Complex(double real) : mReal(real), mImag(0.0)									{}
		~Complex()																		{}
	
		// assignment	
		Complex& operator= (const double val)									{ mReal = val; mImag = 0.; return *this; }
		Complex& operator= (const Complex& other)								{ mReal = other.mReal; mImag = other.mImag; return *this; }

		// arithmetic operations
		Complex operator+ (const Complex& other) const							{ return Complex(mReal + other.mReal, mImag + other.mImag); }
		Complex operator- (const Complex& other) const							{ return Complex(mReal - other.mReal, mImag - other.mImag); }
		Complex operator* (const Complex& other) const							{ return Complex(mReal * other.mReal - mImag * other.mImag, mReal * other.mImag + mImag * other.mReal); }
		Complex operator/ (const Complex& other) const							{ const double denominator = other.mReal * other.mReal + other.mImag * other.mImag; return Complex((mReal * other.mReal + mImag * other.mImag) / denominator, (mImag * other.mReal - mReal * other.mImag) / denominator); }
		Complex& operator+= (const Complex& other)								{ mReal += other.mReal; mImag += other.mImag; return *this; }
		Complex& operator-= (const Complex& other)								{ mReal -= other.mReal; mImag -= other.mImag; return *this; }
		Complex& operator*= (const Complex& other)								{ const double temp = mReal; mReal = mReal * other.mReal - mImag * other.mImag; mImag = mImag * other.mReal + temp * other.mImag; return *this; }
		Complex& operator/= (const Complex& other)								{ const double denominator = other.mReal * other.mReal + other.mImag * other.mImag; const double temp = mReal; mReal = (mReal * other.mReal + mImag * other.mImag) / denominator; mImag = (mImag * other.mReal - temp * other.mImag) / denominator; return *this; }
		Complex& operator++ ()													{ ++mReal; return *this; }
		Complex operator++ (int)												{ Complex temp(*this); ++mReal; return temp; }
		Complex& operator-- ()													{ --mReal; return *this; }
		Complex operator-- (int)												{ Complex temp(*this); --mReal; return temp; }
		Complex operator+ (const double val) const								{ return Complex(mReal + val, mImag); }
		Complex operator- (const double val) const								{ return Complex(mReal - val, mImag); }
		Complex operator* (const double val) const								{ return Complex(mReal * val, mImag * val); }
		Complex operator/ (const double val) const								{ return Complex(mReal / val, mImag / val); }
		Complex& operator+= (const double val)									{ mReal += val; return *this; }
		Complex& operator-= (const double val)									{ mReal -= val; return *this; }
		Complex& operator*= (const double val)									{ mReal *= val; mImag *= val; return *this; }
		Complex& operator/= (const double val)									{ mReal /= val; mImag /= val; return *this; }

		friend Complex operator+ (const double left, const Complex& right)		{ return Complex(left + right.mReal, right.mImag); }
		friend Complex operator- (const double left, const Complex& right)		{ return Complex(left - right.mReal, -right.mImag); }
		friend Complex operator* (const double left, const Complex& right)		{ return Complex(left * right.mReal, left * right.mImag); }
		friend Complex operator/ (const double left, const Complex& right)		{ const double denominator = right.mReal * right.mReal + right.mImag * right.mImag; return Complex(left * right.mReal / denominator, -left * right.mImag / denominator); }

		// boolean operators
		//bool operator== (const Complex &other) const							{ return mReal == other.mReal && mImag == other.mImag; }
		//bool operator!= (const Complex &other) const							{ return mReal != other.mReal || mImag != other.mImag; }
		//bool operator== (const double val) const								{ return mReal == val && mImag == 0.; }
		//bool operator!= (const double val) const								{ return mReal != val || mImag != 0.; }

		//friend bool operator== (const double left, const Complex& right)		{ return left == right.mReal && right.mImag == 0.; }
		//friend bool operator!= (const double left, const Complex& right)		{ return left != right.mReal || right.mImag != 0.; }

		// Squared L2-Norm
		inline double SquaredNorm() const										{ return mReal * mReal + mImag * mImag; }
	
		// L2 (Euclidian) Norm (absolute value = length of the 'vector')
		inline double Norm() const												{ return Math::SqrtD(mReal * mReal + mImag * mImag); }

		// argument (phase) of the complex number
		inline double Arg() const												{ return Math::ATan2D(mImag, mReal); }

	public:
		double mReal;
		double mImag;
};
// complex functions (mostly using std::complex<double>)
class ComplexMath 
{
	public:
		static inline Complex Conjugate(const Complex& z) 						{ return Complex(z.mReal, -z.mImag); }

		// fast integer powers
		static inline Complex Pow(const Complex& z, uint32 exponent) 
		{
			Complex x = z;
			Complex y = (exponent % 2) ? z  : Complex(1.0);

			while (exponent >>= 1) 
			{
				x = x * x;
				if (exponent % 2) 
					y = y * x;
			}

			return y;
		}

		static inline Complex Pow(const Complex& z, const Complex& exp)
		{
			std::complex<double> z_val (z.mReal, z.mImag);
			std::complex<double> exp_val (exp.mReal, exp.mImag);
			std::complex<double> result = std::pow(z_val, exp_val);
			return Complex(result.real(), result.imag());
		}
		
		static inline Complex Sqrt(const Complex& z)
		{
			std::complex<double> z_val (z.mReal, z.mImag);
			std::complex<double> result = std::sqrt(z_val);
			return Complex(result.real(), result.imag());
		}

		static inline Complex Exp(const Complex& z)
		{
			std::complex<double> z_val (z.mReal, z.mImag);
			std::complex<double> result = std::exp(z_val);
			return Complex(result.real(), result.imag());
		}

		static inline Complex ExpJ (double x)			
		{
			std::complex<double> a (0, x);		// a = i*x 
			std::complex<double> r = std::exp(a);
			return Complex(r.real(), r.imag());
		}

		static inline Complex Sin(const Complex& z)
		{
			std::complex<double> z_val (z.mReal, z.mImag);
			std::complex<double> result = std::sin(z_val);
			return Complex(result.real(), result.imag());
		}

		static inline Complex ASin(const Complex& z)
		{
			std::complex<double> z_val (z.mReal, z.mImag);
			std::complex<double> result = std::asin(z_val);
			return Complex(result.real(), result.imag());
		}

		static inline Complex Cos(const Complex& z)
		{
			std::complex<double> z_val (z.mReal, z.mImag);
			std::complex<double> result = std::cos(z_val);
			return Complex(result.real(), result.imag());
		}

		static inline Complex ACos(const Complex& z)
		{
			std::complex<double> z_val (z.mReal, z.mImag);
			std::complex<double> result = std::acos(z_val);
			return Complex(result.real(), result.imag());
		}

		static inline Complex Tan(const Complex& z)
		{
			std::complex<double> z_val (z.mReal, z.mImag);
			std::complex<double> result = std::tan(z_val);
			return Complex(result.real(), result.imag());
		}
		
	/*	static inline Complex ATan2(const Complex& a, const Complex& b)
		{
			std::complex<double> a_val   (a.mReal, a.mImag);
			std::complex<double> b_val   (b.mReal, b.mImag);
			double result = atan2(a_val, b_val);
			return Complex(result.real(), result.imag());
		}*/

		static inline Complex Sinh(const Complex& z)
		{
			std::complex<double> z_val (z.mReal, z.mImag);
			std::complex<double> result = std::sinh(z_val);
			return Complex(result.real(), result.imag());
		}
		
		static inline Complex ASinh(const Complex& z)
		{
			std::complex<double> z_val (z.mReal, z.mImag);
			std::complex<double> result = std::asinh(z_val);
			return Complex(result.real(), result.imag());
		}

		static inline Complex Cosh(const Complex& z)
		{
			std::complex<double> z_val (z.mReal, z.mImag);
			std::complex<double> result = std::cosh(z_val);
			return Complex(result.real(), result.imag());
		}
		
		static inline Complex ACosh(const Complex& z)
		{
			std::complex<double> z_val (z.mReal, z.mImag);
			std::complex<double> result = std::acosh(z_val);
			return Complex(result.real(), result.imag());
		}

		static inline Complex Log(const Complex& z)
		{
			std::complex<double> z_val (z.mReal, z.mImag);
			std::complex<double> result = std::log(z_val);
			return Complex(result.real(), result.imag());
		}

		static inline bool IsInf(const Complex& z)
		{
			return std::isinf(z.mReal) || std::isinf(z.mImag);
		}

		static inline bool IsNaN(const Complex& z)
		{
			return std::isnan(z.mReal) || std::isinf(z.mImag);
		}

};

}	// end namespace


#endif
