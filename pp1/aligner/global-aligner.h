#include "aligner/aligner.h"

class GlobalAligner : public Aligner
{
    // use base class's constructor
    using Aligner::Aligner;

    int RunDP();

    Alignment RetraceDP();
};
