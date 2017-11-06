
#include "command_controller.h"

#include <string>
#include <iostream>
#include "command_parser.h"
#include "configurations.h"
#include "extension.h"
#include "extension_test_runner.h"
#include "utils.h"
#include "text_writer.h"

using namespace std;
using namespace Lya::lib;
using namespace Lya::lib::types;
using namespace Lya::lib::utils;
using namespace Lya::extension;
using namespace Lya::CommandParser;

namespace Lya::program {

	void print_default_help_info() {
	    TextWriter w;
	    w.add_tab(2);
	    w.add_tab(10);
	    w.write_line("Usage: lya [<options>] <command>");
	    w.newline();
	    w.write_line("Commands:");
	    w.newline();
	    for (const auto& command : commands) {
	        w.tab();
	        w.write(command.name);
	        w.tab();
	        w.write_line(command.description);
	    }
	    w.newline();
	    w.write_line("For more details: 'lya <command> --help'.");
	    w.newline();
	    w.clear_tabs();
	    w.add_tab(2);
	    w.add_tab(17);
	    w.write_line("Options:");
	    w.newline();
	    for (const auto& flag : default_flags) {
	        w.tab();
	        if (flag.alias.length() != 0) {
	            w.write(flag.name + ", " + flag.alias);
	        }
	        else {
	            w.write(flag.name);
	        }
	        w.tab();
	        w.write_line(flag.description);
	    }
	    w.print();
	}

	Command get_command(CommandKind command) {
	    for (unsigned int i = 0; i < commands.size(); i++) {
	        if (commands[i].kind == command) {
	            return commands[i];
	        }
	    }

	    throw logic_error("Could not get command name.");
	}

	void print_command_help_info(CommandKind command) {
	    auto a = get_command(command);
	    TextWriter w;
	    w.write_line(a.info);
	    w.newline();
	    w.write_line("Options:");
	    w.clear_tabs();
	    w.add_tab(2);
	    w.add_tab(24);
	    w.newline();
	    for (const auto& flag : get_command_flags(command)) {
	        w.tab();
	        if (flag.alias.length() != 0) {
	            w.write(flag.name + ", " + flag.alias);
	        }
	        else {
	            w.write(flag.name);
	        }
	        w.tab();
	        w.write_line(flag.description);
	    }
	    w.print();
	}

	void print_help_info(const Session& session) {
	    if (session.command == CommandKind::None) {
	        print_default_help_info();
	    }
	    else {
	        print_command_help_info(session.command);
	    }
	}

	void print_diagnostics(vector<Diagnostic> diagnostics) {
	    for (auto const& d : diagnostics) {
	        cout << d.message << endl;
	    }
	}

	int init(int argc, char* argv[]) {
	    shared_ptr<Session> session = make_shared<Session>(parse_command_args(argc, argv));
	    if (session->diagnostics.size() > 0) {
	        print_diagnostics(session->diagnostics);
	        return 1;
	    }
	    if (session->is_requesting_version) {
	        println("LYA version ", VERSION, ".");
	    }
	    else if (session->is_requesting_help) {
	        print_help_info(*session);
	    }
	    else {
	        switch (session->command) {
	            case CommandKind::Init:
	                break;
	            case CommandKind::Log:
	                break;
	            case CommandKind::Set:
	                break;
	            case CommandKind::Extension_RunTests: {
		            ExtensionTestRunner runner(session);
		            runner.run_extension_tests();
		            break;
	            }
	            case CommandKind::Extension_AcceptBaselines: {
	                string extension_file = resolve_paths(session->root_dir, "extension.json");
	                Lya::extension::Extension extension = Lya::extension::Extension::create(session, extension_file);
	                string currents_dir = resolve_paths(session->root_dir, extension.test_dir + "/currents");
	                string references_dir = replace_string(currents_dir, "currents", "references");
		            remove_folder(references_dir);
	                copy_folder(currents_dir, references_dir);
	                break;
	            }
	            default:
		            throw invalid_argument("Invalid parsed command.");
	        }
	    }
	    return 0;
	}
} // Lya::program
