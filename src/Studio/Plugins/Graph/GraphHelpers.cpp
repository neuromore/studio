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

// include required headers
#include "GraphHelpers.h"
#include "GraphShared.h"
#include "GraphRenderer.h"
#include <Core/Math.h>
#include <EngineManager.h>


using namespace Core;

// constructor
GraphHelpers::GraphHelpers()
{
	const uint32 size = 2048;

	mSubGridLines.resize( 5 * size);
	mMainGridLines.resize(size);
}

// destructor
GraphHelpers::~GraphHelpers()
{
}


// render the grid
void GraphHelpers::RenderGrid(GraphShared* shared, QPainter& painter, const QTransform& transform, float scale, const QColor& gridColor, const QColor& subGridColor, int32 width, int32 height)
{
	// calculate the alpha
	float alpha = Core::Clamp<float>(scale * scale * scale, 0.0f, 1.0f);

	if (alpha < 0.1f)
		return;

	const float invScale = 1.0 / scale;

	// NOTE: scale ranges from 1.0 (full pixel perfect rendering) to 0.15 (very zoomed out)

	// setup spacing and size of the grid
	const int32 spacing			= shared->GetScreenScaling() * 7.0f; // grid cell size of 20
	const int32 mainLineDist	= shared->GetScreenScaling() * 70.0f;

	// calculate the coordinates in 'zoomed out and scrolled' coordinates, of the window rect
	const QPoint upperLeft	= transform.inverted().map( QPoint(0,0));
	const QPoint lowerRight	= transform.inverted().map( QPoint(width,height) );

	// calculate the start and end ranges in 'scrolled and zoomed out' coordinates
	// we need to render sub-grids covering that area
	const int32 startX	= upperLeft.x() - (upperLeft.x() % mainLineDist) - mainLineDist;
	const int32 startY	= upperLeft.y() - (upperLeft.y() % mainLineDist) - mainLineDist;
	const int32 endX	= lowerRight.x();
	const int32 endY	= lowerRight.y();
	//LogInfo( "startX=%i, startY=%i, endX=%i, endY=%i", startX, startY, endX, endY );

	QPen pen;
	qreal oldOpacity = painter.opacity();
	painter.setOpacity(alpha);

	//////////////////////////////////////////////////////////////////////////////
	// sub grid lines
	//////////////////////////////////////////////////////////////////////////////

	// draw subgridlines first
	pen.setColor(subGridColor);
	pen.setWidthF(shared->GetScreenScaling() * 0.5 * invScale);
	painter.setPen(pen);

	uint32 numSubGridLines = 0;

	// draw vertical lines
	for (int32 x=startX; x<endX; x+=spacing)
	{
		if ((x - startX) % mainLineDist != 0)
		{
			mSubGridLines[numSubGridLines] = QLine(x, startY, x, endY);
			numSubGridLines++;
		}
	}

	// draw horizontal lines
	for (int32 y=startY; y<endY; y+=spacing)
	{
		if ((y - startY) % mainLineDist != 0)
		{
			mSubGridLines[numSubGridLines] = QLine(startX, y, endX, y);
			numSubGridLines++;
		}
	}

	painter.drawLines( mSubGridLines.constData(), numSubGridLines );

	//////////////////////////////////////////////////////////////////////////////
	// main grid lines
	//////////////////////////////////////////////////////////////////////////////

	// draw render grid lines
	pen.setColor(gridColor);
	pen.setWidthF(shared->GetScreenScaling() * 1.0 * invScale);
	painter.setPen(pen);

	uint32 numMainGridLines = 0;

	// draw vertical lines
	for (int32 x=startX; x<endX; x+=spacing)
	{
		if ((x - startX) % mainLineDist == 0)
		{
			mMainGridLines[numMainGridLines] = QLine(x, startY, x, endY);
			numMainGridLines++;
		}
	}

	// draw horizontal lines
	for (int32 y=startY; y<endY; y+=spacing)
	{
		if ((y - startY) % mainLineDist == 0)
		{
			mMainGridLines[numMainGridLines] = QLine(startX, y, endX, y);
			numMainGridLines++;
		}
	}

	painter.drawLines( mMainGridLines.constData(), numMainGridLines );

	painter.setOpacity( oldOpacity );
}


