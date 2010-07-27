#include <libpltagger/morph/dispatchanalyser.h>
#include <libtoki/foreach.h>

namespace PlTagger {

	DispatchAnalyser::DispatchAnalyser(const Tagset* tagset)
		: MorphAnalyser(tagset), type_handlers_(), analysers_(), default_()
	{
	}

	DispatchAnalyser::~DispatchAnalyser()
	{
		foreach (MorphAnalyser* ma, analysers_) {
			delete ma;
		}
	}

	void DispatchAnalyser::add_type_handler(const std::string &type, MorphAnalyser *a)
	{
		if (a->tagset().id() != tagset().id()) throw TagsetMismatch("dispatch analyser handler", tagset(), a->tagset());
		analysers_.insert(a);
		type_handlers_[type].push_back(a);
	}

	bool DispatchAnalyser::process_functional(const Toki::Token &t, boost::function<void (Token*)> sink)
	{
		std::map<std::string, std::vector<MorphAnalyser*> >::const_iterator i;
		i = type_handlers_.find(t.type());
		const std::vector<MorphAnalyser*>* v = &default_;
		if (i != type_handlers_.end()) {
			v = &i->second;
		}
		foreach (MorphAnalyser* ma, *v) {
			if (ma->process_functional(t, sink)) {
				return true;
			}
		}
		return false;
	}

	void DispatchAnalyser::add_default_handler(MorphAnalyser* a)
	{
		default_.push_back(a);
	}

	size_t DispatchAnalyser::default_handlers_count() const
	{
		return default_.size();
	}

	size_t DispatchAnalyser::handler_count() const
	{
		return type_handlers_.size();
	}


} /* end ns PlTagger */
