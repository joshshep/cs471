#ifndef LOCAL_ALIGNER_H_
#define LOCAL_ALIGNER_H_

#include "aligner/aligner.h"

class LocalAligner : public Aligner
{
    // use base class's constructor
    using Aligner::Aligner;

    int RunDP();

    Alignment RetraceDP();
};

#endif
