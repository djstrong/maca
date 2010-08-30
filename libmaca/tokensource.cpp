#include <vector>
#include <libmaca/token.h>

#include <libmaca/tokensource.h>

namespace Maca
{

	void foo()
	{
		std::vector<Token*> v;
		boost::iterator_range<std::vector<Token*>::const_iterator> r(v.begin(), v.end());
		TokenSource* ts = make_range_source(r);
		ts = make_range_source(v.begin(), v.end());
		ts = make_range_source(v);
	}

} /* end ns Maca */
