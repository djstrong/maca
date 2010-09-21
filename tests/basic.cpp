#include <boost/test/unit_test.hpp>

#include <libcorpus2/token.h>

const char tagsetstr1[] = "[ATTR]\n"
	"A tag tog other a3 \n"
	"B data thing tag-thing thang\n"
	"C a b c \n"
	"[POS]\n some A B [C]\n";

BOOST_AUTO_TEST_CASE( token )
{
	Corpus2::Token t(UnicodeString::fromUTF8("ZZ"), PwrNlp::Whitespace::ManySpaces);
	BOOST_CHECK_EQUAL(t.orth_utf8(), "ZZ");
	BOOST_CHECK_EQUAL(t.wa(), PwrNlp::Whitespace::ManySpaces);
	BOOST_CHECK(t.lexemes().empty());
}

BOOST_AUTO_TEST_CASE( token_dup_lexemes )
{
	Corpus2::Token t(UnicodeString::fromUTF8("ZZ"), PwrNlp::Whitespace::ManySpaces);
	//Corpus2::Tagset tagset(tagsetstr1);
	Corpus2::Tag t1(Corpus2::tagset_idx_t(0), Corpus2::pos_idx_t(0));
	Corpus2::Lexeme l1(UnicodeString::fromUTF8("aaa"), t1);
	Corpus2::Lexeme l2(UnicodeString::fromUTF8("bbb"), t1);
	BOOST_CHECK(!t.check_duplicate_lexemes());
	BOOST_CHECK(!t.remove_duplicate_lexemes());
	t.add_lexeme(l1);
	BOOST_CHECK(!t.check_duplicate_lexemes());
	BOOST_CHECK(!t.remove_duplicate_lexemes());
	t.add_lexeme(l2);
	BOOST_CHECK(!t.check_duplicate_lexemes());
	BOOST_CHECK(!t.remove_duplicate_lexemes());
	Corpus2::Token tt(t);
	t.add_lexeme(l1);
	BOOST_CHECK(t != tt);
	BOOST_CHECK(t.check_duplicate_lexemes());
	BOOST_CHECK(t.remove_duplicate_lexemes());
	BOOST_CHECK(!t.check_duplicate_lexemes());
	BOOST_CHECK(!t.remove_duplicate_lexemes());
	BOOST_CHECK(t == tt);
}
