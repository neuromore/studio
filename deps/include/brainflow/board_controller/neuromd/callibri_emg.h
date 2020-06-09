#pragma once

#ifndef BRAINFLOW_NO_NEUROMD

#include "callibri.h"

class CallibriEMG : public Callibri
{

#if defined _WIN32 || defined __APPLE__

protected:
    int apply_initial_settings ();
#endif

public:
    CallibriEMG (struct BrainFlowInputParams params) : Callibri ((int)CALLIBRI_EMG_BOARD, params)
    {
    }
    ~CallibriEMG ()
    {
    }
};
#endif
