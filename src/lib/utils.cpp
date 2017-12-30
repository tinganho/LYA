
#include "utils.h"
#include <unistd.h>
#include <boost/asio.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#ifdef WINDOWS
#include <windows.h>
#else
#include <limits.h>
#include <unistd.h>
#endif
#include <codecvt>
#include <boost/regex.hpp>

using namespace std;
using namespace Lya::lib::types;
using boost::asio::ip::tcp;

namespace Lya::lib::utils {

	static wstring_convert<codecvt_utf8<char32_t>, char32_t> cvt;
	u32string from_utf8_to_u32(const string& text) {
		return cvt.from_bytes(text);
	}

	Diagnostic create_diagnostic(DiagnosticTemplate& d) {
		string message = d.message_template;
		return Diagnostic { message };
	}

	Diagnostic create_diagnostic(DiagnosticTemplate& d, const string& arg1) {
		string message = boost::regex_replace(d.message_template, boost::regex("\\{0\\}"), arg1);
		return Diagnostic { message };
	}

	Diagnostic create_diagnostic(DiagnosticTemplate& d, const string& arg1, const string& arg2) {
		string message1 = boost::regex_replace(d.message_template, boost::regex("\\{0\\}"), arg1);
		string message2 = boost::regex_replace(message1, boost::regex("\\{1\\}"), arg2);
		return Diagnostic { message2 };
	}

	Diagnostic create_diagnostic(SpanLocation location, DiagnosticTemplate& d) {
	    string message = d.message_template;
	    return Diagnostic { message, location };
	}

	Diagnostic create_diagnostic(SpanLocation location, DiagnosticTemplate& d, const string& arg1) {
	    string message = boost::regex_replace(d.message_template, boost::regex("\\{0\\}"), arg1);
	    return Diagnostic { message, location };
	}

	Diagnostic create_diagnostic(SpanLocation location, DiagnosticTemplate& d, const string& arg1, const string& arg2) {
	    string message1 = boost::regex_replace(d.message_template, boost::regex("\\{0\\}"), arg1);
	    string message2 = boost::regex_replace(message1, boost::regex("\\{1\\}"), arg2);
	    return Diagnostic { message2, location };
	}

	void add_diagnostic(Session& session, DiagnosticTemplate& d) {
		session.add_diagnostic(create_diagnostic(d));
	}

	void add_diagnostic(Session& session, DiagnosticTemplate& d, const string& arg1) {
		session.add_diagnostic(create_diagnostic(d, arg1));
	}

	void add_diagnostic(Session& session, DiagnosticTemplate& d, const string& arg1, const string& arg2) {
		session.add_diagnostic(create_diagnostic(d, arg1, arg2));
	}

	void add_diagnostic(Session& session, SpanLocation location, DiagnosticTemplate& d) {
	    session.add_diagnostic(create_diagnostic(location, d));
	}

	void add_diagnostic(Session& session, SpanLocation location, DiagnosticTemplate& d, const string& arg1) {
	    session.add_diagnostic(create_diagnostic(location, d, arg1));
	}

	void add_diagnostic(Session& session, SpanLocation location, DiagnosticTemplate& d, const string& arg1, const string& arg2) {
	    session.add_diagnostic(create_diagnostic(location, d, arg1, arg2));
	}

	string execute_command(const string command) {
	    char buffer[128];
	    string result = "";
	#if defined(__APPLE__) || defined(__linux__)
	    shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
	    if (!pipe) {
	        throw runtime_error("popen() failed!");
	    }
	    while (!feof(pipe.get())) {
	        if (fgets(buffer, 128, pipe.get()) != NULL) {
	            result += buffer;
	        }
	    }
	#endif
	    return result;
	}

	string execute_command(const string command, string cwd) {
	    return execute_command("cd " + cwd + " && " + command);
	}

	void newline() {
	    cout << endl;
	}

	void println(string text) {
	    cout << text << endl;
	}

	void println(string text1, string text2) {
	    cout << text1 << text2 << endl;
	}

	void println(string text1, string text2, string text3) {
	    cout << text1 << text2 << text3 << endl;
	}

	void sleep(int ms) {
	    usleep(ms);
	}

	bool path_exists(const string &path) {
		boost::filesystem::path candidate(path);
		return boost::filesystem::exists(candidate);
	}

	string read_file(const string& file) {
	    ifstream t(file);
	    stringstream buffer;
	    buffer << t.rdbuf();
	    return buffer.str();
	}

	void write_file(const string& file, const string& content) {
	    ofstream f;
	    f.open(file);
	    f << content;
	    f.close();
	}

