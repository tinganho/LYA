//
// Created by Tingan Ho on 2017-10-14.
//

#include "TextWriter.h"
#include <string>
#include <iostream>
#include <vector>

using namespace std;


namespace Lya {

	TextWriter::TextWriter():
			column(0),
			indentation(0),
			tabs(),
			indentation_step(2) {
		if (getenv("COLUMNS") != NULL) {
			window_width = *(int *)(getenv("COLUMNS"));
		}
		else {
	#ifdef __unix__
			struct winsize w;
	        ioctl(0, TIOCGWINSZ, &w);
	        window_width = w.ws_col;
	#endif
		}
	}

	void TextWriter::indent() {
		indentation += indentation_step;
	}

	void TextWriter::unindent() {
		indentation -= indentation_step;
	}

	void TextWriter::print_indentation() {
		for (int i = 0; i < indentation; i++) {
			text += " ";
			column += 1;
		}
	}

	void TextWriter::add_tab(unsigned int indentation) {
	    tabs.push_back(indentation);
	}

	void TextWriter::tab() {
	    for (int i_tab = 0; i_tab < tabs.size(); i_tab++) {
	        if (column < tabs[i_tab]) {
	            int diff = tabs[i_tab] - column;
	            for (int i_diff = 0; i_diff < diff; i_diff++) {
	                text += " ";
	            }
	            column += diff;
	            break;
	        }
	        else {
	            text += " ";
	        }
	    }
	}

	void TextWriter::clear_tabs() {
	    tabs.clear();
	}

	void TextWriter::newline() {
	    text += '\n';
	    column = 0;
	    print_indentation();
	}

	void TextWriter::newline(unsigned int amount) {
	    for (int i = 0; i < amount; i++) {
	        text += '\n';
	    }
	    column = 0;
	    print_indentation();
	}

	void TextWriter::write(string t) {
	    text += t;
	    column += text.size();
	}

	void TextWriter::write_line(string t) {
	    write(t);
	    newline();
	}

	void TextWriter::print() {
	    cout << text;
	}

} // Lya