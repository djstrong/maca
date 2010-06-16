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


} /* end ns PlTagger */
