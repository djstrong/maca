#include <libtoki/tokenizer/layertokenizer.h>
#include <libtoki/sentencesplitter.h>
#include <libtoki/util/foreach.h>

#include <libpltagger/morph/dispatchanalyser.h>
#include <libpltagger/util/settings.h>

#include <boost/scoped_ptr.hpp>

#include <iostream>

int main(int argc, char** argv)
{
	std::string config;
	if (argc > 1) config = argv[1];
	else config = "morfeusz";	
	PlTagger::Config::Node cfg = PlTagger::get_named_config(config);
	
	Toki::LayerTokenizer tokr(std::cin);
	Toki::SentenceSplitter sentr(tokr);
	
	PlTagger::DispatchAnalyser analyser(cfg);
	
	while (sentr.has_more()) {
		boost::scoped_ptr<Toki::Sentence> sentence(sentr.get_next_sentence());
		assert(!sentence->empty());
		boost::scoped_ptr<PlTagger::Sentence> analysed_sentence(analyser.process_dispose(sentence.get()));
		std::cout << "{\n";
		foreach (PlTagger::Token *tok, analysed_sentence->tokens())
		{
			std::cout << "\t" << tok->orth_utf8();
			foreach(const PlTagger::Lexeme &lex, tok->lexemes())
			{
				std::cout << "\n\t\t" << lex.lemma_utf8();
			}
			std::cout << "\n";
		};
		std::cout << "}\n";
	}
}
