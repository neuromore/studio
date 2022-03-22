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
#include "LoretaThreadHandler.h"
#ifdef OPENCV_SUPPORT
using namespace Core;

LoretaThreadHandler::LoretaThreadHandler(LoretaWidget* loretaWidget) : ThreadHandler()
{
	mNumBaseVoxels = 0;
	mVisualSize = 1.0;
	mLoretaWidget = loretaWidget;
	mConductivity = 0.0;
	mRegParameter = 0.0;
	mCenter = Vector3(0, 0, 0);
	mLoreta = new Loreta(this);
}


LoretaThreadHandler::~LoretaThreadHandler()
{
	mCalcLoreta = false;
	delete mLoreta;
}


void LoretaThreadHandler::Execute()
{
	const uint32 numElectrodes = mElectrodes.Size();
	// E x 1 matrix (E = amount of active electrodes)
	mMatrixPhi = cv::Mat(numElectrodes, 1, CV_64F, double(0));
	mLoreta->SetMatrixPhi(mMatrixPhi);
	
	// precaluclate loreta values
	mLoreta->PrecalculateLoretaMatrices();

	while (mCalcLoreta)
	{

		// start timing
		mLoretaFpsCounter.BeginTiming();
		mLoreta->CalcLoreta();
		// end timing
		mLoretaFpsCounter.StopTiming();

		// save data into members, so the widget can get the data
		// mMatrixJCaret is passed for the dipole debug field
		mMatrixJCaret = mLoreta->GetMatrixJCaret();
		// mLoretaResult contains the result
		mLoretaResult = mLoreta->GetLoretaResult();
	}
}


void LoretaThreadHandler::Terminate()
{
	mCalcLoreta = false;
}


void LoretaThreadHandler::Reset()
{
	mElectrodes.Clear();
	mChannels.Clear();
	mVoxels.Clear();
	mMaskingMatrix.Clear();
}


// add electrode channel
void LoretaThreadHandler::AddElectrode(const EEGElectrodes::Electrode& electrode, Channel<double>* channel)
{
	mElectrodes.Add(electrode);
	mChannels.Add(channel);
}


void LoretaThreadHandler::Init()
{
	//  create all voxels
	CreateVoxels();
	const uint32 numVoxels		= mVoxels.Size();
	// init show-all masking matrix
	mMaskingMatrix.Resize(numVoxels);
}


// construct the voxel array
void LoretaThreadHandler::CreateVoxels()
{
	uint32 surfaceSize = Math::Pow(mNumBaseVoxels, 2);
	uint32 numVoxels = Math::Pow(mNumBaseVoxels, 3);
	uint32 numElectrodes = mElectrodes.Size();
	LogDebug("Created %i voxels and %i electrodes", numVoxels, numElectrodes);

	double visualSizeHalf = mVisualSize * 0.5;
	double voxelSize = (mNumBaseVoxels > 0 ? mVisualSize / mNumBaseVoxels : 0);
	double halfVoxelSize = voxelSize * 0.5;

	// allocate voxels
	mVoxels.Resize(numVoxels);

	for (uint32 y = 0; y<mNumBaseVoxels; ++y)
	{
		for (uint32 z = 0; z<mNumBaseVoxels; ++z)
		{
			for (uint32 x = 0; x<mNumBaseVoxels; ++x)
			{
				// calculate the position of the voxel based on the size of a voxel
				float xVoxelCenter = mCenter.x - visualSizeHalf + (x * voxelSize + halfVoxelSize);
				float yVoxelCenter = mCenter.y - visualSizeHalf + (y * voxelSize + halfVoxelSize);
				float zVoxelCenter = mCenter.z - visualSizeHalf + (z * voxelSize + halfVoxelSize);
				uint32 voxelIndex = CalcVoxelIndex(x, y, z);
				Vector3 voxelCenter = Vector3(xVoxelCenter, yVoxelCenter, zVoxelCenter);

				// create the bounding box and fill it into an array
				AABB voxelBox;
				voxelBox.Init();
				voxelBox.Add(voxelCenter + Vector3(halfVoxelSize, halfVoxelSize, halfVoxelSize));
				voxelBox.Add(voxelCenter - Vector3(halfVoxelSize, halfVoxelSize, halfVoxelSize));
				mVoxels[voxelIndex] = Voxel(voxelBox, Vector4(1.0f, 0.0f, 0.0f, 0.1f));
				Vector3 voxelBoxMiddle = voxelBox.GetCenter();

				float middleVoxelBoxX = voxelBoxMiddle.x;
				float middleVoxelBoxY = voxelBoxMiddle.y;
				float middleVoxelBoxZ = voxelBoxMiddle.z;

			}
		}
	}
}


