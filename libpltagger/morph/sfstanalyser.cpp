#include <libpltagger/morph/sfstanalyser.h>
#include <libpltagger/exception.h>

#include <sfst/interface.h>
#include <sfst/compact.h>
#include <cstdio>
#include <libtoki/foreach.h>
#include <sstream>
#include <iostream>

#include <boost/range.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

namespace PlTagger {

	SfstAnalyser::SfstAnalyser(const Tagset* tagset, const std::string &filename)
		: MorphAnalyser(tagset)
	{
		open_transducer(filename);
	}

	SfstAnalyser::SfstAnalyser(const Config::Node &cfg)
		: MorphAnalyser(cfg)
	{
		std::string filename = cfg.get("file", "");
		if (filename.empty()) throw ConfigValueMissing("file", "SfstAnalyser");
		open_transducer(filename);
	}

	SfstAnalyser::~SfstAnalyser()
	{
		delete ct_;
	}

	void SfstAnalyser::open_transducer(const std::string& filename)
	{
		FILE* f = fopen(filename.c_str(), "rb");
		if (!f) {
			throw PlTaggerError("File open error");
		}
		try {
			ct_ = new CompactTransducer(f);
		} catch (const char* e) {
			fclose(f);
			throw PlTaggerError(e);
		}
		fclose(f);
	}

	bool SfstAnalyser::process_functional(const Toki::Token &t, boost::function<void (Token*)> sink)
	{
		std::vector< CAnalysis > a;
		std::string s = t.orth_utf8();
		ct_->analyze_string(const_cast<char*>(s.c_str()), a);
		if (a.empty()) {
			return false;
		} else {
			Token* tt = new Token(t);
			foreach (CAnalysis& ca, a) {
				//std::cout << s << "\t" <<<  unescape_analysis(ct_->print_analysis(ca)) << "\n";
				std::string analysis = unescape_analysis((ct_->print_analysis(ca)));
				// this function assumes the SFST analyses are returned the
				// LEMMA_STRING<TAG_STRING> format, TAG_STRING may be complex
				size_t pos = analysis.find('<');
				if (pos != std::string::npos || analysis.size() > pos) {
					UnicodeString lemma(analysis.c_str(), pos);
					//strip < and > from the tag string
					string_range sr(analysis.begin() + pos + 1, analysis.end() - 1);
					tagset().lexemes_into_token(*tt, lemma, sr);
				} else {
					throw TagParseError("SFST format: < missing", "", analysis, "");
				}
			}
			sink(tt);
			return true;
		}
	}

	std::string SfstAnalyser::unescape_analysis(const std::string &sfst_analysis)
	{
		std::string rv;
		rv = boost::algorithm::replace_all_copy(sfst_analysis, "\\:", ":");
		boost::algorithm::replace_all(rv, "\\.", ".");
		boost::algorithm::replace_all(rv, "\\+", "+");
		boost::algorithm::replace_all(rv, "\\-", "-");
		boost::algorithm::replace_all(rv, "\\\\", "\\");
		return rv;
	}

} /* end ns PlTagger */
