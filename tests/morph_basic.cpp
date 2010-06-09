#include <boost/test/unit_test.hpp>

#include "morph/constanalyser.h"
#include "morph/dispatchanalyser.h"
#include "tagsetparser.h"

struct F {
	F() {
		const char tagset_string[] = "[ATTR]\nA a1 a2 a3\nB b1 b2 b3\n[POS]\n P1 A [B]\n P2 A\n";
		try {
			tagset.reset(new PlTagger::Tagset(tagset_string));
		} catch (PlTagger::TagsetParseError& e) {
			BOOST_FAIL(e.info());
		}
	}
	boost::shared_ptr<PlTagger::Tagset> tagset;
};

BOOST_FIXTURE_TEST_CASE( morph_const, F )
{
	Toki::Token t(UnicodeString::fromUTF8("aaa"), "ttt", Toki::Whitespace::ManySpaces);
	PlTagger::ConstAnalyser a(tagset.get(), "P1:a1");
	PlTagger::Token* tt = a.process(t);
	BOOST_CHECK_EQUAL(tt->orth_utf8(), t.orth_utf8());
	BOOST_CHECK_EQUAL(tt->wa(), t.preceeding_whitespace());
	BOOST_REQUIRE_EQUAL(tt->lexemes().size(), 1);
	const PlTagger::Lexeme& lex = tt->lexemes()[0];
	BOOST_CHECK(lex.lemma() == t.orth());
	BOOST_CHECK_EQUAL(tagset->tag_to_string(lex.tag()), "P1:a1");
}

BOOST_FIXTURE_TEST_CASE( morph_dispatch, F )
{
	Toki::Token t(UnicodeString::fromUTF8("aaa"), "t", Toki::Whitespace::ManySpaces);
	PlTagger::DispatchAnalyser a(tagset.get());
	std::string tag1s = "P1:a2:b1";
	std::string tag2s = "P2:a1";
	PlTagger::ConstAnalyser* ca1 = new PlTagger::ConstAnalyser(tagset.get(), tag1s);
	PlTagger::ConstAnalyser* ca2 = new PlTagger::ConstAnalyser(tagset.get(), tag2s);
	a.add_type_handler("t", ca1);
	a.add_type_handler("a", ca2);
	a.add_type_handler("b", ca2);
	PlTagger::Token* tt = a.process(t);
	BOOST_REQUIRE(tt);
	BOOST_REQUIRE_EQUAL(tt->lexemes().size(), 1);
	const PlTagger::Lexeme& lex = tt->lexemes()[0];
	BOOST_CHECK(lex.lemma() == t.orth());
	BOOST_CHECK_EQUAL(tagset->tag_to_string(lex.tag()), tag1s);
	delete tt;

	t.set_type("a");
	tt = a.process(t);
	BOOST_REQUIRE(tt);
	BOOST_REQUIRE_EQUAL(tt->lexemes().size(), 1);
	const PlTagger::Lexeme& lex2 = tt->lexemes()[0];
	BOOST_CHECK(lex2.lemma() == t.orth());
	BOOST_CHECK_EQUAL(tagset->tag_to_string(lex2.tag()), tag2s);
	delete tt;

	t.set_type("b");
	tt = a.process(t);
	BOOST_REQUIRE(tt);
	BOOST_REQUIRE_EQUAL(tt->lexemes().size(), 1);
	const PlTagger::Lexeme& lex3 = tt->lexemes()[0];
	BOOST_CHECK(lex3.lemma() == t.orth());
	BOOST_CHECK_EQUAL(tagset->tag_to_string(lex3.tag()), tag2s);
	delete tt;

	t.set_type("ZZZ");
	tt = a.process(t);
	BOOST_CHECK(tt == NULL);
}
