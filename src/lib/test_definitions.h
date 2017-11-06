
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <exception>
#include <string>
#include <iostream>
#include <vector>
#include <functional>
#include "utils.h"

using namespace std;

namespace Lya::lib::test_definitions {

	struct Test {
	    string name;
	    function<void(Test* t)> procedure;
	    bool success;

	    Test(string name, function<void(Test* t)> procedure);
	};

	struct Domain {
	    string name;
	    vector<Test*>tests = {};

	    Domain(string name);
	};

	void domain(string name);

	void test(string name, function<void(Test* t)> procedure);

	int print_result();

	void run_tests();

} // Lya::TestFramework

#endif // DEFINITIONS_H
