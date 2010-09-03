#ifndef LIBMACA_DISPATCHANALYSER_H
#define LIBMACA_DISPATCHANALYSER_H

#include <libmaca/morph/morphanalyser.h>

#include <map>
#include <set>

namespace Maca {

	/**
	 * An analyser that delegates analysis to other analysers based on the
	 * Toki token type.
	 */
	class DispatchAnalyser : public MorphAnalyser
	{
	public:
		/// Constructor for a DispatchAnalyser working with a tagset
		DispatchAnalyser(const Tagset* tagset);

		/// Constructor for a DispatchAnalyser from config
		DispatchAnalyser(const Config::Node& cfg);

		/// Destructor
		~DispatchAnalyser();

		/// Cloning
		DispatchAnalyser* clone() const;

		/// MorphAnalyser override
		bool process_functional(const Toki::Token &t, boost::function<void (Token*)> sink);

		/**
		 * Handler adding function. The passed analyser should have the same
		 * output tagset as the DispatchAnalyser's tagset. The DispatchAnalyser
		 * takes ownership of the passed analyser.
		 */
		void add_type_handler(const std::string& type,  MorphAnalyser* a);

		/// adder for the default analyser list, used when no analyser matches
		/// the Toki token type
		void add_default_handler(MorphAnalyser* a);

		/// getter for the default handlers size
		size_t default_handlers_count() const;

		/// getter for the number of handlers
		size_t handler_count() const;

	private:
		typedef std::vector<MorphAnalyser*> analyser_vector_t;
		typedef std::map<std::string, analyser_vector_t > analyser_map_t;

		/// the toki type -> analyser map
		analyser_map_t type_handlers_;

		/// the set of analysers this DispatchAnalyser owns
		std::set<MorphAnalyser*> analysers_;

		/// the default analysers
		analyser_vector_t default_;

		/// the fallback analyser
		MorphAnalyser* fallback_;
	};

} /* end ns Maca */

#endif // LIBMACA_DISPATCHANALYSER_H
