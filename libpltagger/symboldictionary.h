#ifndef LIBPLTAGGER_SYMBOLDICTIONARY_H
#define LIBPLTAGGER_SYMBOLDICTIONARY_H

#include <vector>
#include <string>
#include <boost/range.hpp>
#include <map>

namespace PlTagger {

	template <typename IndexT>
	class SymbolDictionary
	{
	public:
		SymbolDictionary();

		void load_data(const std::vector<std::string>& data);

		void load_sorted_data(const std::vector<std::string>& data);

		bool is_id_valid(IndexT idx) const;

		size_t  size() const;

		IndexT get_id(const char* c) const;

		IndexT get_id(const boost::iterator_range<std::string::const_iterator>& r) const;

		const std::string& get_string(IndexT id) const;


		typedef typename std::vector<std::string>::iterator iterator;

		typedef typename std::vector<std::string>::const_iterator const_iterator;

		typedef typename std::vector<std::string>::value_type value_type;

		iterator begin() {
			return data_.begin();
		}

		const_iterator begin() const {
			return data_.begin();
		}

		iterator end() {
			return data_.end();
		}

		const_iterator end() const {
			return data_.end();
		}

		void create_mapping_to(const SymbolDictionary<IndexT>& other,
				std::map<IndexT, IndexT>& map) const;

	private:
		std::vector<std::string> data_;
		static std::string nullstr;
	};

	template <typename IndexT>
	std::string SymbolDictionary<IndexT>::nullstr;

	template <typename IndexT>
	SymbolDictionary<IndexT>::SymbolDictionary()
		: data_()
	{
	}

	template <typename IndexT>
	void SymbolDictionary<IndexT>::load_data(const std::vector<std::string> &data)
	{
		data_ = data;
		std::sort(data_.begin(), data_.end());
	}

	template <typename IndexT>
	void SymbolDictionary<IndexT>::load_sorted_data(const std::vector<std::string> &data)
	{
		data_ = data;
	}

	template <typename IndexT>
	bool SymbolDictionary<IndexT>::is_id_valid(IndexT idx) const
	{
		return static_cast<size_t>(idx) < data_.size();
	}

	template <typename IndexT>
	size_t SymbolDictionary<IndexT>::size() const
	{
		return data_.size();
	}

	template <typename IndexT>
	IndexT SymbolDictionary<IndexT>::get_id(const char *c) const
	{
		return get_id(std::make_pair(c, c + strlen(c)));
	}

		template <typename IndexT>
	IndexT SymbolDictionary<IndexT>::get_id(const boost::iterator_range<std::string::const_iterator>& r) const
	{
		//std::cerr << "get_id called with '" << r << "' (" << r.size() << ")\n";
		boost::sub_range< const std::vector<std::string> > sr =
				std::equal_range(data_.begin(), data_.end(), r);
		if (!sr.empty()) {
			return static_cast<IndexT>(sr.begin() - data_.begin());
		} else {
			return static_cast<IndexT>(data_.size());
		}
	}

	template <typename IndexT>
	const std::string& SymbolDictionary<IndexT>::get_string(IndexT id) const
	{
		size_t idx = static_cast<size_t>(id);
		if (id < data_.size()) {
			return data_[idx];
		} else {
			return nullstr;
		}
	}

	template <typename IndexT>
	void SymbolDictionary<IndexT>::create_mapping_to(const SymbolDictionary<IndexT> &other,
			std::map<IndexT, IndexT> &map) const
	{
		for (IndexT i = static_cast<IndexT>(0); i < size(); ++i) {
			std::string name = get_string(i);
			IndexT t = other.get_id(name);
			map.insert(std::make_pair(i, t));
		}
	}

} /* end ns PlTagger */

#endif // LIBPLTAGGER_SYMBOLDICTIONARY_H
