#include <libmaca/io/xces.h>
#include <libmaca/io/reader.h>
#include <libtoki/util/foreach.h>
#include <libxml++/libxml++.h>
#include <libxml++/nodes/node.h>
#include <libxml++/nodes/element.h>
#include <libxml++/parsers/saxparser.h>

#include <iostream>

namespace Maca {

	XcesError::XcesError(const std::string &what)
		: MacaError(what)
	{
	}

	XcesError::~XcesError() throw()
	{
	}

} /* end ns Maca */
