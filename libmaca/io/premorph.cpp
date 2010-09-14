#include <libmaca/io/premorph.h>
#include <libmaca/io/sax.h>
#include <libmaca/io/xces.h>
#include <libmaca/io/xcescommon.h>
#include <libmaca/util/sentenceanalyser.h>
#include <libmaca/util/tokentimer.h>
#include <libtoki/util/foreach.h>
#include <libtoki/sentencesplitter.h>
#include <vector>
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

namespace Maca {

	class PremorphProcessorImpl : public BasicSaxParser
	{
	public:
		PremorphProcessorImpl(std::ostream& os,
				const boost::shared_ptr<SentenceAnalyser>& sp);

		void set_stats(bool v) {
			stats_ = v;
		}

	protected:
		void on_start_document();
		void on_end_document();
		void on_start_element(const Glib::ustring & name,
				const AttributeList& attributes);
		void on_end_element(const Glib::ustring & name);

		void output_sentence(Sentence* s);

	private:
		std::ostream& os_;
		boost::shared_ptr<SentenceAnalyser> sa_;
		bool stats_;
		TokenTimer timer_;
	};

	PremorphProcessor::PremorphProcessor(std::ostream &os,
			const boost::shared_ptr<Toki::Tokenizer>& tok,
			const boost::shared_ptr<Maca::MorphAnalyser>& ma)
	: impl_(new PremorphProcessorImpl(os,
			boost::make_shared<SentenceAnalyser>(tok, ma)))
	{
	}

	PremorphProcessor::PremorphProcessor(std::ostream &os,
			const boost::shared_ptr<SentenceAnalyser>& sa)
	: impl_(new PremorphProcessorImpl(os, sa))
	{
	}

	PremorphProcessor::~PremorphProcessor()
	{
	}

	void PremorphProcessor::parse_stream(std::istream& is)
	{
		impl_->parse_stream(is);
	}

	void PremorphProcessor::set_stats(bool v)
	{
		impl_->set_stats(v);
	}

	PremorphProcessorImpl::PremorphProcessorImpl(std::ostream& os,
			const boost::shared_ptr<SentenceAnalyser>& sa)
		: BasicSaxParser(), os_(os), sa_(sa), stats_(false), timer_()
	{
	}

	void PremorphProcessorImpl::on_start_document()
	{
		timer_.restart();
		os_ << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
		os_ << "<!DOCTYPE cesAna SYSTEM \"xcesAnaIPI.dtd\">\n";
	}

	void PremorphProcessorImpl::on_end_document()
	{
		if (stats_) {
			timer_.stats();
		}
	}

	void PremorphProcessorImpl::on_start_element(const Glib::ustring &name,
			const AttributeList &attributes)
	{
		os_ << "<" << name;
		foreach (const xmlpp::SaxParser::Attribute& a, attributes) {
			os_ << " " << a.name << "=\"";
			encode_xml_entities_into(os_, a.value);
			os_ << "\"";
		}
		os_ << ">";
		os_ << "\n";
	}

	void PremorphProcessorImpl::on_end_element(const Glib::ustring &name)
	{
		sa_->set_input_source(buf_);
		sa_->process(boost::bind(
				&PremorphProcessorImpl::output_sentence, this, _1));
		buf_.str("");
		buf_.clear();
		os_ << "</" << name << ">" << "\n";
	}

	void PremorphProcessorImpl::output_sentence(Sentence* s)
	{
		boost::scoped_ptr<Sentence> deleter(s);
		if (!s->empty()) {
			os_ << " <chunk type=\"s\">\n";
			foreach (Token* t, s->tokens()) {
				token_as_xces_xml(os_, sa_->tagset(), *t, 1);
			}
			os_ << " </chunk>\n";
			timer_.count_sentence(*s);
			if (stats_) {
				timer_.check_slice();
			}
		}
	}

	class PremorphReaderImpl : public BasicSaxParser
	{
	public:
		PremorphReaderImpl(const boost::shared_ptr<SentenceAnalyser>& sa,
				std::deque<Chunk*>& chunks);
	protected:
		void on_start_element(const Glib::ustring & name,
				const AttributeList& attributes);
		void on_end_element(const Glib::ustring & name);
	private:
		enum state_t { XS_NONE, XS_CHUNK, XS_SENTENCE, XS_TOK, XS_ORTH, XS_LEX,
				XS_LEMMA, XS_TAG };
		state_t state_;

		Chunk* chunk_;
		boost::shared_ptr<SentenceAnalyser> sa_;
		std::deque<Chunk*>& chunks_;
	};

	PremorphReader::PremorphReader(std::istream& is,
			const boost::shared_ptr<Toki::Tokenizer>& tok,
			const boost::shared_ptr<Maca::MorphAnalyser>& ma)
		: BufferedTokenReader(ma->tagset()), is_(is)
		, impl_(new PremorphReaderImpl(
				boost::make_shared<SentenceAnalyser>(tok, ma), chunk_buf_))
	{
	}

	PremorphReader::PremorphReader(std::istream& is,
			const boost::shared_ptr<SentenceAnalyser>& sa)
		: BufferedTokenReader(sa->tagset()), is_(is)
		, impl_(new PremorphReaderImpl(sa, chunk_buf_))
	{
	}

	PremorphReader::~PremorphReader()
	{
	}

	void PremorphReader::ensure_more()
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

	PremorphReaderImpl::PremorphReaderImpl(
			const boost::shared_ptr<SentenceAnalyser>& sa,
			std::deque<Chunk*> &chunks)
		: BasicSaxParser(), state_(XS_NONE), chunk_(NULL), sa_(sa)
		, chunks_(chunks)
	{
	}

	void PremorphReaderImpl::on_start_element(const Glib::ustring &name,
			const AttributeList &attributes)
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
				throw XcesError("More than one level of <chunk>s in premorph");
			} else {
				throw XcesError("Unexpected <chunk>");
			}
		}
	}

	void PremorphReaderImpl::on_end_element(const Glib::ustring &name)
	{
		if (state_ == XS_CHUNK && name == "chunk") {
			assert(chunk_);
			sa_->set_input_source(buf_);
			sa_->process(boost::bind(&Chunk::append, chunk_, _1));
			buf_.str("");
			buf_.clear();
			chunks_.push_back(chunk_);
			chunk_ = NULL;
			state_ = XS_NONE;
		}
	}

} /* end ns Maca */
