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
#include <libpwrutils/plural.h>
#include <libtoki/util/settings.h>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/make_shared.hpp>

#include <fstream>
#include <omp.h>

void usage(char** argv)
{
	std::cerr << "Usage: " << argv[0]
		<< " -c config-file [-T num_threads]"
		<< " [-i list-file] [file1 file2 ...]"
		<< "\n";
	std::cerr << "Try " << argv[0] << " --help for more info\n";
}

int main(int argc, char** argv)
{
	std::string config, toki_config;
	std::vector<std::string> plugins;
	std::string config_path, toki_config_path;
	std::vector<std::string> files;
	std::string file_list;
	std::string suffix;
	size_t threads = 1;
	bool quiet = false, progress = false, split_chunks = false;
	using boost::program_options::value;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
			("config,c", value(&config),
			 "Morphological analyser configuration file\n")
			("config-path,C", value(&config_path)->default_value(""),
			 "Override config search path")
			("toki-config,t", value(&toki_config),
			 "Tokenizer configuration file. "
			 "Overrides config value, only used in some input modes.\n")
			("toki-config-path", value(&toki_config_path)->default_value(""),
			 "Override toki config search path")
			("split,s", value(&split_chunks)->zero_tokens(),
			 "Split output into chunks on many-newline tokens")
			("plugin,P", value(&plugins),
			 "Additional plugins to load")
			("file", value(&files),
			 "File to process")
			("file-list,i", value(&file_list),
			 "File-list to process")
			("suffix,S", value(&suffix)->default_value(".morph"),
			 "Output filename suffix")
			("threads,T", value(&threads),
			 "Threads to use")
			("progress,p", value(&progress)->zero_tokens(),
			 "Show progress info")
			("quiet,q", value(&quiet)->zero_tokens(),
			 "Suppress startup info when loading a tagset\n")
			("help,h", "Show help")
			;
	boost::program_options::variables_map vm;
	boost::program_options::positional_options_description p;
	p.add("file", -1);

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
		return 1;
	}
	Toki::Path::Instance().set_verbose(!quiet);
	Maca::Path::Instance().set_verbose(!quiet);
	Corpus2::Path::Instance().set_verbose(!quiet);

	if (config.empty()) {
		std::cerr << "Error: No configuration\n";
		usage(argv);
		return 2;
	}

	if (!file_list.empty()) {
		int added = 0;
		std::ifstream ifs(file_list.c_str());
		if (!ifs.good()) {
			std::cerr << "Error: Unable to read file list " << file_list << "\n";
			usage(argv);
			return 3;
		}
		std::string buf;
		while (ifs.good()) {
			std::getline(ifs, buf);
			if (!buf.empty()) {
				files.push_back(buf);
				++added;
			}
		}
		if (!quiet) {
			std::cerr << "File list processed (" << added << ")\n";
		}
	}

	if (files.empty()) {
		std::cerr << "Error: No files to process\n";
		usage(argv);
		return 2;
	}

	try {
		std::vector< boost::shared_ptr<Maca::SentenceAnalyser> > sas;
		if (toki_config.empty()) {
			sas.push_back(Maca::SentenceAnalyser::create_from_named_config(config));
		} else {
			sas.push_back(Maca::SentenceAnalyser::create_from_named_config(config,
					toki_config));
		}
		if (threads > 1) {
			while (sas.size() < threads) {
				//sas.push_back(sas[0]->clone());
				if (toki_config.empty()) {
					sas.push_back(Maca::SentenceAnalyser::create_from_named_config(config));
				} else {
					sas.push_back(Maca::SentenceAnalyser::create_from_named_config(config,
							toki_config));
				}
			}
		}

		int errors = 0;
		omp_set_num_threads(threads);
#pragma omp parallel for
		for (size_t i = 0; i < files.size(); ++i) {
			std::string in_filename = files[i];
			std::ifstream ifs(in_filename.c_str());
			if (!ifs.good()) {
				std::cerr << "Unable to open " << in_filename << " for reading\n";
				++errors;
				continue;
			}
			std::string out_filename = in_filename + suffix;
			std::ofstream ofs(out_filename.c_str());
			if (!ofs.good()) {
				std::cerr << "Unable to open " << out_filename << " for writing";
				++errors;
				continue;
			}
			Maca::PremorphProcessor pp(ofs, sas[omp_get_thread_num()]);
			pp.set_stats(progress);
			pp.parse_stream(ifs);
		}
		if (errors > 0) {
			std::cerr << "There " << PwrNlp::enpl(errors, "was", "were") << " "
				<< PwrNlp::enpln(errors, "error") << "\n";
		}
	} catch (PwrNlp::PwrNlpError& e) {
		std::cerr << e.scope() << " error " << e.info() << "\n";
	}
}
