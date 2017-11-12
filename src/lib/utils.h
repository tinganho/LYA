
#ifndef UTILS_H
#define UTILS_H

#include <cstdio>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <json/json.h>
#include <glob.h>
#include "types.h"
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

using namespace std;
using namespace Lya::lib::types;

namespace Lya::lib::utils {

	Diagnostic create_diagnostic(SpanLocation location, DiagnosticTemplate& d);
	Diagnostic create_diagnostic(SpanLocation location, DiagnosticTemplate& d, const string& arg1);
	Diagnostic create_diagnostic(SpanLocation location, DiagnosticTemplate& d, const string& arg1, const string& arg2);

	void add_diagnostic(Session& session, DiagnosticTemplate& d);
	void add_diagnostic(Session& session, DiagnosticTemplate& d, const string& arg1);
	void add_diagnostic(Session& session, DiagnosticTemplate& d, const string& arg1, const string& arg2);
	void add_diagnostic(Session& session, SpanLocation location, DiagnosticTemplate& d);
	void add_diagnostic(Session& session, SpanLocation location, DiagnosticTemplate& d, const string& arg1);
	void add_diagnostic(Session& session, SpanLocation location, DiagnosticTemplate& d, const string& arg1, const string& arg2);

	string execute_command(const string command);
	string execute_command(const string command, string cwd);

	void newline();

	void println(string text);
	void println(string text1, string text2);
	void println(string text1, string text2, string text3);

	bool path_exists(const string &path);

	string read_file(const string& file);
	void write_file(const string& file, const string& content);
	void write_file(const string& file, const string& content, const string& cwd);

	void create_folder(string dir);
	void remove_folder(const string &path);
	string folder_path(const string &path);
	bool copy_folder(const boost::filesystem::path& source, const boost::filesystem::path& destination);

	string resolve_paths(string path1, string path2);
	string join_paths(string path1, string path2, string path3);
	string root_path(string path);

	vector<string> find_files(string pattern);
	vector<string> find_files(string pattern, string cwd);

	string replace_string(const string& target, const string& pattern, const string& replacement);

	string get_cwd();
	string get_exec_path();

	vector<string> to_vector_of_strings(const Json::Value& vec);

	template<typename K, typename V>
	map<V, K> create_reverse_map(const map<K, V> &input_map);

	template<typename Out>
	void split(const string &s, char delimiter, Out result);
	vector<string> split(const string &s, char delimiter);

	void sleep(int ms);
	namespace Debug {
	    void fail(string err);
	}

} // Lya::Utils

#endif // UTILS_H
