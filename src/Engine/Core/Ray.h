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

#ifndef __CORE_RAY_H
#define __CORE_RAY_H

// include the required headers
#include "StandardHeaders.h"
#include "Vector.h"


namespace Core
{

    class ENGINE_API Ray
    {
    public:
        Ray() {}
        Ray(const Vector3& a, const Vector3& b) : m_vA(a), m_vB(b) {
            m_vA = a; m_vB = b;
            m_vInvDir = 1.0 / m_vB;
            m_iSign[0] = (m_vInvDir.x < 0);
            m_iSign[1] = (m_vInvDir.y < 0);
            m_iSign[2] = (m_vInvDir.z < 0);
            m_vUnitDirection = m_vB.Normalize();
        }

        Vector3 Origin() const { return m_vA; }
        Vector3 Direction() const { return m_vB; }
        Vector3 PointAtParameter(double t) const { return m_vA + t * m_vB; }

        int m_iSign[3];
        Vector3 m_vA;
        Vector3 m_vB;
        Vector3 m_vInvDir;
        Vector3 m_vUnitDirection;
    };

} // namespace Core


#endif
