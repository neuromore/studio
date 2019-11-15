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

#include "ThermometerWidget.h"
#include <QtGui>
#include <cmath>
#include <assert.h>


// constructor
ThermometerWidget::ThermometerWidget(QWidget *parent) : QWidget(parent)
{
	// colors
	mWidgetBackgroundColor	= QColor( 0, 0, 0, 0 );
	mQuicksilverColor		= Qt::red;
	mBorderColor			= QColor( 190, 190, 190, 255);
	mInnerColor				= QColor( 20, 20, 20, 255);
	mTextColor				= QColor( 190, 190, 190, 255);


	mPixmap					= new QPixmap(size());
	mModified				= false;
	mEndValue				= 80;
	mStartValue				= 0;
	mMax					= 80;
	mMin					= 0;

	SetValue(0);
	calcMaxMin();
	setDigitOffset(10);
	mDigitsFont.setPointSize(15);

	//setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	//resize( 30, 100 );
	assert(mEndValue-mStartValue != 0);
}


// destructor
ThermometerWidget::~ThermometerWidget()
{
	delete mPixmap;
	mPixmap = NULL;
}

void ThermometerWidget::initCoordinateSystem(QPainter & painter)
{
	painter.setRenderHint(QPainter::Antialiasing);
	painter.translate( width()/2.0,0.0);
	painter.scale( height()/ 300.0, height()/307.0);
}

void ThermometerWidget::paintBackground(QPainter & painter)
{
	initCoordinateSystem(painter);

	QPainterPath glass;

	const double x1 = 267.5;
	const double x2 = 12.5;
	const double x3 = 7.5;

	glass.moveTo(x2,x1);
	glass.quadTo(x2,263.0, x3,257.0);

	glass.lineTo(x3,25.0);

	glass.quadTo(x3,x2 , 0,x2);
	glass.quadTo(-x3,x2,-x3,25.0);
	glass.lineTo(-x3,257.0);
	glass.quadTo(-x2,263.0, -x2,x1);
	glass.quadTo(-x2,278.0,  0.0,280.0);
	glass.quadTo( x2,278.0,  x2,x1);

	// transparent background
	painter.setBrush(QBrush(QColor(255, 255, 255, 0)));
	painter.setPen(mBorderColor);
	painter.drawPath(glass);

	QPen pen;
	int length = 12;
	for (int i=0;i<=32;i++)
	{
		pen.setColor(mInnerColor);
		pen.setWidthF(1.0);
		length = 12;
		if (i%4) { length = 8; pen.setWidthF(0.75); }
		if (i%2) { length = 5; pen.setWidthF(0.5);  }
		painter.setPen(pen);
		painter.drawLine(-7,28+i*7, -7+length,28+i*7);
	}

	if (GetDigitsOffset())
	{
		painter.setPen(mTextColor);
		painter.setFont(mDigitsFont);
		QString val;
		QSize size;
		for (int i=0;i<9;i++)
		{
			val = QString("%1").arg(mStartValue + i*(mEndValue - mStartValue)/8.0 );
			size = painter.fontMetrics().size(Qt::TextSingleLine, val);

			painter.drawText( QPointF( GetDigitsOffset(),252 -  i * 28 + size.width()/4.0) , val);
		}
	}
}

void ThermometerWidget::paintEvent(QPaintEvent*)
{
	int OFFSET = 10;

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::HighQualityAntialiasing);

	initCoordinateSystem(painter);

	QColor color= mQuicksilverColor;

//	if (valueOffset())
	{
/*		painter.setFont(valueFont());
		QString Str = GetPrefix() + QString("%1").arg(GetValue()) + GetSuffix();
		QSize Size = painter.fontMetrics().size(Qt::TextSingleLine, Str);
		painter.drawText(QPointF (Size.width() / -2,valueOffset() + Size.height()) , Str);*/
	}

	double factor =  mValue - mStartValue;
	factor /= mEndValue - mStartValue;
	int temp = static_cast<int> (224.0 *  factor), height = temp + OFFSET;
	if (231 < temp  ) height = 231 + OFFSET;
	if (OFFSET-5 >= height )  height = OFFSET-5;
	painter.setPen(Qt::NoPen);
	painter.setBrush(color);
	painter.drawRect(-5,252+OFFSET - height ,10, height);
	painter.setBrush(color);
	painter.drawEllipse(QRectF(-10.0,257.5,20.0,20.0));
	painter.end();

	drawBackground();
}


bool ThermometerWidget::calcMaxMin()
{
	unsigned int steps = 8;
	bool left = true;
	double inc = 5.0;

 //cout<<"("<<mMin<<","<<mMax<<")  ("<<mStartValue<<","<<mEndValue<<")"<<endl;
  double max_tmp = mEndValue, min_tmp = mStartValue;
  mEndValue=mMax;
  mStartValue=mMin;
  assert( mEndValue > mStartValue );
  //  assert( (mEndValue - mStartValue) > 0 );
  //  if (mEndValue<0) left!=left; 

    double diff = std::abs(mEndValue - mStartValue);
  double scale = 0, factor = 0 ;

  while (inc * steps > (mMax-mMin))
  if (inc/10 > 0 ) inc/=10;
  else break;

  bool done = false;
  while ( diff > scale ) 
   { factor+=static_cast<double>(inc);  scale = factor * steps;  }
   
  while (!done)
  {
  
    // dirty hack to have zero equal exactly zero 
    if (mEndValue<0)  mEndValue=mStartValue - fmodf(mStartValue,steps);
    else mEndValue = 0.0; 
    
     while ( mEndValue < mMax ) mEndValue +=factor;
     mStartValue = mEndValue - scale;
     if (mStartValue <= mMin ) done = true;
     else { factor+=static_cast<double>(inc); scale = factor * steps; }
  }
  if (left)
  	while (mStartValue + factor <= mMin)
  	{
	   	mStartValue+=factor;
   		mEndValue+=factor;
  	}
//  cout<<"Min:"<<mStartValue<<" Max:"<<mEndValue<<endl;
 return (mEndValue != max_tmp) | (mStartValue != min_tmp);
}

void ThermometerWidget::SetValue( double val )
{
  if ( mValue != val )
  {
    mValue = val;
    update(); 
    emit ValueChanged(val);
    emit ValueChanged((int)val); 
  }
}

void ThermometerWidget::SetMin(double i)
{
  if ((mMax - i) > 0.00001 )
  {
    mMin = i;
    if (calcMaxMin()) updateWithBackground();
  }
}

void ThermometerWidget::SetMax(double i)
{
  if ( (i - mMin) > 0.00001 )
  {
    mMax = i;
    if (calcMaxMin()) updateWithBackground();
  }
}


void ThermometerWidget::drawBackground()
{
  if (mPixmap->size() != size() || mModified )
    {
	delete mPixmap;
	mPixmap = new QPixmap(size());
	mModified=true;
	repaintBackground();
	mModified=false;
    }
    QPainter painter(this);
    painter.drawPixmap(0,0,*mPixmap);
}

void ThermometerWidget::updateWithBackground()
{
  mModified=true;
  update();
}

bool ThermometerWidget::doRepaintBackground()
{
  return mModified;
}


void ThermometerWidget::repaintBackground()
{
	mPixmap->fill( mWidgetBackgroundColor );
	QPainter painter( mPixmap );
	paintBackground( painter );
}
