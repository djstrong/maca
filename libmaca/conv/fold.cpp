#include <libmaca/conv/fold.h>
#include <libpwrutils/foreach.h>
#include <libcorpus2/token.h>

namespace Maca { namespace Conversion {

size_t find_shortest(const std::vector<std::vector<Token *> >& v,
		size_t& min_len_path)
{
	size_t min_len = v[0].size();
	min_len_path = 0;
	for (size_t pi = 0; pi < v.size(); ++pi) {
		if (v[pi].size() < min_len) {
			min_len = v[pi].size();
			min_len_path = pi;
		}
	}
	return min_len;
}

bool try_fold_paths(const std::vector< std::vector<Token*> >& v,
		boost::function<void (Token*)> sink)
{
	size_t min_len = v[0].size();
	size_t max_len = v[0].size();
	for (size_t pi = 0; pi < v.size(); ++pi) {
		if (v[pi].size() < min_len) {
			min_len = v[pi].size();
		} else if (v[pi].size() > max_len) {
			max_len = v[pi].size();
		}
	}
	// folding is only possible if the paths all have the same length
	if (max_len != min_len) return false;
	for (size_t ti = 0; ti < min_len; ++ti) {
		Token* t = v[0][ti];
		for (size_t pi = 1; pi < v.size(); ++pi) {
			// if orths vary, merging is not possible
			if (t->orth() != v[pi][ti]->orth()) return false;
		}
	}
	// actual merging
	for (size_t ti = 0; ti < min_len; ++ti) {
		Token* t = v[0][ti];
		for (size_t pi = 1; pi < v.size(); ++pi) {
			foreach (const Lexeme& lex, v[pi][ti]->lexemes()) {
				t->add_lexeme(lex);
			}
			delete v[pi][ti];
		}
		t->remove_duplicate_lexemes();
		sink(t);
	}
	return true;
}

std::vector<Token*> choose_path(
		const std::vector< std::vector<Token*> >& v, size_t n)
{
	assert(n < v.size());
	for (size_t i = 0; i < v.size(); ++i) {
		if (i != n) {
			foreach (Token* t, v[i]) {
				delete t;
			}
		}
	}
	return v[n];
}

void choose_path(const std::vector< std::vector<Token*> >& v, size_t n,
		boost::function<void (Token*)> sink)
{
	assert(n < v.size());
	for (size_t i = 0; i < v.size(); ++i) {
		if (i != n) {
			foreach (Token* t, v[i]) {
				delete t;
			}
		}
	}
	foreach (Token* t, v[n]) {
		sink(t);
	}
}

std::vector<Token*> choose_shortest_path(
		const std::vector< std::vector<Token*> >& v)
{
	size_t min_len_path;
	find_shortest(v, min_len_path);
	return choose_path(v, min_len_path);
}

void choose_shortest_path(const std::vector< std::vector<Token*> >& v,
		boost::function<void (Token*)> sink)
{
	size_t min_len_path;
	find_shortest(v, min_len_path);
	choose_path(v, min_len_path, sink);
}

} /* end ns Conversion */
} /* end ns Maca */
