#include "dispatchanalyser.h"
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

	Token* DispatchAnalyser::process(const Toki::Token &t)
	{
		std::map<std::string, MorphAnalyser*>::const_iterator i;
		i = type_handlers_.find(t.type());
		if (i != type_handlers_.end()) {
			return i->second->process(t);
		} else {
			if (default_) {
				return default_->process(t);
			} else {
				return NULL;
			}
		}
	}


} /* end ns PlTagger */
