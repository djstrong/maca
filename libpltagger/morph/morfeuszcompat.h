#ifndef LIBPLTAGGER_MORFEUSZCOMPAT_H
#define LIBPLTAGGER_MORFEUSZCOMPAT_H

#include <morfeusz.h>
#include <list>
#include <libpltagger/token.h>
#include <libpltagger/morph/morphanalyser.h>

namespace PlTagger {
	namespace MorfeuszCompat {
		void Analyse(std::vector<Token*> &out, const std::string &form,
				const MorphAnalyser& ma);

		/** Part of DAG, being a sequence of pmorf edges that corresponds to segmentation ambiguity of one word.
		  * For instance, analysing form miałem will result in one part containing three edges: miał, em, miałem.
		  */
		struct Part
		{
			int beg, end;
			std::list<InterpMorf *> content;

			/// Creates a new part containing given node only.
			Part(InterpMorf *pmorf);

			/** Tries to join a part with other.
			Precondition: parts must not be disjoint.
			Postcondition: this part has edges from both parts, that part has no edge and is marked defunct. */
			bool TryJoining(Part &other);

			/** Gathers tokens contained in this part and flushes new CToken objects into given output vector.
			  * Should be called after joining parts. Will resolve segmentation ambiguity and mark tokens'
			  * variant numbers (or set to 0 if no ambiguity). */
			void HarvestTokens(std::vector<Token*> &out, const MorphAnalyser& ma);
			/// Creates an ign lexem, sets base to orth.

			bool Disjoint(const Part &other) const;
			void Join(Part &other);

			void Dump();
		};

	} /* end ns MorfeuszCompat */
} /* end ns PlTagger */

#endif // LIBPLTAGGER_MORFEUSZCOMPAT_H
