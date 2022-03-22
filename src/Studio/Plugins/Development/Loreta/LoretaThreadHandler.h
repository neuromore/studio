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
#ifndef LORETATHREADHANDLER_H
#define LORETATHREADHANDLER_H

#include "../../../Config.h"
#ifdef OPENCV_SUPPORT
#include <Engine/Core/ThreadHandler.h>
#include <Engine/Core/LogManager.h>
#include "Loreta.h"
#include "LoretaWidget.h"
#include "Core/Vector.h"
#include "Core/Ray.h"

// forward declaration
class Loreta;
class LoretaWidget;

class LoretaThreadHandler : public Core::ThreadHandler
{
	public:
		LoretaThreadHandler(LoretaWidget* loretaWidget);
		virtual ~LoretaThreadHandler();

		void Execute();
		void Terminate();

		// setter
		inline void					SetCalcLoreta(bool calcLoreta)						{ mCalcLoreta = calcLoreta; }
		void						SetNumBaseVoxels(uint32 numBaseVoxels)				{ mNumBaseVoxels = numBaseVoxels; }
		inline void					SetRegParameter(double regParameter)				{ mRegParameter = regParameter; }
		inline void					SetConductivity(double conductivity)				{ mConductivity = conductivity; }


		// setup rendering 
		inline void					SetCenter(Core::Vector3 center)						{ mCenter = center; }
		inline void					SetVisualSize(double size)							{ mVisualSize = size; }

		// getter
		uint32												GetNumElectrodes() const							{ return mElectrodes.Size(); }
		inline const EEGElectrodes::Electrode&				GetElectrode(uint32 index) const					{ return mElectrodes[index]; }
		inline const Core::Array<EEGElectrodes::Electrode>& GetElectrodes() const								{ return mElectrodes; }
		inline Channel<double>*								GetChannel(uint32 index) const						{ return mChannels[index]; }
		inline const Core::Array<Voxel>&					GetVoxels()											{ return mVoxels; }
		inline Voxel*										GetVoxel(uint32 index)								{ return &mVoxels[index]; }
		inline const Core::Array<bool>&						GetMaskingMatrix() const							{ return mMaskingMatrix; }
		inline uint32										GetNumBaseVoxel()									{ return mNumBaseVoxels; }
		inline const double									GetRegParameter()									{ return mRegParameter; }
		inline const double									GetConductivity()									{ return mConductivity; }
		inline const Core::AABB&							GetAABB()											{ return mAABB; }
		inline const cv::Mat&								GetJCaret()											{ return mMatrixJCaret; }
		inline const cv::Mat&								GetLoretaResult()									{ return mLoretaResult; }
		inline double										GetVisualSize()										{ return mVisualSize; }
		const inline char*									GetFpsText()										{ return mLoretaFpsCounter.GetText(); }

		void Init();
		void Update();
		void Reset();

		// setup loreta
		void										AddElectrode(const EEGElectrodes::Electrode& electrode, Channel<double>* channel);
		void										CalcMaskingMatrix(Mesh* mesh);
		uint32										CalcVoxelIndex(uint32 xIndex, uint32 yIndex, uint32 zIndex);
		bool										CheckIfMeshIsInside(Mesh* mesh, Core::Ray* rayUp, Core::Ray* rayDown);



	private:

		Loreta*										mLoreta;
		bool										mCalcLoreta;

		Core::Array<EEGElectrodes::Electrode>		mElectrodes;
		Core::Array<Channel<double>*>				mChannels;
		Core::Array<Voxel>							mVoxels;
		Core::Array<bool>							mMaskingMatrix;
		double										mRegParameter;
		double										mConductivity;
		uint32										mNumBaseVoxels;
		Core::Vector3								mCenter;
		double										mVisualSize;
		Core::AABB									mAABB;
		LoretaWidget*								mLoretaWidget;

		cv::Mat										mMatrixPhi;				// input Phi
		cv::Mat										mMatrixJCaret;			// output J^ = T*Phi
		cv::Mat										mLoretaResult;			// resultset from loreta by multiplying the SMatrix with JCaret

		// fps counter
		Core::FpsCounter							mLoretaFpsCounter;

		// output voxels
		void CreateVoxels();

		bool CheckRayIntersection(const Core::Ray& ray, const Core::Vector3& v0, const Core::Vector3& v1, const Core::Vector3& v2);
};

#endif
#endif
