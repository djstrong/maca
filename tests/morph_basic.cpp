#include <boost/test/unit_test.hpp>

#include "morph/constanalyser.h"

BOOST_AUTO_TEST_CASE( morph_const )
{
	Toki::Token t(UnicodeString::fromUTF8("aaa"), "ttt", Toki::Whitespace::ManySpaces);
	PlTagger::ConstAnalyser a("tag");
	PlTagger::Token* tt = a.process(t);
	BOOST_CHECK_EQUAL(tt->orth_utf8(), t.orth_utf8());
	BOOST_CHECK_EQUAL(tt->wa(), t.preceeding_whitespace());
	BOOST_REQUIRE_EQUAL(tt->lexemes().size(), 1);
	const PlTagger::Lexeme& lex = tt->lexemes()[0];
	BOOST_CHECK(lex.lemma() == t.orth());
	BOOST_CHECK_EQUAL(lex.tag().to_string(), "tag");
}

