#include "script_libriary_test.h"
#include "vano_files.h"
#include "vma.hpp"

CREATE_SCRIPTLIBRIARY(SCRIPT_LIBRIARY_TEST)

bool SCRIPT_LIBRIARY_TEST::Init()
{
    Vano_Init();

    return true;
};
