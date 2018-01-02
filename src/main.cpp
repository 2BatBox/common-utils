#include <thread>
#include <vector>
#include <cstring>
#include <stdio.h>

#include "Log.h"

#include "cli_helper/sample.h"

int main(int argc, char** argv)
{

    sample_main(argc, argv);
    LOG("<---- the end of main() ---->");
    return 0;
}
