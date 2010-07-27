#ifndef LIBPLTAGGER_IO_XCES_H
#define LIBPLTAGGER_IO_XCES_H

#include <libpltagger/token.h>

namespace PlTagger {

	class XcesWriter {
	public:

		XcesWriter(std::ostream& os, const Tagset& tagset);

		~XcesWriter();

		/// Output the footer string.
		/// Optional, the destructor calls this if necessary.
		void finish();

		/// Output the footer if needed and start off a new document header
		void reset();

		void write_sentence(const std::vector<Token*>& v);

		void write_paragraph(const std::vector< std::vector<Token*> >& v);

	private:
		void iwrite(const std::string& s);

		void indent();

		void header();

		void footer();

		void token(const Token& t);

		void sentence(const std::vector<Token*>& v);

		void paragraph_head();

		void paragraph(const std::vector< std::vector<Token*> >& v);

		std::ostream& os_;

		const Tagset& tagset_;

		bool needs_footer_;

		int indent_;

		int cid_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_IO_XCES_H
