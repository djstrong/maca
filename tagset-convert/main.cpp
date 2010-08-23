#include <libpltagger/conv/tagsetconverter.h>
#include <libpltagger/io/xcesvalidate.h>
#include <libpltagger/io/xcesreader.h>
#include <libpltagger/io/xceswriter.h>
#include <libpltagger/util/settings.h>
#include <libpltagger/tagsetmanager.h>

#include <libtoki/util/foreach.h>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>

#include <fstream>

int main(int argc, char** argv)
{
	std::string converter, verify_tagset, force_tagset;
	std::string input_format, output_format;
	bool quiet = false;
	bool disamb = false;
	using boost::program_options::value;

	std::string writers = boost::algorithm::join(PlTagger::TokenWriter::available_writer_types(), " ");
	std::string writers_help = "Output format, any of: " + writers + "\n";

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
			("converter,c", value(&converter),
			 "Tagset converter configuration\n")
			("disamb-only,d", value(&disamb)->zero_tokens(),
			 "Only read lexemes marked as disambiguated\n")
			("verify,v", value(&verify_tagset),
			 "Verify tags within a tagset\n")
			("tagset,t", value(&force_tagset),
			 "Tagset override\n")
			("input-format,i", value(&input_format)->default_value("xces"),
			 "Input format\n")
			("output-format,o", value(&output_format)->default_value("xces"),
			 writers_help.c_str())
			("quiet,q", value(&quiet)->zero_tokens(),
			 "Suppress startup info \n")
			("help,h", "Show help")
			;
	boost::program_options::variables_map vm;
	boost::program_options::positional_options_description p;
	p.add("converter", -1);

	try {
		boost::program_options::store(
			boost::program_options::command_line_parser(argc, argv)
			.options(desc).positional(p).run(), vm);
	} catch (boost::program_options::error& e) {
		std::cerr << e.what() << "\n";
		return 2;
	}
	boost::program_options::notify(vm);

	if (vm.count("help")) {
		std::cout << desc << "\n";
		return 1;
	}
	PlTagger::Path::Instance().set_verbose(!quiet);

	if (!verify_tagset.empty()) {
		const PlTagger::Tagset& tagset = PlTagger::get_named_tagset(verify_tagset);
		PlTagger::XcesValidator xv(tagset, std::cout);
		xv.validate_stream(std::cin);
	} else if (converter == "nop") {
		try {
			const PlTagger::Tagset& tagset = PlTagger::get_named_tagset(force_tagset);
			PlTagger::XcesReader reader(tagset, std::cin, disamb);
			boost::scoped_ptr<PlTagger::TokenWriter> writer;
			writer.reset(PlTagger::TokenWriter::create(output_format, std::cout, tagset));
			while (PlTagger::Chunk* c = reader.get_next_chunk()) {
				writer->write_chunk(*c);
				delete c;
			}
		} catch (PlTagger::PlTaggerError& e) {
			std::cerr << "Error: " << e.info() << "\n";
		}
	} else if (!converter.empty()) {
		try {
			//const PlTagger::Tagset& tagset = PlTagger::get_named_tagset(converter);
			std::string fn = PlTagger::Path::Instance().find_file_or_throw(
					converter, "converter");
			std::cerr << "Loading converter from " << fn << "\n";
			PlTagger::Config::Node n = PlTagger::Config::from_file(fn);
			PlTagger::Conversion::TagsetConverter conv(n);
			PlTagger::XcesReader reader(conv.tagset_from(), std::cin, disamb);
			boost::scoped_ptr<PlTagger::TokenWriter> writer;
			writer.reset(PlTagger::TokenWriter::create(output_format, std::cout, conv.tagset_to()));
			while (PlTagger::Chunk* c = reader.get_next_chunk()) {
				foreach (PlTagger::Sentence*& s, c->sentences()) {
					s = conv.convert_sentence(s);
				}
				writer->write_chunk(*c);
				delete c;
			}
		} catch (PlTagger::PlTaggerError& e) {
			std::cerr << "Error: " << e.info() << "\n";
		}
	} else {
		std::cerr << "Usage: tagset-convert [OPTIONS] <converter>\n";
		std::cerr << "See tagset-convert --help\n";
		return 1;
	}
	return 0;
}
