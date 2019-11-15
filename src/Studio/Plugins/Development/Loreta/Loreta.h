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

#ifndef LORETA_H
#define LORETA_H

#include "../../../Config.h"
#ifdef OPENCV_SUPPORT
#include <opencv2/core/core.hpp>
#include <Core/Ray.h>
#include <Core/AABB.h>
#include "../../../../../Engine/Source/Core/FpsCounter.h"
#include "../../../Rendering/Mesh.h"
#include "EngineManager.h"
#include "Voxel.h"
#include "LoretaThreadHandler.h"

// forward declaration
class LoretaThreadHandler;

class Loreta
{
	public:

		Loreta(LoretaThreadHandler* loretaThreadHandler);
		~Loreta();

		// setter
		inline void									SetMatrixS(const Core::Array<cv::Mat>& matrixS)							 { mMatrixS = matrixS; }
		inline void									SetMatrixPhi(const cv::Mat& matrixPhi)									 { mMatrixPhi = matrixPhi; }
		inline void									SetMatrixJCaret(const cv::Mat& matrixJCaret)							 { mMatrixJCaret = matrixJCaret; }
		inline void									SetLoretaResult(const cv::Mat& loretaResult)							 { mLoretaResult = loretaResult; }

		// getter
		inline cv::Mat&								GetMatrixJCaret()														 { return mMatrixJCaret; }
		inline cv::Mat&								GetLoretaResult()														 { return mLoretaResult; }


		cv::Mat										CalcLeadField();
		void										PrecalculateLoretaMatrices();
		void										CalcLoreta();
		cv::Mat										GetCenteringMatrix(const uint32 size);

	private:
		LoretaThreadHandler*						mLoretaThreadHandler;

		cv::Mat										mLeadField;				// input lead matrix, as single matrix storing vec3ds
		cv::Mat										mMatrixPhi;
		cv::Mat										mMatrixT;				// matrix T, storing the defined lead fields
		Core::Array<cv::Mat>						mMatrixS;				// matrix S, storing the correlation between every dipole moment
		cv::Mat										mMatrixJCaret;			// output J^ = T*Phi
		cv::Mat										mLoretaResult;			// resultset from loreta by multiplying the SMatrix with JCaret
		cv::Mat 									mSummedResult;
		cv::Mat										mVoxelResult;
		cv::Mat										mTransposedJCaret;
		Core::Array<cv::Mat>						mMatrixTs;
		Core::Array<cv::Mat>						mMatrixJCarets;

		// helper
		void    PrintMatrixToFile(cv::Mat matrix, const char* filename);

};

#endif

#endif
