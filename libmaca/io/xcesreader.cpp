#include <libmaca/io/xcesreader.h>
#include <libtoki/util/xmlutil.h>
#include <libtoki/util/foreach.h>
#include <libxml++/libxml++.h>

namespace Maca {

	XcesReaderError::XcesReaderError(const std::string &what)
		: MacaError(what)
	{
	}

	XcesReaderError::~XcesReaderError() throw()
	{
	}

	class XcesReaderImpl : public xmlpp::SaxParser
	{
	public:
		XcesReaderImpl(const Tagset& tagset, bool disamb_only);

		~XcesReaderImpl();

		Chunk* try_get_next();

	protected:
		void on_start_element(const Glib::ustring & name,
				const AttributeList& attributes);
		void on_end_element(const Glib::ustring & name);
		void on_cdata_block(const Glib::ustring & text);
		void on_characters(const Glib::ustring & text);

		const Tagset& tagset_;

		enum state_t { XS_NONE, XS_CHUNK, XS_SENTENCE, XS_TOK, XS_ORTH, XS_LEX, XS_LEMMA, XS_TAG };
		state_t state_;

		Toki::Whitespace::Enum wa_;

		std::string sbuf_;

		Token* tok_;

		Sentence* sent_;

		Chunk* chunk_;

		std::deque<Chunk*> obuf_;

		bool disamb_only_;
	};

	XcesReader::XcesReader(const Tagset& tagset, std::istream& is, bool disamb_only)
		: is_(is), impl_(new XcesReaderImpl(tagset, disamb_only))
	{
	}

	XcesReader::~XcesReader()
	{
	}

	Chunk* XcesReader::get_next_chunk()
	{
		static const int BUFSIZE=10240;
		Chunk* t = impl_->try_get_next();
		while (t == NULL && is_.good()) {
			unsigned char buf[BUFSIZE+1];
			is_.read(reinterpret_cast<char*>(buf), BUFSIZE);
			impl_->parse_chunk_raw(buf, is_.gcount());
			if (is_.eof()) {
				impl_->finish_chunk_parsing();
			}
			t = impl_->try_get_next();
		}
		return t;
	}

	XcesReaderImpl::XcesReaderImpl(const Tagset& tagset, bool disamb_only)
		: xmlpp::SaxParser()
		, tagset_(tagset), state_(XS_NONE), wa_(Toki::Whitespace::Newline)
		, sbuf_(), tok_(NULL), sent_(NULL), chunk_(NULL), obuf_()
		, disamb_only_(disamb_only)
	{
	}

	XcesReaderImpl::~XcesReaderImpl()
	{
		delete tok_;
		delete sent_;
		delete chunk_;
		foreach (Chunk* c, obuf_) {
			delete c;
		}
	}

	Chunk* XcesReaderImpl::try_get_next()
	{
		if (obuf_.empty()) {
			return NULL;
		} else {
			Chunk* t = obuf_.front();
			obuf_.pop_front();
			return t;
		}
	}

	void XcesReaderImpl::on_start_element(const Glib::ustring &name, const AttributeList& attributes)
	{
		if (name == "chunk") {
			std::string type;
			foreach (const Attribute& a, attributes) {
				if (a.name == "type") {
					type = a.value;
				}
			}
			if (state_ == XS_NONE) {
				if (type == "s") {
					throw XcesReaderError("Top level <chunk> is type=\"s\"");
				}
				state_ = XS_CHUNK;
				chunk_ = new Chunk;
				foreach (const Attribute& a, attributes) {
					chunk_->set_attribute(a.name, a.value);
				}
			} else if (state_ == XS_CHUNK) {
				if (type != "s") {
					throw XcesReaderError("Sub level <chunk> not type=\"s\"");
				}
				state_ = XS_SENTENCE;
				sent_ = new Sentence;
			} else {
				throw XcesReaderError("Unexpected <chunk>");
			}
		} else if (state_ == XS_SENTENCE && name == "tok") {
			state_ = XS_TOK;
			tok_ = new Token();
			tok_->set_wa(wa_);
			wa_ = Toki::Whitespace::Space;
		} else if (state_ == XS_TOK && name == "orth") {
			state_ = XS_ORTH;
			sbuf_ = "";
		} else if (state_ == XS_TOK && name == "lex") {
			assert(tok_ != NULL);
			bool okay = true;
			if (disamb_only_) {
				okay = false;
				foreach (const Attribute& a, attributes) {
					if (a.name == "disamb" && a.value == "1") {
						okay = true;
					}
				}
			}
			if (okay) {
				tok_->add_lexeme(Lexeme());
				state_ = XS_LEX;
			}
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

	void XcesReaderImpl::on_end_element(const Glib::ustring &name)
	{
		if (state_ == XS_ORTH && name == "orth") {
			tok_->set_orth(UnicodeString::fromUTF8(sbuf_));
			state_ = XS_TOK;
		} else if (state_ == XS_LEMMA && name == "base") {
			tok_->lexemes().back().set_lemma(UnicodeString::fromUTF8(sbuf_));
			state_ = XS_LEX;
		} else if (state_ == XS_TAG && name == "ctag") {
			Tag tag = tagset_.parse_simple_tag(sbuf_, true);
			tok_->lexemes().back().set_tag(tag);
			state_ = XS_LEX;
		} else if (state_ == XS_LEX && name == "lex") {
			state_ = XS_TOK;
		} else if (state_ == XS_TOK && name == "tok") {
			sent_->append(tok_);
			tok_ = NULL;
			state_ = XS_SENTENCE;
		} else if (state_ == XS_SENTENCE && name == "chunk") {
			chunk_->append(sent_);
			sent_ = NULL;
			state_ = XS_CHUNK;
		} else if (state_ == XS_CHUNK && name == "chunk") {
			obuf_.push_back(chunk_);
			chunk_ = NULL;
			state_ = XS_NONE;
		}
	}

	void XcesReaderImpl::on_cdata_block(const Glib::ustring &/*text*/)
	{
	}

	void XcesReaderImpl::on_characters(const Glib::ustring &text)
	{
		sbuf_ += (std::string)text;
	}



} /* end ns Maca */
