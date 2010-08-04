#include <libpltagger/io/init.h>
#include <libpltagger/io/plain.h>
#include <libpltagger/io/xces.h>

namespace PlTagger {

	bool init_token_writers()
	{
		static bool init_done = false;
		if (init_done) return false;
		TokenWriterFactory::Instance().Register("xces", writer_creator<XcesWriter>);
		TokenWriterFactory::Instance().Register("xces-flat", XcesWriter::create_flat);
		TokenWriterFactory::Instance().Register("xces-nochunk", XcesWriter::create_nochunk);
		TokenWriterFactory::Instance().Register("xces-flat-nochunk", XcesWriter::create_flat_nochunk);
		TokenWriterFactory::Instance().Register("plain", writer_creator<PlainWriter>);
		init_done = true;
		return true;
	}

} /* end ns PlTagger */
