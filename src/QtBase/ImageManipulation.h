/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#ifndef __QTBASE_IMAGEMANIPULATION_H
#define __QTBASE_IMAGEMANIPULATION_H

#include "QtBaseConfig.h"
#include <QImage>
#include <QPixmap>


// brightness is multiplied by 100 in order to avoid floating point numbers
QImage ChangeBrightness(const QImage& image, int brightness);

// contrast is multiplied by 100 in order to avoid floating point numbers
QImage ChangeContrast(const QImage& image, int contrast);

// gamma is multiplied by 100 in order to avoid floating point numbers
QImage ChangeGamma(const QImage& image, int gamma);

QPixmap ChangeOpacity(const QPixmap& pixmap, float opacity);


#endif
