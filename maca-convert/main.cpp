/*
    Copyright (C) 2010 Tomasz Śniatowski, Adam Radziszewski
    Part of the libmaca project

    This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.

    This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. 

    See the LICENSE and COPYING files for more details.
*/

#include <libmaca/conv/tagsetconverter.h>
#include <libcorpus2/io/xcesvalidate.h>
#include <libcorpus2/io/rft.h>
#include <libcorpus2/io/xcesreader.h>
#include <libcorpus2/io/xceswriter.h>
#include <libmaca/util/settings.h>
#include <libcorpus2/util/tokentimer.h>
#include <libcorpus2/tagsetmanager.h>

#include <libpwrutils/foreach.h>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/make_shared.hpp>

#include <fstream>
#include <iomanip>

void write_folds(Corpus2::TokenReader& reader,
		const std::string& output_format, const std::string& prefix, int folds,
		Maca::Conversion::TagsetConverter* conv = NULL)
{
	std::vector< boost::shared_ptr<std::ofstream> > streams;
	std::vector< boost::shared_ptr<Corpus2::TokenWriter> > writers;
	std::vector< boost::shared_ptr<std::ofstream> > streams2;
	std::vector< boost::shared_ptr<Corpus2::TokenWriter> > writers2;
	std::vector<int> sentences, tokens;
	for (int i = 0; i < folds; ++i) {
		std::stringstream fn, fn2;
		fn << prefix << std::setw(2) << std::setfill('0') << i + 1 << ".xml";
		fn2 << prefix << "x"<< std::setw(2) << std::setfill('0') << i + 1 << ".xml";
		std::string s = fn.str();
		std::string s2 = fn2.str();
		streams.push_back(boost::make_shared<std::ofstream>(s.c_str()));
		streams2.push_back(boost::make_shared<std::ofstream>(s2.c_str()));
		boost::shared_ptr<Corpus2::TokenWriter> w;
		w.reset(Corpus2::TokenWriter::create(output_format, *streams.back(), reader.tagset()));
		writers.push_back(w);
		w.reset(Corpus2::TokenWriter::create(output_format, *streams2.back(), reader.tagset()));
		writers2.push_back(w);
		sentences.push_back(0);
		tokens.push_back(0);
	}
	while (Corpus2::Sentence* s = reader.get_next_sentence()) {
		int f = rand() % folds;
		if (conv) {
			s = conv->convert_sentence(s);
		}
		writers[f]->write_sentence(*s);
		for (int i = 0; i < folds; ++i) {
			if (i != f) {
				writers2[i]->write_sentence(*s);
			}
		}
		sentences[f]++;
		tokens[f] += s->size();
		delete s;
	}
	std::cout << "Folds created:";
	for (int i = 0; i < folds; ++i) {
		std::cout << "Fold Sentences Tokens\n";
		std::cout << std::setw(2) << std::setfill('0') << i << "   ";
		std::cout << std::setfill(' ');
		std::cout << std::setw(9) << sentences[i] << " ";
		std::cout << std::setw(9) << tokens[i] << "\n";
	}
}

int main(int argc, char** argv)
{
	std::string converter, verify_tagset, force_tagset;
	std::string input_format, output_format;
	bool quiet = false;
	bool disamb = false;
	bool progress = false;
	int folds = 0;
	std::string folds_file_prefix;
	using boost::program_options::value;

	std::string writers = boost::algorithm::join(Corpus2::TokenWriter::available_writer_types_help(), " ");
	std::string writers_help = "Output format, any of: " + writers + "\n";

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
			("converter,c", value(&converter),
			 "Corpus2::Tagset converter configuration\n")
			("disamb-only,d", value(&disamb)->zero_tokens(),
			 "Only read lexemes marked as disambiguated\n")
			("verify,v", value(&verify_tagset),
			 "Verify tags within a tagset\n")
			("tagset,t", value(&force_tagset),
			 "Corpus2::Tagset override\n")
			("input-format,i", value(&input_format)->default_value("xces"),
			 "Input format [xces,rft,xces-sh]\n")
			("output-format,o", value(&output_format)->default_value("xces"),
			 writers_help.c_str())
			("progress,p", value(&progress)->zero_tokens(),
			 "Show progress info")
			("folds,F", value(&folds),
			 "Spread sentences across folds")
			("folds-file-name,f", value(&folds_file_prefix),
			 "Prefix for fold filenames")
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

	try {
		if (!verify_tagset.empty()) {
			const Corpus2::Tagset& tagset = Corpus2::get_named_tagset(verify_tagset);
			Corpus2::XcesValidator xv(tagset, std::cout);
			xv.validate_stream(std::cin);
		} else if (converter == "nop") {
				const Corpus2::Tagset& tagset = Corpus2::get_named_tagset(force_tagset);
				boost::scoped_ptr<Corpus2::TokenReader> reader;
				if (input_format == "xces") {
					reader.reset(new Corpus2::XcesReader(tagset, std::cin, disamb));
				} else if (input_format == "xces-sh") {
					reader.reset(new Corpus2::XcesReader(tagset, std::cin, disamb, true));
				} else if (input_format == "rft") {
					reader.reset(new Corpus2::RftReader(tagset, std::cin, disamb));
				} else {
					std::cerr << "Unknown input format: " << input_format << "\n";
					return 2;
				}
				if (folds > 0) {
					write_folds(*reader, output_format, folds_file_prefix, folds);
					return 0;
				}
				boost::scoped_ptr<Corpus2::TokenWriter> writer;
				writer.reset(Corpus2::TokenWriter::create(output_format, std::cout, tagset));
				Corpus2::TokenTimer& timer = Corpus2::global_timer();
				timer.register_signal_handler();
				while (Corpus2::Chunk* c = reader->get_next_chunk()) {
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
		} else if (!converter.empty()) {
			//const Corpus2::Tagset& tagset = Corpus2::get_named_tagset(converter);
			std::string fn = Maca::Path::Instance().find_file_or_throw(
					converter, "converter");
			Maca::Config::Node n = Maca::Config::from_file(fn);
			Maca::Conversion::TagsetConverter conv(n);
			Corpus2::XcesReader reader(conv.tagset_from(), std::cin, disamb);
			if (folds > 0) {
				write_folds(reader, output_format, folds_file_prefix, folds, &conv);
				return 0;
			}
			boost::scoped_ptr<Corpus2::TokenWriter> writer;
			writer.reset(Corpus2::TokenWriter::create(output_format, std::cout, conv.tagset_to()));
			Corpus2::TokenTimer timer;
			while (Corpus2::Chunk* c = reader.get_next_chunk()) {
				foreach (Corpus2::Sentence*& s, c->sentences()) {
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
		} else {
			std::cerr << "Usage: maca-convert [OPTIONS] <converter>\n";
			std::cerr << "See maca-convert --help\n";
			return 1;
		}
	} catch (PwrNlp::PwrNlpError& e) {
		std::cerr << "Error: " << e.scope() << " " << e.info() << "\n";
	}
	return 0;
}
