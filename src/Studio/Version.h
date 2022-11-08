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

#ifndef __NEUROMORE_STUDIO_VERSION_H
#define __NEUROMORE_STUDIO_VERSION_H

#define NEUROMORE_STUDIO_VERSION_MAJOR 1
#define NEUROMORE_STUDIO_VERSION_MINOR 6
#define NEUROMORE_STUDIO_VERSION_PATCH 5

// Macros

#define NEUROMORE_STUDIO_STR_HELPER(x) #x
#define NEUROMORE_STUDIO_STR(x) NEUROMORE_STUDIO_STR_HELPER(x)

// Microsoft RC File Format (MAJOR,MINOR,PATCH,0)
#define NEUROMORE_STUDIO_VERSION_RESRC NEUROMORE_STUDIO_VERSION_MAJOR,NEUROMORE_STUDIO_VERSION_MINOR,NEUROMORE_STUDIO_VERSION_PATCH,0

// Strings of Version (From "MAJOR" to "MAJOR.MINOR.PATCH.0")
#define NEUROMORE_STUDIO_VERSION_STR1 NEUROMORE_STUDIO_STR(NEUROMORE_STUDIO_VERSION_MAJOR)
#define NEUROMORE_STUDIO_VERSION_STR2 NEUROMORE_STUDIO_VERSION_STR1 "." NEUROMORE_STUDIO_STR(NEUROMORE_STUDIO_VERSION_MINOR)
#define NEUROMORE_STUDIO_VERSION_STR3 NEUROMORE_STUDIO_VERSION_STR2 "." NEUROMORE_STUDIO_STR(NEUROMORE_STUDIO_VERSION_PATCH)
#define NEUROMORE_STUDIO_VERSION_STR4 NEUROMORE_STUDIO_VERSION_STR3 ".0"

#endif
