#include <libpwrutils/util.h>

#include "morfeusz2analyser.h"

namespace Maca {

const char* MorfeuszAnalyser::identifier = "morfeusz2";

bool MorfeuszAnalyser::registered =
		MorphAnalyser::register_analyser<MorfeuszAnalyser>();

MorfeuszAnalyser::MorfeuszAnalyser(const Corpus2::Tagset* tagset,
				Conversion::TagsetConverter* conv)
{}

MorfeuszAnalyser::MorfeuszAnalyser(const Config::Node& cfg)
{}

MorfeuszAnalyser::MorfeuszAnalyser* clone() const
{}

MorfeuszAnalyser::~MorfeuszAnalyser()
{}

bool MorfeuszAnalyser::process_functional(const Toki::Token &t,
		boost::function<void(Corpus2::Token *)> sink)
{
	throw new std::error("Not implemented");
	return false;
}

void MorfeuszAnalyser::flush_convert(std::vector<Corpus2::Token*>& vec,
		boost::function<void(Corpus2::Token *)> sink)
{}

void MorfeuszAnalyser::flush_convert(std::vector< std::vector<Corpus2::Token*> >& vec,
		boost::function<void(Corpus2::Token *)> sink)
{}

bool MorfeuszAnalyser::process_complex_analysis(const Toki::Token &t,
							std::vector<details::MorfeuszEdge>& pmorf,
							boost::function<void(Corpus2::Token *)>sink)
{
	throw new std::error("Not implemented");
	return false;
}

Corpus2::Token* make_token(const Toki::Token& t,
						   const details::MorfeuszEdge& m) const
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

void morfeusz_into_token(Corpus2::Token* tt, const details::MorfeuszEdge& m) const
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
	MorfeuszEdge::MorfeuszEdge(const morfeusz::MorphInterpretation& morf)
	: node_from(morf.getStartNode()), node_to(morf.getEndNode())
	, orth(UnicodeString::fromUTF8(morf.getOrth()))
	, lemma(UnicodeString::fromUTF8(morf.getLemma()))
	, token(NULL)//, tag_string(???)
	{
	}
}

}
