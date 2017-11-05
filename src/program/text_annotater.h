//
// Created by Tingan Ho on 2017-10-14.
//

#ifndef LYA_TEXTANNOTATEWRITER_H
#define LYA_TEXTANNOTATEWRITER_H
#include <string>
#include <vector>
#include <map>
#include "types.h"

using namespace std;
using namespace Lya::types;

namespace Lya {
	struct SpanLocationAnnotation {
		SpanLocation location;
		string text;
	};

	class TextAnnotater {
		typedef map<uint64_t, vector<SpanLocationAnnotation>> AnnotationMap;
	public:
		TextAnnotater(const string& text);
		void annotate(const SpanLocation& location, const string& text);
		string get_text();
	private:
		vector<string> lines;
		AnnotationMap annotations;
	};
}

#endif //LYA_TEXTANNOTATEWRITER_H
