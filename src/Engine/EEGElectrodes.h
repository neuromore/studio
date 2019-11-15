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

#ifndef __NEUROMORE_EEGELECTRODES_H
#define __NEUROMORE_EEGELECTRODES_H

// include required headers
#include "Config.h"
#include "Core/StandardHeaders.h"
#include "Core/String.h"
#include "Core/Array.h"
#include "Core/Color.h"
#include "Core/Vector.h"
#include <unordered_map>


/*
 * The 10-20 system (EEG)
 * The 10–20 system or International 10–20 system is an internationally recognized method to describe and apply the location of scalp electrodes
 * in the context of an EEG test or experiment. This method was developed to ensure standardized reproducibility so that a subject's studies could be
 * compared over time and subjects could be compared to each other. This system is based on the relationship between the location of an electrode
 * and the underlying area of cerebral cortex. The "10" and "20" refer to the fact that the actual distances between adjacent electrodes are either 10%
 * or 20% of the total front–back or right–left distance of the skull.
 *
 * Each site has a letter to identify the lobe and a number to identify the hemisphere location. The letters F, T, C, P and O stand for frontal, temporal, central, parietal, and occipital
 * lobes, respectively. Note that there exists no central lobe; the "C" letter is used only for identification purposes. A "z" (zero) refers to an electrode placed on the midline. Even numbers (2,4,6,8)
 * refer to electrode positions on the right hemisphere, whereas odd numbers (1,3,5,7) refer to those on the left hemisphere. In addition, the letter codes A, Pg and Fp identify the earlobes,
 * nasopharyngeal and frontal polar sites respectively.
 *
 * Two anatomical landmarks are used for the essential positioning of the EEG electrodes: first, the nasion which is the distinctly depressed area between the eyes, just above the bridge of the nose; second,
 * the inion, which is the lowest point of the skull from the back of the head and is normally indicated by a prominent bump.
 *
 * When recording a more detailed EEG with more electrodes, extra electrodes are added using the 10% division, which fills in intermediate sites halfway between those of the existing 10–20 system. This new
 * electrode-naming-system is more complicated giving rise to the Modified Combinatorial Nomenclature (MCN). This MCN system uses 1, 3, 5, 7, 9 for the left hemisphere which represents 10%, 20%, 30%, 40%, 50% of
 * the inion-to-nasion distance respectively. The introduction of extra letter codes allows the naming of intermediate electrode sites. Note that these new letter codes do not necessarily refer to an area on
 * the underlying cerebral cortex. The new letter codes for intermediate sites are: AF – intermediate between Fp and F, FC – between F and C, FT – between F and T, CP – between C and P, TP – between T and P,
 * PO – between P and O. Also, the MCN system renames four points of the 10–20 system—T3, T4, T5 and T6—asT7, T8, P7 and P8 respectively.
 * [source: Wikipedia]
 */
class ENGINE_API EEGElectrodes
{
	public:
        class ENGINE_API Electrode
		{
			public:
				// constructor & destructor
				Electrode();
				Electrode(const char* name, double theta, double phi);

				inline double GetTheta() const						{ return mTheta; }
				inline double GetPhi() const						{ return mPhi; }

				const char* GetName() const;
				const Core::String& GetNameString() const;

				bool operator==(const Electrode &other) const		{ return (mName == other.mName); }

			private:
				Core::String mName;
				double mTheta;
				double mPhi;
		};

		// constructor & destructor
		EEGElectrodes();
		~EEGElectrodes();

		inline uint32 GetNumElectrodes() const						{ return mElectrodes.Size(); }
		inline const Electrode& GetElectrode(uint32 index) const	{ return mElectrodes[index]; }

		bool IsValidElectrodeID(const Core::String& electrodeID) const;

        Electrode GetElectrodeByID(const Core::String& electrodeID) const;

		Core::Vector2 Get2DPosition(const Electrode& electrode) const;
		Core::Vector3 Get3DPosition(const double radius, const Electrode& electrode) const;

	private:
		void Init();

		inline double GetRadiusFrom3DProjection(double phi) const;

		Core::Array<Electrode>	                        mElectrodes;
		std::unordered_map<Core::String, Electrode, Core::StringHasher> mElectrodesHashmap;
};


#endif
