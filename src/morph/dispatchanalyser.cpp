#include "dispatchanalyser.h"
#include <libtoki/foreach.h>

namespace PlTagger {

	DispatchAnalyser::DispatchAnalyser()
		: type_handlers_(), analysers_(), default_(NULL)
	{
	}

	DispatchAnalyser::~DispatchAnalyser()
	{
		foreach (MorphAnalyser* ma, analysers_) {
			delete ma;
		}
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
