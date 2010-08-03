#ifdef HAVE_CONFIG_D_H
#include <libpltagger/config_d.h>
#endif
#include <libpltagger/conv/tagsetconverter.h>
#include <libpltagger/debug.h>
#include <libpltagger/settings.h>
#include <libpltagger/tagsetmanager.h>
#include <libpltagger/tagsetparser.h>

#include <libpltagger/io/xces.h>

#include <libtoki/foreach.h>

#include <boost/program_options.hpp>

#include <fstream>

int main(int argc, char** argv)
{
	std::string converter;
	std::string input_format, output_format;
	bool quiet = false;
	using boost::program_options::value;

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
			("converter,c", value(&converter),
			 "Tagset converter configuration\n")
			("input-format,i", value(&input_format)->default_value("plain"),
			 "Input format\n")
			("output-format,o", value(&output_format)->default_value("plain"),
			 "Output format\n")
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


	if (!converter.empty()) {
		const PlTagger::Tagset& tagset = PlTagger::get_named_tagset(converter);
		PlTagger::XcesReader reader(std::cin, tagset);
		PlTagger::XcesWriter writer(std::cout, tagset);
		writer.write_paragraph(reader.read_paragraph());
	} else {
		std::cerr << "Usage: tagset-convert [OPTIONS] <converter>\n";
		std::cerr << "See tagset-convert --help\n";
		return 1;
	}
	return 0;
}