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

// include precompiled header
#include <Studio/Precompiled.h>

#define STB_IMAGE_IMPLEMENTATION
#include "LoretaWidget.h"
#include <QOpenGLFunctions>
#include <qwindow.h>
#include "LoretaPlugin.h"
#include "Loreta.h"


#ifdef OPENCV_SUPPORT

using namespace Core;


//------------------------------------------------------------------------//
//--------------------Sorting Function------------------------------------//
//------------------------------------------------------------------------//
// static value for retrieving camera
//static OrbitCamera* camera;

// sort function
template <class T>
static int32 CORE_CDECL VoxelCameraDistanceCompare(const T& pairA, const T& pairB)
{
	// TODO: recode without orbit camera
	return -1;

	/*const float distanceA = (pairA.mVoxel->GetVoxelAABB().GetCenter() - camera->GetPosition()).Length();
	const float distanceB = (pairB.mVoxel->GetVoxelAABB().GetCenter() - camera->GetPosition()).Length();

	if (distanceA > distanceB)
		return -1;
	else if (distanceA < distanceB)
		return 1;
	else
		return 0;*/
}
//------------------------------------------------------------------------//


// constructor
LoretaWidget::LoretaWidget(LoretaPlugin* plugin, QWidget* parent) : mRenderCallback(NULL), OpenGLWidget(parent)
{
	mPlugin = plugin;
	// create the render callback
	mRenderCallback = new RenderCallback(this);
	SetCallback(mRenderCallback);
	mEmptyText = "This is the text shown in case no split view is rendered (e.g. in case no classifier or device is active)";
}


// destructor
LoretaWidget::~LoretaWidget()
{
	// destroy the render callback
	delete mRenderCallback;
}


// render frame
void LoretaWidget::paintGL()
{
	// initialize the painter and get the font metrics
	QPainter painter(this);
	mRenderCallback->SetPainter( &painter );
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::HighQualityAntialiasing);

	// pre rendering
	if (PreRendering() == false)
		return;

	const uint32 numViews = 9;
	const uint32 numViewsPerColumn = 3;

	// render single or multi view
	Render();

	// post rendering
	PostRendering();
}


void LoretaWidget::resizeGL(int width, int height)
{
	OpenGLWidget::resizeGL(width, height);
	ViewCloseup();
}


void LoretaWidget::ViewCloseup()
{
	// zoom to the mesh AABB
	//const AABB& brainAABB = mPlugin->GetBrainMeshAABB();
	//if (mCamera != NULL && mRenderCallback != NULL && brainAABB.IsValid() == true)
	//	mCamera->ViewCloseup(brainAABB, 1.0);
}


LoretaWidget::RenderCallback::RenderCallback(LoretaWidget* parent) : OpenGLWidgetCallback(parent)
{
	LogDebug("Creating RenderCallback...");

	mParent = parent;

	// on default, zoom to our mesh
	mParent->ViewCloseup();

	// load the brain
	mHeadMesh = new Mesh();
	mHeadAABB.Init();
	if (mHeadMesh->Load(":/Assets/Models/Head.obj") == false)
	{
		delete mHeadMesh;
		mHeadMesh = NULL;
	}
	else
		mHeadMesh->CalcAABB( &mHeadAABB );

	mBrainMesh = new Mesh();
	mBrainAABB.Init();
	if (mBrainMesh->Load(":/Assets/Models/Brain.obj") == false)
	{
		delete mBrainMesh;
		mBrainMesh = NULL;
	}
	else
		mBrainMesh->CalcAABB( &mBrainAABB );

}


// destructor
LoretaWidget::RenderCallback::~RenderCallback()
{
	delete mHeadMesh;
	delete mBrainMesh;
}


