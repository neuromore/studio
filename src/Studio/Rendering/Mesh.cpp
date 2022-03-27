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

// include required headers
#include "Mesh.h"
#include <tinyobjloader/tiny_obj_loader.h>
#include <Core/String.h>
#include <Core/LogManager.h>
#include <Core/Timer.h>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <sstream>
#ifdef NEUROMORE_PLATFORM_WINDOWS
#include <gl/glu.h>
#include <gl/gl.h>
#endif
#ifdef NEUROMORE_PLATFORM_OSX
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#endif
#ifdef NEUROMORE_PLATFORM_LINUX
#include <GL/glu.h>
#include <GL/gl.h>
#endif

using namespace Core;

// constructor
Mesh::Mesh()
{
	mColor = Core::Vector3 (0.5f, 0.5f, 0.5f);

//	if (mVertexBuffer.create() == false)
//		LogError("OpenGL: Cannot create vertex buffer.");
}


// destructor
Mesh::~Mesh()
{
//	mVertexBuffer.destroy();
}


void Mesh::Clear()
{
	mVertices.Clear();
	mNormals.Clear(); 
	mTexCoords.Clear();
	mColor.Set(1, 1, 0);
}


// load the mesh from an .obj file
bool Mesh::Load(const char* objFilename)
{
	Timer objLoadTimer;
	LogInfo("Loading .OBJ 3D model ...");

	// load the file
	QFile objFile(objFilename);
	if (objFile.open(QIODevice::ReadOnly | QIODevice::Text) == false)
	{
		LogError( "Mesh::Load(): Cannot read .OBJ file from '%s'.", objFilename );
		return false;
	}

	// put everything into a string
	std::string objString = objFile.readAll().data();

	// create the standard input stream
    std::istringstream inputStream( objString );

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	tinyobj::MaterialFileReader matFileReader( std::string("") );  
	
	// load the obj file
	std::string error = tinyobj::LoadObj(shapes, materials, inputStream, matFileReader);
	if (error.empty() == false)
	{
		LogError( "Mesh::Load(): Error loading .OBJ file '%s': %s", objFilename, error.data() );
		return false;
	}

	// clear arrays
	Clear();

	// get the number of faces
	uint32 numVertices = 0;
	const uint32 numShapes = (uint32)shapes.size();
	for (uint32 i=0; i<numShapes; i++)
		numVertices += (uint32)shapes[i].mesh.indices.size();

	// grow arrays
	mVertices.Reserve( numVertices );
	mNormals.Reserve( numVertices );
	mTexCoords.Reserve( numVertices );
	mVertices.Reserve( numVertices );

	for (uint32 s=0; s<numShapes; s++)
	{
		//LogDebug("shape[%ld].name = %s\n", s, shapes[s].name.c_str());
		//LogDebug("shape[%ld].indices: %ld\n", s, shapes[s].mesh.indices.size());
		const uint32 numIndices = (uint32)shapes[s].mesh.indices.size();
		CORE_ASSERT((numIndices % 3) == 0);
		for (uint32 i=0; i<numIndices; i++)
		{
			//LogDebug("  idx[%ld] = %d\n", i, shapes[s].mesh.indices[f]);

			const uint32 vtxIndex = 3 * shapes[s].mesh.indices[i];

			const Vector3 position	= Vector3( shapes[s].mesh.positions[vtxIndex+0], shapes[s].mesh.positions[vtxIndex+1], shapes[s].mesh.positions[vtxIndex+2] );
			const Vector3 normal	= Vector3( shapes[s].mesh.normals[vtxIndex+0], shapes[s].mesh.normals[vtxIndex+1], shapes[s].mesh.normals[vtxIndex+2] );
			
			// no texture coordinate support yet!
			const Vector2 uvCoord	= Vector2();// shapes[s].mesh.texcoords[vtxIndex+0], shapes[s].mesh.texcoords[vtxIndex+1] );

			mVertices.Add( position );
			mNormals.Add( normal );
			mTexCoords.Add( uvCoord );

			//mVertexArray.Add( Vertex(position, normal, uvCoord) );
		}
	}

	// calculate face normals and overwrite the vertex normals with the newly calculated face normals
	CalcNormals();

	/*mVertexBuffer.bind();
	mVertexBuffer.allocate( mVertexArray.GetReadPtr(), numVertices * sizeof(Vertex) );

	InitShaders();*/

	const float objLoadTime = objLoadTimer.GetTime().InSeconds();
	LogInfo( "Successfully loaded '%s' .OBJ 3D model in %.2f ms", objFilename, objLoadTime * 1000.0f );
	return true;
}


