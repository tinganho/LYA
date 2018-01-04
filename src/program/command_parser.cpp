
#include <vector>
#include <string>
#include "types.h"
#include "utils.h"
#include "diagnostics/diagnostics.h"
#include "command_parser.h"

using namespace std;
using namespace Lya::lib::types;
using namespace Lya::lib::utils;
using namespace Lya::core::diagnostics;

namespace Lya::CommandParser {

static Flag help_flag = Flag(FlagKind::Help, "--help", "-h", "Print help description.", /*has_value*/ false);
static Flag language_flag = Flag(FlagKind::Language, "--language", "-l", "Specify language.", false);
static Flag root_dir = Flag(FlagKind::RootDir, "--root-dir", "-rd", "Specify current root directory(mainly for testing purposes).", /*has_value*/ true);

const vector<Flag> default_flags = {
    help_flag,
    root_dir,
    Flag(FlagKind::Version, "--version", "", "Print current version.", /*has_value*/ false),
};

static const vector<Flag> help_flags = {
    help_flag,
};

static const vector<Flag> set_flags = {
    Flag(FlagKind::Id, "--id", "-i", "Specify localization key.", /*has_value*/ true),
    Flag(FlagKind::Value, "--value", "-v", "Specify localization value.", /*has_value*/ true),
    Flag(FlagKind::Value, "--log-index", "-li", "Specify log index.", /*has_value*/ true),
    Flag(FlagKind::Value, "--search-index", "-se", "Specify latest search index.", /*has_value*/ true),
    language_flag,
};

static const vector<Flag> log_flags = {
    language_flag,
};

static const vector<Flag> extension_run_tests_flags = {
    Flag(FlagKind::NoServer, "--no-server", "-ns", "Do not open extension server(for testing purposes).", /*has_value*/ false),
    Flag(FlagKind::Grep, "--grep", "-g", "Grep test.", /*has_value*/ true),
    Flag(FlagKind::StartLine, "--start-line", "-sl", "Specify start line of test run(for testing purposes).", /* has_value */ true),
    Flag(FlagKind::Test, "--test", "-t", "Specify a test case to run.", /*has_value*/ true),
};

static const char* init_info =
    "Initialize a LYA project. This command creates a 'lya.json' "
    "file, with sane default options applied.\n\n"
    "Usage: lya init";

static const char* sync_info =
    "Synchronize your keys between source code and storage.\n\n"
    "Usage: lya sync";

static const char* log_info =
    "Show latest localizations.\n\n"
    "Usage: lya log [options]\n"
    "       lya log\n"
    "       lya log --language en-US";

static const char* set_info =
    "Set new localizations.\n\n"
    "Usage: lya set --key <key> --value <value> [options]\n"
    "       lya set --key LOGIN_TEXT --value \"Please login.\"\n"
    "       lya set --log-index 1 --value \"Please login.\"\n"
    "       lya set --search-index 1 --value \"Please login.\"";

static const char* extension_run_tests_info =
    "Run extension tests.\n\n"
    "Usage: lya extension-run-tests\n"
    "Usage: lya extension-run-tests --grep \"some-test-case\"\n";

const char* extension_accept_baselines_info =
    "Accept baselines.\n\n"
    "Usage: lya extension-accept-baselines\n";

const vector<Command> commands = {
    Command(CommandKind::Init, "init", "Initialize project.", init_info),
    Command(CommandKind::Sync, "sync", "Synchronize localization keys.", sync_info),
    Command(CommandKind::Log, "log", "Show latest added localizations.", log_info, log_flags),
    Command(CommandKind::Set, "set", "Set localization.", set_info, set_flags),
    Command(CommandKind::Extension_RunTests, "extension-run-tests", "Run extension tests.", extension_run_tests_info, extension_run_tests_flags),
    Command(CommandKind::Extension_AcceptBaselines, "extension-accept-baselines", "Accept baselines.", extension_accept_baselines_info),
};

const vector<Flag>& get_command_flags(CommandKind kind) {
    switch (kind) {
        case CommandKind::Init:
            return help_flags;
        case CommandKind::Sync:
            return help_flags;
        case CommandKind::Log:
            return log_flags;
        case CommandKind::Set:
            return set_flags;
        case CommandKind::Extension_RunTests:
            return extension_run_tests_flags;
        case CommandKind::Extension_AcceptBaselines:
            return set_flags;
        default:
            throw invalid_argument("Could not get action flag.");
    }
}

void set_command_flag(Session& session, const Flag* flag, const char* value = nullptr) {
    switch (flag->kind) {
        case FlagKind::Help:
            session.is_requesting_help = true;
            return;
        case FlagKind::Version:
            session.is_requesting_version = true;
            return;
	    case FlagKind::StartLine:
		    session.start_line = std::stoull(value);
		    return;
        case FlagKind::NoServer:
            session.start_server = false;
            return;
	    case FlagKind::Test:
		    session.test = make_unique<string>(string(value));
		    return;
        case FlagKind::RootDir:
            if (value[0] == '/') {
                session.root_dir = value;
            }
            else {
                session.root_dir = resolve_paths(session.root_dir, value) + "/";
            }
            return;
        default:
            throw invalid_argument("Unknown command flag.");
    }
}

Session parse_command_args(int argc, char* argv[]) {
    Session session;
    session.root_dir = get_cwd();

    // Flag to optimize parsing.
    bool has_command = false;

    // The option flag that is pending for a value.
    const Flag* flag_which_awaits_value = nullptr;
    vector<Flag> all_flags(default_flags);

    auto add_command = [&](const char* command) -> void {
        if (has_command) {
            add_diagnostic(session, D::You_cannot_run_several_commands);
            return;
        }

        for (auto const& c : commands) {
            if (strcmp(c.name.c_str(), command) == 0) {
                session.command = c.kind;
                all_flags.insert(all_flags.end(), c.flags.begin(), c.flags.end());
                has_command = true;
                return;
            }
        }

        // We can only reach here if the command is unknown.
        add_diagnostic(session, D::Unknown_command_0, command);
    };

    auto add_command_flag = [&](const char* arg) -> bool {
	    string extracted_flag;
	    string extracted_value;
	    bool is_equal_argument = false;
	    vector<string> flag_and_value = split_string(arg, '=');
	    extracted_flag = flag_and_value.at(0);
	    if (flag_and_value.size() == 2) {
		    extracted_value = flag_and_value.at(1);
		    is_equal_argument = true;
	    }
        for (auto const& flag : all_flags) {
            if (strcmp(flag.name.c_str(), extracted_flag.c_str()) == 0 || (flag.alias.length() != 0 && strcmp(flag.alias.c_str(), extracted_flag.c_str()) == 0)) {
                if (flag.has_value) {
	                if (is_equal_argument) {
		                set_command_flag(session, &flag, extracted_value.c_str());
	                }
	                else {
		                flag_which_awaits_value = &flag;
	                }
                }
                else {
                    set_command_flag(session, &flag);
                }
                return true;
            }
        }

        return false;
    };

    auto for_each_arg = [&](std::function<void (char*)> callback) -> void {
        for (int arg_index = 1; arg_index < argc; arg_index++) {
            callback(argv[arg_index]);
        }
    };

    for_each_arg([&](char* arg) -> void {
        if (flag_which_awaits_value == nullptr) {
            if (arg[0] != '-') {
                add_command(arg);
            }
            else if (!add_command_flag(arg)) {
                add_diagnostic(session, D::Unknown_command_flag_0, arg);
            }
        }
        else {
            set_command_flag(session, flag_which_awaits_value, arg);
            flag_which_awaits_value = nullptr;
        }
    });

    bool has_project_file = path_exists(session.root_dir + "lya.json");
    bool is_requesting_help_or_version = (session.is_requesting_help || session.is_requesting_version);
    bool is_running_extension_command = (session.command == CommandKind::Extension_RunTests || session.command == CommandKind::Extension_AcceptBaselines);
    if (!has_project_file && !is_requesting_help_or_version && !is_running_extension_command) {
        add_diagnostic(session, D::You_are_not_inside_a_LYA_project);
    }

    return session;
}

} // Lya::CommandParser
