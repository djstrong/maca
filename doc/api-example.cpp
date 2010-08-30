#include <libtoki/tokenizer/layertokenizer.h>
#include <libtoki/sentencesplitter.h>
#include <libtoki/util/foreach.h>

#include <libmaca/morph/dispatchanalyser.h>
#include <libmaca/util/settings.h>

#include <boost/scoped_ptr.hpp>

#include <iostream>

int main(int argc, char** argv)
{
	std::string config;
	if (argc > 1) config = argv[1];
	else config = "morfeusz";	
	Maca::Config::Node cfg = Maca::get_named_config(config);
	
	Toki::LayerTokenizer tokr(std::cin);
	Toki::SentenceSplitter sentr(tokr);
	
	Maca::DispatchAnalyser analyser(cfg);
	
	while (sentr.has_more()) {
		boost::scoped_ptr<Toki::Sentence> sentence(sentr.get_next_sentence());
		assert(!sentence->empty());
		boost::scoped_ptr<Maca::Sentence> analysed_sentence(analyser.process_dispose(sentence.get()));
		std::cout << "{\n";
		foreach (Maca::Token *tok, analysed_sentence->tokens())
		{
			std::cout << "\t" << tok->orth_utf8();
			foreach(const Maca::Lexeme &lex, tok->lexemes())
			{
				std::cout << "\n\t\t" << lex.lemma_utf8();
			}
			std::cout << "\n";
		};
		std::cout << "}\n";
	}
}
