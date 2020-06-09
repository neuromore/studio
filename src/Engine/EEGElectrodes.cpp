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

// include required files
#include "EEGElectrodes.h"
#include "Core/LogManager.h"
#include "EngineManager.h"


using namespace Core;

// constructor
EEGElectrodes::EEGElectrodes()
{
	// initialize
	Init();
}


// destructor
EEGElectrodes::~EEGElectrodes()
{
}


// initialize all electrodes
void EEGElectrodes::Init()
{
	// reserve memory to avoid allocations
	//mElectrodes.Reserve( 1+464 );
	mElectrodes.Reserve( 1+464-127);

	// default electrode (the +1)
	mElectrodes.Add( Electrode() );


    // 10-20 System
	mElectrodes.Add( Electrode("Fpz",  90.000,  0.000) );
    mElectrodes.Add( Electrode("Fp1",  108.000,  0.000) );
    mElectrodes.Add( Electrode("Fp2",  72.000,  0.000) );
    mElectrodes.Add( Electrode("F7",  144.000,  0.000) );
    mElectrodes.Add( Electrode("F8",  36.000,  0.000) );
    mElectrodes.Add( Electrode("F3",  129.254,  29.833) );
	mElectrodes.Add( Electrode("F4",  50.746,  29.833) );
    mElectrodes.Add( Electrode("Fz",  90.000,  45.000) );
    mElectrodes.Add( Electrode("T7",  180.000,  0.000) );
    mElectrodes.Add( Electrode("T8",  0.000,  0.000) );
    mElectrodes.Add( Electrode("C3",  180.000,  45.000) );
    mElectrodes.Add( Electrode("C4",  0.000,  45.000) );
    mElectrodes.Add( Electrode("Cz",  0.000,  90.000) );
    mElectrodes.Add( Electrode("P7",  -144.000,  0.000) );
    mElectrodes.Add( Electrode("P8",  -36.000,  0.000) );
    mElectrodes.Add( Electrode("P3",  -129.254,  29.833) );
	mElectrodes.Add( Electrode("P4",  -50.746,  29.833) );
    mElectrodes.Add( Electrode("Pz",  -90.000,  45.000) );
    mElectrodes.Add( Electrode("O1",  -108.000,  0.000) );
    mElectrodes.Add( Electrode("O2",  -72.000,  0.000) );
    mElectrodes.Add( Electrode("Oz",  -90.000,  0.000) );

    // 10-10 System
    mElectrodes.Add( Electrode("F9",  144.000,  -22.500) );
    mElectrodes.Add( Electrode("FT9",  162.000,  -22.500) );
    mElectrodes.Add( Electrode("T9",  180.000,  -22.500) );
    mElectrodes.Add( Electrode("TP9",  -162.000,  -22.500) );
    mElectrodes.Add( Electrode("P9",  -144.000,  -22.500) );
    mElectrodes.Add( Electrode("AF7",  126.000,  0.000) );
    mElectrodes.Add( Electrode("FT7",  162.000,  0.000) );
    mElectrodes.Add( Electrode("TP7",  -162.000,  0.000) );
    mElectrodes.Add( Electrode("PO7",  -126.000,  0.000) );
	mElectrodes.Add( Electrode("AF3",  113.312,  15.040) );
	mElectrodes.Add( Electrode("F5",  138.891,  15.619) );
	mElectrodes.Add( Electrode("FC5",  158.854,  20.773) );
	mElectrodes.Add( Electrode("C5",  180.000,  22.500) );
	mElectrodes.Add( Electrode("CP5",  -158.854,  20.773) );
	mElectrodes.Add( Electrode("P5",  -138.891,  15.619) );
	mElectrodes.Add( Electrode("PO3",  -113.312,  15.040) );
	mElectrodes.Add( Electrode("F1",  112.953,  40.722) );
	mElectrodes.Add( Electrode("FC1",  133.587,  58.627) );
	mElectrodes.Add( Electrode("C1",  180.000,  67.500) );
	mElectrodes.Add( Electrode("CP1",  -133.587,  58.627) );
	mElectrodes.Add( Electrode("F6",  41.109,  15.619) );
	mElectrodes.Add( Electrode("P6",  -41.109,  15.619) );
	mElectrodes.Add( Electrode("F2",  67.047,  40.722) );
	mElectrodes.Add( Electrode("P1",  -112.953,  40.722) );
	mElectrodes.Add( Electrode("P2",  -67.047,  40.722) );
	mElectrodes.Add( Electrode("AFz",  90.000,  22.500) );
	mElectrodes.Add( Electrode("FCz",  90.000,  67.500) );
	mElectrodes.Add( Electrode("CPz",  -90.000,  67.500) );
	mElectrodes.Add( Electrode("POz",  -90.000,  22.500) );
	mElectrodes.Add( Electrode("FC2",  46.413,  58.627) );
	mElectrodes.Add( Electrode("C2",  0.000,  67.500) );
	mElectrodes.Add( Electrode("CP2",  -46.413,  58.627) );
	mElectrodes.Add( Electrode("AF4",  66.688,  15.040) );
	mElectrodes.Add( Electrode("PO4",  -66.688,  15.040) );
	mElectrodes.Add( Electrode("FC3",  151.481,  40.847) );
	mElectrodes.Add( Electrode("FC4",  28.519,  40.847) );
	mElectrodes.Add( Electrode("CP4",  -28.519,  40.847) );
	mElectrodes.Add( Electrode("FC6",  21.146,  20.773) );
	mElectrodes.Add( Electrode("C6",  0.000,  22.500) );
	mElectrodes.Add( Electrode("CP6",  -21.146,  20.773) );
	mElectrodes.Add( Electrode("AF8",  54.000,  0.000) );
	mElectrodes.Add( Electrode("FT8",  18.000,  0.000) );
	mElectrodes.Add( Electrode("TP8",  -18.000,  0.000) );
	mElectrodes.Add( Electrode("PO8",  -54.000,  0.000) );
	mElectrodes.Add( Electrode("F10",  36.000,  -22.500) );
	mElectrodes.Add( Electrode("FT10",  18.000,  -22.500) );
	mElectrodes.Add( Electrode("T10",  0.000,  -22.500) );
	mElectrodes.Add( Electrode("TP10",  -18.000,  -22.500) );
	mElectrodes.Add( Electrode("P10",  -36.000,  -22.500) );

	// 10-20 modifications (renamed electrodes, used by Mitsar; apparently there are two conventions for 10-20)
	// see page 2 of https://www.acns.org/pdf/guidelines/Guideline-5.pdf 
	mElectrodes.Add(Electrode("T3", 180.000, 0.000));		// same as T7
	mElectrodes.Add(Electrode("T4", 0.000, 0.000));			// same as T8
	mElectrodes.Add(Electrode("T5", -144.000, 0.000));		// same as P7
	mElectrodes.Add(Electrode("T6", -36.000, 0.000));		// same as P8

    // 10-5 System
	// 258 electrodes
	mElectrodes.Add( Electrode("Nz",  90.000,  -22.500) );
	mElectrodes.Add( Electrode("NFpz",  90.000,  -11.250) );
	mElectrodes.Add( Electrode("AFpz",  90.000,  11.250) );
	mElectrodes.Add( Electrode("AFFz",  90.000,  33.750) );
	mElectrodes.Add( Electrode("FFCz",  90.000,  56.250) );
	mElectrodes.Add( Electrode("FCCz",  90.000,  78.750) );
	mElectrodes.Add( Electrode("CCPz",  -90.000,  78.750) );
	mElectrodes.Add( Electrode("CPPz",  -90.000,  56.250) );
	mElectrodes.Add( Electrode("PPOz",  -90.000,  33.750) );
	mElectrodes.Add( Electrode("POOz",  -90.000,  11.250) );
	mElectrodes.Add( Electrode("OIz",  -90.000,  -11.250) );
	mElectrodes.Add( Electrode("Iz",  -90.000,  -22.500) );
	mElectrodes.Add( Electrode("N1h",  99.000,  -22.500) );
	mElectrodes.Add( Electrode("N1",  108.000,  -22.500) );
	mElectrodes.Add( Electrode("AFp9",  117.000,  -22.500) );
	mElectrodes.Add( Electrode("AF9",  126.000,  -22.500) );
	mElectrodes.Add( Electrode("AFF9",  135.000,  -22.500) );
	mElectrodes.Add( Electrode("FFT9",  153.000,  -22.500) );
	mElectrodes.Add( Electrode("FTT9",  171.000,  -22.500) );
	mElectrodes.Add( Electrode("TTP9",  -171.000,  -22.500) );
	mElectrodes.Add( Electrode("TPP9",  -153.000,  -22.500) );
	mElectrodes.Add( Electrode("PPO9",  -135.000,  -22.500) );
	mElectrodes.Add( Electrode("PO9",  -126.000,  -22.500) );
	mElectrodes.Add( Electrode("POO9",  -117.000,  -22.500) );
	mElectrodes.Add( Electrode("I1",  -108.000,  -22.500) );
	mElectrodes.Add( Electrode("I1h",  -99.000,  -22.500) );
	mElectrodes.Add( Electrode("NFp1h",  99.000,  -11.250) );
	mElectrodes.Add( Electrode("NFp1",  108.000,  -11.250) );
	mElectrodes.Add( Electrode("AFp9h",  117.000,  -11.250) );
	mElectrodes.Add( Electrode("AF9h",  126.000,  -11.250) );
	mElectrodes.Add( Electrode("AFF9h",  135.000,  -11.250) );
	mElectrodes.Add( Electrode("F9h",  144.000,  -11.250) );
	mElectrodes.Add( Electrode("FFT9h",  153.000,  -11.250) );
	mElectrodes.Add( Electrode("FT9h",  162.000,  -11.250) );
	mElectrodes.Add( Electrode("FTT9h",  171.000,  -11.250) );
	mElectrodes.Add( Electrode("T9h",  180.000,  -11.250) );
	mElectrodes.Add( Electrode("TTP9h",  -171.000,  -11.250) );
	mElectrodes.Add( Electrode("TP9h",  -162.000,  -11.250) );
	mElectrodes.Add( Electrode("TPP9h",  -153.000,  -11.250) );
	mElectrodes.Add( Electrode("P9h",  -144.000,  -11.250) );
	mElectrodes.Add( Electrode("PPO9h",  -135.000,  -11.250) );
	mElectrodes.Add( Electrode("PO9h",  -126.000,  -11.250) );
	mElectrodes.Add( Electrode("POO9h",  -117.000,  -11.250) );
	mElectrodes.Add( Electrode("OI1",  -108.000,  -11.250) );
	mElectrodes.Add( Electrode("OI1h",  -99.000,  -11.250) );
	mElectrodes.Add( Electrode("Fp1h",  99.000,  0.000) );
	mElectrodes.Add( Electrode("AFp7",  117.000,  0.000) );
	mElectrodes.Add( Electrode("AFF7",  135.000,  0.000) );
	mElectrodes.Add( Electrode("FFT7",  153.000,  0.000) );
	mElectrodes.Add( Electrode("FTT7",  171.000,  0.000) );
	mElectrodes.Add( Electrode("TTP7",  -171.000,  0.000) );
	mElectrodes.Add( Electrode("TPP7",  -153.000,  0.000) );
	mElectrodes.Add( Electrode("PPO7",  -135.000,  0.000) );
	mElectrodes.Add( Electrode("POO7",  -117.000,  0.000) );
	mElectrodes.Add( Electrode("O1h",  -99.000,  0.000) );
	mElectrodes.Add( Electrode("N2h",  81.000,  -22.500) );
	mElectrodes.Add( Electrode("N2",  72.000,  -22.500) );
	mElectrodes.Add( Electrode("AFp10",  63.000,  -22.500) );
	mElectrodes.Add( Electrode("AF10",  54.000,  -22.500) );
	mElectrodes.Add( Electrode("AFF10",  45.000,  -22.500) );
	mElectrodes.Add( Electrode("FFT10",  27.000,  -22.500) );
	mElectrodes.Add( Electrode("FTT10",  9.000,  -22.500) );
	mElectrodes.Add( Electrode("TTP10",  -9.000,  -22.500) );
	mElectrodes.Add( Electrode("TPP10",  -27.000,  -22.500) );
	mElectrodes.Add( Electrode("PPO10",  -45.000,  -22.500) );
	mElectrodes.Add( Electrode("PO10",  -54.000,  -22.500) );
	mElectrodes.Add( Electrode("POO10",  -63.000,  -22.500) );
	mElectrodes.Add( Electrode("I2",  -72.000,  -22.500) );
	mElectrodes.Add( Electrode("I2h",  -81.000,  -22.500) );
	mElectrodes.Add( Electrode("NFp2h",  81.000,  -11.250) );
	mElectrodes.Add( Electrode("NFp2",  72.000,  -11.250) );
	mElectrodes.Add( Electrode("AFp10h",  63.000,  -11.250) );
	mElectrodes.Add( Electrode("AF10h",  54.000,  -11.250) );
	mElectrodes.Add( Electrode("AFF10h",  45.000,  -11.250) );
	mElectrodes.Add( Electrode("F10h",  36.000,  -11.250) );
	mElectrodes.Add( Electrode("FFT10h",  27.000,  -11.250) );
	mElectrodes.Add( Electrode("FT10h",  18.000,  -11.250) );
	mElectrodes.Add( Electrode("FTT10h",  9.000,  -11.250) );
	mElectrodes.Add( Electrode("T10h",  0.000,  -11.250) );
	mElectrodes.Add( Electrode("TTP10h",  -9.000,  -11.250) );
	mElectrodes.Add( Electrode("TP10h",  -18.000,  -11.250) );
	mElectrodes.Add( Electrode("TPP10h",  -27.000,  -11.250) );
	mElectrodes.Add( Electrode("P10h",  -36.000,  -11.250) );
	mElectrodes.Add( Electrode("PPO10h",  -45.000,  -11.250) );
	mElectrodes.Add( Electrode("PO10h",  -54.000,  -11.250) );
	mElectrodes.Add( Electrode("POO10h",  -63.000,  -11.250) );
	mElectrodes.Add( Electrode("OI2",  -72.000,  -11.250) );
	mElectrodes.Add( Electrode("OI2h",  -81.000,  -11.250) );
	mElectrodes.Add( Electrode("Fp2h",  81.000,  0.000) );
	mElectrodes.Add( Electrode("AFp8",  63.000,  0.000) );
	mElectrodes.Add( Electrode("AFF8",  45.000,  0.000) );
	mElectrodes.Add( Electrode("FFT8",  27.000,  0.000) );
	mElectrodes.Add( Electrode("FTT8",  9.000,  0.000) );
	mElectrodes.Add( Electrode("TTP8",  -9.000,  0.000) );
	mElectrodes.Add( Electrode("TPP8",  -27.000,  0.000) );
	mElectrodes.Add( Electrode("PPO8",  -45.000,  0.000) );
	mElectrodes.Add( Electrode("POO8",  -63.000,  0.000) );
	mElectrodes.Add( Electrode("O2h",  -81.000,  0.000) );
	mElectrodes.Add( Electrode("T7h",  180.000,  11.250) );
	mElectrodes.Add( Electrode("C5h",  180.000,  33.750) );
	mElectrodes.Add( Electrode("C3h",  180.000,  56.250) );
	mElectrodes.Add( Electrode("C1h",  180.000,  78.750) );
	mElectrodes.Add( Electrode("T8h",  0.000,  11.250) );
	mElectrodes.Add( Electrode("C6h",  0.000,  33.750) );
	mElectrodes.Add( Electrode("C4h",  0.000,  56.250) );
	mElectrodes.Add( Electrode("C2h",  0.000,  78.750) );
	mElectrodes.Add( Electrode("F7h",  141.913,  7.907) );
	mElectrodes.Add( Electrode("F8h",  38.087,  7.907) );
	mElectrodes.Add( Electrode("P7h",  -141.913,  7.907) );
	mElectrodes.Add( Electrode("P8h",  -38.087,  7.907) );
	mElectrodes.Add( Electrode("F5h",  134.752,  22.998) );
	mElectrodes.Add( Electrode("F6h",  45.248,  22.998) );
	mElectrodes.Add( Electrode("P5h",  -134.752,  22.998) );
	mElectrodes.Add( Electrode("P6h",  -45.248,  22.998) );
	mElectrodes.Add( Electrode("F3h",  122.046,  35.889) );
	mElectrodes.Add( Electrode("F4h",  57.954,  35.889) );
	mElectrodes.Add( Electrode("P3h",  -122.046,  35.889) );
	mElectrodes.Add( Electrode("P4h",  -57.954,  35.889) );
	mElectrodes.Add( Electrode("F1h",  102.055,  43.890) );
	mElectrodes.Add( Electrode("F2h",  77.945,  43.890) );
	mElectrodes.Add( Electrode("P1h",  -102.055,  43.890) );
	mElectrodes.Add( Electrode("P2h",  -77.945,  43.890) );
	mElectrodes.Add( Electrode("CP3",  -151.481,  40.847) );
	mElectrodes.Add( Electrode("FT7h",  160.798,  10.433) );
	mElectrodes.Add( Electrode("FT8h",  19.202,  10.433) );
	mElectrodes.Add( Electrode("TP7h",  -160.798,  10.433) );
	mElectrodes.Add( Electrode("TP8h",  -19.202,  10.433) );
	mElectrodes.Add( Electrode("FC5h",  155.912,  30.952) );
	mElectrodes.Add( Electrode("FC6h",  24.088,  30.952) );
	mElectrodes.Add( Electrode("CP5h",  -155.912,  30.952) );
	mElectrodes.Add( Electrode("CP6h",  -24.088,  30.952) );
	mElectrodes.Add( Electrode("FC3h",  144.625,  50.235) );
	mElectrodes.Add( Electrode("FC4h",  35.375,  50.235) );
	mElectrodes.Add( Electrode("CP3h",  -144.625,  50.235) );
	mElectrodes.Add( Electrode("CP4h",  -35.375,  50.235) );
	mElectrodes.Add( Electrode("FC1h",  115.626,  64.984) );
	mElectrodes.Add( Electrode("FC2h",  64.374,  64.984) );
	mElectrodes.Add( Electrode("CP1h",  -115.626,  64.984) );
	mElectrodes.Add( Electrode("CP2h",  -64.374,  64.984) );
	mElectrodes.Add( Electrode("AF5",  120.854,  7.908) );
	mElectrodes.Add( Electrode("AF6",  59.146,  7.908) );
	mElectrodes.Add( Electrode("PO5",  -120.854,  7.908) );
	mElectrodes.Add( Electrode("PO6",  -59.146,  7.908) );
	mElectrodes.Add( Electrode("AF1",  102.721,  20.458) );
	mElectrodes.Add( Electrode("AF2",  77.279,  20.458) );
	mElectrodes.Add( Electrode("PO1",  -102.721,  20.458) );
	mElectrodes.Add( Electrode("PO2",  -77.279,  20.458) );
	mElectrodes.Add( Electrode("AF7h",  123.694,  4.005) );
	mElectrodes.Add( Electrode("AF8h",  56.306,  4.005) );
	mElectrodes.Add( Electrode("PO7h",  -123.694,  4.005) );
	mElectrodes.Add( Electrode("PO8h",  -56.306,  4.005) );
	mElectrodes.Add( Electrode("AF5h",  117.408,  11.630) );
	mElectrodes.Add( Electrode("AF6h",  62.592,  11.630) );
	mElectrodes.Add( Electrode("PO5h",  -117.408,  11.630) );
	mElectrodes.Add( Electrode("PO6h",  -62.592,  11.630) );
	mElectrodes.Add( Electrode("AF3h",  108.359,  18.087) );
	mElectrodes.Add( Electrode("AF4h",  71.641,  18.087) );
	mElectrodes.Add( Electrode("PO3h",  -108.359,  18.087) );
	mElectrodes.Add( Electrode("PO4h",  -71.641,  18.087) );
	mElectrodes.Add( Electrode("AF1h",  96.517,  21.977) );
	mElectrodes.Add( Electrode("AF2h",  83.483,  21.977) );
	mElectrodes.Add( Electrode("PO1h",  -96.517,  21.977) );
	mElectrodes.Add( Electrode("PO2h",  -83.483,  21.977) );
	mElectrodes.Add( Electrode("AFp3",  106.794,  7.311) );
	mElectrodes.Add( Electrode("AFp4",  73.206,  7.311) );
	mElectrodes.Add( Electrode("POO3",  -106.794,  7.311) );
	mElectrodes.Add( Electrode("POO4",  -73.206,  7.311) );
	mElectrodes.Add( Electrode("AFp5",  112.551,  3.800) );
	mElectrodes.Add( Electrode("AFp6",  67.449,  3.800) );
	mElectrodes.Add( Electrode("POO5",  -112.551,  3.800) );
	mElectrodes.Add( Electrode("POO6",  -67.449,  3.800) );
	mElectrodes.Add( Electrode("AFp1",  99.082,  10.141) );
	mElectrodes.Add( Electrode("AFp2",  80.918,  10.141) );
	mElectrodes.Add( Electrode("POO1",  -99.082,  10.141) );
	mElectrodes.Add( Electrode("POO2",  -80.918,  10.141) );
	mElectrodes.Add( Electrode("AFF3",  120.670,  22.700) );
	mElectrodes.Add( Electrode("AFF4",  59.330,  22.700) );
	mElectrodes.Add( Electrode("PPO3",  -120.670,  22.700) );
	mElectrodes.Add( Electrode("PPO4",  -59.330,  22.700) );
	mElectrodes.Add( Electrode("AFF5",  129.623,  11.966) );
	mElectrodes.Add( Electrode("AFF6",  50.377,  11.966) );
	mElectrodes.Add( Electrode("PPO5",  -129.623,  11.966) );
	mElectrodes.Add( Electrode("PPO6",  -50.377,  11.966) );
	mElectrodes.Add( Electrode("AFF1",  107.147,  30.720) );
	mElectrodes.Add( Electrode("AFF2",  72.853,  30.720) );
	mElectrodes.Add( Electrode("PPO1",  -107.147,  30.720) );
	mElectrodes.Add( Electrode("PPO2",  -72.853,  30.720) );
	mElectrodes.Add( Electrode("AFF7h",  132.702,  6.068) );
	mElectrodes.Add( Electrode("AFF8h",  47.298,  6.068) );
	mElectrodes.Add( Electrode("PPO7h",  -132.702,  6.068) );
	mElectrodes.Add( Electrode("PPO8h",  -47.298,  6.068) );
	mElectrodes.Add( Electrode("AFF5h",  125.648,  17.573) );
	mElectrodes.Add( Electrode("AFF6h",  54.352,  17.573) );
	mElectrodes.Add( Electrode("PPO5h",  -125.648,  17.573) );
	mElectrodes.Add( Electrode("PPO6h",  -54.352,  17.573) );
	mElectrodes.Add( Electrode("AFF3h",  114.474,  27.207) );
	mElectrodes.Add( Electrode("AFF4h",  65.526,  27.207) );
	mElectrodes.Add( Electrode("PPO3h",  -114.474,  27.207) );
	mElectrodes.Add( Electrode("PPO4h",  -65.526,  27.207) );
	mElectrodes.Add( Electrode("AFF1h",  98.854,  32.973) );
	mElectrodes.Add( Electrode("AFF2h",  81.146,  32.973) );
	mElectrodes.Add( Electrode("PPO1h",  -98.854,  32.973) );
	mElectrodes.Add( Electrode("PPO2h",  -81.146,  32.973) );
	mElectrodes.Add( Electrode("FFC3",  139.449,  36.019) );
	mElectrodes.Add( Electrode("FFC4",  40.551,  36.019) );
	mElectrodes.Add( Electrode("CPP3",  -139.449,  36.019) );
	mElectrodes.Add( Electrode("CPP4",  -40.551,  36.019) );
	mElectrodes.Add( Electrode("FFC5",  148.658,  18.605) );
	mElectrodes.Add( Electrode("FFC6",  31.342,  18.605) );
	mElectrodes.Add( Electrode("CPP5",  -148.658,  18.605) );
	mElectrodes.Add( Electrode("CPP6",  -31.342,  18.605) );
	mElectrodes.Add( Electrode("FFC1",  121.162,  50.192) );
	mElectrodes.Add( Electrode("FFC2",  58.838,  50.192) );
	mElectrodes.Add( Electrode("CPP1",  -121.162,  50.192) );
	mElectrodes.Add( Electrode("CPP2",  -58.838,  50.192) );
	mElectrodes.Add( Electrode("FFT7h",  151.293,  9.383) );
	mElectrodes.Add( Electrode("FFT8h",  28.707,  9.383) );
	mElectrodes.Add( Electrode("TPP7h",  -151.293,  9.383) );
	mElectrodes.Add( Electrode("TPP8h",  -28.707,  9.383) );
	mElectrodes.Add( Electrode("FFC5h",  144.847,  27.547) );
	mElectrodes.Add( Electrode("FFC6h",  35.153,  27.547) );
	mElectrodes.Add( Electrode("CPP5h",  -144.847,  27.547) );
	mElectrodes.Add( Electrode("CPP6h",  -35.153,  27.547) );
	mElectrodes.Add( Electrode("FFC3h",  131.815,  43.741) );
	mElectrodes.Add( Electrode("FFC4h",  48.185,  43.741) );
	mElectrodes.Add( Electrode("CPP3h",  -131.815,  43.741) );
	mElectrodes.Add( Electrode("CPP4h",  -48.185,  43.741) );
	mElectrodes.Add( Electrode("FFC1h",  106.951,  54.636) );
	mElectrodes.Add( Electrode("FFC2h",  73.049,  54.636) );
	mElectrodes.Add( Electrode("CPP1h",  -106.951,  54.636) );
	mElectrodes.Add( Electrode("CPP2h",  -73.049,  54.636) );
	mElectrodes.Add( Electrode("FCC3",  165.214,  43.935) );
	mElectrodes.Add( Electrode("FCC4",  14.786,  43.935) );
	mElectrodes.Add( Electrode("CCP3",  -165.214,  43.935) );
	mElectrodes.Add( Electrode("CCP4",  -14.786,  43.935) );
	mElectrodes.Add( Electrode("FCC5",  169.351,  22.070) );
	mElectrodes.Add( Electrode("FCC6",  10.649,  22.070) );
	mElectrodes.Add( Electrode("CCP5",  -169.351,  22.070) );
	mElectrodes.Add( Electrode("CCP6",  -10.649,  22.070) );
	mElectrodes.Add( Electrode("FCC1",  152.968,  64.990) );
	mElectrodes.Add( Electrode("FCC2",  27.032,  64.990) );
	mElectrodes.Add( Electrode("CCP1",  -152.968,  64.990) );
	mElectrodes.Add( Electrode("CCP2",  -27.032,  64.990) );
	mElectrodes.Add( Electrode("FTT7h",  170.382,  11.048) );
	mElectrodes.Add( Electrode("FTT8h",  9.618,  11.048) );
	mElectrodes.Add( Electrode("TTP7h",  -170.382,  11.048) );
	mElectrodes.Add( Electrode("TTP8h",  -9.618,  11.048) );	
	mElectrodes.Add( Electrode("FCC5h",  167.745,  33.045) );
	mElectrodes.Add( Electrode("FCC6h",  12.255,  33.045) );
	mElectrodes.Add( Electrode("CCP5h",  -167.745,  33.045) );
	mElectrodes.Add( Electrode("CCP6h",  -12.255,  33.045) );
	mElectrodes.Add( Electrode("FCC3h",  160.973,  54.657) );
	mElectrodes.Add( Electrode("FCC4h",  19.027,  54.657) );
	mElectrodes.Add( Electrode("CCP3h",  -160.973,  54.657) );
	mElectrodes.Add( Electrode("CCP4h",  -19.027,  54.657) );
	mElectrodes.Add( Electrode("FCC1h",  134.645,  74.147) );
	mElectrodes.Add( Electrode("FCC2h",  45.355,  74.147) );
	mElectrodes.Add( Electrode("CCP1h",  -134.645,  74.147) );
	mElectrodes.Add( Electrode("CCP2h",  -45.355,  74.147) );

	// Others
	// Note: wrong position: electrodes can lie only on the sphere so we use the outer most electrode positions. 
	mElectrodes.Add( Electrode("A1",  180.000,  -22.500) );		// left ear: drawn at same pos as T9
	mElectrodes.Add( Electrode("A2",  0.000,  -22.500) );		// left ear: drawn at same pos as T10
	mElectrodes.Add( Electrode("LPA",  180.000,  -22.500) );	// same as A1
	mElectrodes.Add( Electrode("RPA",  0.000,  -22.500) );		// same as A2
	mElectrodes.Add( Electrode("LM",  -162.000,  -22.500) );	// not sure what position this is
	mElectrodes.Add( Electrode("RM",  -18.000,  -22.500) );	    // not sure what position this is
	mElectrodes.Add( Electrode("Nose", 90.000, -22.500) );
	mElectrodes.Add( Electrode("EOG", 0.000, 0.000));
	mElectrodes.Add( Electrode("REF", 90.000, 22.500));
	mElectrodes.Add( Electrode("GND", 90.000, 22.500));

    // Others
	// 127 electrodes
	//mElectrodes.Add( Electrode("1",  36.000,  -22.000) );
	//mElectrodes.Add( Electrode("2",  47.000,  -6.000) );
	//mElectrodes.Add( Electrode("3",  56.000,  10.000) );
	//mElectrodes.Add( Electrode("4",  72.000,  26.000) );
	//mElectrodes.Add( Electrode("5",  78.000,  42.000) );
	//mElectrodes.Add( Electrode("6",  90.000,  58.000) );
	//mElectrodes.Add( Electrode("7",  126.000,  74.000) );
	//mElectrodes.Add( Electrode("8",  54.000,  -22.000) );
	//mElectrodes.Add( Electrode("9",  64.000,  -6.000) );
	//mElectrodes.Add( Electrode("10",  73.000,  10.000) );
	//mElectrodes.Add( Electrode("11",  90.000,  26.000) );
	//mElectrodes.Add( Electrode("12",  102.000,  42.000) );
	//mElectrodes.Add( Electrode("13",  126.000,  58.000) );
	//mElectrodes.Add( Electrode("14",  72.000,  -22.000) );
	//mElectrodes.Add( Electrode("15",  81.000,  -6.000) );
	//mElectrodes.Add( Electrode("16",  90.000,  10.000) );
	//mElectrodes.Add( Electrode("17",  90.000,  -33.750) );
	//mElectrodes.Add( Electrode("18",  99.000,  -6.000) );
	//mElectrodes.Add( Electrode("19",  108.000,  10.000) );
	//mElectrodes.Add( Electrode("20",  108.000,  26.000) );
	//mElectrodes.Add( Electrode("21",  126.000,  42.000) );
	//mElectrodes.Add( Electrode("22",  108.000,  -22.000) );
	//mElectrodes.Add( Electrode("23",  116.000,  -6.000) );
	//mElectrodes.Add( Electrode("24",  126.000,  10.000) );
	//mElectrodes.Add( Electrode("25",  126.000,  26.000) );
	//mElectrodes.Add( Electrode("26",  126.000,  -22.000) );
	//mElectrodes.Add( Electrode("27",  133.000,  -6.000) );
	//mElectrodes.Add( Electrode("28",  142.000,  10.000) );
	//mElectrodes.Add( Electrode("29",  144.000,  26.000) );
	//mElectrodes.Add( Electrode("30",  150.000,  42.000) );
	//mElectrodes.Add( Electrode("31",  162.000,  58.000) );
	//mElectrodes.Add( Electrode("32",  -162.000,  74.000) );
	//mElectrodes.Add( Electrode("33",  144.000,  -22.000) );
	//mElectrodes.Add( Electrode("34",  150.000,  -6.000) );
	//mElectrodes.Add( Electrode("35",  159.000,  10.000) );
	//mElectrodes.Add( Electrode("36",  162.000,  26.000) );
	//mElectrodes.Add( Electrode("37",  174.000,  42.000) );
	//mElectrodes.Add( Electrode("38",  -162.000,  58.000) );
	//mElectrodes.Add( Electrode("39",  162.000,  -22.000) );
	//mElectrodes.Add( Electrode("40",  167.000,  -6.000) );
	//mElectrodes.Add( Electrode("41",  176.000,  10.000) );
	//mElectrodes.Add( Electrode("42",  180.000,  26.000) );
	//mElectrodes.Add( Electrode("43",  -162.000,  42.000) );
	//mElectrodes.Add( Electrode("44",  150.000,  -38.000) );
	//mElectrodes.Add( Electrode("45",  180.000,  -22.000) );
	//mElectrodes.Add( Electrode("46",  -176.000,  -6.000) );
	//mElectrodes.Add( Electrode("47",  -167.000,  10.000) );
	//mElectrodes.Add( Electrode("48",  -162.000,  26.000) );
	//mElectrodes.Add( Electrode("49",  170.000,  -38.000) );
	//mElectrodes.Add( Electrode("50",  -159.000,  -6.000) );
	//mElectrodes.Add( Electrode("51",  -150.000,  10.000) );
	//mElectrodes.Add( Electrode("52",  -144.000,  26.000) );
	//mElectrodes.Add( Electrode("53",  -138.000,  42.000) );
	//mElectrodes.Add( Electrode("54",  -126.000,  58.000) );
	//mElectrodes.Add( Electrode("55",  -90.000,  74.000) );
	//mElectrodes.Add( Electrode("56",  -170.000,  -38.000) );
	//mElectrodes.Add( Electrode("57",  -157.000,  -22.000) );
	//mElectrodes.Add( Electrode("58",  -142.000,  -6.000) );
	//mElectrodes.Add( Electrode("59",  -133.000,  10.000) );
	//mElectrodes.Add( Electrode("60",  -126.000,  26.000) );
	//mElectrodes.Add( Electrode("61",  -114.000,  42.000) );
	//mElectrodes.Add( Electrode("62",  -90.000,  58.000) );
	//mElectrodes.Add( Electrode("63",  -150.000,  -38.000) );
	//mElectrodes.Add( Electrode("64",  -139.000,  -22.000) );
	//mElectrodes.Add( Electrode("65",  -125.000,  -6.000) );
	//mElectrodes.Add( Electrode("66",  -116.000,  10.000) );
	//mElectrodes.Add( Electrode("67",  -108.000,  26.000) );
	//mElectrodes.Add( Electrode("68",  -90.000,  42.000) );
	//mElectrodes.Add( Electrode("69",  -130.000,  -38.000) );
	//mElectrodes.Add( Electrode("70",  -122.000,  -22.000) );
	//mElectrodes.Add( Electrode("71",  -108.000,  -6.000) );
	//mElectrodes.Add( Electrode("72",  -99.000,  10.000) );
	//mElectrodes.Add( Electrode("73",  -90.000,  26.000) );
	//mElectrodes.Add( Electrode("74",  -110.000,  -38.000) );
	//mElectrodes.Add( Electrode("75",  -100.000,  -22.000) );
	//mElectrodes.Add( Electrode("76",  -90.000,  -6.000) );
	//mElectrodes.Add( Electrode("77",  -81.000,  10.000) );
	//mElectrodes.Add( Electrode("78",  -72.000,  26.000) );
	//mElectrodes.Add( Electrode("79",  -66.000,  42.000) );
	//mElectrodes.Add( Electrode("80",  -54.000,  58.000) );
	//mElectrodes.Add( Electrode("81",  -18.000,  74.000) );
	//mElectrodes.Add( Electrode("82",  -90.000,  -38.000) );
	//mElectrodes.Add( Electrode("83",  -80.000,  -22.000) );
	//mElectrodes.Add( Electrode("84",  -72.000,  -6.000) );
	//mElectrodes.Add( Electrode("85",  -64.000,  10.000) );
	//mElectrodes.Add( Electrode("86",  -54.000,  26.000) );
	//mElectrodes.Add( Electrode("87",  -42.000,  42.000) );
	//mElectrodes.Add( Electrode("88",  -18.000,  58.000) );
	//mElectrodes.Add( Electrode("89",  -70.000,  -38.000) );
	//mElectrodes.Add( Electrode("90",  -59.000,  -22.000) );
	//mElectrodes.Add( Electrode("91",  -55.000,  -6.000) );
	//mElectrodes.Add( Electrode("92",  -47.000,  10.000) );
	//mElectrodes.Add( Electrode("93",  -36.000,  26.000) );
	//mElectrodes.Add( Electrode("94",  -18.000,  42.000) );
	//mElectrodes.Add( Electrode("95",  -50.000,  -38.000) );
	//mElectrodes.Add( Electrode("96",  -41.000,  -22.000) );
	//mElectrodes.Add( Electrode("97",  -38.000,  -6.000) );
	//mElectrodes.Add( Electrode("98",  -30.000,  10.000) );
	//mElectrodes.Add( Electrode("99",  -18.000,  26.000) );
	//mElectrodes.Add( Electrode("100",  -30.000,  -38.000) );
	//mElectrodes.Add( Electrode("101",  -23.000,  -22.000) );
	//mElectrodes.Add( Electrode("102",  -21.000,  -6.000) );
	//mElectrodes.Add( Electrode("103",  -13.000,  10.000) );
	//mElectrodes.Add( Electrode("104",  0.000,  26.000) );
	//mElectrodes.Add( Electrode("105",  6.000,  42.000) );
	//mElectrodes.Add( Electrode("106",  18.000,  58.000) );
	//mElectrodes.Add( Electrode("107",  54.000,  74.000) );
	//mElectrodes.Add( Electrode("108",  -10.000,  -38.000) );
	//mElectrodes.Add( Electrode("109",  -4.000,  -6.000) );
	//mElectrodes.Add( Electrode("110",  4.000,  10.000) );
	//mElectrodes.Add( Electrode("111",  18.000,  26.000) );
	//mElectrodes.Add( Electrode("112",  30.000,  42.000) );
	//mElectrodes.Add( Electrode("113",  54.000,  58.000) );
	//mElectrodes.Add( Electrode("114",  10.000,  -38.000) );
	//mElectrodes.Add( Electrode("115",  0.000,  -22.000) );
	//mElectrodes.Add( Electrode("116",  13.000,  -6.000) );
	//mElectrodes.Add( Electrode("117",  21.000,  10.000) );
	//mElectrodes.Add( Electrode("118",  36.000,  26.000) );
	//mElectrodes.Add( Electrode("119",  54.000,  42.000) );
	//mElectrodes.Add( Electrode("120",  30.000,  -38.000) );
	//mElectrodes.Add( Electrode("121",  18.000,  -22.000) );
	//mElectrodes.Add( Electrode("122",  30.000,  -6.000) );
	//mElectrodes.Add( Electrode("123",  38.000,  10.000) );
	//mElectrodes.Add( Electrode("124",  54.000,  26.000) );
	//mElectrodes.Add( Electrode("125",  50.000,  -38.000) );
	//mElectrodes.Add( Electrode("126",  70.000,  -38.000) );
	//mElectrodes.Add( Electrode("127",  110.000,  -38.000) );
	//mElectrodes.Add( Electrode("128",  130.000,  -38.000) );

	// initialize hash map
	const uint32 numElectrodes = mElectrodes.Size();
	mElectrodesHashmap.reserve( numElectrodes );
	for (uint32 i=0; i<numElectrodes; ++i)
	{
		mElectrodesHashmap[mElectrodes[i].GetName()] = mElectrodes[i];
	}
}


