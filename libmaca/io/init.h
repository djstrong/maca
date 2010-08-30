#ifndef LIBMACA_IO_INIT_H
#define LIBMACA_IO_INIT_H

namespace Maca {

	/**
	 * Classes derived from TokenWriter in the library that want to be avaliable via
	 * the factory should add relevant register calls in the implementation of this
	 * function. @see TokenWriter for details.
	 */
	bool init_token_writers();

} /* end ns Maca */

#endif // LIBMACA_IO_INIT_H
