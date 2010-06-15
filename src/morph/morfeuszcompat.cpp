#include "morfeuszcompat.h"
#include <libtoki/foreach.h>
#include <deque>

namespace PlTagger { namespace MorfeuszCompat {
	bool splitTags = true;

	void SetEncoding(int encoding)
	{
		morfeusz_set_option(MORFOPT_ENCODING,encoding);
	}

	void Analyse(std::vector<Token*> &out, const std::string &form,
			const MorphAnalyser& ma)
	{
		InterpMorf *pmorf = morfeusz_analyse(const_cast<char*>(form.c_str()));

		std::vector<Part> parts;

		// create a part for each node
		while (pmorf->p != -1) {
			parts.push_back(Part(pmorf));
			++pmorf; // next node
		}

		// join parts where possible
		for (size_t pos_src = 0; pos_src < parts.size(); ++pos_src) {
			for (size_t pos_base = 0; pos_base < parts.size(); ++pos_base) {
				if(pos_base != pos_src) {
					parts[pos_base].TryJoining(parts[pos_src]);
				}
			}
		}

		for(size_t pos_src = 0; pos_src < parts.size(); ++pos_src) {
			parts[pos_src].HarvestTokens(out, ma);
		}
	}

	Part::Part(InterpMorf *pmorf)
	{
		beg = pmorf->p;
		end = pmorf->k;
		content.push_back(pmorf);
	}

	bool Part::Disjoint(const Part &other) const
	{
		return (end <= other.beg) || (beg >= other.end);
	}

	bool Part::TryJoining(Part &other)
	{
		if(Disjoint(other)) {
			return false;
		} else {
			Join(other);
			return true;
		}
	}

	void Part::Join(Part &other)
	{
		// move elements from other.content to this->content
		content.splice(content.end(), other.content);
		// update this range
		if(beg > other.beg) beg = other.beg;
		if(end < other.end) end = other.end;
		// update that range (mark it defunct)
		other.beg = -1;
		other.end = -1;
	}

	void Part::HarvestTokens(std::vector<Token*> &out, const MorphAnalyser& ma)
	{
		std::deque<Token*> preque;

		// variantNum will be set to 1 at first
		// and then reverted to 0 if no ambiguity (one-token part)
		int last_p = -1, last_k = -1;
		int lastvarnum = 0;
		Token *tok = NULL;

		foreach (InterpMorf* pmorf, content) {
			if(pmorf->p != last_p || pmorf->k != last_k) {
				// create a new token
				std::string orth(pmorf->forma);
				UnicodeString uorth = UnicodeString::fromUTF8(orth);
				if(tok) preque.push_back(tok);

				tok = new Token(uorth, Toki::Whitespace::None);
				// if this is part-initial token, increase the variant number
				if (pmorf->p == this->beg) {
					++lastvarnum;
				}
				// mark this token as belonging to last variant (to be reverted to zero if one token only)
				// tok->variantNum = lastvarnum;
			} else {
				// keep old token, just append new lexems
				if (tok == NULL) {
					// should NEVER happen
					throw PlTaggerError("iternal error in morfeusz-compat interface");
				}
			}

			if(pmorf->interp == NULL) {
				// unrecognised => create an IGN lexem (lemma := orth)
				tok->make_ign(ma.tagset());
			} else {
				//try {
					UnicodeString lemma = UnicodeString::fromUTF8(pmorf->haslo);
					tok->set_orth(UnicodeString::fromUTF8(pmorf->forma));
					ma.parse_tag_into_token(tok, lemma, pmorf->interp);
				//}
			}
			last_p = pmorf->p;
			last_k = pmorf->k;
		}
		// flush last one
		if (tok) preque.push_back(tok);

		while(!preque.empty()) {
			out.push_back(preque.front());
			preque.pop_front();
		}
	}

	void Part::Dump()
	{
		std::cout << "Part <" << beg << "," << end << ">:\n";
		for(std::list<InterpMorf *>::iterator iter = content.begin(); iter != content.end(); ++iter)
		{
			std::cout << "\t" << (*iter)->p << "," << (*iter)->k << "," << (*iter)->forma << "\n";
		}
	}

} /* end ns MorfeuszCompat */ } /* end ns PlTagger */