// convert to a global position
QPoint GraphHelpers::LocalToGlobal(const QTransform& transform, const QPoint& inPoint)
{
	return transform.inverted().map( inPoint );
}


// convert to a local position
QPoint GraphHelpers::GlobalToLocal(const QTransform& transform, const QPoint& inPoint)
{
	return transform.map( inPoint );
}


// check if a point is close to a given smoothed line
bool GraphHelpers::IsPointCloseToSmoothedLine(int32 x1, int32 y1, int32 x2, int32 y2, int32 px, int32 py)
{
	if (x2 >= x1)
	{
		// find the min and max points
		int32 minX, maxX, startY, endY;
		if (x1 <= x2)
		{
			minX	= x1;
			maxX	= x2;
			startY	= y1;
			endY	= y2;
		}
		else
		{
			minX	= x2;
			maxX	= x1;
			startY	= y2;
			endY	= y1;
		}

		// draw the lines
		int32 lastX = minX;
		int32 lastY = startY;

		if (minX != maxX)
		{
			for (int32 x=minX; x<=maxX; x++)
			{
				const float t = Core::CalcCosineInterpolationWeight((x-minX) / (float)(maxX-minX));		// calculate the smooth interpolated value
				const int32 y = startY + (endY - startY) * t;	// calculate the y coordinate
				if (DistanceToLine(lastX, lastY, x, y, px, py) <= 5.0f)	return true;
				lastX = x;
				lastY = y;
			}
		}
		else	// special case where there is just one line up
			return (DistanceToLine(x1, y1, x2, y2, px, py) <= 5.0f);
	}
	else
	{
		// find the min and max points
		int32 minY, maxY, startX, endX;
		if (y1 <= y2)
		{
			minY	= y1;
			maxY	= y2;
			startX	= x1;
			endX	= x2;
		}
		else
		{
			minY	= y2;
			maxY	= y1;
			startX	= x2;
			endX	= x1;
		}

		// draw the lines
		int32 lastY = minY;
		int32 lastX = startX;

		if (minY != maxY)
		{
			for (int32 y=minY; y<=maxY; y++)
			{
				const float t = Core::CalcCosineInterpolationWeight((y-minY) / (float)(maxY-minY));		// calculate the smooth interpolated value
				const int32 x = startX + (endX - startX) * t;	// calculate the y coordinate
				if (DistanceToLine(lastX, lastY, x, y, px, py) <= 5.0f)	return true;
				lastX = x;
				lastY = y;
			}
		}
		else	// special case where there is just one line up
			return (DistanceToLine(x1, y1, x2, y2, px, py) <= 5.0f);
	}

	return false;
}


// distance to a line
float GraphHelpers::DistanceToLine(float x1, float y1, float x2, float y2, float px, float py)
{
	const Core::Vector2 pos(px, py);
	const Core::Vector2 lineStart(x1, y1);
	const Core::Vector2 lineEnd(x2, y2);

	// a vector from start to end of the line
	const Core::Vector2 startToEnd = lineEnd - lineStart;

	// the distance of pos projected on the line
	float t = (pos - lineStart).Dot(startToEnd) / startToEnd.SquareLength();

	// make sure that we clip this distance to be sure its on the line segment
	if (t < 0.0f) t = 0.0f;
	if (t > 1.0f) t = 1.0f;	

	// calculate the position projected on the line
	const Core::Vector2 projected = lineStart + t * startToEnd;

	// the vector from the projected position to the point we are testing with
	return (pos - projected).Length();
}