// calculate the masking for a given mesh
void LoretaThreadHandler::CalcMaskingMatrix(Mesh* mesh)
{
	// create masking matrix
	uint32 surfaceSize = Math::Pow(mNumBaseVoxels, 2);
	uint32 numVoxels = Math::Pow(mNumBaseVoxels, 3);
	uint32 numElectrodes = mElectrodes.Size();

	// reset mask array
	mMaskingMatrix.Resize(numVoxels);

	for (uint32 i = 0; i<numVoxels; ++i)
	{
		mMaskingMatrix[i]   = false;
	}

	// calculate the vertex-based axis aligned bounding box
	AABB boundingBox;
	mesh->CalcAABB(&boundingBox);
	const float AABBDepth = boundingBox.GetDepth();
	const float cubeSize = AABBDepth / mNumBaseVoxels;

	double visualSizeHalf = mVisualSize * 0.5;
	double voxelSize = mVisualSize / mNumBaseVoxels;
	double halfVoxelSize = voxelSize * 0.5;

	// unmask on all visible voxels
	for (uint32 y = 0; y<mNumBaseVoxels; ++y)
	{
		for (uint32 z = 0; z<mNumBaseVoxels; ++z)
		{
			for (uint32 x = 0; x<mNumBaseVoxels; ++x)
			{
				uint32 voxelIndex = CalcVoxelIndex(x, y, z);

				// get the bounding box from voxel
				AABB voxelBox = mVoxels[voxelIndex].GetVoxelAABB();

				Vector3 voxelBoxMiddle = voxelBox.GetCenter();

				float middleVoxelBoxX = voxelBoxMiddle.x;
				float middleVoxelBoxY = voxelBoxMiddle.y;
				float middleVoxelBoxZ = voxelBoxMiddle.z;

				Ray rayUp   = Ray(Vector3(middleVoxelBoxX, middleVoxelBoxY, middleVoxelBoxZ), Vector3(middleVoxelBoxX, mVisualSize * 1.1, middleVoxelBoxZ));
				Ray rayDown = Ray(Vector3(middleVoxelBoxX, middleVoxelBoxY, middleVoxelBoxZ), Vector3(middleVoxelBoxX, mVisualSize * -1.1, middleVoxelBoxZ));

				if (CheckIfMeshIsInside(mesh, &rayUp, &rayDown) == true)
					mMaskingMatrix[voxelIndex] = true;

			}
		}
	}
}


uint32 LoretaThreadHandler::CalcVoxelIndex(uint32 xIndex, uint32 yIndex, uint32 zIndex)
{
	return xIndex + (zIndex * mNumBaseVoxels) + (yIndex * mNumBaseVoxels * mNumBaseVoxels);
}


// calculate voxel values (use sLoreta, for now)
void LoretaThreadHandler::Update() 
{
	const uint32 numElectrodes	 = mChannels.Size();
	const uint32 numVoxels		 = mVoxels.Size();

	// check wether Loreta can operate or not
	if (mNumBaseVoxels == 0)
		return;

	double value;

	// 1) get last values from channel, construct dipole moments and fill the input matrices
	for (uint32 i = 0; i < numElectrodes; ++i)
	{
		// use zero values if channels has no samples
		if (mChannels[i]->GetNumSamples() == 0)
			value = 0;
		else
			value = mChannels[i]->GetLastSample();

		if (mMatrixPhi.rows == numElectrodes)
			mMatrixPhi.at<double>(i, 0) = value;
	}
}


// check if mesh is inside the voxel space
bool LoretaThreadHandler::CheckIfMeshIsInside(Mesh* mesh, Ray* rayUp, Ray* rayDown)
{
	bool rayUpIntersects = false;
	bool rayDownIntersects = false;
	bool isInsideUp = false;
	bool isInsideDown = false;

	const Core::Array<Core::Vector3>& allMeshVertices = mesh->GetVertices();
	uint32 numMeshVertices = allMeshVertices.Size();

	for (uint32 i=0; i<numMeshVertices; i+=3)
	{
		Vector3 p1 = allMeshVertices[i + 0];
		Vector3 p2 = allMeshVertices[i + 1];
		Vector3 p3 = allMeshVertices[i + 2];

		rayUpIntersects   = CheckRayIntersection(*rayUp, p1, p2, p3);
		if (rayUpIntersects)
			isInsideUp = true;

		rayDownIntersects = CheckRayIntersection(*rayDown, p1, p2, p3);
		if (rayDownIntersects)
			isInsideDown = true;

	    if (isInsideUp && isInsideDown)
		    return true;
	}
	return false;
}

// copypasted with minor changes
bool LoretaThreadHandler::CheckRayIntersection(const Core::Ray& ray, const Core::Vector3& v0, const Core::Vector3& v1, const Core::Vector3& v2)
{
	Core::Vector3 e1 = v1 - v0;
	Core::Vector3 e2 = v2 - v0;
	//find perpendicular vector between ray direction and one of the sides
	Core::Vector3 p = ray.Direction().Cross(e2);
	//take the dot product between this vector and the other side of the triangle
	float a = e1.Dot(p);
	//if a is equal to zero then the ray is parallel with the triangle and no intersection ocurs
	if (a == 0 || (a < 0.001f && a > -0.001f))
	{
		return false;
	}
	//compute denominator
	float f = 1.0f / a;
	//compute barycentric coordinates and check if they are within the accepted boundaries
	Core::Vector3 s = ray.Origin() - v0;
	float v = f * s.Dot(p);

	if (v < 0.0f || v > 1.0f)
		return false;

	Core::Vector3 q = s.Cross(e1);
	float w = f * ray.Direction().Dot(q);

	if (w < 0.0f || w + v > 1.0f)
		return false;
	return true;
}


#endif