bool EEGElectrodes::IsValidElectrodeID(const String& electrodeID) const
{
	auto itr = mElectrodesHashmap.find(electrodeID);
	if (itr != mElectrodesHashmap.end())
		return true;
	else
		return false;
}


EEGElectrodes::Electrode EEGElectrodes::GetElectrodeByID(const String& electrodeID) const
{
	auto itr = mElectrodesHashmap.find(electrodeID);
	if (itr != mElectrodesHashmap.end())
	{
		// found
		return itr->second;
	}
	else
	{
		return Electrode();
	}
}


Vector3 EEGElectrodes::Get3DPosition(const double radius, const Electrode& electrode) const
{
	// convert degrees in radians to match formula
	double thetaRadians = Math::DegreesToRadians( electrode.GetTheta() );
	double phiRadians   = Math::DegreesToRadians( electrode.GetPhi() - 90.0 );

	// calculate concrete coordinates on sphere with radius = 1
	double x = radius * Math::SinD(phiRadians) * Math::CosD(thetaRadians);
	double y = radius * Math::SinD(phiRadians) * Math::SinD(thetaRadians);
	double z = radius * Math::CosD(phiRadians);

	return Vector3(x, y, z);
}


Vector2 EEGElectrodes::Get2DPosition(const Electrode& electrode) const
{
	// get radius from 3D projection
	double radius       = GetRadiusFrom3DProjection( electrode.GetPhi() );
	double thetaRadians = Math::DegreesToRadians( electrode.GetTheta() );

	return Vector2( radius * Math::CosD(thetaRadians), radius * Math::SinD(thetaRadians) );
}


double EEGElectrodes::GetRadiusFrom3DProjection(double phi) const
{
	return Math::AbsD( (phi - 90.0) / 90.0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// constructor
EEGElectrodes::Electrode::Electrode()
{
	mTheta	= 0.0;
	mPhi	= 90.0;
}


// constructor
EEGElectrodes::Electrode::Electrode(const char* name, double theta, double phi)
{
	mName	= name;
	mTheta	= theta;
	mPhi	= phi;
}


// get the name
const char* EEGElectrodes::Electrode::GetName() const
{
	return mName.AsChar();
}


// get the name as string object
const Core::String& EEGElectrodes::Electrode::GetNameString() const
{
	return mName;
}
