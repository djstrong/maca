#include <boost/test/unit_test.hpp>

#include <libpltagger/token.h>

const char tagsetstr1[] = "[ATTR]\n"
	"A tag tog other a3 \n"
	"B data thing tag-thing thang\n"
	"C a b c \n"
	"[POS]\n some A B [C]\n";

BOOST_AUTO_TEST_CASE( token )
{
	PlTagger::Token t(UnicodeString::fromUTF8("ZZ"), Toki::Whitespace::ManySpaces);
	BOOST_CHECK_EQUAL(t.orth_utf8(), "ZZ");
	BOOST_CHECK_EQUAL(t.wa(), Toki::Whitespace::ManySpaces);
	BOOST_CHECK(t.lexemes().empty());
}

BOOST_AUTO_TEST_CASE( token_dup_lexemes )
{
	PlTagger::Token t(UnicodeString::fromUTF8("ZZ"), Toki::Whitespace::ManySpaces);
	//PlTagger::Tagset tagset(tagsetstr1);
	PlTagger::Tag t1(PlTagger::tagset_idx_t(0), PlTagger::pos_idx_t(0));
	PlTagger::Lexeme l1(UnicodeString::fromUTF8("aaa"), t1);
	PlTagger::Lexeme l2(UnicodeString::fromUTF8("bbb"), t1);
	BOOST_CHECK(!t.check_duplicate_lexemes());
	BOOST_CHECK(!t.remove_duplicate_lexemes());
	t.add_lexeme(l1);
	BOOST_CHECK(!t.check_duplicate_lexemes());
	BOOST_CHECK(!t.remove_duplicate_lexemes());
	t.add_lexeme(l2);
	BOOST_CHECK(!t.check_duplicate_lexemes());
	BOOST_CHECK(!t.remove_duplicate_lexemes());
	PlTagger::Token tt(t);
	t.add_lexeme(l1);
	BOOST_CHECK(t != tt);
	BOOST_CHECK(t.check_duplicate_lexemes());
	BOOST_CHECK(t.remove_duplicate_lexemes());
	BOOST_CHECK(!t.check_duplicate_lexemes());
	BOOST_CHECK(!t.remove_duplicate_lexemes());
	BOOST_CHECK(t == tt);
}
