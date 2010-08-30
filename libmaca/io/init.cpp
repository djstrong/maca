#include <libmaca/io/init.h>
#include <libmaca/io/orthwriter.h>
#include <libmaca/io/plain.h>
#include <libmaca/io/xceswriter.h>

namespace Maca {

	bool init_token_writers()
	{
		static bool init_done = false;
		if (init_done) return false;
		TokenWriterFactory::Instance().Register("xces", writer_creator<XcesWriter>);
		TokenWriterFactory::Instance().Register("plain", writer_creator<PlainWriter>);
		TokenWriterFactory::Instance().Register("orth", writer_creator<OrthWriter>);
		init_done = true;
		return true;
	}

} /* end ns Maca */
