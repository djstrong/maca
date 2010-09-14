#include <libmaca/io/init.h>
#include <libmaca/io/orthwriter.h>
#include <libmaca/io/plain.h>
#include <libmaca/io/xceswriter.h>

namespace Maca {

	bool init_token_writers()
	{
		static bool init_done = false;
		if (init_done) return false;
		TokenWriter::register_writer<XcesWriter>("xces", "flat,chunk,nochunk,disamb,sort,split");
		TokenWriter::register_writer<PlainWriter>("plain", "");
		TokenWriter::register_writer<OrthWriter>("orth", "actual_ws,end_nl");
		init_done = true;
		return true;
	}

} /* end ns Maca */
