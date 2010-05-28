#include "sfstanalyser.h"
#include "exception.h"

#include <sfst/interface.h>
#include <sfst/compact.h>
#include <cstdio>
#include <libtoki/foreach.h>
#include <sstream>
#include <iostream>

#include <boost/range.hpp>
#include <boost/algorithm/string.hpp>

namespace PlTagger {

	SfstAnalyser::SfstAnalyser(const std::string &filename)
	{
		FILE* f = fopen(filename.c_str(), "rb");
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

	void split_tag_into(const boost::iterator_range<std::string::const_iterator>& sr, const UnicodeString& lemma, std::vector<Lexeme>& lv)
	{
		typedef std::vector< boost::iterator_range<std::string::const_iterator> > find_vector_type;
		find_vector_type fields;
		boost::algorithm::split(fields, sr, boost::is_any_of(":"));
		std::vector< find_vector_type > opts(1);
		foreach (const boost::iterator_range<std::string::const_iterator>& r, fields) {
			find_vector_type dots;
			boost::algorithm::split(dots, r, boost::is_any_of("."));
			foreach (find_vector_type& o, opts) {
				o.push_back(dots.front());
			}
			size_t opts_size = opts.size();
			for (size_t di = 1; di < dots.size(); ++di) {
				for (size_t oi = 0; oi < opts_size; ++oi) {
					opts.push_back(opts[oi]);
					opts.back().back() = dots[di];
				}
			}
		}
		foreach (const find_vector_type& o, opts) {
			std::stringstream ss;
			if (!o.empty()) {
				ss << o.front();
				for (size_t i = 1; i < o.size(); ++i) {
					ss << ":";
					ss << o[i];
				}
			}
			std::string tag_string = ss.str();
			lv.push_back(Lexeme(lemma, Tag(tag_string)));
		}
	}

	void SfstAnalyser::split_analysis_into(const std::string &sfst_analysis, std::vector<Lexeme>& lv)
	{
		size_t pos = sfst_analysis.find('<');
		if (pos != std::string::npos) {
			UnicodeString lemma(sfst_analysis.c_str(), pos);
			boost::sub_range<const std::string> sr(sfst_analysis.begin() + pos + 1, sfst_analysis.end() - 1);
			//lv.push_back(Lexeme(lemma, Tag(boost::copy_range<std::string>(sr))));
			typedef std::vector< boost::iterator_range<std::string::const_iterator> > find_vector_type;
			find_vector_type options;
			boost::algorithm::split(options, sr, boost::is_any_of("+"));
			foreach (const boost::iterator_range<std::string::const_iterator>& o, options) {
				split_tag_into(o, lemma, lv);
			}
		}
	}

} /* end ns PlTagger */
