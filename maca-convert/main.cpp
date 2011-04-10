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

class Folder
{
public:
	Folder(Corpus2::TokenReader& reader,
		const std::string& output_format, const std::string& prefix,
		Maca::Conversion::TagsetConverter* conv = NULL);

	void init_writers(int folds);
	void stats() const;
	void write_seq_folds();
	void write_random_folds(double train_ratio, double test_ratio = -1);

private:
	Corpus2::TokenReader& reader_;
	Maca::Conversion::TagsetConverter *conv_;
	std::string prefix_;
	std::string output_format_;

	std::vector< boost::shared_ptr<std::ofstream> > streams_train_;
	std::vector< boost::shared_ptr<Corpus2::TokenWriter> > writers_train_;
	std::vector< boost::shared_ptr<std::ofstream> > streams_test_;
	std::vector< boost::shared_ptr<Corpus2::TokenWriter> > writers_test_;
	std::vector<int> sentences_train_, tokens_train_;
	std::vector<int> sentences_test_, tokens_test_;
	std::vector<int> sentences_total_, tokens_total_;
};

Folder::Folder(Corpus2::TokenReader &reader, const std::string &output_format,
	const std::string &prefix, Maca::Conversion::TagsetConverter *conv)
 : reader_(reader), conv_(conv), prefix_(prefix), output_format_(output_format)
{
}

void Folder::init_writers(int folds)
{
	for (int i = 0; i < folds; ++i) {
		std::stringstream fntrain, fntest;
		fntrain << prefix_ << "train"
			<< std::setw(2) << std::setfill('0') << i + 1 << ".xml";
		fntest << prefix_ << "test"
			<< std::setw(2) << std::setfill('0') << i + 1 << ".xml";
		std::string strain = fntrain.str();
		std::string stest = fntest.str();
		streams_train_.push_back(boost::make_shared<std::ofstream>(strain.c_str()));
		streams_test_.push_back(boost::make_shared<std::ofstream>(stest.c_str()));
		boost::shared_ptr<Corpus2::TokenWriter> w;
		w.reset(Corpus2::TokenWriter::create(output_format_, *streams_train_.back(), reader_.tagset()));
		writers_train_.push_back(w);
		w.reset(Corpus2::TokenWriter::create(output_format_, *streams_test_.back(), reader_.tagset()));
		writers_test_.push_back(w);
		sentences_train_.push_back(0);
		tokens_train_.push_back(0);
		sentences_test_.push_back(0);
		tokens_test_.push_back(0);
		sentences_total_.push_back(0);
		tokens_total_.push_back(0);
	}
}

void Folder::stats() const
{
	std::cout << "    |          Sentences            |              Tokens               |\n";
	std::cout << "Fold|Train   Test    Train%   Test% |Train     Test      Train%   Test% |\n";
	for (size_t i = 0; i < writers_test_.size(); ++i) {
		std::cout << " " << std::setw(2) << std::setfill('0') << (i+1) << " |";
		std::cout << std::setfill(' ');
		std::cout << std::setw(7) << sentences_train_[i] << " ";
		std::cout << std::setw(7) << sentences_test_[i] << " ";
		std::cout << std::setw(7) << 100 * static_cast<double>(sentences_train_[i]) / (sentences_total_[i]);
		std::cout << " ";
		std::cout << std::setw(7) << 100 * static_cast<double>(sentences_test_[i]) / (sentences_total_[i]);
		std::cout << "|";
		std::cout << std::setw(9) << tokens_train_[i] << " ";
		std::cout << std::setw(9) << tokens_test_[i] << " ";
		std::cout << std::setw(7) << 100 * static_cast<double>(tokens_train_[i]) / (tokens_total_[i]);
		std::cout << " ";
		std::cout << std::setw(7) << 100 * static_cast<double>(tokens_test_[i]) / (tokens_total_[i]);
		std::cout << "|\n";
	}
}

void Folder::write_seq_folds()
{
	while (Corpus2::Sentence::Ptr s = reader_.get_next_sentence()) {
		size_t f = rand() % writers_test_.size();
		if (conv_) {
			s = conv_->convert_sentence(s);
		}
		writers_train_[f]->write_sentence(*s);
		tokens_train_[f] += s->size();
		sentences_train_[f]++;
		for (size_t i = 0; i < writers_test_.size(); ++i) {
			if (i != f) {
				writers_test_[i]->write_sentence(*s);
				tokens_test_[f] += s->size();
				sentences_test_[f]++;
			}
		}
	}
}

