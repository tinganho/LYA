﻿
#include "utils.h"
#include <unistd.h>
#include <iostream>
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

using namespace Lya::lib::types;
using boost::asio::ip::tcp;

namespace Lya::lib::utils {

	static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
	std::u32string from_utf8_to_u32(const std::string& text) {
		return cvt.from_bytes(text);
	}

	Diagnostic create_diagnostic(DiagnosticTemplate& d) {
		std::string message = d.message_template;
		return Diagnostic { message };
	}

	Diagnostic create_diagnostic(DiagnosticTemplate& d, const std::string& arg1) {
		std::string message = boost::regex_replace(d.message_template, boost::regex("\\{0\\}"), arg1);
		return Diagnostic { message };
	}

	Diagnostic create_diagnostic(DiagnosticTemplate& d, const std::string& arg1, const std::string& arg2) {
		std::string message1 = boost::regex_replace(d.message_template, boost::regex("\\{0\\}"), arg1);
		std::string message2 = boost::regex_replace(message1, boost::regex("\\{1\\}"), arg2);
		return Diagnostic { message2 };
	}

	Diagnostic create_diagnostic(SpanLocation location, DiagnosticTemplate& d) {
	    std::string message = d.message_template;
	    return Diagnostic { message, location };
	}

	Diagnostic create_diagnostic(SpanLocation location, DiagnosticTemplate& d, const std::string& arg1) {
	    std::string message = boost::regex_replace(d.message_template, boost::regex("\\{0\\}"), arg1);
	    return Diagnostic { message, location };
	}

	Diagnostic create_diagnostic(SpanLocation location, DiagnosticTemplate& d, const std::string& arg1, const std::string& arg2) {
	    std::string message1 = boost::regex_replace(d.message_template, boost::regex("\\{0\\}"), arg1);
	    std::string message2 = boost::regex_replace(message1, boost::regex("\\{1\\}"), arg2);
	    return Diagnostic { message2, location };
	}

	void add_diagnostic(Session& session, DiagnosticTemplate& d) {
		session.add_diagnostic(create_diagnostic(d));
	}

	void add_diagnostic(Session& session, DiagnosticTemplate& d, const std::string& arg1) {
		session.add_diagnostic(create_diagnostic(d, arg1));
	}

	void add_diagnostic(Session& session, DiagnosticTemplate& d, const std::string& arg1, const std::string& arg2) {
		session.add_diagnostic(create_diagnostic(d, arg1, arg2));
	}

	void add_diagnostic(Session& session, SpanLocation location, DiagnosticTemplate& d) {
	    session.add_diagnostic(create_diagnostic(location, d));
	}

	void add_diagnostic(Session& session, SpanLocation location, DiagnosticTemplate& d, const std::string& arg1) {
	    session.add_diagnostic(create_diagnostic(location, d, arg1));
	}

	void add_diagnostic(Session& session, SpanLocation location, DiagnosticTemplate& d, const std::string& arg1, const std::string& arg2) {
	    session.add_diagnostic(create_diagnostic(location, d, arg1, arg2));
	}

	std::string execute_command(const std::string& command) {
	    char buffer[128];
	    std::string result = "";
	#if defined(__APPLE__) || defined(__linux__)
	    std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
	    if (!pipe) {
	        throw std::runtime_error("popen() failed!");
	    }
	    while (!feof(pipe.get())) {
	        if (fgets(buffer, 128, pipe.get()) != NULL) {
	            result += buffer;
	        }
	    }
	#endif
	    return result;
	}

	std::string execute_command(const std::string& command, const std::string& cwd) {
	    return execute_command("cd " + cwd + " && " + command);
	}

	void println(const std::string& text) {
	    std::cout << text << std::endl;
	}

	void println(const std::string& text1, const std::string& text2) {
	    std::cout << text1 << text2 << std::endl;
	}

	void println(const std::string& text1, const std::string& text2, const std::string& text3) {
	    std::cout << text1 << text2 << text3 << std::endl;
	}

	void sleep(int ms) {
	    usleep(ms);
	}

	bool path_exists(const std::string& path) {
		boost::filesystem::path candidate(path);
		return boost::filesystem::exists(candidate);
	}

	std::string read_file(const std::string& file) {
	    std::ifstream t(file);
	    std::stringstream buffer;
	    buffer << t.rdbuf();
	    return buffer.str();
	}

	void write_file(const std::string& file, const std::string& content) {
	    std::ofstream f;
	    f.open(file);
	    f << content;
	    f.close();
	}

