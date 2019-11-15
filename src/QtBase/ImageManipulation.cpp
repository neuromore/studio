/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include the required headers
#include "ImageManipulation.h"
#include <QPainter>
#include <Core/Timer.h>
#include <Core/LogManager.h>


using namespace Core;

template<class T>
const T& kClamp(const T& x, const T& low, const T& high)
{
	if (x < low)       return low;
	else if (high < x) return high;
	else                 return x;
}


int ChangeBrightness(int value, int brightness)
{
	return kClamp(value + brightness * 255 / 100, 0, 255);
}


int ChangeContrast(int value, int contrast)
{
	return kClamp(((value - 127) * contrast / 100) + 127, 0, 255);
}


int ChangeGamma(int value, int gamma)
{
	return kClamp(int(pow(value / 255.0, 100.0 / gamma) * 255), 0, 255);
}


int ChangeUsingTable(int value, const int table[])
{
	return table[value];
}


template< int operation(int, int) >
static QImage ChangeImage(const QImage& image, int value)
{
	QImage im = image;
	im.detach();

	// true color?
	if (im.colorCount() == 0)
	{
		// safety conversion
		if (im.format() != QImage::Format_RGB32)
			im = im.convertToFormat(QImage::QImage::Format_ARGB32);

		int table[256];
		for (int i = 0; i < 256; ++i)
			table[i] = operation(i, value);

		if (im.hasAlphaChannel() == true)
		{
			for (int y=0; y<im.height(); ++y)
			{
				QRgb* line = reinterpret_cast<QRgb*>(im.scanLine(y));
				for (int x=0; x<im.width(); ++x)
					line[x] = qRgba(ChangeUsingTable(qRed(line[x]), table), ChangeUsingTable(qGreen(line[x]), table), ChangeUsingTable(qBlue(line[x]), table), qAlpha(line[x]) );	// note: modify the color channels but not the alpha value!
			}
		}
		else
		{
			for (int y=0; y<im.height(); ++y)
			{
				QRgb* line = reinterpret_cast< QRgb* >(im.scanLine(y));
				for (int x = 0; x < im.width(); ++x)
					line[x] = qRgb(ChangeUsingTable(qRed(line[x]), table), ChangeUsingTable(qGreen(line[x]), table), ChangeUsingTable(qBlue(line[x]), table) );
			}
		}
	}
	else
	{
		QVector<QRgb> colors = im.colorTable();
		for (int i=0; i<im.colorCount(); ++i)
		{
			colors[i] = qRgba( operation(qRed(colors[i]), value), operation(qGreen(colors[i]), value), operation(qBlue(colors[i]), value), qAlpha(colors[i]) );
		}
	}

	return im;
}


// brightness is multiplied by 100 in order to avoid floating point numbers
QImage ChangeBrightness(const QImage& image, int brightness)
{
	if (brightness == 0) // no change
		return image;

	return ChangeImage<ChangeBrightness>(image, brightness);
}


// contrast is multiplied by 100 in order to avoid floating point numbers
QImage ChangeContrast(const QImage& image, int contrast)
{
	if (contrast == 100) // no change
		return image;

	return ChangeImage<ChangeContrast>(image, contrast);
}


// gamma is multiplied by 100 in order to avoid floating point numbers
QImage ChangeGamma(const QImage& image, int gamma)
{
	if (gamma == 100) // no change
		return image;

	return ChangeImage<ChangeGamma>(image, gamma);
}


QPixmap ChangeOpacity(const QPixmap& pixmap, float opacity)
{
	QPixmap result(pixmap.size());
	result.fill(Qt::transparent);

	QPainter painter(&result);
	painter.setOpacity(opacity);
	painter.drawPixmap(0, 0, pixmap);
	painter.end();

	return result;
}