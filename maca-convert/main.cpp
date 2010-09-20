#include <libmaca/conv/tagsetconverter.h>
#include <libmaca/io/xcesvalidate.h>
#include <libmaca/io/rft.h>
#include <libmaca/io/xcesreader.h>
#include <libmaca/io/xceswriter.h>
#include <libmaca/util/settings.h>
#include <libmaca/util/tokentimer.h>
#include <libmaca/tagsetmanager.h>

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
	bool progress = false;
	using boost::program_options::value;

	std::string writers = boost::algorithm::join(Maca::TokenWriter::available_writer_types_help(), " ");
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
			 "Input format [xces,rft]\n")
			("output-format,o", value(&output_format)->default_value("xces"),
			 writers_help.c_str())
			("progress,p", value(&progress)->zero_tokens(),
			 "Show progress info")
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
	Maca::Path::Instance().set_verbose(!quiet);

	if (!verify_tagset.empty()) {
		const Maca::Tagset& tagset = Maca::get_named_tagset(verify_tagset);
		Maca::XcesValidator xv(tagset, std::cout);
		xv.validate_stream(std::cin);
	} else if (converter == "nop") {
		try {
			const Maca::Tagset& tagset = Maca::get_named_tagset(force_tagset);
			boost::scoped_ptr<Maca::TokenReader> reader;
			if (input_format == "xces") {
				reader.reset(new Maca::XcesReader(tagset, std::cin, disamb));
			} else if (input_format == "rft") {
				reader.reset(new Maca::RftReader(tagset, std::cin, disamb));
			} else {
				std::cerr << "Unknown inut format: " << input_format << "\n";
				return 2;
			}
			boost::scoped_ptr<Maca::TokenWriter> writer;
			writer.reset(Maca::TokenWriter::create(output_format, std::cout, tagset));
			Maca::TokenTimer& timer = Maca::global_timer();
			timer.register_signal_handler();
			while (Maca::Chunk* c = reader->get_next_chunk()) {
				writer->write_chunk(*c);
				timer.count_chunk(*c);
				if (progress) {
					timer.check_slice();
				}
				delete c;
			}
			if (progress) {
				timer.stats();
			}
		} catch (Maca::MacaError& e) {
			std::cerr << "Error: " << e.info() << "\n";
		}
	} else if (!converter.empty()) {
		try {
			//const Maca::Tagset& tagset = Maca::get_named_tagset(converter);
			std::string fn = Maca::Path::Instance().find_file_or_throw(
					converter, "converter");
			std::cerr << "Loading converter from " << fn << "\n";
			Maca::Config::Node n = Maca::Config::from_file(fn);
			Maca::Conversion::TagsetConverter conv(n);
			Maca::XcesReader reader(conv.tagset_from(), std::cin, disamb);
			boost::scoped_ptr<Maca::TokenWriter> writer;
			writer.reset(Maca::TokenWriter::create(output_format, std::cout, conv.tagset_to()));
			Maca::TokenTimer timer;
			while (Maca::Chunk* c = reader.get_next_chunk()) {
				foreach (Maca::Sentence*& s, c->sentences()) {
					s = conv.convert_sentence(s);
					timer.count_sentence(*s);
					if (progress) {
						timer.check_slice();
					}
				}
				writer->write_chunk(*c);
				delete c;
			}
			if (progress) {
				timer.stats();
			}
		} catch (Maca::MacaError& e) {
			std::cerr << "Error: " << e.info() << "\n";
		}
	} else {
		std::cerr << "Usage: tagset-convert [OPTIONS] <converter>\n";
		std::cerr << "See tagset-convert --help\n";
		return 1;
	}
	return 0;
}
