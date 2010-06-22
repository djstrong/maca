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
	std::cerr << "XXXXXX";
	tc.convert_simple(tv, boost::bind(&std::vector<Token*>::push_back, boost::ref(tv2), _1));
	BOOST_CHECK_EQUAL(tagset2->tag_to_string(tv[0]->lexemes()[0].tag()),
			tagset1->tag_to_string(tv2[0]->lexemes()[0].tag()));
}

BOOST_AUTO_TEST_SUITE_END()

