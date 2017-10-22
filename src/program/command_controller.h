
#ifndef COMMAND_CONTROLLER_H
#define COMMAND_CONTROLLER_H

#include <string>
#include "configurations.h"
#include "types.h"

using namespace std;
using namespace Lya::Types;

namespace Lya {

void print_default_help_info();

Command get_command(CommandKind command);

void print_command_help_info(CommandKind command);

void print_help_info(const Session& session);

void print_diagnostics(vector<Diagnostic> diagnostics);

int init(int argc, char* argv[]);

} // Lya

#endif // COMMAND_CONTROLLER_H