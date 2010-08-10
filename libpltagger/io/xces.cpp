#include <libpltagger/io/xces.h>
#include <libpltagger/io/reader.h>
#include <libtoki/util/foreach.h>
#include <libxml++/libxml++.h>
#include <libxml++/nodes/node.h>
#include <libxml++/nodes/element.h>
#include <libxml++/parsers/saxparser.h>

#include <iostream>

namespace PlTagger {

	class XcesTokenReaderImpl : public xmlpp::SaxParser
	{
	public:
		XcesTokenReaderImpl(const Tagset& tagset);

		Token* try_get_next();

	protected:
		void on_start_element(const Glib::ustring & name,
				const AttributeList& attributes);
		void on_end_element(const Glib::ustring & name);
		void on_cdata_block(const Glib::ustring & text);
		void on_characters(const Glib::ustring & text);

		const Tagset& tagset_;

		enum state_t { XS_NONE, XS_TOK, XS_ORTH, XS_LEX, XS_LEMMA, XS_TAG };
		state_t state_;

		Toki::Whitespace::Enum wa_;

		Token* tok_;

		std::string sbuf_;

		std::deque<Token*> buf_;
	};

	XcesTokenReader::XcesTokenReader(const Tagset& tagset, std::istream& is)
		: is_(is), impl_(new XcesTokenReaderImpl(tagset))
	{
	}

	XcesTokenReader::~XcesTokenReader()
	{
	}

	Token* XcesTokenReader::get_next_token()
	{
		Token* t = impl_->try_get_next();
		if (t != NULL) {
			return t;
		}
		if (!is_.good()) {
			return NULL;
		}
		static const int BUFSIZE=10240;
		unsigned char buf[BUFSIZE+1];
		is_.read(reinterpret_cast<char*>(buf), BUFSIZE);
		impl_->parse_chunk_raw(buf, is_.gcount());
		if (is_.eof()) {
			impl_->finish_chunk_parsing();
		}
		return impl_->try_get_next();
	}

	XcesTokenReaderImpl::XcesTokenReaderImpl(const Tagset& tagset)
		: xmlpp::SaxParser()
		, tagset_(tagset), state_(XS_NONE), wa_(Toki::Whitespace::Newline)
		,tok_(NULL), sbuf_(), buf_()
	{
	}

	Token* XcesTokenReaderImpl::try_get_next()
	{
		if (buf_.empty()) {
			return NULL;
		} else {
			Token* t = buf_.front();
			buf_.pop_front();
			return t;
		}
	}

	void XcesTokenReaderImpl::on_start_element(const Glib::ustring &name, const AttributeList& /*attributes*/)
	{
		if (name == "tok") {
			state_ = XS_TOK;
			tok_ = new Token();
			tok_->set_wa(wa_);
			wa_ = Toki::Whitespace::Space;
		} else if (state_ == XS_TOK && name == "orth") {
			state_ = XS_ORTH;
			sbuf_ = "";
		} else if (state_ == XS_TOK && name == "lex") {
			assert(tok_ != NULL);
			tok_->add_lexeme(Lexeme());
			state_ = XS_LEX;
		} else if (state_ == XS_LEX && name == "base") {
			state_ = XS_LEMMA;
			sbuf_ = "";
		} else if (state_ == XS_LEX && name == "ctag") {
			state_ = XS_TAG;
			sbuf_ = "";
		} else if (name == "ns") {
			wa_ = Toki::Whitespace::None;
		}
	}

	void XcesTokenReaderImpl::on_end_element(const Glib::ustring &name)
	{
		if (state_ == XS_ORTH && name == "orth") {
			tok_->set_orth(UnicodeString::fromUTF8(sbuf_));
			state_ = XS_TOK;
		} else if (state_ == XS_LEMMA && name == "base") {
			tok_->lexemes().back().set_lemma(UnicodeString::fromUTF8(sbuf_));
			state_ = XS_LEX;
		} else if (state_ == XS_TAG && name == "ctag") {
			Tag tag = tagset_.parse_simple_tag(sbuf_, true);
			tok_->lexemes().back().tag() = tag;
			state_ = XS_LEX;
		} else if (state_ == XS_LEX && name == "lex") {
			state_ = XS_TOK;
		} else if (state_ == XS_TOK && name == "tok") {
			buf_.push_back(tok_);
			tok_ = NULL;
			state_ = XS_NONE;
		}
	}

	void XcesTokenReaderImpl::on_cdata_block(const Glib::ustring &/*text*/)
	{
	}

	void XcesTokenReaderImpl::on_characters(const Glib::ustring &text)
	{
		sbuf_ += (std::string)text;
	}



} /* end ns PlTagger */
