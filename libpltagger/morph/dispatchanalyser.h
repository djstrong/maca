#ifndef LIBPLTAGGER_DISPATCHANALYSER_H
#define LIBPLTAGGER_DISPATCHANALYSER_H

#include <libpltagger/morph/morphanalyser.h>

#include <map>
#include <set>

namespace PlTagger {

	/**
	 * An analyser that delegates analysis to other analysers based on the
	 * Toki token type.
	 */
	class DispatchAnalyser : public MorphAnalyser
	{
	public:
		/// Constructor for a DispatchAnalyser working with a tagset
		DispatchAnalyser(const Tagset* tagset);

		/// Destructor
		~DispatchAnalyser();

		/// MorphAnalyser override
		void process_functional(const Toki::Token &t, boost::function<void (Token*)> sink);

		/**
		 * Handler adding function. The passed analyser should have the same
		 * output tagset as the DispatchAnalyser's tagset. The DispatchAnalyser
		 * takes ownership of the passed analyser.
		 */
		void add_type_handler(const std::string& type,  MorphAnalyser* a);

		/// setter for the default analyser, used when no analyser matches
		/// the Toki token type
		void set_default_handler(MorphAnalyser* a);

		/// getter for the default handler
		MorphAnalyser* default_handler();

		/// getter for the number of handlers
		size_t handler_count() const;

	private:
		/// the toki type -> analyser map
		std::map<std::string, MorphAnalyser*> type_handlers_;

		/// the set of analysers this DispatchAnalyser owns
		std::set<MorphAnalyser*> analysers_;

		/// the default analyser
		MorphAnalyser* default_;
	};

} /* end ns PlTagger */

#endif // LIBPLTAGGER_DISPATCHANALYSER_H
