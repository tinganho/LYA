//
// Created by Tingan Ho on 2017-10-14.
//

#ifndef LYA_TEXTWRITER_H
#define LYA_TEXTWRITER_H

#include <string>
#include <vector>

namespace Lya::lib {

	class TextWriter {
	public:
		TextWriter();
		std::string text;
		void add_tab(unsigned int indentation);
		void tab();
		void clear_tabs();
		void newline();
		void newline(unsigned int amount);
		void write(const std::string& text);
		void write_line(const std::string& text);
		void print();
		void indent();
		void unindent();
	private:
		std::vector<int> tabs;
		int window_width;
		unsigned int column;
		int indentation;
		const unsigned int indentation_step;
		void print_indentation();
	};

}// Lya

#endif // LYA_TEXTWRITER_H