	void write_file(const std::string& file, const std::string& content, const std::string& cwd) {
	    write_file(cwd + file, content);
	}

	void remove_folder(const std::string &path) {
	    boost::filesystem::remove_all(boost::filesystem::path(path));
	}

	bool copy_folder(const boost::filesystem::path& source, const boost::filesystem::path& destination) {
	    try {
	        if (!boost::filesystem::exists(source) || !boost::filesystem::is_directory(source)) {
	            std::cerr << "Source directory '" << source.string()
	                << "' does not exist or is not a directory." << '\n';
	            return false;
	        }
	        if (boost::filesystem::exists(destination)) {
	            std::cerr << "Destination directory '" << destination.string()
	                << "' already exists." << '\n';
	            return false;
	        }
	        if (!boost::filesystem::create_directory(destination)) {
	            std::cerr << "Unable to create destination directory '" << destination.string() << "'.\n";
	            return false;
	        }
	    }
	    catch (boost::filesystem::filesystem_error const & e) {
	        std::cerr << e.what() << '\n';
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
	            std::cerr << e.what() << '\n';
	        }
	    }
	    return true;
	}

	std::string folder_path(const std::string& path) {
#if defined _WIN32 || defined __CYGWIN__
		return path.substr(0, path.find_last_of("\\"));
#else
		return path.substr(0, path.find_last_of("/"));
#endif
	}

	std::string replace_string(const std::string& target, const std::string& pattern, const std::string& replacement) {
	    return boost::replace_all_copy(target, pattern, replacement);
	}

	namespace Debug {
	    void fail(std::string err) {
	        throw std::logic_error(err);
	    }
	}

	void create_folder(std::string dir) {
	    boost::filesystem::path d(dir);
	    boost::filesystem::create_directories(d);
	}

	std::vector<std::string> find_files(const std::string& pattern) {
	    glob::Glob glob(pattern);
	    std::vector<std::string> files;
	    while (glob) {
	        const std::string path = pattern.substr(0, pattern.find_last_of('/'));
	        files.push_back(path + '/' + glob.GetFileName());
	        glob.Next();
	    }
	    return files;
	}

	std::vector<std::string> find_files(const std::string& pattern, const std::string& cwd) {
	    if (cwd.front() != '/') {
	        throw std::invalid_argument("Utils::find_files: Current working directory 'p_cwd', must be an absolute path. Got '" + cwd + "'.");
	    }
		std::string _cwd = cwd;
	    if (cwd.back() != '/') {
	        _cwd = _cwd + '/';
	    }
	    return find_files(_cwd + pattern);
	}

	std::string resolve_paths(const std::string& path1, const std::string& path2) {
	    boost::filesystem::path p1 (path1);
	    boost::filesystem::path p2 (path2);
	    return boost::filesystem::canonical(p1 / p2).string();
	}

	std::string resolve_paths(const std::string& path1, const std::string& path2, const std::string& path3) {
	    boost::filesystem::path p1 (path1);
	    boost::filesystem::path p2 (path2);
	    boost::filesystem::path p3 (path3);
	    return boost::filesystem::canonical(p1 / p2 / p3).string();
	}

	std::string root_path(const std::string& path) {
		std::string exec_path = get_exec_path();
		return resolve_paths(exec_path, "../");
	}

	std::string get_cwd() {
	    boost::filesystem::path full_path(boost::filesystem::current_path());
	    return full_path.string() + "/";
	}

	std::string get_exec_path() {
#ifdef WINDOWS
		char result[MAX_PATH];
		return string(result, GetModuleFileName(NULL, result, MAX_PATH));
#else
		char result[PATH_MAX];
		ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
		return std::string(result, (count > 0) ? count : 0);
#endif
	}

	std::vector<std::string> to_vector_of_strings(const Json::Value& vec) {
	    std::vector<std::string> res;
	    for (const Json::Value& item : vec) {
	        res.push_back(item.asString());
	    }
	    return res;
	}

	template<typename Out>
	void split(const std::string& s, char delimiter, Out result) {
		std::stringstream ss;
		ss.str(s);
		std::string item;
		while (getline(ss, item, delimiter)) {
			*(result++) = item;
		}
	}

	std::vector<std::string> split_string(const std::string& s, char delimiter) {
		std::vector<std::string> elements;
		split(s, delimiter, back_inserter(elements));
		return elements;
	}
} // Lya::Utils
