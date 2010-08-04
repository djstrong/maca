#include <libpltagger/io/premorph.h>
#include <libtoki/foreach.h>
#include <libxml++/parsers/saxparser.h>
#include <libtoki/sentencesplitter.h>
#include <vector>

namespace PlTagger {

	class PremorphProcessorImpl : public xmlpp::SaxParser
	{
	public:
		PremorphProcessorImpl(std::ostream& os, Toki::Tokenizer& tok, PlTagger::MorphAnalyser& ma);

	protected:
		void on_start_document();
		void on_start_element(const Glib::ustring & name,
				const AttributeList& attributes);
		void on_end_element(const Glib::ustring & name);
		void on_cdata_block(const Glib::ustring & text);
		void on_characters(const Glib::ustring & text);
	private:
		std::ostream& os_;
		std::stringstream buf_;
		Toki::Tokenizer& tok_;
		PlTagger::MorphAnalyser& ma_;
	};

	PremorphProcessor::PremorphProcessor(std::ostream &os, Toki::Tokenizer& tok, PlTagger::MorphAnalyser& ma)
		: impl_(new PremorphProcessorImpl(os, tok, ma))
	{
	}

	PremorphProcessor::~PremorphProcessor()
	{
	}

	void PremorphProcessor::parse_stream(std::istream &is)
	{
		impl_->parse_stream(is);
	}

	PremorphProcessorImpl::PremorphProcessorImpl(std::ostream& os, Toki::Tokenizer& tok, PlTagger::MorphAnalyser& ma)
		: xmlpp::SaxParser(), os_(os), tok_(tok), ma_(ma)
	{
	}

	void PremorphProcessorImpl::on_start_document()
	{
		os_ << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
		os_ << "<!DOCTYPE cesAna SYSTEM \"xcesAnaIPI.dtd\">\n";
	}

	void PremorphProcessorImpl::on_start_element(const Glib::ustring &name, const AttributeList &attributes)
	{
		os_ << "<" << name;
		foreach (const xmlpp::SaxParser::Attribute& a, attributes) {
			os_ << " " << a.name << "=\"" << a.value << "\"";
		}
		os_ << ">";
		os_ << "\n";
	}

	void PremorphProcessorImpl::on_end_element(const Glib::ustring &name)
	{
		UnicodeString s = UnicodeString::fromUTF8(buf_.str());
		s.trim();
		if (s.length() > 0) {
			tok_.reset();
			tok_.set_input_source(s);
			Toki::SentenceSplitter sen(tok_);
			while (sen.has_more()) {
				os_ << " <chunk type=\"s\">\n";
				std::vector<Toki::Token*> sentence = sen.get_next_sentence();
				foreach (Token* t, ma_.process_dispose(sentence)) {
					if (t->wa() == Toki::Whitespace::None) {
						os_ << "  <ns/>\n";
					}
					os_ << "  <tok>\n";
					os_ << "   <orth>" << t->orth_utf8() << "</orth>\n";
					foreach (const Lexeme& lex, t->lexemes()) {
						os_ << "   <lex>"
							<< "<base>" << lex.lemma_utf8() << "</base>"
							<< "<ctag>" << ma_.tagset().tag_to_string(lex.tag()) << "</ctag>"
							<< "</lex>\n";
					}
					os_ << "  </tok>\n";
					delete t;
				}
				os_ << " </chunk>\n";
			}
			buf_.str("");
		}
		os_ << "</" << name << ">";
		os_ << "\n";
	}

	void PremorphProcessorImpl::on_characters(const Glib::ustring &text)
	{
		buf_ << static_cast<std::string>(text);
	}

	void PremorphProcessorImpl::on_cdata_block(const Glib::ustring &text)
	{

	}

} /* end ns PlTagger */
