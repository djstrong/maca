/* Code to display version info and greeting message when the .so is run */

#ifdef __GNUG__
#ifdef __linux__

#ifndef LIBMACA_INTERP
#ifdef __LP64__
#define LIBMACA_INTERP "/lib/ld-linux-x86-64.so.2";
#else
#define LIBMACA_INTERP "/lib/ld-linux.so.2";
#endif
#endif

#ifdef LIBMACA_INTERP
extern "C"
const char my_interp[] __attribute__((section(".interp"))) = LIBMACA_INTERP;
#endif

#ifdef HAVE_CONFIG_D_H
#include <libmaca/config_d.h>
#endif
#include <cstdlib>
#include <iostream>
#include <boost/algorithm/string.hpp>

#ifndef LIBMACA_VERSION
#define LIBMACA_VERSION "?"
#endif

extern "C"
int LIBMACA_entry_()
{
	std::cout << "This is libmaca-" LIBMACA_VERSION ", a configurable morphological analysis library.\n";
	std::cout << "Data dir configured as: " << LIBMACA_DATA_DIR << "\n";
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