// render callback
void LoretaWidget::RenderCallback::Render(uint32 index, bool isHighlighted, double x, double y, double width, double height)
{

	
	// get the time delta since the last render call
	const double timeDelta = mParent->GetTimeDelta();

	// base class render
	OpenGLWidgetCallback::Render(index, isHighlighted, x, y, width, height);

	// get LORETA from plugin
	LoretaThreadHandler* loretaThreadHandler = mParent->mPlugin->GetLoretaThreadHandler();

	// FIXME do not do this every frame, only if classifier has changed! not possible right now, see Node in header
	mParent->mPlugin->ReInitLoreta();
	
	// perform LORETA on latest values
	loretaThreadHandler->Update();

	// set the viewport to the multi view area
	// NOTE: Retina Support: glViewport expects device pixels, but the various geometry accessors returns values in device-independent pixels
	const double devicePixelRatio = mParent->devicePixelRatio();
	mParent->glViewport(mOffsetX * devicePixelRatio, mOffsetY * devicePixelRatio, width * devicePixelRatio, height * devicePixelRatio);
	
	// TODO: use QMatrix4x4 here and its lookat etc.

	// set camera
	//mParent->mCamera->SetAspectRatio( height / width );
	//mParent->mCamera->SetScreenDimensions( width, height );
	//mParent->mCamera->Update();

	// set the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glLoadMatrixf(mParent->mCamera->GetProjectionMatrix().m16);

	// set the camera matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glLoadMatrixf(mParent->mCamera->GetViewMatrix().m16);

	mSortFpsCounter.BeginTiming();
	// depth-sort voxels for display (fills mSortingMap)
	DepthSortVoxels();
	mSortFpsCounter.StopTiming();

	// loreta is not initialized -> render notification text instead of voxels
	if (loretaThreadHandler->GetNumBaseVoxel() == 0)
	{
		RenderText( "Please select a classifier with a LORETA node", GetOpenGLWidget()->GetDefaultFontSize(), Color(1.0f, 1.0f, 0.0f), (float)width*0.5f, (float)height*0.5, OpenGLWidget::ALIGN_MIDDLE | OpenGLWidget::ALIGN_CENTER );
	}
	else
	{
		// start render logging
		mRenderFpsCounter.BeginTiming();

		// disable depth testing so that manual sorted alpha blending can work
		mParent->glDisable(GL_DEPTH_TEST);

		// enable alpha blending
		mParent->glEnable(GL_BLEND);
		mParent->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// enable backface culling, font faces are counter-clock wise
		::glEnable(GL_CULL_FACE);
		::glFrontFace(GL_CCW);
		::glEnable(GL_TEXTURE_2D);

		const uint32 numVoxels   = loretaThreadHandler->GetVoxels().Size();
		const uint32 numVertices = numVoxels * 4 * 6;
		mLoretaVertices.Resize(numVertices);

		#ifdef LORETA_DEBUG
		// debugmode for cubes
		if (mParent->IsDebugGridEnabled())
		{
			//RenderAxis(1, Vector3(0.0, 0.0, 0.0), Vector3(1.0, 0.0, 0.0), Vector3(0.0, 1.0, 0.0), Vector3(0.0, 0.0, 1.0));
			RenderAABB(loretaThreadHandler->GetAABB(), Color(0, 0, 1));
			for (uint32 i = 0; i < numVoxels; ++i)
			{
				const uint32 mappedIndex = mSortedVoxels[i].mIndex;
				Voxel* voxel = mSortedVoxels[i].mVoxel;

				AABB voxelBox = loretaThreadHandler->GetVoxel(i)->GetVoxelAABB();
				Color color = Color(0.5 + i * (1.0 / numVoxels) * 0.5, 0.0, 0.0, 1.0);
				RenderAABB(voxelBox, color);

				// render arrows
				cv::Vec3d currentVector = loretaThreadHandler->GetJCaret().at<cv::Vec3d>(mappedIndex, 0);
				Vector3 arrowDirection  = Vector3(currentVector[0], currentVector[1], currentVector[2]);
				double magnitude		= Math::SqrtD(Math::PowD(arrowDirection.x, 2) + Math::PowD(arrowDirection.y, 2) + Math::PowD(arrowDirection.z, 2));

				// draw arrow for debugging
				//RenderArrow(voxel->GetVoxelAABB().GetCenter(), arrowDirection, magnitude, Color(1.0f));
				//RenderLines();
			}
		}

		// preparations for TIFF file export
		const uint32 numBaseVoxel = mParent->mPlugin->GetNumVoxelSpinner();
		const uint32 numBaseVoxelSquared = Math::Pow(numBaseVoxel, 2);
		QImage tiffImage(numBaseVoxelSquared, numBaseVoxelSquared, QImage::Format_Grayscale8);
		
		uint32 tiffDirCount = 0;
		uint32 tiffRow      = 0;
		uint32 tiffCol      = 0;
		#endif

		const Array<bool>& mask = loretaThreadHandler->GetMaskingMatrix();
		for (uint32 i = 0; i < numVoxels; ++i)
		{
			// map depthsorted index to unsorted voxel index
			const uint32 mappedIndex = mSortedVoxels[i].mIndex;
			Voxel* voxel = mSortedVoxels[i].mVoxel;

			// map sorted index to unsorted
			if (mask[mappedIndex] == true)
			{
				const AABB voxelBox = voxel->GetVoxelAABB();
				const Vector3 boundingBoxMiddle = voxelBox.GetCenter();

				float xPointPositive = boundingBoxMiddle.x + voxelBox.GetWidth() * 0.5;
				float xPointNegative = boundingBoxMiddle.x - voxelBox.GetWidth() * 0.5;
				float yPointPositive = boundingBoxMiddle.y + voxelBox.GetHeight() * 0.5;
				float yPointNegative = boundingBoxMiddle.y - voxelBox.GetHeight() * 0.5;
				float zPointPositive = boundingBoxMiddle.z + voxelBox.GetDepth() * 0.5;
				float zPointNegative = boundingBoxMiddle.z - voxelBox.GetDepth() * 0.5;

				double currentMagnitude = 0.0;
				cv::Mat loretaResult = loretaThreadHandler->GetLoretaResult();
				if (!loretaResult.empty())
					currentMagnitude = loretaResult.at<double>(mappedIndex);
				
				// get color from magnitude
				if (currentMagnitude > 1.0)
					currentMagnitude = 1.0;

				Color color = mParent->mPlugin->GetColorMapper().CalcColor(currentMagnitude);

				Vector4 voxelValue = Vector4(color.r, color.g, color.b, (1.0 - mParent->mPlugin->GetTransparency()) * currentMagnitude);
				voxel->SetVoxelValue(voxelValue);

				#ifdef LORETA_DEBUG
				if (mParent->mPlugin->GetTIFFExportCheckBox())
				{
					uint32 convertedVoxelGrayScaleValue = (voxelValue.x + voxelValue.y + voxelValue.z) / 3;
					if ((tiffRow * tiffCol) < numBaseVoxelSquared)
					{
						
						if (tiffCol < numBaseVoxel)
						{
							tiffImage.setPixel(tiffCol, tiffRow, convertedVoxelGrayScaleValue);
							tiffCol++;
						}
						else
						{
							tiffRow++;
							tiffCol = 0;
						}
					}
					else
					{
						// TODO: save image as multipage tiff

						tiffRow = 0;
						tiffCol = 0;
					}
				}
				#endif

				Vector4 renderColor = voxel->GetVoxelValue();
				uint32  vertexIndex = i * 6 * 4;

				for (uint32 j=vertexIndex;j<(vertexIndex + 6 * 4);++j)
				{
					mLoretaVertices[j].mColor = Color(renderColor.x, renderColor.y, renderColor.z, renderColor.w);
				}

				// vertices for voxels
				// top
				mLoretaVertices[vertexIndex + 0].mPosition  = Vector3(xPointPositive, yPointPositive, zPointNegative);
				mLoretaVertices[vertexIndex + 1].mPosition  = Vector3(xPointNegative, yPointPositive, zPointNegative);
				mLoretaVertices[vertexIndex + 2].mPosition  = Vector3(xPointNegative, yPointPositive, zPointPositive);
				mLoretaVertices[vertexIndex + 3].mPosition  = Vector3(xPointPositive, yPointPositive, zPointPositive);
							
				// bottom
				mLoretaVertices[vertexIndex + 4].mPosition  = Vector3(xPointPositive, yPointNegative, zPointPositive);
				mLoretaVertices[vertexIndex + 5].mPosition  = Vector3(xPointNegative, yPointNegative, zPointPositive);
				mLoretaVertices[vertexIndex + 6].mPosition  = Vector3(xPointNegative, yPointNegative, zPointNegative);
				mLoretaVertices[vertexIndex + 7].mPosition  = Vector3(xPointPositive, yPointNegative, zPointNegative);
									
				// back
				mLoretaVertices[vertexIndex + 8].mPosition   = Vector3(xPointPositive, yPointPositive, zPointPositive);
				mLoretaVertices[vertexIndex + 9].mPosition   = Vector3(xPointNegative, yPointPositive, zPointPositive);
				mLoretaVertices[vertexIndex + 10].mPosition  = Vector3(xPointNegative, yPointNegative, zPointPositive);
				mLoretaVertices[vertexIndex + 11].mPosition  = Vector3(xPointPositive, yPointNegative, zPointPositive);
											
				// front
				mLoretaVertices[vertexIndex + 12].mPosition = Vector3(xPointPositive, yPointNegative, zPointNegative);
				mLoretaVertices[vertexIndex + 13].mPosition = Vector3(xPointNegative, yPointNegative, zPointNegative);
				mLoretaVertices[vertexIndex + 14].mPosition = Vector3(xPointNegative, yPointPositive, zPointNegative);
				mLoretaVertices[vertexIndex + 15].mPosition = Vector3(xPointPositive, yPointPositive, zPointNegative);
				
				// left
				mLoretaVertices[vertexIndex + 16].mPosition = Vector3(xPointNegative, yPointPositive, zPointPositive);
				mLoretaVertices[vertexIndex + 17].mPosition = Vector3(xPointNegative, yPointPositive, zPointNegative);
				mLoretaVertices[vertexIndex + 18].mPosition = Vector3(xPointNegative, yPointNegative, zPointNegative);
				mLoretaVertices[vertexIndex + 19].mPosition = Vector3(xPointNegative, yPointNegative, zPointPositive);
						
				// right
				mLoretaVertices[vertexIndex + 20].mPosition = Vector3(xPointPositive, yPointPositive, zPointNegative);
				mLoretaVertices[vertexIndex + 21].mPosition = Vector3(xPointPositive, yPointPositive, zPointPositive);
				mLoretaVertices[vertexIndex + 22].mPosition = Vector3(xPointPositive, yPointNegative, zPointPositive);
				mLoretaVertices[vertexIndex + 23].mPosition = Vector3(xPointPositive, yPointNegative, zPointNegative);


				// normal vectors
				// top
				mLoretaVertices[vertexIndex + 0].mNormal  = Vector3(0.0f, yPointPositive, 0.0f);
				mLoretaVertices[vertexIndex + 1].mNormal  = Vector3(0.0f, yPointPositive, 0.0f);
				mLoretaVertices[vertexIndex + 2].mNormal  = Vector3(0.0f, yPointPositive, 0.0f);
				mLoretaVertices[vertexIndex + 3].mNormal  = Vector3(0.0f, yPointPositive, 0.0f);

				// bottom
				mLoretaVertices[vertexIndex + 4].mNormal  = Vector3(0.0f, yPointNegative, 0.0f);
				mLoretaVertices[vertexIndex + 5].mNormal  = Vector3(0.0f, yPointNegative, 0.0f);
				mLoretaVertices[vertexIndex + 6].mNormal  = Vector3(0.0f, yPointNegative, 0.0f);
				mLoretaVertices[vertexIndex + 7].mNormal  = Vector3(0.0f, yPointNegative, 0.0f);

				// back
				mLoretaVertices[vertexIndex + 8].mNormal  = Vector3(0.0f, 0.0f, zPointPositive);
				mLoretaVertices[vertexIndex + 9].mNormal  = Vector3(0.0f, 0.0f, zPointPositive);
				mLoretaVertices[vertexIndex + 10].mNormal = Vector3(0.0f, 0.0f, zPointPositive);
				mLoretaVertices[vertexIndex + 11].mNormal = Vector3(0.0f, 0.0f, zPointPositive);
															  
				// front
				mLoretaVertices[vertexIndex + 12].mNormal = Vector3(0.0f, 0.0f, zPointNegative);
				mLoretaVertices[vertexIndex + 13].mNormal = Vector3(0.0f, 0.0f, zPointNegative);
				mLoretaVertices[vertexIndex + 14].mNormal = Vector3(0.0f, 0.0f, zPointNegative);
				mLoretaVertices[vertexIndex + 15].mNormal = Vector3(0.0f, 0.0f, zPointNegative);

				// left
				mLoretaVertices[vertexIndex + 16].mNormal = Vector3(xPointNegative, 0.0f, 0.0f);
				mLoretaVertices[vertexIndex + 17].mNormal = Vector3(xPointNegative, 0.0f, 0.0f);
				mLoretaVertices[vertexIndex + 18].mNormal = Vector3(xPointNegative, 0.0f, 0.0f);
				mLoretaVertices[vertexIndex + 19].mNormal = Vector3(xPointNegative, 0.0f, 0.0f);
												  
				// right							  
				mLoretaVertices[vertexIndex + 20].mNormal = Vector3(xPointPositive, 0.0f, 0.0f);
				mLoretaVertices[vertexIndex + 21].mNormal = Vector3(xPointPositive, 0.0f, 0.0f);
				mLoretaVertices[vertexIndex + 22].mNormal = Vector3(xPointPositive, 0.0f, 0.0f);
				mLoretaVertices[vertexIndex + 23].mNormal = Vector3(xPointPositive, 0.0f, 0.0f);
			}
		}

		#ifdef LORETA_DEBUG
		if (mParent->mPlugin->GetTIFFExportCheckBox())
		{
			bool success = tiffImage.save("test.tif", "TIFF");
			//LogDebug("Result from tiffImage.save(): %d", success);
		}
		#endif

		// draw vertex buffer array (replaces old fixed pipeline)
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);

		glVertexPointer(3, GL_FLOAT, sizeof(LoretaVertex), &(mLoretaVertices[0].mPosition));
		glColorPointer(4, GL_FLOAT, sizeof(LoretaVertex), &(mLoretaVertices[0].mColor));
		glTexCoordPointer(2, GL_FLOAT, sizeof(LoretaVertex), &(mLoretaVertices[0].mTexCoord));
		glNormalPointer(GL_FLOAT, sizeof(LoretaVertex), &(mLoretaVertices[0].mNormal));

		mParent->glDrawArrays(GL_QUADS, 0, numVertices);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);

		mParent->glBindTexture(GL_TEXTURE_2D, 0);

		// clear array after everything was drawn
		mLoretaVertices.Clear();

		::glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
		QMatrix4x4 viewProjMat;

		if (mParent->mPlugin->GetRenderBrainCheckbox())
		{
			// render the brain
			if (mBrainMesh != NULL)
				mBrainMesh->Render( mParent, viewProjMat );
		}

		if (mParent->mPlugin->GetRenderHeadCheckbox())
		{
			// render the head
			if (mHeadMesh != NULL)
				mHeadMesh->Render( mParent, viewProjMat );
		}
	}

	// render text
	uint32 numCurrentElectrodes = loretaThreadHandler->GetNumElectrodes();
	for (uint32 i=0;i<numCurrentElectrodes;++i)
	{
		// get electrode positions
		Vector3 electrodePosition3D = GetEEGElectrodes()->Get3DPosition(loretaThreadHandler->GetVisualSize() * 0.5, loretaThreadHandler->GetElectrode(i));
		Vector3 normalizedElectrodePosition = Vector3(electrodePosition3D.x * (-1.0), electrodePosition3D.z, electrodePosition3D.y * (-1.0));

		//RenderSphere(normalizedElectrodePosition, 0.1f, Color(0.0, 1.0, 0.0, 1.0));

	}

	mRenderFpsCounter.StopTiming();

	if (GetQtBaseManager()->GetMainWindow()->GetShowFPS() == true || GetQtBaseManager()->GetMainWindow()->GetShowPerformanceInfo() == true)
	{
		// DEBUG: Render fps on screen
		RenderText( "Render:", GetOpenGLWidget()->GetDefaultFontSize(), Color(1.0f, 1.0f, 1.0f), 3, 25, OpenGLWidget::ALIGN_TOP | OpenGLWidget::ALIGN_LEFT );
		RenderText( mRenderFpsCounter.GetText(), GetOpenGLWidget()->GetDefaultFontSize(), Color(1.0f, 1.0f, 1.0f), 120, 25, OpenGLWidget::ALIGN_TOP | OpenGLWidget::ALIGN_LEFT );

		RenderText( "LORETA Thread:", GetOpenGLWidget()->GetDefaultFontSize(), Color(1.0f, 1.0f, 1.0f), 3, 40, OpenGLWidget::ALIGN_TOP | OpenGLWidget::ALIGN_LEFT );
		RenderText(loretaThreadHandler->GetFpsText(), GetOpenGLWidget()->GetDefaultFontSize(), Color(1.0f, 1.0f, 1.0f), 120, 40, OpenGLWidget::ALIGN_TOP | OpenGLWidget::ALIGN_LEFT);

		RenderText( "Voxel Depth Sort:", GetOpenGLWidget()->GetDefaultFontSize(), Color(1.0f, 1.0f, 1.0f), 3, 55, OpenGLWidget::ALIGN_TOP | OpenGLWidget::ALIGN_LEFT );
		RenderText( mSortFpsCounter.GetText(), GetOpenGLWidget()->GetDefaultFontSize(), Color(1.0f, 1.0f, 1.0f), 120, 55, OpenGLWidget::ALIGN_TOP | OpenGLWidget::ALIGN_LEFT );
	}
}


void LoretaWidget::RenderCallback::DepthSortVoxels()
{
	LoretaThreadHandler* loretaThreadHandler = mParent->mPlugin->GetLoretaThreadHandler();
	uint32 numVoxels = loretaThreadHandler->GetVoxels().Size();

	// setup sorting array
	mSortedVoxels.Resize(numVoxels);
	for (uint32 i = 0; i < numVoxels; ++i)
	{
		// first: voxel reference
		mSortedVoxels[i].mVoxel = loretaThreadHandler->GetVoxel(i);

		// second: unsorted index
		mSortedVoxels[i].mIndex = i;
	}

	// sort voxel by render depth
	mSortedVoxels.Sort(VoxelCameraDistanceCompare);
}


#ifdef LORETA_DEBUG
// getter for the attributes from the plugin
bool LoretaWidget::IsDebugGridEnabled()
{
	return mPlugin->GetDebugCheckbox();
}
#endif

#endif
