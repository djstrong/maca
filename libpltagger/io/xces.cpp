#include <libpltagger/io/xces.h>
#include <libpltagger/io/reader.h>
#include <libtoki/foreach.h>
#include <libxml++/libxml++.h>
#include <libxml++/nodes/node.h>
#include <libxml++/nodes/element.h>
#include <libxml++/parsers/saxparser.h>

#include <iostream>

namespace PlTagger {

	class XcesNoMorphReaderImpl : public xmlpp::SaxParser
	{
	public:
		XcesNoMorphReaderImpl();

	protected:
		void on_start_element(const Glib::ustring & name,
				const AttributeList& attributes);
		void on_end_element(const Glib::ustring & name);
		void on_cdata_block(const Glib::ustring & text);
		void on_characters(const Glib::ustring & text);

		bool had_ns_;
		bool in_tok_;
		bool in_orth_;
		std::string orth_;
		std::deque<Toki::Token*> data_;
	};

	XcesNoMorphReader::XcesNoMorphReader()
		: impl_(new XcesNoMorphReaderImpl)
	{
	}

	XcesNoMorphReader::~XcesNoMorphReader()
	{
	}

	void XcesNoMorphReader::parse_file(const std::string& filename)
	{
		impl_->parse_file(filename);
	}

	void XcesNoMorphReader::parse_stream(std::istream& is)
	{
		impl_->parse_stream(is);
	}

	XcesNoMorphReaderImpl::XcesNoMorphReaderImpl()
		: xmlpp::SaxParser()
		, had_ns_(false), in_tok_(false), in_orth_(false), orth_()
		, data_()
	{
	}

	void XcesNoMorphReaderImpl::on_start_element(const Glib::ustring &name, const AttributeList &attributes)
	{
		if (name == "tok") {
			in_tok_ = true;
			orth_ = "";
		} else if (in_tok_ && name == "orth") {
			in_orth_ = true;
		} else if (name == "ns") {
			had_ns_ = true;
		}
	}

	void XcesNoMorphReaderImpl::on_end_element(const Glib::ustring &name)
	{
		if (name == "tok") {
			if (!orth_.empty()) {
				Toki::Whitespace::Enum wa;
				if (had_ns_) {
					wa = Toki::Whitespace::None;
				} else {
					wa = Toki::Whitespace::Space;
				}
				Toki::Token* t = new Toki::Token(
						UnicodeString::fromUTF8(orth_), "", wa);
				data_.push_back(t);
			}
			in_tok_ = false;
			in_orth_ = false;
		} else if (name == "orth") {
			in_orth_ = false;
		}
	}

	void XcesNoMorphReaderImpl::on_cdata_block(const Glib::ustring &text)
	{
	}

	void XcesNoMorphReaderImpl::on_characters(const Glib::ustring &text)
	{
		if (in_orth_) {
			orth_ += (std::string)text;
		}
	}



} /* end ns PlTagger */
