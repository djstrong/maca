#include <boost/test/unit_test.hpp>

#include <libmaca/morph/constanalyser.h>
#include <libmaca/morph/dispatchanalyser.h>
#include <libmaca/tagsetparser.h>

struct F {
	F() : t(UnicodeString::fromUTF8("aaa"), "t", Toki::Whitespace::ManySpaces)
	{
		const char tagset_string[] = "[ATTR]\nA a1 a2 a3\nB b1 b2 b3\n[POS]\n P1 A [B]\n P2 A\n";
		try {
			tagset.reset(new Maca::Tagset(tagset_string));
		} catch (Maca::TagsetParseError& e) {
			BOOST_FAIL(e.info());
		}
	}
	~F() {
	}

	boost::shared_ptr<Maca::Tagset> tagset;
	Toki::Token t;
};

BOOST_FIXTURE_TEST_CASE( morph_const, F )
{
	Maca::ConstAnalyser a(tagset.get(), "P1:a1");
	std::vector<Maca::Token*> tv = a.process(t);
	BOOST_REQUIRE_EQUAL(tv.size(), 1);
	Maca::Token* tt = tv[0];
	BOOST_CHECK_EQUAL(tt->orth_utf8(), t.orth_utf8());
	BOOST_CHECK_EQUAL(tt->wa(), t.preceeding_whitespace());
	BOOST_REQUIRE_EQUAL(tt->lexemes().size(), 1);
	const Maca::Lexeme& lex = tt->lexemes()[0];
	BOOST_CHECK(lex.lemma() == t.orth());
	BOOST_CHECK_EQUAL(tagset->tag_to_string(lex.tag()), "P1:a1");
	delete tt;
}

struct Fd : public F
{
	Fd() : F(), a(tagset.get()) {
		tag1s = "P1:a2:b1";
		tag2s = "P2:a1";
		Maca::ConstAnalyser* ca1 = new Maca::ConstAnalyser(tagset.get(), tag1s);
		Maca::ConstAnalyser* ca2 = new Maca::ConstAnalyser(tagset.get(), tag2s);
		a.add_type_handler("t", ca1);
		a.add_type_handler("a", ca2);
		a.add_type_handler("b", ca2);
	}

	~Fd() {

	}

	Maca::DispatchAnalyser a;
	std::string tag1s;
	std::string tag2s;
};

BOOST_FIXTURE_TEST_CASE( morph_dispatch1, Fd )
{
	std::vector<Maca::Token*> tv = a.process(t);
	BOOST_REQUIRE_EQUAL(tv.size(), 1);
	Maca::Token* tt = tv[0];
	BOOST_REQUIRE_EQUAL(tt->lexemes().size(), 1);
	const Maca::Lexeme& lex = tt->lexemes()[0];
	BOOST_CHECK(lex.lemma() == t.orth());
	BOOST_CHECK_EQUAL(tagset->tag_to_string(lex.tag()), tag1s);
	delete tt;
}

BOOST_FIXTURE_TEST_CASE( morph_dispatch2, Fd )
{
	t.set_type("a");
	std::vector<Maca::Token*> tv = a.process(t);
	BOOST_REQUIRE_EQUAL(tv.size(), 1);
	Maca::Token* tt = tv[0];
	BOOST_REQUIRE_EQUAL(tt->lexemes().size(), 1);
	const Maca::Lexeme& lex2 = tt->lexemes()[0];
	BOOST_CHECK(lex2.lemma() == t.orth());
	BOOST_CHECK_EQUAL(tagset->tag_to_string(lex2.tag()), tag2s);
	delete tt;
}

BOOST_FIXTURE_TEST_CASE( morph_dispatch3, Fd )
{
	t.set_type("b");
	std::vector<Maca::Token*> tv = a.process(t);
	BOOST_REQUIRE_EQUAL(tv.size(), 1);
	Maca::Token* tt = tv[0];
	BOOST_REQUIRE_EQUAL(tt->lexemes().size(), 1);
	const Maca::Lexeme& lex = tt->lexemes()[0];
	BOOST_CHECK(lex.lemma() == t.orth());
	BOOST_CHECK_EQUAL(tagset->tag_to_string(lex.tag()), tag2s);
	delete tt;
}

BOOST_FIXTURE_TEST_CASE( morph_dispatch4, Fd )
{
	t.set_type("ZZZ");
	BOOST_CHECK_THROW(a.process(t), Maca::MacaError);
}
