
#ifndef EXTENSION_TEST_RUNNER_H
#define EXTENSION_TEST_RUNNER_H

#include "Types.h"

using namespace Lya::Types;

namespace Lya::Extension {

void kill_all_processes(int signum);

void run_extension_tests(Lya::Types::Session& session);

} // Lya::Extension

#endif // EXTENSION_TEST_RUNNER_H
