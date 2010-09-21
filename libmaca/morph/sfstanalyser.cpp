#include <libmaca/morph/sfstanalyser.h>
#include <libmaca/exception.h>
#include <libmaca/util/settings.h>

#include <sfst/interface.h>
#include <sfst/compact.h>
#include <cstdio>
#include <libpwrutils/foreach.h>
#include <sstream>
#include <iostream>

#include <boost/range.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

namespace Maca {

const char* SfstAnalyser::identifier = "sfst";

bool SfstAnalyser::registered =
		MorphAnalyser::register_analyser<SfstAnalyser>();

SfstAnalyser::SfstAnalyser(const Tagset* tagset,
		const std::string &filename)
	: MorphAnalyser(tagset), lcase_(false)
{
	open_transducer(filename);
}

SfstAnalyser::SfstAnalyser(const Tagset* tagset)
	: MorphAnalyser(tagset), lcase_(false)
{
}

SfstAnalyser::SfstAnalyser(const Config::Node &cfg)
	: MorphAnalyser(cfg), lcase_(cfg.get("lower-case", false))
{
	std::string filename = cfg.get("file", "");
	if (filename.empty()) throw ConfigValueMissing("file", "SfstAnalyser");
	filename = Path::Instance().find_file_or_throw(filename, "SFST");
	open_transducer(filename);
}

SfstAnalyser::~SfstAnalyser()
{
}

SfstAnalyser* SfstAnalyser::clone() const
{
	SfstAnalyser* copy = new SfstAnalyser(&tagset());
	copy->lcase_ = lcase_;
	copy->ct_ = ct_;
	copy->mutex_ = mutex_;
	return copy;
}

void SfstAnalyser::open_transducer(const std::string& filename)
{
	FILE* f = fopen(filename.c_str(), "rb");
	if (!f) {
		throw MacaError("File open error");
	}
	try {
		ct_.reset(new CompactTransducer(f));
	} catch (const char* e) {
		fclose(f);
		throw MacaError(e);
	}
	fclose(f);
}

bool SfstAnalyser::process_functional(const Toki::Token &t,
		boost::function<void (Token*)> sink)
{
	std::vector< CAnalysis > a;
	std::string s;
	if (lcase_) {
		UnicodeString u = t.orth();
		u.toLower();
		u.toUTF8String(s);
	} else {
		s = t.orth_utf8();
	}
	ct_->analyze_string(const_cast<char*>(s.c_str()), a);
	if (a.empty()) {
		return false;
	} else {
		Token* tt = new Token(t);
		foreach (CAnalysis& ca, a) {
			//std::cout << s << "\t"
			//	<<  unescape_analysis(ct_->print_analysis(ca)) << "\n";
			std::string analysis = unescape_analysis(
					ct_->print_analysis(ca));
			// this function assumes the SFST analyses are returned the
			// LEMMA_STRING<TAG_STRING> format, TAG_STRING may be complex
			size_t pos = analysis.find('<');
			if (pos != std::string::npos || analysis.size() > pos) {
				UnicodeString lemma(analysis.c_str(), pos);
				//strip < and > from the tag string
				string_range sr(analysis.begin() + pos + 1,
						analysis.end() - 1);
				tagset().lexemes_into_token(*tt, lemma, sr);
			} else {
				throw TagParseError("SFST format: < missing", "", analysis,
						"");
			}
		}
		sink(tt);
		return true;
	}
}

std::string SfstAnalyser::unescape_analysis(
		const std::string &sfst_analysis)
{
	std::string rv;
	rv = boost::algorithm::replace_all_copy(sfst_analysis, "\\:", ":");
	boost::algorithm::replace_all(rv, "\\.", ".");
	boost::algorithm::replace_all(rv, "\\+", "+");
	boost::algorithm::replace_all(rv, "\\-", "-");
	boost::algorithm::replace_all(rv, "\\\\", "\\");
	return rv;
}

} /* end ns Maca */
