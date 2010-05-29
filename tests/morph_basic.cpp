#include <boost/test/unit_test.hpp>

#include "morph/constanalyser.h"
#include "morph/dispatchanalyser.h"

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

BOOST_AUTO_TEST_CASE( morph_dispatch )
{
	Toki::Token t(UnicodeString::fromUTF8("aaa"), "t", Toki::Whitespace::ManySpaces);
	PlTagger::DispatchAnalyser a;
	PlTagger::ConstAnalyser* ca1 = new PlTagger::ConstAnalyser("tag1");
	PlTagger::ConstAnalyser* ca2 = new PlTagger::ConstAnalyser("tag2");
	a.add_type_handler("t", ca1);
	a.add_type_handler("a", ca2);
	a.add_type_handler("b", ca2);
	PlTagger::Token* tt = a.process(t);
	BOOST_REQUIRE(tt);
	BOOST_REQUIRE_EQUAL(tt->lexemes().size(), 1);
	const PlTagger::Lexeme& lex = tt->lexemes()[0];
	BOOST_CHECK(lex.lemma() == t.orth());
	BOOST_CHECK_EQUAL(lex.tag().to_string(), "tag1");
	delete tt;

	t.set_type("a");
	tt = a.process(t);
	BOOST_REQUIRE(tt);
	BOOST_REQUIRE_EQUAL(tt->lexemes().size(), 1);
	const PlTagger::Lexeme& lex2 = tt->lexemes()[0];
	BOOST_CHECK(lex2.lemma() == t.orth());
	BOOST_CHECK_EQUAL(lex2.tag().to_string(), "tag2");
	delete tt;

	t.set_type("b");
	tt = a.process(t);
	BOOST_REQUIRE(tt);
	BOOST_REQUIRE_EQUAL(tt->lexemes().size(), 1);
	const PlTagger::Lexeme& lex3 = tt->lexemes()[0];
	BOOST_CHECK(lex3.lemma() == t.orth());
	BOOST_CHECK_EQUAL(lex3.tag().to_string(), "tag2");
	delete tt;

	t.set_type("ZZZ");
	tt = a.process(t);
	BOOST_CHECK(tt == NULL);
}