void Folder::write_random_folds(double train_ratio, double test_ratio /* = -1 */)
{
	int threshold = static_cast<int>((1-train_ratio) * RAND_MAX);
	int discard_threshold = RAND_MAX;
	if (test_ratio > 0) {
		discard_threshold = static_cast<int>(test_ratio * RAND_MAX);
	}
	std::cerr << "Running random-folds with thresholds "
		<< train_ratio << " "  << test_ratio << " -> "
		<< discard_threshold << " " << threshold << " " << RAND_MAX << "\n";
	while (Corpus2::Sentence::Ptr s = reader_.get_next_sentence()) {
		if (conv_) {
			s = conv_->convert_sentence(s);
		}
		for (size_t i = 0; i < writers_test_.size(); ++i) {
			int r = rand();
			if (r > threshold) {
				writers_train_[i]->write_sentence(*s);
				tokens_train_[i] += s->size();
				sentences_train_[i]++;
			} else if ( r < discard_threshold ) {
				writers_test_[i]->write_sentence(*s);
				tokens_test_[i] += s->size();
				sentences_test_[i]++;
			}
			sentences_total_[i]++;
			tokens_total_[i] += s->size();
		}
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
	double random_folds_train = 0;
	double random_folds_test = 0;
	int seed = -1;
	using boost::program_options::value;

	std::string writers = boost::algorithm::join(Corpus2::TokenWriter::available_writer_types_help(), " ");
	std::string writers_help = "Output format, any of: " + writers + "\n";

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
			("converter,c", value(&converter),
			 "Tagset converter configuration")
			("disamb-only,d", value(&disamb)->zero_tokens(),
			 "Only read lexemes marked as disambiguated")
			("verify,v", value(&verify_tagset),
			 "Verify tags within a tagset")
			("tagset,t", value(&force_tagset),
			 "Tagset override (required in nop conversion)")
			("input-format,i", value(&input_format)->default_value("xces"),
			 "Input format [xces,rft,xces-sh]")
			("output-format,o", value(&output_format)->default_value("xces"),
			 writers_help.c_str())
			("progress,p", value(&progress)->zero_tokens(),
			 "Show progress info")
			("folds,F", value(&folds),
			 "Spread sentences across folds")
			("train-ratio,r", value(&random_folds_train),
			 "Random spread across folds with train ratio")
			("test-ratio,R", value(&random_folds_test),
			 "Random spread across folds with train ratio, (1-r) by default")
			("seed", value(&seed),
			 "random seed, -1 to use time(0)")
			("folds-file-name,f", value(&folds_file_prefix),
			 "Prefix for fold filenames")
			("quiet,q", value(&quiet)->zero_tokens(),
			 "Suppress startup info")
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
	if (seed == -1) {
		seed = time(0);
	}
	srand(seed);


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
				Folder f(*reader, output_format, folds_file_prefix, NULL);
				f.init_writers(folds);
				if (random_folds_train > 0) {
					f.write_random_folds(random_folds_train, random_folds_test);
				} else {
					f.write_seq_folds();
				}
				f.stats();
				return 0;
			}
			boost::scoped_ptr<Corpus2::TokenWriter> writer;
			writer.reset(Corpus2::TokenWriter::create(output_format, std::cout, tagset));
			Corpus2::TokenTimer& timer = Corpus2::global_timer();
			timer.register_signal_handler();
			while (boost::shared_ptr<Corpus2::Chunk> c = reader->get_next_chunk()) {
				writer->write_chunk(*c);
				timer.count_chunk(*c);
				if (progress) {
					timer.check_slice();
				}
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
				Folder f(reader, output_format, folds_file_prefix, &conv);
				f.init_writers(folds);
				if (random_folds_train > 0) {
					f.write_random_folds(random_folds_train, random_folds_test);
				} else {
					f.write_seq_folds();
				}
				f.stats();
				return 0;
			}
			boost::scoped_ptr<Corpus2::TokenWriter> writer;
			writer.reset(Corpus2::TokenWriter::create(output_format, std::cout, conv.tagset_to()));
			Corpus2::TokenTimer timer;
			while (boost::shared_ptr<Corpus2::Chunk> c = reader.get_next_chunk()) {
				foreach (boost::shared_ptr<Corpus2::Sentence>& s, c->sentences()) {
					s = conv.convert_sentence(s);
					timer.count_sentence(*s);
					if (progress) {
						timer.check_slice();
					}
				}
				writer->write_chunk(*c);
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
