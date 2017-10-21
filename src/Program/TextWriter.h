//
// Created by Tingan Ho on 2017-10-14.
//

#ifndef LYA_TEXTWRITER_H
#define LYA_TEXTWRITER_H

#include <string>
#include <iostream>
#include <vector>

using namespace std;

namespace Lya {

	class TextWriter {
	public:
		TextWriter();
		string text;
		void add_tab(unsigned int indentation);
		void tab();
		void clear_tabs();
		void newline();
		void newline(unsigned int amount);
		void write(string text);
		void write_line(string text);
		void print();
		void indent();
		void unindent();
	private:
		vector<int> tabs;
		int window_width;
		unsigned int column;
		int indentation;
		const unsigned int indentation_step;
		void print_indentation();
	};

}// Lya

#endif // LYA_TEXTWRITER_H
