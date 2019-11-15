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

#ifndef __MESH_H
#define __MESH_H

// include required headers
#include "../Config.h"
#include <Core/Array.h>
#include <Core/AABB.h>
#include <Core/Vector.h>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QVector2D>
#include <QVector3D>


class Mesh
{
	public:
		// constructor & destructor
		Mesh();
		virtual ~Mesh();

		// main render function
		void Render(QOpenGLFunctions* oglFunctions, const QMatrix4x4 viewProjMat);

		// reset mesh without deallocating anything
		void Clear();

		// load from .obj file
		bool Load(const char* objFilename);
		bool InitShaders();

		// helper functions
		void CalcNormals();
		void CalcAABB(Core::AABB* outBoundingBox);

		// getter
		inline const Core::Array<Core::Vector3>& GetVertices()     { return mVertices; }

	private:
		/*struct Vertex
		{
			QVector3D mPos;
			QVector3D mNormal;
		//	QVector2D mTexCoord;

			Vertex(const Core::Vector3& pos, const Core::Vector3& normal, const Core::Vector2& texCoord)
			{
				mPos		= QVector3D(pos.x, pos.y, pos.z);
				mNormal		= QVector3D(normal.x, normal.y, normal.z);
		//		mTexCoord	= QVector2D(texCoord.x, texCoord.y);
			}
		};

		Core::Array<Vertex>		mVertexArray;
	    QOpenGLBuffer			mVertexBuffer;

		QOpenGLShaderProgram	mShader;*/

		inline bool HasVertex()		{ return !mVertices.IsEmpty(); }
		inline bool HasNormal()		{ return !mNormals.IsEmpty(); }
		inline bool HasTexCoord()	{ return !mTexCoords.IsEmpty(); }

		Core::Array<Core::Vector3>	mVertices;
		Core::Array<Core::Vector3>  mNormals;
		Core::Array<Core::Vector2>  mTexCoords;
		Core::Vector3				mColor;
};


#endif
