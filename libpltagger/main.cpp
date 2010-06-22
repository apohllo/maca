/* Code to display version info and greeting message when the .so is run */

#ifdef __GNUG__
#ifdef __linux__

#ifndef LIBPLTAGGER_INTERP
#ifdef __LP64__
#define LIBPLTAGGER_INTERP "/lib/ld-linux-x86-64.so.2";
#else
#define LIBPLTAGGER_INTERP "/lib/ld-linux.so.2";
#endif
#endif

#ifdef LIBPLTAGGER_INTERP
extern "C"
const char my_interp[] __attribute__((section(".interp"))) = LIBPLTAGGER_INTERP;
#endif

#ifdef HAVE_CONFIG_D_H
#include <libpltagger/config_d.h>
#endif
#include <cstdlib>
#include <iostream>
#include <boost/algorithm/string.hpp>

#ifndef LIBPLTAGGER_VERSION
#define LIBPLTAGGER_VERSION "?"
#endif

extern "C"
int LIBPLTAGGER_entry_()
{
	std::cout << "This is libpltagger-" LIBPLTAGGER_VERSION ", a configurable tagger library.\n";
	std::cout << "Data dir configured as: " << LIBPLTAGGER_DATA_DIR << "\n";
#ifdef HAVE_MORFEUSZ
	std::cout << "Built with Morfeusz support.\n";
#else
	std::cout << "Built without Morfeusz support.\n";
#endif
#ifdef HAVE_SFST
	std::cout << "Built with SFST support.\n";
#else
	std::cout << "Built without SFST support.\n";
#endif
	exit(0);
}

#endif
#endif
