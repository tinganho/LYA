
#include <sstream>
#include <string>
#include "TextAnnotater.h"
#include "TextWriter.h"
#include "Utils.h"

using namespace Lya::Utils;

namespace Lya {

	TextAnnotater::TextAnnotater(const string& _text) {
		stringstream ss;
		ss.str(_text);
		string item;
		while (getline(ss, item)) {
			lines.push_back(item);
		}
	}

	void TextAnnotater::annotate(const SpanLocation& location, const string& text) {
		auto it = annotations.find(location.line);
		SpanLocationAnnotation span_annotation { location, text };
		if (it != annotations.end()) {
			auto& vec = it->second;
			auto last_position = vec.back().location.position;
			if (location.position < last_position) {
				throw invalid_argument("You cannot insert an annotation that has a position lower than the last annotation.");
			}
			vec.push_back(span_annotation);
		}
		else {
			vector<SpanLocationAnnotation> a = { span_annotation };
			annotations[location.line] = a;
		}
	}

	string TextAnnotater::get_text() {
		TextWriter writer;
		for (unsigned int i = 0; i < lines.size(); i++) {
			writer.write_line(lines[i]);
			auto size = (unsigned int)lines[i].size();
			auto it = annotations.find(i + 1);
			if (it != annotations.end()) {
				vector<SpanLocationAnnotation> span_annotations = it->second;
				for (const auto & sa : span_annotations) {
					auto l = sa.location;
					for (int i1 = 1; i1 < l.column; i1++) {
						writer.write(" ");
					}
					int max = min(l.length, size - l.column);
					for (int i2 = 0; i2 < max; i2++) {
						writer.write("~");
					}
					writer.write(" (" + to_string(l.line) + "," + to_string(l.column) + "," + to_string(l.length) + ")");
					writer.newline();
					writer.write_line("!!! " + sa.text);
				}
			}
		}
		return writer.text;
	}
};
