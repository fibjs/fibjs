/* *****************************************************************************
** $Id$
** Initial file written and documented by:
** Kevin Shepherd <kshepherd@php.net> December 2007
** of Scarlet Line http://www.scarletline.com/
*******************************************************************************/
/** \file gd_io_stream.h
	\brief C++ standard library iostream specializations of gdIOCtx.
	
	Note that all of the methods defined in this header are internal to the
	libgd library, except for the constructors.
	Only the constructors are needed by a user of the libgd API.
	This file does not use or need gdpp.h, but if GD::Image is
	used, then C++ coding becomes even simpler, and the classes below 
	become entirely hidden implementation details.
	Example usage, convert png to gif:
	#include <fstream>
	#include "gd_io_stream.h"
	std::ifstream in("image.png", std::ios_base::in | std::ios_base::binary );
	if (in.good())
		{
		istreamIOCtx _in_ctx(in);
		gdImagePtr im_in = gdImageCreateFromPngCtx ( & _in_ctx);
		std::ofstream out("image.gif", std::ios_base::out | std::ios_base::binary );
		ostreamIOCtx _out_ctx(out);
		gdImageGifCtx(im_in, & _out_ctx);
		}
	gdImageDestroy(im_in);
*/
#ifdef __cplusplus
#ifndef _gd_io_stream_h
#define _gd_io_stream_h

#include "gd.h"
#include <iostream>

/** Standard library input stream specialization of gdIOCtx
*/
class BGD_EXPORT_DATA_IMPL istreamIOCtx : public gdIOCtx
	{
public:
	typedef std::istream	stream_type;
	/** Construct an instance of this input stream specialization,
		given an input stream.
		For example:
		std::ifstream in("image.png", std::ios_base::in | std::ios_base::binary );
		istreamIOCtx in_ctx(in);
	*/
	istreamIOCtx(stream_type & __stream)
		{ init( & __stream); }

	static int	Getbuf (struct gdIOCtx * ctx, void * buf, int size);
	static int	Putbuf (struct gdIOCtx * , const void * , int );
	static void	Putchar (struct gdIOCtx * , int );
	static int	Getchar (struct gdIOCtx * ctx);
	static int	Seek (struct gdIOCtx * ctx, const int pos);
	static long	Tell (struct gdIOCtx * ctx);
	static void	FreeCtx (struct gdIOCtx * ctx);

	void init(stream_type * __stream)
		{
		getC = Getchar;
		putC = Putchar;
		getBuf = Getbuf;
		putBuf = Putbuf;
		tell = Tell;
		seek = Seek;
		gd_free = FreeCtx;
		_M_stream = __stream;
		}
private:
	stream_type *	_M_stream;
	};
/** Allocate a new instance of the class
*/
inline gdIOCtx * gdNewIstreamCtx (std::istream * __stream)
	{
	return new istreamIOCtx(* __stream);
	}

/** Standard library output stream specialization of gdIOCtx
*/
class BGD_EXPORT_DATA_IMPL ostreamIOCtx : public gdIOCtx
	{
public:
	typedef std::ostream	stream_type;
	/** Construct an instance of this output stream specialization,
		given an output stream.
		For example:
		std::ofstream out("image.gif", std::ios_base::out | std::ios_base::binary );
		ostreamIOCtx out_ctx(out);
	*/
	ostreamIOCtx(stream_type & __stream)
		{ init( & __stream); }

	static int	Getbuf (struct gdIOCtx * , void * , int );
	static int	Putbuf (struct gdIOCtx * ctx, const void * buf, int size);
	static int	Getchar (struct gdIOCtx * );
	static void	Putchar (struct gdIOCtx * ctx, int a);
	static int	Seek (struct gdIOCtx * ctx, const int pos);
	static long	Tell (struct gdIOCtx * ctx);
	static void	FreeCtx (struct gdIOCtx * ctx);

	void init(stream_type * __stream)
		{
		getC = Getchar;
		putC = Putchar;
		getBuf = Getbuf;
		putBuf = Putbuf;
		tell = Tell;
		seek = Seek;
		gd_free = FreeCtx;
		_M_stream = __stream;
		}
private:
	stream_type *	_M_stream;
	};
/** Allocate a new instance of the class
*/
inline gdIOCtx * gdNewOstreamCtx (std::ostream * __stream)
	{
	return new ostreamIOCtx(* __stream);
	}

#endif /* _gd_io_stream_h */
#endif /* __cplusplus */
