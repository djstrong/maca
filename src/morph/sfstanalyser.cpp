#include "sfstanalyser.h"
#include "exception.h"

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

	SfstAnalyser::~SfstAnalyser()
	{
		delete ct_;
	}

	Token* SfstAnalyser::process(const Toki::Token &t)
	{
		std::vector< CAnalysis > a;
		std::string s = t.orth_utf8();
		ct_->analyze_string(const_cast<char*>(s.c_str()), a);
		Token* tt = new Token(t);
		foreach (CAnalysis& ca, a) {
			//std::cout << s << "\t" <<<  unescape_analysis(ct_->print_analysis(ca)) << "\n";
			split_analysis_into(unescape_analysis((ct_->print_analysis(ca))), tt->lexemes());
		}
		return tt;
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

	Lexeme SfstAnalyser::split_analysis(const std::string &sfst_analysis)
	{
		size_t pos = sfst_analysis.find('<');
		if (pos == std::string::npos) {
			return Lexeme();
		}
		UnicodeString lemma(sfst_analysis.c_str(), pos);
		std::string tag_string(sfst_analysis.substr(pos + 1, sfst_analysis.length() - pos - 2));
		return Lexeme(lemma, Tag(tag_string));
	}

	void SfstAnalyser::split_analysis_into(const std::string &sfst_analysis, std::vector<Lexeme>& lv)
	{
		// this function assumes the SFST analyses are returned in one of the
		// following formats:
		// LEMMA_STRING<TAG_STRING>
		// LEMMA_STRING<TAG_STRING1+TAG_STRING_2+TAG_STRING3+...>
		size_t pos = sfst_analysis.find('<');
		if (pos != std::string::npos) {
			UnicodeString lemma(sfst_analysis.c_str(), pos);

			string_range sr(sfst_analysis.begin() + pos + 1, sfst_analysis.end() - 1);
			string_range_vector options;
			boost::algorithm::split(options, sr, boost::is_any_of("+"));

			boost::function<Lexeme (const Tag&)> lex;
			lex = boost::bind(&Lexeme::create, boost::cref(lemma), _1);

			boost::function<void (const Tag&)> func;
			func = boost::bind(&std::vector<Lexeme>::push_back, &lv, boost::bind(lex, _1));

			foreach (const string_range& o, options) {
				tagset().parse_tag(o, true, func);
			}
		}
	}


} /* end ns PlTagger */
