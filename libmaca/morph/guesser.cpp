#include <libmaca/morph/guesser.h>
#include <boost/algorithm/string.hpp>
#include <Corpus/guesser_api.h>

namespace Maca {

const char* GuesserAnalyser::identifier = "guesser";

bool GuesserAnalyser::registered =
		MorphAnalyser::register_analyser<GuesserAnalyser>();

GuesserAnalyser::GuesserAnalyser(const Config::Node &cfg)
	: MorphAnalyser(cfg)
{
	SetCorpusEncoding(GUESSER_UTF8);
}

GuesserAnalyser::GuesserAnalyser(const Corpus2::Tagset *tagset)
	: MorphAnalyser(tagset)
{
	SetCorpusEncoding(GUESSER_UTF8);
}

GuesserAnalyser::~GuesserAnalyser()
{
}

GuesserAnalyser* GuesserAnalyser::clone() const
{
	GuesserAnalyser* copy = new GuesserAnalyser(*this);
	return copy;
}

bool GuesserAnalyser::process_functional(const Toki::Token &t,
	boost::function<void (Corpus2::Token *)> sink)
{
	std::string orthu8 = t.orth_utf8();
	const char* guesser_response = GuessForm(orthu8.c_str());
	if (!guesser_response) {
		throw MacaError("Guesser returned NULL");
	}
	std::string gs(guesser_response);
	std::vector<std::string> lines;
	boost::algorithm::split(lines, gs, boost::is_any_of("\n"));
	PwrNlp::Whitespace::Enum wa = t.preceeding_whitespace();
	foreach (const std::string& line, lines) {
		if (line.empty()) continue;
		std::vector<std::string> olt;
		boost::algorithm::split(olt, line, boost::is_any_of(" \t"));
		if (((olt.size() - 1) % 2 != 0) || (olt.size() < 3)) {
			throw MacaError("Unexpected number of tokens returned by Guesser: " + line);
		}
		UnicodeString the_orth = UnicodeString::fromUTF8(olt[0]);
		Corpus2::Token* tt = new Corpus2::Token(the_orth, wa);
		wa = PwrNlp::Whitespace::None;
		size_t lexeme_start_idx = 1;
		while (lexeme_start_idx + 1 < olt.size()) {
			UnicodeString the_lemma = UnicodeString::fromUTF8(olt[lexeme_start_idx]);
			Corpus2::Tag the_tag = tagset().parse_simple_tag(olt[lexeme_start_idx + 1]);
			Corpus2::Lexeme lex(the_lemma, the_tag);
			tt->add_lexeme(lex);
			lexeme_start_idx += 2;
		}
		sink(tt);
	}
	return true;
}

} /* end ns Maca */
