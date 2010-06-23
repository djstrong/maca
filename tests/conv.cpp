#include <boost/test/unit_test.hpp>
#include <boost/bind.hpp>

#include <libpltagger/morph/constanalyser.h>
#include <libpltagger/morph/dispatchanalyser.h>
#include <libpltagger/tagsetparser.h>
#include <libpltagger/conv/tagconvert.h>
#include <libpltagger/conv/tagsetconverter.h>
#include <libpltagger/conv/tagrulelayer.h>
#include <libpltagger/conv/joinlayer.h>

#include <iostream>

using namespace PlTagger;

BOOST_AUTO_TEST_SUITE(conv)

struct Fxz {
	Fxz() {
		const char tagset1_string[] = "[ATTR]\n"
			"A a1 a2 \n"
			"B b1 b2 b3 \n"
			"C c1 c2 \n"
			"[POS]\n "
			"P1 A B \n"
			"P2 A B C\n"
			;
		const char tagset2_string[] = "[ATTR]\n"
			"A a1 a2 a3 \n"
			"B b1 b2 b3 \n"
			"C c1 c2 c3\n"
			"D d1 d2 \n"
			"[POS]\n "
			"P0 C [D] \n"
			"P1 A B \n"
			"P2 A B C\n"
			;
		tagset1.reset(new PlTagger::Tagset(tagset1_string));
		tagset2.reset(new PlTagger::Tagset(tagset2_string));
	}

	boost::shared_ptr<PlTagger::Tagset> tagset1, tagset2;
};

BOOST_FIXTURE_TEST_CASE(cast1, Fxz)
{
	char tag_string[] = "P1:a2:b1";
	Tag t1 = tagset1->parse_simple_tag(tag_string, false);
	BOOST_REQUIRE_EQUAL(tagset1->tag_to_string(t1), tag_string);
	Conversion::TagConverter cv(*tagset1, *tagset2);
	Tag t2 = cv.cast(t1);
	BOOST_CHECK_EQUAL(tagset2->tag_to_string(t2), tagset1->tag_to_string(t1));
}

BOOST_FIXTURE_TEST_CASE(castlayer, Fxz)
{
	Conversion::TagsetConverter tc;
	Conversion::TagConverter tcv(*tagset1, *tagset2);
	tc.add_layer(new Conversion::TagConvertLayer(tcv));
	char tag_string[] = "P1:a2:b1";
	Tag t1 = tagset1->parse_simple_tag(tag_string, false);
	Token* tok = new Token(UnicodeString::fromUTF8("abc"), Toki::Whitespace::None);
	tok->add_lexeme(Lexeme(UnicodeString::fromUTF8("aaa"), t1));
	std::vector<Token*> tv, tv2;
	tv.push_back(tok);
	tc.convert_simple(tv, boost::bind(&std::vector<Token*>::push_back, boost::ref(tv2), _1));
	BOOST_CHECK_EQUAL(tag_string,
			tagset2->tag_to_string(tv2[0]->lexemes()[0].tag()));
	delete tv2[0];
}

BOOST_FIXTURE_TEST_CASE(tagpred, Fxz)
{
	Conversion::TagsetConverter tc;
	Conversion::TagRule tr(*tagset1);
	tr.add_precondition("P1");
	tr.add_precondition("a2");
	tr.add_postcondition("b2");
	Tag tag;
	tag = tagset1->parse_simple_tag("P1:a2:b1", false);
	tr.apply(tag);
	BOOST_CHECK_EQUAL(tagset1->tag_to_string(tag), "P1:a2:b2");

	tag = tagset1->parse_simple_tag("P2:a2:b1:c2", false);
	tr.apply(tag);
	BOOST_CHECK_EQUAL(tagset1->tag_to_string(tag), "P2:a2:b1:c2");

	tag = tagset1->parse_simple_tag("P1::b1", false);
	tr.apply(tag);
	BOOST_CHECK_EQUAL(tagset1->tag_to_string(tag), "P1::b1");

	tag = tagset1->parse_simple_tag("P1:a2", false);
	tr.apply(tag);
	BOOST_CHECK_EQUAL(tagset1->tag_to_string(tag), "P1:a2:b2");
}

BOOST_FIXTURE_TEST_CASE(tagpredlayer, Fxz)
{
	Conversion::TagsetConverter tc;
	Conversion::TagRule tr(*tagset1);
	tr.add_precondition("P1");
	tr.add_precondition("a2");
	tr.add_postcondition("a1");
	Conversion::TagRuleLayer* trl = new Conversion::TagRuleLayer(*tagset1);
	trl->append_rule(tr);
	tc.add_layer(trl);
	char tag_string[] = "P1:a2:b1";
	Tag t1 = tagset1->parse_simple_tag(tag_string, false);
	Token* tok = new Token(UnicodeString::fromUTF8("abc"), Toki::Whitespace::None);
	tok->add_lexeme(Lexeme(UnicodeString::fromUTF8("aaa"), t1));
	std::vector<Token*> tv, tv2;
	tv.push_back(tok);
	tc.convert_simple(tv, boost::bind(&std::vector<Token*>::push_back, boost::ref(tv2), _1));
	BOOST_CHECK_EQUAL("P1:a1:b1",
			tagset1->tag_to_string(tv2[0]->lexemes()[0].tag()));
	delete tv2[0];
}

BOOST_FIXTURE_TEST_CASE(join, Fxz)
{
	Conversion::TagsetConverter tc;
	Conversion::JoinRule jr(*tagset1);
	jr.set_token1_preconditions("P1", UnicodeString::fromUTF8("abcd"));
	jr.set_token2_preconditions("", UnicodeString::fromUTF8("efg"));
	jr.add_copy_attr("C");
	jr.add_postcondition("A");

	std::vector<Token*> tv, tv2;
	tv.push_back(new Token(UnicodeString::fromUTF8("abc"), Toki::Whitespace::None));
	tv.push_back(new Token(UnicodeString::fromUTF8("abcd"), Toki::Whitespace::None));
	tv.push_back(new Token(UnicodeString::fromUTF8("efg"), Toki::Whitespace::None));
	Tag tag1 = tagset1->parse_simple_tag("P1:a2:b1", false);
	Tag tag2 = tagset1->parse_simple_tag("P2:a2:b1:c2", false);
	std::cerr << tag2.raw_dump() << tagset1->tag_to_string(tag2);
	tv[0]->add_lexeme(Lexeme(UnicodeString::fromUTF8("lex1"), tag1));
	tv[1]->add_lexeme(Lexeme(UnicodeString::fromUTF8("lex2"), tag1));
	tv[2]->add_lexeme(Lexeme(UnicodeString::fromUTF8("lex3"), tag2));

	BOOST_CHECK(jr.try_join(tv[0], tv[1]) == NULL);
	delete tv[0];
	Token* t = jr.try_join(tv[1], tv[2]);
	BOOST_REQUIRE(t != NULL);
	BOOST_CHECK_EQUAL(t->orth_utf8(), "abcdefg");
	BOOST_CHECK_EQUAL(t->lexemes().size(), 1);
	BOOST_CHECK_EQUAL(t->lexemes()[0].lemma_utf8(), "lex2");
	BOOST_CHECK_EQUAL(tagset1->tag_to_string(t->lexemes()[0].tag()), "P1::b1:c2");
	std::cerr << t->lexemes()[0].tag().raw_dump() << tagset1->tag_to_string(t->lexemes()[0].tag());
}

BOOST_AUTO_TEST_SUITE_END()

