#include <libpltagger/morph/dispatchanalyser.h>
#include <libtoki/foreach.h>

namespace PlTagger {

	DispatchAnalyser::DispatchAnalyser(const Tagset* tagset)
		: MorphAnalyser(tagset), type_handlers_(), analysers_(), default_(NULL)
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
		type_handlers_.insert(std::make_pair(type, a));
	}

	void DispatchAnalyser::process_functional(const Toki::Token &t, boost::function<void (Token*)> sink)
	{
		std::map<std::string, MorphAnalyser*>::const_iterator i;
		i = type_handlers_.find(t.type());
		if (i != type_handlers_.end()) {
			i->second->process_functional(t, sink);
		} else {
			if (default_) {
				default_->process_functional(t, sink);
			}
		}
	}

	void DispatchAnalyser::set_default_handler(MorphAnalyser* a)
	{
		default_ = a;
	}

	MorphAnalyser* DispatchAnalyser::default_handler()
	{
		return default_;
	}

	size_t DispatchAnalyser::handler_count() const
	{
		return type_handlers_.size();
	}


} /* end ns PlTagger */
