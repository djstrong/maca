#include <libmaca/io/writer.h>
#include <libmaca/io/init.h>
#include <libtoki/util/foreach.h>
#include <boost/algorithm/string.hpp>

#include <sstream>

namespace Maca {

	TokenWriter::TokenWriter(std::ostream& os, const Tagset& tagset,
			const string_range_vector& /*params*/)
		: os_(os), tagset_(tagset), needs_footer_(true), indent_(0)
	{
	}

	TokenWriter::~TokenWriter()
	{
	}

	void TokenWriter::finish()
	{
		if (needs_footer_) {
			do_footer();
			needs_footer_ = false;
		}
	}

	std::ostream& TokenWriter::osi()
	{
		for (int i = 0; i < indent_; ++i) {
			os_ << " ";
		}
		return os_;
	}

	void TokenWriter::indent_more(int n)
	{
		indent_ += n;
	}

	void TokenWriter::indent_less(int n)
	{
		indent_ -= n;
	}

	void TokenWriter::write_token_dispose(Token* t)
	{
		write_token(*t);
		delete t;
	}


	TokenWriter* TokenWriter::create(const std::string class_id,
			std::ostream& os,
			const Tagset& tagset,
			const string_range_vector& params)
	{
		return TokenWriterFactorySingleton::Instance().factory.CreateObject(
				class_id, os, tagset, params);
	}

	TokenWriter* TokenWriter::create(const std::string class_id_params,
			std::ostream& os,
			const Tagset& tagset)
	{
		string_range_vector params;
		boost::algorithm::split(params, class_id_params,
								boost::is_any_of(std::string(",")));
		std::string class_id = boost::copy_range<std::string>(params[0]);
		params.erase(params.begin(), params.begin() + 1);
		return TokenWriterFactorySingleton::Instance().factory.CreateObject(
				class_id, os, tagset, params);
	}

	std::vector<std::string> TokenWriter::available_writer_types()
	{
		return TokenWriterFactorySingleton::Instance().factory.RegisteredIds();
	}

	std::vector<std::string> TokenWriter::available_writer_types_help()
	{
		std::vector<std::string> v = available_writer_types();
		foreach (std::string& id, v) {
			std::stringstream ss;
			std::map<std::string, std::string>::const_iterator c;
			c = TokenWriterFactorySingleton::Instance().help.find(id);
			if (c != TokenWriterFactorySingleton::Instance().help.end()) {
				ss << id << "[";
				ss << c->second;
				ss << "]";
			}
			id = ss.str();
		}
		return v;
	}

	static bool registered = init_token_writers();

} /* end ns Maca */
