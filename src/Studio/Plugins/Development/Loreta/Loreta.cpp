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

#include "Loreta.h"
#ifdef OPENCV_SUPPORT

using namespace Core;

Loreta::Loreta(LoretaThreadHandler* loretaThreadHandler)
{
	mLoretaThreadHandler = loretaThreadHandler;
}


Loreta::~Loreta()
{
}


void Loreta::PrecalculateLoretaMatrices()
{
	const uint32 numElectrodes = mLoretaThreadHandler->GetElectrodes().Size();
	const uint32 numVoxels = mLoretaThreadHandler->GetVoxels().Size();

	try
	{
		// set the size of the matrices
		mMatrixJCaret = cv::Mat(numVoxels, 1, CV_64FC3, cv::Vec3d(0.0, 0.0, 0.0));
		mLoretaResult = cv::Mat(numVoxels, 1, CV_64F, double(0));
		mMatrixT = cv::Mat(numVoxels, numElectrodes, CV_64FC3, cv::Vec3d(0, 0, 0));

		// init matrix Ts first
		const uint32 numDimensions = 3;
		mMatrixTs.Resize(numDimensions);
		mMatrixJCarets.Resize(numDimensions);
		for (uint32 i=0;i<numDimensions;++i)
		{
			mMatrixTs[i]      = cv::Mat(numVoxels, numElectrodes, CV_64F, double(0));
			mMatrixJCarets[i] = cv::Mat(numVoxels, 1, CV_64F, double(0));
		}

		// result matrices for jcaret and for every direction
		mMatrixS.Clear();
		mMatrixS.Resize(numVoxels);

		// calculate the lead field K (contains values with vec3 for every dipole moment)
		cv::Mat leadField = CalcLeadField();

		// precalculations for S matrix
		cv::Mat transposedLeadField = leadField.t();

		// create pointer access for matrices
		const cv::Vec3d* ptrMatrixLeadField						= (cv::Vec3d*) leadField.data;
		const cv::Vec3d* ptrMatrixLeadFieldTransposed			= (cv::Vec3d*) transposedLeadField.data;
		const size_t	 stepSizeVec3dMatrixLeadField			= leadField.step / sizeof(cv::Vec3d);
		const size_t	 stepSizeVec3dMatrixTransposedLeadField = transposedLeadField.step / sizeof(cv::Vec3d);

		if (numElectrodes != 0)
		{
			// get the centering matrix for averaging the result
			cv::Mat centeringMatrix = GetCenteringMatrix(numElectrodes);
			const double* ptrCenteringMatrix = (double*) centeringMatrix.data;
			const size_t  stepSizeCenteringMatrix = centeringMatrix.step / sizeof(double);

			// calculate S matrix
			cv::Mat intermediateResultSWithReg = cv::Mat(numElectrodes, numElectrodes, CV_64F, double(0));
			for (uint32 i = 0; i<numElectrodes; ++i)
			{
				for (uint32 j = 0; j<numElectrodes; ++j)
				{
					mSummedResult = cv::Mat(1, 1, CV_64F, double(0));
					for (uint32 k = 0; k<numVoxels; ++k)
					{
						mSummedResult = mSummedResult + (cv::Mat(ptrMatrixLeadField[i * stepSizeVec3dMatrixLeadField + k]).t() * cv::Mat(ptrMatrixLeadFieldTransposed[k * stepSizeVec3dMatrixTransposedLeadField + j]));
					}

					intermediateResultSWithReg.ptr<double>(i)[j] = mSummedResult.ptr<double>(0)[0] + mLoretaThreadHandler->GetRegParameter() * ptrCenteringMatrix[i * stepSizeCenteringMatrix + j];
				}
			}

			cv::Mat invertedResultS;
			cv::invert(intermediateResultSWithReg, invertedResultS, cv::DECOMP_SVD);
			const double* ptrMatrixInvertedResultS = (double*) invertedResultS.data;
			const size_t  stepSizeMatrixInvertedResultS = invertedResultS.step / sizeof(double);


			for (uint32 i = 0; i<numVoxels; ++i)
			{
				for (uint32 j = 0; j<numElectrodes; ++j)
				{
					mSummedResult = cv::Mat(3, 1, CV_64F, double(0));
					for (uint32 k = 0; k<numElectrodes; ++k)
					{
						mSummedResult = mSummedResult + (cv::Mat(ptrMatrixLeadFieldTransposed[i * stepSizeVec3dMatrixTransposedLeadField + k]) * ptrMatrixInvertedResultS[k * stepSizeMatrixInvertedResultS + j]);
					}
					mMatrixT.ptr<cv::Vec3d>(i)[j] = mSummedResult;
				}
			}

			const cv::Vec3d* ptrMatrixT = (cv::Vec3d*) mMatrixT.data;
			const size_t     stepSizeMatrixT = mMatrixT.step / sizeof(cv::Vec3d);

			// fill matrix Ts with double values
			for (uint32 i=0;i<numVoxels;++i)
			{
				for (uint32 j=0;j<numElectrodes;++j)
				{
					mMatrixTs[0].ptr<double>(i)[j] = ptrMatrixT[i * stepSizeMatrixT + j][0];
					mMatrixTs[1].ptr<double>(i)[j] = ptrMatrixT[i * stepSizeMatrixT + j][1];
					mMatrixTs[2].ptr<double>(i)[j] = ptrMatrixT[i * stepSizeMatrixT + j][2];
				}
			}


			for (uint32 i = 0; i<numVoxels; ++i)
			{
				mSummedResult = cv::Mat(3, 3, CV_64F, double(0));
				for (uint32 j = 0; j<numVoxels; ++j)
				{
					for (uint32 k = 0; k<numElectrodes; ++k)
					{
						if (i == j)
						{
							mSummedResult = mSummedResult + (cv::Mat(ptrMatrixT[i * stepSizeMatrixT + k]) * cv::Mat(ptrMatrixLeadField[k * stepSizeVec3dMatrixLeadField + j]).t());
						}
						else
						{
							break;
						}
					}
				}
				mMatrixS[i] = mSummedResult.inv();
			}
		}
	}
	catch (cv::Exception cvException)
	{
		LogDebug("Exception occured in LoretaThreadHandler::Execute(): %s", cvException.what());
	}
}


