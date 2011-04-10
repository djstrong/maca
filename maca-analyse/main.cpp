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

#include <libcorpus2/io/writer.h>
#include <libmaca/io/text.h>
#include <libmaca/io/premorph.h>
#include <libmaca/morph/dispatchanalyser.h>
#include <libmaca/util/settings.h>
#include <libcorpus2/util/settings.h>
#include <libmaca/util/sentenceanalyser.h>
#include <libcorpus2/util/tokentimer.h>

#include <libtoki/sentencesplitter.h>
#include <libtoki/tokenizer/layertokenizer.h>
#include <libpwrutils/foreach.h>
#include <libtoki/util/settings.h>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/make_shared.hpp>

#include <fstream>
#include <omp.h>

int main(int argc, char** argv)
{
	std::string config, toki_config;
	std::string input_format, output_format;
	std::vector<std::string> plugins;
	std::string config_path, toki_config_path, initial_wa_override;
	int threads = 0;
	bool quiet = false, progress = false, split_chunks = false;
	using boost::program_options::value;

	std::string writers = boost::algorithm::join(Corpus2::TokenWriter::available_writer_types_help(), " ");

	std::string writers_help = "Output format, any of: " + writers + "\n";

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
			("config,c", value(&config),
			 "Morphological analyser configuration file")
			("config-path,C", value(&config_path)->default_value(""),
			 "Override config search path")
			("toki-config,t", value(&toki_config),
			 "Tokenizer configuration file. "
			 "Overrides config value, only used in some input modes.")
			("toki-config-path", value(&toki_config_path)->default_value(""),
			 "Override toki config search path")
			("initial-wa-override", value(&initial_wa_override),
			 "Initial whitespace (overrides toki config file)")
			("input-format,i", value(&input_format)->default_value("text"),
			 "Input format, any of: text premorph premorph-stream")
			("output-format,o", value(&output_format)->default_value("plain"),
			 writers_help.c_str())
			("split,s", value(&split_chunks)->zero_tokens(),
			 "Split output into chunks on many-newline tokens")
			("plugin,P", value(&plugins),
			 "Additional plugins to load")
			("threads,T", value(&threads),
			 "Threads to use")
			("progress,p", value(&progress)->zero_tokens(),
			 "Show progress info")
			("quiet,q", value(&quiet)->zero_tokens(),
			 "Suppress startup info when loading a tagset")
			("help,h", "Show help")
			;
	boost::program_options::variables_map vm;
	boost::program_options::positional_options_description p;
	p.add("config", -1);

	try {
		boost::program_options::store(
			boost::program_options::command_line_parser(argc, argv)
			.options(desc).positional(p).run(), vm);
	} catch (boost::program_options::error& e) {
		std::cerr << e.what() << "\n";
		return 2;
	}
	boost::program_options::notify(vm);

	if (!config_path.empty()) {
		Maca::Path::Instance().set_search_path(config_path);
	}
	if (!toki_config_path.empty()) {
		Toki::Path::Instance().set_search_path(config_path);
	}

	foreach (const std::string& s, plugins) {
		Maca::MorphAnalyser::load_plugin(s, false);
	}

	if (vm.count("help")) {
		std::cout << desc << "\n";
		std::cout << "Available analyser types: ";
		std::cout << boost::algorithm::join(
				Maca::MorphAnalyser::available_analyser_types(), " ") << "\n";
		std::cout << "Available configurations: ";
		std::cout << Maca::SentenceAnalyser::available_configurations() << "\n";
		return 1;
	}
	Toki::Path::Instance().set_verbose(!quiet);
	Maca::Path::Instance().set_verbose(!quiet);
	Corpus2::Path::Instance().set_verbose(!quiet);

	if (!config.empty()) {
		try {
			boost::shared_ptr<Maca::SentenceAnalyser> sa;
			if (toki_config.empty()) {
				sa = Maca::SentenceAnalyser::create_from_named_config(config);
			} else {
				sa = Maca::SentenceAnalyser::create_from_named_config(config,
						toki_config);
			}

			if (!initial_wa_override.empty()) {
				PwrNlp::Whitespace::Enum wa = PwrNlp::Whitespace::from_string(
						initial_wa_override);
				if (PwrNlp::Whitespace::is_valid(wa)) {
					Toki::Tokenizer& tok = sa->tokenizer();
					dynamic_cast<Toki::LayerTokenizer&>(tok).input_tokenizer().set_initial_whitespace(wa);
				} else {
					std::cerr << "Invalid initial whitespace: "
							<< initial_wa_override << "\n";
				}
			}

			if (input_format == "premorph-stream") {
				Maca::PremorphProcessor pp(std::cout, sa);
				pp.set_stats(progress);
				pp.parse_stream(std::cin);
				return 0;
			}
			boost::scoped_ptr<Corpus2::TokenWriter> writer;
			writer.reset(Corpus2::TokenWriter::create(output_format, std::cout, sa->tagset()));

			boost::shared_ptr<Corpus2::TokenReader> tr;
			if (input_format == "premorph") {
				tr = boost::make_shared<Maca::PremorphReader>(boost::ref(std::cin), sa);
			} else {
				tr = boost::make_shared<Maca::TextReader>(boost::ref(std::cin), sa);
			}
			Corpus2::TokenTimer& timer = Corpus2::global_timer();
			timer.register_signal_handler();
			if (split_chunks) {
				/// TODO empty chunks
				while (boost::shared_ptr<Corpus2::Chunk> chunk = tr->get_next_chunk()) {
					writer->write_chunk(*chunk);
					timer.count_chunk(*chunk);
					if (progress) {
						timer.check_slice();
					}
				}
			} else {
				while (Corpus2::Sentence::Ptr sentence = tr->get_next_sentence()) {
					assert(!sentence->empty());
					timer.count_sentence(*sentence);
					writer->write_sentence(*sentence);
					if (progress) {
						timer.check_slice();
					}
				}
			}
			if (progress) {
				timer.stats();
			}
		} catch (Maca::MacaError& e) {
			std::cerr << "Maca Error: " << e.info() << "\n";
			return 4;
		} catch (Toki::TokiError& e) {
			std::cerr << "Tokenizer Error: " << e.info() << "\n";
			return 6;
		} catch (Corpus2::Corpus2Error& e) {
			std::cerr << "Corpus2 Error: " << e.info() << "\n";
			return 8;
		}
	} else {
		std::cerr << "Usage: analyse -c CONFIG [OPTIONS]\n";
		std::cerr << "See analyse --help\n";
		return 1;
	}
	return 0;
}