	void write_file(const string& file, const string& content, const string& cwd) {
	    write_file(cwd + file, content);
	}

	void remove_folder(const string &path) {
	    boost::filesystem::remove_all(boost::filesystem::path(path));
	}

	bool copy_folder(const boost::filesystem::path& source, const boost::filesystem::path& destination) {
	    try {
	        if (!boost::filesystem::exists(source) || !boost::filesystem::is_directory(source)) {
	            cerr << "Source directory '" << source.string()
	                << "' does not exist or is not a directory." << '\n';
	            return false;
	        }
	        if (boost::filesystem::exists(destination)) {
	            cerr << "Destination directory '" << destination.string()
	                << "' already exists." << '\n';
	            return false;
	        }
	        if (!boost::filesystem::create_directory(destination)) {
	            cerr << "Unable to create destination directory '" << destination.string() << "'.\n";
	            return false;
	        }
	    }
	    catch (boost::filesystem::filesystem_error const & e) {
	        cerr << e.what() << '\n';
	        return false;
	    }
	    for (boost::filesystem::directory_iterator file(source); file != boost::filesystem::directory_iterator(); ++file) {
	        try {
	            boost::filesystem::path current(file->path());
	            if (boost::filesystem::is_directory(current)) {
	                if (!copy_folder(current, destination / current.filename())) {
	                    return false;
	                }
	            }
	            else {
	                boost::filesystem::copy_file(current, destination / current.filename());
	            }
	        }
	        catch (boost::filesystem::filesystem_error const & e) {
	            cerr << e.what() << '\n';
	        }
	    }
	    return true;
	}

	string folder_path(const string &path) {
#if defined _WIN32 || defined __CYGWIN__
		return path.substr(0, path.find_last_of("\\"));
#else
		return path.substr(0, path.find_last_of("/"));
#endif
	}

	string replace_string(const string& target, const string& pattern, const string& replacement) {
	    return boost::replace_all_copy(target, pattern, replacement);
	}

	namespace Debug {
	    void fail(string err) {
	        throw logic_error(err);
	    }
	}

	void create_folder(string dir) {
	    boost::filesystem::path d(dir);
	    boost::filesystem::create_directories(d);
	}

	vector<string> find_files(string pattern) {
	    glob::Glob glob(pattern);
	    vector<string> files;
	    while (glob) {
	        string path = pattern.substr(0, pattern.find_last_of('/'));
	        files.push_back(path + '/' + glob.GetFileName());
	        glob.Next();
	    }
	    return files;
	}

	vector<string> find_files(string pattern, string cwd) {
	    if (cwd.front() != '/') {
	        throw invalid_argument("Utils::find_files: Current working directory 'p_cwd', must be an absolute path. Got '" + cwd + "'.");
	    }
	    if (cwd.back() != '/') {
	        cwd += '/';
	    }
	    return find_files(cwd + pattern);
	}

	string resolve_paths(string path1, string path2) {
	    boost::filesystem::path p1 (path1);
	    boost::filesystem::path p2 (path2);
	    return boost::filesystem::canonical(p1 / p2).string();
	}

	string join_paths(string path1, string path2, string path3) {
	    boost::filesystem::path p1 (path1);
	    boost::filesystem::path p2 (path2);
	    boost::filesystem::path p3 (path2);
	    return boost::filesystem::canonical(p1 / p2 / p3).string();
	}

	string root_path(string path) {
		string exec_path = get_exec_path();
		cout << exec_path << endl;
		return resolve_paths(exec_path, "../");
	}

	string get_cwd() {
	    boost::filesystem::path full_path(boost::filesystem::current_path());
	    return full_path.string() + "/";
	}

	string get_exec_path() {
#ifdef WINDOWS
		char result[MAX_PATH];
		return string(result, GetModuleFileName(NULL, result, MAX_PATH));
#else
		char result[PATH_MAX];
		ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
		return string(result, (count > 0) ? count : 0);
#endif
	}

	vector<string> to_vector_of_strings(const Json::Value& vec) {
	    vector<string> res;
	    for (const Json::Value& item : vec) {
	        res.push_back(item.asString());
	    }
	    return res;
	}

	template<typename Out>
	void split(const string& s, char delimiter, Out result) {
		stringstream ss;
		ss.str(s);
		string item;
		while (getline(ss, item, delimiter)) {
			*(result++) = item;
		}
	}

	vector<string> split(const string &s, char delimiter) {
		vector<string> elements;
		split(s, delimiter, back_inserter(elements));
		return elements;
	}
} // Lya::Utils
