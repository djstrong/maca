#include <libpltagger/io/init.h>
#include <libpltagger/io/orthwriter.h>
#include <libpltagger/io/plain.h>
#include <libpltagger/io/xceswriter.h>

namespace PlTagger {

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

} /* end ns PlTagger */
