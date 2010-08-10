#ifndef LIBPLTAGGER_CHUNK_H
#define LIBPLTAGGER_CHUNK_H

#include <libpltagger/sentence.h>

namespace PlTagger {

	class Chunk
	{
	public:
		Chunk();

		template <typename T>
		explicit Chunk(const T& range);

		bool has_attribute(const std::string& name) const;

		std::string get_attribute(const std::string& name) const;

		void set_attribute(const std::string& name, const std::string& value);

		std::vector<Sentence*>& sentences() {
			return sentences_;
		}

		const std::vector<Sentence*>& sentences() const {
			return sentences_;
		}

		void append(Sentence* s) {
			sentences_.push_back(s);
		}

		typedef std::map<std::string, std::string> attr_map_t;

		const attr_map_t& attributes() const {
			return attributes_;
		}

	private:
		std::vector<Sentence*> sentences_;
		attr_map_t attributes_;
	};

	template <typename T>
	Chunk::Chunk(const T &range)
		: sentences_(), attributes_()
	{
		std::copy(range.begin(), range.end(), std::back_inserter(sentences_));
	}

} /* end ns PlTagger */

#endif // LIBPLTAGGER_CHUNK_H