// render the little triangle on the right side of the port circle
void GraphHelpers::RenderPortArrow(QPainter& painter, const QRect& portRect)
{
	const float right = portRect.right();
	const float mid = portRect.top() + (float)portRect.height() * 0.5f;

	const QPointF points[3] =
	{
		QPointF(right + 2.0f,	mid - 2.0f),
		QPointF(right + 4.0f,	mid + 0.0f),
		QPointF(right + 2.0f,	mid + 2.0f)
	};

	painter.drawPolygon( points, 3 );
}


// check intersection between line and rect
bool GraphHelpers::LineIntersectsRect(const QRect& b, float x1, float y1, float x2, float y2, double* outX, double* outY)
{
	// check first if any of the points are inside the rect
	if (outX == NULL && outY == NULL)
		if (b.contains(QPoint(x1, y1)) || b.contains(QPoint(x2, y2)))
			return true;

	// if not test for intersection with the line segments
	// check the top
	if (LinesIntersect(x1, y1, x2, y2, b.topLeft().x(), b.topLeft().y(), b.topRight().x(), b.topRight().y(), outX, outY))
		return true;

	// check the bottom
	if (LinesIntersect(x1, y1, x2, y2, b.bottomLeft().x(), b.bottomLeft().y(), b.bottomRight().x(), b.bottomRight().y(), outX, outY))
		return true;

	// check the left
	if (LinesIntersect(x1, y1, x2, y2, b.topLeft().x(), b.topLeft().y(), b.bottomLeft().x(), b.bottomLeft().y(), outX, outY))
		return true;

	// check the right
	if (LinesIntersect(x1, y1, x2, y2, b.topRight().x(), b.topRight().y(), b.bottomRight().x(), b.bottomRight().y(), outX, outY))
		return true;

	return false;
}


bool GraphHelpers::LineIntersectsCircle(float centerX, float centerY, float radius, float x1, float y1, float x2, float y2, bool startPoint, double* outX, double* outY)
{
	Vector2 O = Vector2(x1, y1);
	Vector2 D = Vector2(x2, y2) - O;
	Vector2 C = Vector2(centerX, centerY);

	float t[2];
	Vector2 point[2];
	Vector2 normal[2];

    Vector2 d = O - C;

    float a = D.Dot(D);
	if (IsClose<float>(a, 0.0f, Math::epsilon) == true)
		return false;

    float b = d.Dot(D);

    float c = d.Dot(d) - radius * radius;



    float disc = b * b - a * c;

    if (disc < 0.0f)
        return false;

    float sqrtDisc = Math::Sqrt(disc);

    float invA = 1.0f / a;

    t[0] = (-b - sqrtDisc) * invA;

    t[1] = (-b + sqrtDisc) * invA;



    float invRadius = 1.0f / radius;

    for (int i = 0; i < 2; ++i)
	{

        point[i] = O + t[i] * D;

        normal[i] = (point[i] - C) * invRadius;

    }

	int outPointIndex = 0;
	if (startPoint == false)
		outPointIndex = 1;

	*outX = point[outPointIndex].x;
	*outY = point[outPointIndex].y;

	return true;
}