cv::Mat Loreta::CalcLeadField()
{
	uint32 numElectrodes = mLoretaThreadHandler->GetElectrodes().Size();
	uint32 numVoxels     = mLoretaThreadHandler->GetVoxels().Size();
	cv::Mat leadField	 = cv::Mat(numElectrodes, numVoxels, CV_64FC3, cv::Vec3d(0.0, 0.0, 0.0));

	try
	{
		for (uint32 i=0;i<numElectrodes;++i)
		{
			// get the current electrode position
			Vector3 currentElectrodePosition = GetEEGElectrodes()->Get3DPosition(mLoretaThreadHandler->GetVisualSize() * 0.5, mLoretaThreadHandler->GetElectrodes()[i]);
			// swap y and z for mapping on the spherical model
			Vector3 normalizedElectrodePosition = Vector3(currentElectrodePosition.x * (-1.0), currentElectrodePosition.z, currentElectrodePosition.y * (-1.0));

			for (uint32 j=0;j<numVoxels;++j)
			{
				// get the current voxel position
				Vector3 currentVoxelPosition	= mLoretaThreadHandler->GetVoxels()[j].GetVoxelAABB().GetCenter();

				// get the upper part of the division
				Vector3 subtractedPositon		= normalizedElectrodePosition - currentVoxelPosition;
				// get the lower part of the division
				double  divisor					= Math::PowD(Math::SqrtD(Math::PowD(subtractedPositon.x, 2) + Math::PowD(subtractedPositon.y, 2) + Math::PowD(subtractedPositon.z, 2)), 3);

				// divide them
				Vector3 dividedResult			= subtractedPositon / divisor;
				// multiply it with a conductivity (in this case 1)
				Vector3 multipliedResult		= (1 / (4 * Math::piD * mLoretaThreadHandler->GetConductivity())) * dividedResult;

				// write everything into the result
				leadField.ptr<cv::Vec3d>(i)[j]	= cv::Vec3d(multipliedResult.x, multipliedResult.y, multipliedResult.z);
			}
		}
	
	}
	catch (cv::Exception cvException)
	{
		LogDebug("Exception occured in CalcLeadField(): %s", cvException.what());
	}

	return leadField;
}