bool Mesh::InitShaders()
{
	/*LogDetailedInfo("Initializing shaders for mesh...");

	// compile vertex shader
	const char* vertexShaderString = "uniform mat4 viewProjMatrix; attribute vec4 vertex; void main() { gl_Position = viewProjMatrix * vertex; }";
	if (mShader.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderString) == false)
	{
		LogError("Mesh::Load(): Cannot compile vertex shader.");
		return false;
	}

	// compile fragment shader
	const char* fragmentShaderString = "void main() { gl_FragColor = vec4(1.0,0.4,0.8,1.0); }";
	if (mShader.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderString) == false)
	{
		LogError("Mesh::Load(): Cannot compile fragment shader.");
		return false;
	}

	// link shader pipeline and bind it
	if (mShader.link() == false)
	{
		LogError("Mesh::Load(): Cannot link shader pipeline.");
		return false;
	}
	
	if (mShader.bind() == false)
	{
		LogError("Mesh::Load(): Cannot bind shader.");
		return false;
	}*/

	return true;
}


// calculate the face normals and overwrite the existing vertex normals
void Mesh::CalcNormals()
{
	Core::Vector3 a, b, c, faceNormal;

	// iterate over all triangles and calculate the face normals
	const uint32 numVertices = mVertices.Size();
	for (uint32 i=0; i<numVertices; i+=3)
	{
		a = mVertices[i+0];
		b = mVertices[i+1];
		c = mVertices[i+2];

		faceNormal = (b-a).Cross(c-a);
		//faceNormal.Normalize(); // TODO: hack, enable this again, but doesn't look as nice ;)

		mNormals[i+0] = faceNormal;
		mNormals[i+1] = faceNormal;
		mNormals[i+2] = faceNormal;
	}
}


// render the mesh
void Mesh::Render(QOpenGLFunctions* oglFunctions, const QMatrix4x4 viewProjMat)
{
	// remember the current render flags and set all needed ones
	bool oldLightingEnabled = glIsEnabled(GL_LIGHTING);
	glEnable(GL_LIGHTING);

	// simple version
	/*glBegin(GL_TRIANGLES);
	for (uint32 i=0; i < mVertices.Size(); i++)
	{
		if (HasTexCoord())	glTexCoord2f(mTexCoords[i].x, mTexCoords[i].y);
		if (HasNormal())	glNormal3f(mNormals[i].x, mNormals[i].y, mNormals[i].z);
		if (HasVertex())	glVertex3f(mVertices[i].x, mVertices[i].y, mVertices[i].z);
	}
	glEnd();*/

	// optimized version
	if (HasVertex())	glEnableClientState( GL_VERTEX_ARRAY );
	if (HasNormal())	glEnableClientState( GL_NORMAL_ARRAY );
	if (HasTexCoord())	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	if (HasVertex())	glVertexPointer( 3, GL_FLOAT, 0, mVertices.GetReadPtr() );
	if (HasNormal())	glNormalPointer( GL_FLOAT, 0, mNormals.GetReadPtr() );
	if (HasTexCoord())	glTexCoordPointer( 2, GL_FLOAT, 0, mTexCoords.GetReadPtr() );

	glDrawArrays( GL_TRIANGLES, 0, mVertices.Size() );

	if (HasVertex())	glDisableClientState( GL_VERTEX_ARRAY );
	if (HasNormal())	glDisableClientState( GL_NORMAL_ARRAY );
	if (HasTexCoord())	glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	/*mShader.bind();
	mShader.setUniformValue("viewProjMatrix", viewProjMat);

	int offset = 0;

	mVertexBuffer.bind();

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = mShader.attributeLocation("vertex");
    mShader.enableAttributeArray(vertexLocation);
    mShader.setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, sizeof(Vertex));

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

	glDrawArrays( GL_TRIANGLES, 0, mVertices.Size() );*/

	// set the render flags back to their old state
	if (oldLightingEnabled == true)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);
}


// calculate the axis aligned bounding box of the mesh
void Mesh::CalcAABB(Core::AABB* outBoundingBox)
{
	// init the bounding box
	outBoundingBox->Init();

	// process all vertices
	const uint32 numVerts = mVertices.Size();
	for (uint32 i=0; i<numVerts; ++i)
		outBoundingBox->Add( mVertices[i] );
}

// check for ray intersection
const bool Mesh::CheckRayIntersection(Ray* ray, Core::Vector3 p1, Core::Vector3 p2, Core::Vector3 p3) const
{
	return ray->Intersects(p1, p2, p3);
}