// NOTE: Based on code from: http://alienryderflex.com/intersect/
//
//  Determines the intersection point of the line segment defined by points A and B
//  with the line segment defined by points C and D.
//
//  Returns true if the intersection point was found, and stores that point in X,Y.
//  Returns false if there is no determinable intersection point, in which case X,Y will
//  be unmodified.
bool GraphHelpers::LinesIntersect( double Ax, double Ay, double Bx, double By,
								   double Cx, double Cy, double Dx, double Dy,
								   double* X, double* Y)
{
	double distAB, theCos, theSin, newX, ABpos;

	//  Fail if either line segment is zero-length.
	if ((Ax==Bx && Ay==By) || (Cx==Dx && Cy==Dy))
		return false;

	//  Fail if the segments share an end-point.
	if ((Ax==Cx && Ay==Cy) || (Bx==Cx && By==Cy) || (Ax==Dx && Ay==Dy) || (Bx==Dx && By==Dy))
		return false;

	//  (1) Translate the system so that point A is on the origin.
	Bx -= Ax; By -= Ay;
	Cx -= Ax; Cy -= Ay;
	Dx -= Ax; Dy -= Ay;

	//  Discover the length of segment A-B.
	distAB = sqrt(Bx*Bx + By*By);

	//  (2) Rotate the system so that point B is on the positive X axis.
	theCos	= Bx / distAB;
	theSin	= By / distAB;
	newX	= Cx * theCos + Cy*theSin;
	Cy		= Cy * theCos - Cx*theSin;
	Cx		= newX;
	newX	= Dx * theCos + Dy*theSin;
	Dy		= Dy * theCos - Dx*theSin;
	Dx		= newX;

	//  Fail if segment C-D doesn't cross line A-B.
	if ((Cy<0.0 && Dy<0.0) || (Cy>=0.0 && Dy>=0.0))
		return false;

	//  (3) Discover the position of the intersection point along line A-B.
	ABpos = Dx + (Cx-Dx) * Dy / (Dy-Cy);

	//  Fail if segment C-D crosses line A-B outside of segment A-B.
	if (ABpos < 0.0 || ABpos > distAB)
		return false;

	//  (4) Apply the discovered position to line A-B in the original coordinate system.
	if (X) *X = Ax + ABpos * theCos;
	if (Y) *Y = Ay + ABpos * theSin;

	// intersection found
	return true;
}


// check intersection between line and rect
bool GraphHelpers::PathIntersectsRect(const QPainterPath& path, const QRect& rect)
{
	// go through all line segments and check if any one of them intersects with the rect
	const int numElements = path.elementCount();

	// must have more than one element
	if (numElements < 2)
		return false;

	for (int i = 2; i < numElements; ++i)
	{
		if (LineIntersectsRect(rect, path.elementAt(i - 1).x, path.elementAt(i - 1).y, path.elementAt(i).x, path.elementAt(i).y))
			return true;
	}

	return false;
}


// shortest distance between a point and all elements of a path
float GraphHelpers::DistanceToPath(QPoint point, const QPainterPath& path)
{
	// go through all line segments and check if any one of them intersects with the rect
	const int numElements = path.elementCount();

	// must have more than one element
	if (numElements < 2)
		return FLT_MAX;

	float minDistance = FLT_MAX;
	for (int i = 2; i < numElements; ++i)
	{
		const float distance = DistanceToLine(path.elementAt(i - 1).x, path.elementAt(i - 1).y, path.elementAt(i).x, path.elementAt(i).y, point.x(), point.y());
		minDistance = Min(minDistance, distance);
	}
	
	return minDistance;
}


