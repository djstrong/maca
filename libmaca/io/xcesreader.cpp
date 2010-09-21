#include <libmaca/io/xcesreader.h>
#include <libmaca/io/sax.h>
#include <libtoki/util/xmlutil.h>
#include <libtoki/util/foreach.h>
#include <libxml++/libxml++.h>

namespace Maca {

	class XcesReaderImpl : public BasicSaxParser
	{
	public:
		XcesReaderImpl(const Tagset& tagset, std::deque<Chunk*>& obuf,
				bool disamb_only);

		~XcesReaderImpl();

	protected:
		void on_start_element(const Glib::ustring & name,
				const AttributeList& attributes);
		void on_end_element(const Glib::ustring & name);

		const Tagset& tagset_;

		enum state_t { XS_NONE, XS_CHUNK, XS_SENTENCE, XS_TOK, XS_ORTH, XS_LEX,
				XS_LEMMA, XS_TAG };
		state_t state_;

		Toki::Whitespace::Enum wa_;

		Glib::ustring sbuf_;

		Token* tok_;

		Sentence* sent_;

		Chunk* chunk_;

		std::deque<Chunk*>& obuf_;

		bool disamb_only_;
	};

	XcesReader::XcesReader(const Tagset& tagset, std::istream& is,
			bool disamb_only)
		: BufferedChunkReader(tagset), is_(is)
		, impl_(new XcesReaderImpl(tagset, chunk_buf_, disamb_only))
	{
	}

	XcesReader::~XcesReader()
	{
	}

	void XcesReader::ensure_more()
	{
		static const int BUFSIZE=1024;
		while (chunk_buf_.empty() && is().good()) {
			unsigned char buf[BUFSIZE+1];
			is().read(reinterpret_cast<char*>(buf), BUFSIZE);
			impl_->parse_chunk_raw(buf, is().gcount());
			if (is().eof()) {
				impl_->finish_chunk_parsing();
			}
		}
	}

	XcesReaderImpl::XcesReaderImpl(const Tagset& tagset,
			std::deque<Chunk*>& obuf, bool disamb_only)
		: BasicSaxParser()
		, tagset_(tagset), state_(XS_NONE), wa_(Toki::Whitespace::Newline)
		, sbuf_(), tok_(NULL), sent_(NULL), chunk_(NULL), obuf_(obuf)
		, disamb_only_(disamb_only)
	{
	}

	XcesReaderImpl::~XcesReaderImpl()
	{
		delete tok_;
		delete sent_;
		delete chunk_;
	}

	void XcesReaderImpl::on_start_element(const Glib::ustring &name,
			const AttributeList& attributes)
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
					throw XcesError("Top level <chunk> is type=\"s\"");
				}
				state_ = XS_CHUNK;
				chunk_ = new Chunk;
				foreach (const Attribute& a, attributes) {
					chunk_->set_attribute(a.name, a.value);
				}
			} else if (state_ == XS_CHUNK) {
				if (type != "s") {
					throw XcesError("Sub level <chunk> not type=\"s\"");
				}
				state_ = XS_SENTENCE;
				sent_ = new Sentence;
			} else {
				throw XcesError("Unexpected <chunk>");
			}
		} else if (state_ == XS_SENTENCE && name == "tok") {
			state_ = XS_TOK;
			tok_ = new Token();
			tok_->set_wa(wa_);
			wa_ = Toki::Whitespace::Space;
		} else if (state_ == XS_TOK && name == "orth") {
			state_ = XS_ORTH;
			grab_characters_ = true;
			clear_buf();
		} else if (state_ == XS_TOK && name == "lex") {
			assert(tok_ != NULL);
			bool is_disamb = false;
			foreach (const Attribute& a, attributes) {
				if (a.name == "disamb" && a.value == "1") {
					is_disamb = true;
				}
			}
			if (!disamb_only_ || is_disamb) {
				tok_->add_lexeme(Lexeme());
				tok_->lexemes().back().set_disamb(is_disamb);
				state_ = XS_LEX;
			}
		} else if (state_ == XS_LEX && name == "base") {
			state_ = XS_LEMMA;
			grab_characters_ = true;
			clear_buf();
		} else if (state_ == XS_LEX && name == "ctag") {
			state_ = XS_TAG;
			grab_characters_ = true;
			clear_buf();
		} else if (name == "ns") {
			wa_ = Toki::Whitespace::None;
		}
	}

	void XcesReaderImpl::on_end_element(const Glib::ustring &name)
	{
		if (state_ == XS_ORTH && name == "orth") {
			tok_->set_orth(UnicodeString::fromUTF8(get_buf()));
			grab_characters_ = false;
			state_ = XS_TOK;
		} else if (state_ == XS_LEMMA && name == "base") {
			tok_->lexemes().back().set_lemma(UnicodeString::fromUTF8(get_buf()));
			grab_characters_ = false;
			state_ = XS_LEX;
		} else if (state_ == XS_TAG && name == "ctag") {
			Tag tag = tagset_.parse_simple_tag(get_buf(), true);
			tok_->lexemes().back().set_tag(tag);
			grab_characters_ = false;
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

} /* end ns Maca */
