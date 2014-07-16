#include <fstream>

#include <libpwrutils/util.h>

#include <libmaca/util/settings.h>

#include "morfeusz2analyser.h"

namespace Maca {

// statics
const char* Morfeusz2Analyser::identifier = "morfeusz2";

bool Morfeusz2Analyser::registered =
		MorphAnalyser::register_analyser<Morfeusz2Analyser>();

const morfeusz::Charset Morfeusz2Analyser::charset = morfeusz::UTF8;

// construct, copy, destruct
Morfeusz2Analyser::Morfeusz2Analyser(const Corpus2::Tagset* tagset,
								Conversion::TagsetConverter* conv)
: MorphAnalyser(tagset), conv_(conv), warn_on_fold_failure_(false)
{
	require_matching_tagsets(conv_->tagset_to(), *tagset,
							"Morfeusz analyser creation");
}

Morfeusz2Analyser::Morfeusz2Analyser(const Config::Node& cfg)
: MorphAnalyser(cfg), conv_(NULL), ign_tag_(), warn_on_ign_(false)
{
	std::string fn = cfg.get("converter", "");
	std::ifstream ifs;
	Path::Instance().open_stream_or_throw(fn, ifs, "converter");

	Config::Node conv_cfg = Config::from_stream(ifs);
	std::auto_ptr<Conversion::TagsetConverter> c(
			new Conversion::TagsetConverter(conv_cfg));

	require_matching_tagsets(c->tagset_to(), *this,
			"Morfeusz analyser creation");
	conv_ = c.release();

	std::string ign_tag_string = cfg.get("ign_tag", "ign");
	ign_tag_ = conv_->tagset_from().parse_simple_tag(ign_tag_string);
	warn_on_ign_ = cfg.get("warn_on_ign", false);
	warn_on_fold_failure_ =  cfg.get("warn_on_fold_failure", false);
}

Morfeusz2Analyser* Morfeusz2Analyser::clone() const
{
	Morfeusz2Analyser* copy = new Morfeusz2Analyser(&tagset(), conv_->clone());
	copy->ign_tag_ = ign_tag_;
	copy->warn_on_ign_ = warn_on_ign_;
	copy->warn_on_fold_failure_ = warn_on_fold_failure_;
	return copy;
}

Morfeusz2Analyser::~Morfeusz2Analyser()
{
	delete conv_;
}

// public methods
bool Morfeusz2Analyser::process_functional(const Toki::Token &t,
					boost::function<void(Corpus2::Token *)> sink)
{
	using namespace morfeusz;

	std::string s = PwrNlp::to_utf8(t.orth());
	std::vector<details::Morfeusz2Edge> pmorf;

	Morfeusz morf = Morfeusz::createInstance();
	morf.setCharset(charset); // TODO: Czy potrzebujemy ustawić coś więcej? w szczególności analyzerDictionary?
	ResultsIterator *res_iter = morf.analyze(s);

	while(res_iter->hasNext())
		pmorf.push_back(details::Morfeusz2Edge(res_iter->next()));

	if(pmorf.size() == 1 && pmorf[0].lemma.length() > 0) { // only one analysis
		Corpus2::Token *tok = make_token(t, pmorf[0]);
		std::vector<Corpus2::Token*> vec(1, tok);
		flush_convert(vec, sink);
		return true;
	} else if(pmorf.size() > 1)
		return process_complex_analysis(t, pmorf, sink);
// TODO
	return false;
}

void Morfeusz2Analyser::flush_convert(std::vector<Corpus2::Token*>& vec,
							boost::function<void(Corpus2::Token *)> sink)
{
	conv_->convert_simple(vec, sink);
}

void Morfeusz2Analyser::flush_convert(std::vector< std::vector<Corpus2::Token*> >& vec,
										boost::function<void(Corpus2::Token *)> sink)
{
	conv_->convert_ambiguous(vec, sink, warn_on_fold_failure_);
}

// private methods
bool Morfeusz2Analyser::process_complex_analysis(const Toki::Token &t,
							std::vector<details::Morfeusz2Edge>& pmorf,
							boost::function<void(Corpus2::Token *)>sink)
{
	
	
	//throw new std::error("Not implemented");
	return false;
}

Corpus2::Token* Morfeusz2Analyser::make_token(const Toki::Token& t,
						   const details::Morfeusz2Edge& m) const
{
	Corpus2::Token* tt = new Corpus2::Token();
	if (m.node_from == 0) {
		tt->set_wa(t.preceeding_whitespace());
	} else {
		tt->set_wa(PwrNlp::Whitespace::None);
	}
	morfeusz_into_token(tt, m);
	return tt;
}

void Morfeusz2Analyser::morfeusz_into_token(Corpus2::Token* tt, const details::Morfeusz2Edge& m) const
{
	tt->set_orth(m.orth);
	if (!m.tag_string.empty()) {
		conv_->tagset_from().lexemes_into_token(*tt, m.lemma,
				m.tag_string);
	} else {
		Corpus2::Lexeme ign_lex(m.orth, ign_tag_);
		tt->add_lexeme(ign_lex);
		if (warn_on_ign_) {
			std::cerr << "Morfeusz: tagging as ign: "
					<< ign_lex.lemma_utf8() << "\n";
		}
	}
}

namespace details {
	Morfeusz2Edge::Morfeusz2Edge(const morfeusz::MorphInterpretation& morf)
	: node_from(morf.getStartNode()), node_to(morf.getEndNode())
	, orth(UnicodeString::fromUTF8(morf.getOrth()))
	, lemma(UnicodeString::fromUTF8(morf.getLemma()))
	, token(NULL)//, tag_string(???) TODO: jak skonstruować tag_string
	{
	}
}

}