void GraphHelpers::AlignSelectedNodes(Graph* graph, ENodeAlignment mode, GraphShared* shared, GraphRenderer* renderer)
{
	// return directly in case no graph is active
	if (graph == NULL)
		return;

	uint32 i;
	int32 alignedXPos, alignedYPos, maxGraphNodeHeight, maxGraphNodeWidth;

	// PHASE 1:
	// calculate the aligned coordinates
	bool firstSelectedNode = true;
	const uint32 numNodes = graph->GetNumNodes();
	for (i=0; i<numNodes; ++i)
	{
		// get the graph node and skip directly in case it is not selected
		Node* graphNode = graph->GetNode(i);
		if (shared->IsNodeSelected(graphNode) == false)
			continue;

		// get the node position
		const int32 xPos = graphNode->GetVisualPosX();
		const int32 yPos = graphNode->GetVisualPosY();

		// calculate the node rect
		QRect nodeRect = renderer->CalcNodeRect( graph, graphNode );
		const int32 graphNodeHeight	= nodeRect.height();
		const int32 graphNodeWidth	= nodeRect.width();

		if (firstSelectedNode == true)
		{
			alignedXPos			= xPos;
			alignedYPos			= yPos;
			maxGraphNodeHeight	= graphNodeHeight;
			maxGraphNodeWidth	= graphNodeWidth;
			firstSelectedNode	= false;
		}

		if (graphNodeHeight > maxGraphNodeHeight)
			maxGraphNodeHeight = graphNodeHeight;

		if (graphNodeWidth > maxGraphNodeWidth)
			maxGraphNodeWidth = graphNodeWidth;

		switch (mode)
		{
			case ALIGN_LEFT:	{ if (xPos < alignedXPos) alignedXPos = xPos; break; }
			case ALIGN_RIGHT:	{ if (xPos + graphNodeWidth > alignedXPos) alignedXPos = xPos + graphNodeWidth; break; }
			case ALIGN_TOP:		{ if (yPos < alignedYPos) alignedYPos = yPos; break; }
			case ALIGN_BOTTOM:	{ if (yPos + graphNodeHeight > alignedYPos) alignedYPos = yPos + graphNodeHeight; break; }
		}
	}

	// PHASE 2:
	// adjust the node positions
	for (i=0; i<numNodes; ++i)
	{
		// get the graph node and skip directly in case it is not selected
		Node* graphNode = graph->GetNode(i);
		if (shared->IsNodeSelected(graphNode) == false)
			continue;

		// calculate the node rect
		QRect nodeRect = renderer->CalcNodeRect( graph, graphNode );
		const int32 graphNodeHeight	= nodeRect.height();
		const int32 graphNodeWidth	= nodeRect.width();

		switch (mode)
		{
			case ALIGN_LEFT:	{ graphNode->SetVisualPos( alignedXPos, graphNode->GetVisualPosY() ); break; }
			case ALIGN_RIGHT:	{ graphNode->SetVisualPos( alignedXPos - graphNodeWidth, graphNode->GetVisualPosY() ); break; }
			case ALIGN_TOP:		{ graphNode->SetVisualPos( graphNode->GetVisualPosX(), alignedYPos ); break; }
			case ALIGN_BOTTOM:	{ graphNode->SetVisualPos( graphNode->GetVisualPosX(), alignedYPos - graphNodeHeight ); break; }
		}
	}
}


// constructor
GraphHelpers::CreateConnectionInfo::CreateConnectionInfo()
{
	// connection creation helper reset
	StopCreateConnection();
}


// destructor
GraphHelpers::CreateConnectionInfo::~CreateConnectionInfo()
{
}


// start creating a connection
void GraphHelpers::CreateConnectionInfo::StartCreateConnection(uint32 portNr, bool isInputPort, Node* portNode, Port* port, const QPoint& startOffset)
{
	mConPortNr			= portNr;
	mConIsInputPort		= isInputPort;
	mConNode			= portNode;
	mConPort			= port;
	mConStartOffset		= startOffset;
}


// stop creating a connection
void GraphHelpers::CreateConnectionInfo::StopCreateConnection()
{
	mConPortNr			= CORE_INVALIDINDEX32;
	mConIsInputPort		= true;
	mConNode			= NULL;		// NULL when no connection is being created
	mConPort			= NULL;
	mConIsValid			= false;
}


bool GraphHelpers::IsInCircle(int32 centerX, int32 centerY, float radius, int32 pointX, int32 pointY)
{
	Vector2 diff	 = Vector2(pointX, pointY) - Vector2(centerX, centerY);
	float	distance = diff.Length();
	
	if (distance < radius)
		return true;

	return false;
}


void GraphHelpers::RelinkConnectionInfo::StartRelinkHead(Connection* connection)
{
	mRelinkHeadConnection = connection;
}


void GraphHelpers::RelinkConnectionInfo::StartRelinkTail(Connection* connection)
{
	mRelinkTailConnection = connection;
}



void GraphHelpers::RelinkConnectionInfo::StopRelinking()
{
	mRelinkHeadConnection = NULL;
	mRelinkTailConnection = NULL;
}


Creud GraphHelpers::GetCreud(Node* node)
{
	if (node == NULL)
		return Creud();

	// allow to create them all
	return Creud( true, true, true, true, true );

	// find the rule for the given node
	const User::Rule* rule = GetUser()->FindRule( node->GetRuleName() );
	if (rule == NULL)
		return Creud();

	return rule->GetCreud();
}