//-------------------Loreta algorithm--------------------//
/* phi is a E x 1 Vector containing every simultan electrode measurement (e.g. volt or alpha measurement).
K is a E x 3V matrix which contains the electrical measurement of each electrode (E)
for every voxel (V). Each input variable is a 3-vector for each dipole
moment in x, y and z direction.
*/

// Loreta implementation (no error, taking noise in account)
void Loreta::CalcLoreta()
{
	const uint32     numVoxels				= mLoretaThreadHandler->GetVoxels().Size();
	const uint32     numElectrodes			= mLoretaThreadHandler->GetElectrodes().Size();
	const uint32	 numDimensions			= 3;

	if (numElectrodes != 0 && numVoxels != 0)
	{
		const cv::Vec3d* ptrMatrixT				= (cv::Vec3d*) mMatrixT.data;
		const cv::Vec3d* ptrMatrixJCaret		= (cv::Vec3d*) mMatrixJCaret.data;
		const double*    ptrMatrixPhi			= (double*) mMatrixPhi.data;
		const double*	 ptrMatrixVoxelResult   = (double*) mVoxelResult.data;
		const size_t	 stepSizeMatrixT		= mMatrixT.step / sizeof(cv::Vec3d);

		try
		{
			for (uint32 i=0;i<numDimensions;++i)
			{
				mMatrixJCarets[i] = mMatrixTs[i] * mMatrixPhi;
			}

			// fill jCaret again
			for (uint32 i=0;i<numVoxels;++i)
			{
				mMatrixJCaret.ptr<cv::Vec3d>(0)[i][0] = mMatrixJCarets[0].ptr<double>(0)[i];
				mMatrixJCaret.ptr<cv::Vec3d>(0)[i][1] = mMatrixJCarets[1].ptr<double>(0)[i];
				mMatrixJCaret.ptr<cv::Vec3d>(0)[i][2] = mMatrixJCarets[2].ptr<double>(0)[i];
			}

			// transpose jcaret
			mTransposedJCaret = mMatrixJCaret.t();
			const cv::Vec3d* ptrMatrixJCaretTransposed = (cv::Vec3d*) mMatrixJCaret.data;

			// calculate final result
			for (uint32 i=0;i<numVoxels;++i)
			{
				mVoxelResult = cv::Mat(ptrMatrixJCaret[i]).t() * mMatrixS[i] * cv::Mat(ptrMatrixJCaret[i]);
				mLoretaResult.ptr<double>(0)[i] = mVoxelResult.ptr<double>(0)[0];
			}
		}
		catch (cv::Exception cvException)
		{
			LogDebug("Exception occured in CalcLoreta(): %s", cvException.what());
		}
	}
}


// calculate the centering matrix
cv::Mat Loreta::GetCenteringMatrix(const uint32 size)
{
	cv::Mat diagonalMatrix = cv::Mat::eye(size, size, CV_64F);
	cv::Mat filledWithOnes = cv::Mat(size, size, CV_64F, cv::Scalar(1));
	cv::Mat result = (cv::Mat_<double>(size, size));
	result = diagonalMatrix - ((1.0 / size) * filledWithOnes);

	return result;
}


// helper method for plotting a matrix into a file
void Loreta::PrintMatrixToFile(cv::Mat matrix, const char* filename)
{
	cv::FileStorage store(filename, cv::FileStorage::WRITE);
	store << filename << matrix;
	store.release();
}

#endif
