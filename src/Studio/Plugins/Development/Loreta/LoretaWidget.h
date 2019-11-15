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
#ifndef LORETAWIDGET_H
#define LORETAWIDGET_H


#include "../../../Config.h"
#ifdef OPENCV_SUPPORT
#include "../../../Rendering/OpenGLWidget.h"
#include "Core/Array.h"
#include "Core/Math.h"
#include "Loreta.h"
#include "ColorMapper.h"
#include "../QtBase/SDKs/stb/stb_image.h"
#include "qfile.h"
#include "LoretaThreadHandler.h"
#include "QtBaseManager.h"
#include "qimage.h"
#include "qimagewriter.h"

// forward declaration
class LoretaPlugin;

class LoretaWidget : public OpenGLWidget
{
	Q_OBJECT
	friend class RenderCallback;

	public:

		// constructor & destructor
		LoretaWidget(LoretaPlugin* plugin, QWidget* parent);
		virtual ~LoretaWidget();

		void ReInit();

		// render frame
		void paintGL() override final;
		void resizeGL(int width, int height) override;

		void ViewCloseup();

	private:
		class RenderCallback : public OpenGLWidgetCallback
		{
			public:

				RenderCallback(LoretaWidget* parent);
				~RenderCallback();

				void Render(uint32 index, bool isHighlighted, double x, double y, double width, double height) override;

			private:
				LoretaWidget*						  mParent;
				
				// array for sorting voxels while preserving their original index
				struct VoxelIndexPair
				{
					VoxelIndexPair() {}
					~VoxelIndexPair() {}

					Voxel*		mVoxel;
					uint32		mIndex;
				};

				struct LoretaVertex
				{
					Core::Vector3		mPosition;
					Core::Color     mColor;
					Core::Vector2		mTexCoord;
					Core::Vector3		mNormal;
				};

				Core::Array <VoxelIndexPair> mSortedVoxels;
				
				void InitRenderMeshes();
				void DepthSortVoxels();

				// head mesh
				Mesh*		mHeadMesh;
				Mesh*		mBrainMesh;
				Core::AABB	mHeadAABB;
				Core::AABB  mBrainAABB;

				Core::FpsCounter			mRenderFpsCounter;
				Core::FpsCounter			mSortFpsCounter;
				Core::Array<LoretaVertex>	mLoretaVertices;
		};

		#ifdef LORETA_DEBUG
		// getter for the attributes from the plugin
		bool IsDebugGridEnabled();
		#endif

		LoretaPlugin*				mPlugin;
		RenderCallback*				mRenderCallback;
		int							mStbiWidth;
		int							mStbiHeight;
		int							mStbiComp;
		GLuint						mTexture;
		unsigned char*				mImage;
		Core::Array<unsigned char>	mImageData;
};

#endif

#endif
