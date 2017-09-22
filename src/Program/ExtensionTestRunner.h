
#ifndef EXTENSION_TEST_RUNNER_H
#define EXTENSION_TEST_RUNNER_H

#include "Types.h"

using namespace Lya::Types;

namespace Lya {
namespace Extension {

void kill_all_processes(int signum);

void run_extension_tests(Lya::Types::Session& session);

} // Extension
} // Lya

#endif // EXTENSION_TEST_RUNNER_H
