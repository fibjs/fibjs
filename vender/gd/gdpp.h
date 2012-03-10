/* *****************************************************************************
** $Id$
** Initial file written and documented by:
** Kevin Shepherd <kshepherd@php.net> December 2007
** of Scarlet Line http://www.scarletline.com/
** with contributions from Torben Nielsen.
*******************************************************************************/
/** \file gdpp.h
	\brief Object Oriented C++ wrappers around libgd functionality.
	
	Example usage, convert png to gif:
	#include <fstream>
	#include <gdpp.h>

	std::ifstream in("image.png", std::ios_base::in | std::ios_base::binary );
	GD::Image im(in, GD::Png_tag());
	if (im.good())
		{
		std::ofstream out("image.gif", std::ios_base::out | std::ios_base::binary );
		im.Gif(out);
		}
*/
#ifdef __cplusplus
#ifndef _gdpp_h
#define _gdpp_h

#include "gd_io_stream.h"
#include <string>

/// namespace GD:: contains the C++ wrapper classes for libgd
/** This namespace is primarily to avoid name clashes, and to
	contain all of the gd classes within one namespace.
	It is not recommended to use the "using namespace" directive with this namespace.
	Example usage:
	GD::Image	im(64, 32, true); // Create a truecolor image 64 pixels wide by 32 pixels high
	GD::Point	pt(10, 8); // The point at x=10, y=8.
	GD::Size	sz(16, 8); // The size width=16, height=8. 
	GD::TrueColor	col(0xFF, 0, 0); // The colour red; R=255, G=0, B=0.
	im.Rectangle(pt, sz, col.Int()); // Draw a red rectangle with top left corner at pt, of size sz.
*/
namespace GD
	{
	/**	This class GD::Point stores a point in two dimensions, somewhere
		on the plane of an image.
	*/
	class Point
		{
	public:
		// Constructors
		Point(int x, int y)
		:_x(x), _y(y) {}
		Point(const Point & p)
		:_x(p._x), _y(p._y) {}
		Point()
		:_x(0), _y(0) {}
		Point & operator=(const Point & p)
			{
			_x = p._x;
			_y = p._y;
			return (* this);
			}
		// Accessors
		int X() const
			{ return _x; }
		int Y() const
			{ return _y; }
		// Updaters
		void X(int x)
			{ _x = x; }
		void Y(int y)
			{ _y = y; }
		void set(int x, int y)
			{ _x = x; _y = y; }
		int & lhsX()
			{ return _x; }
		int & lhsY()
			{ return _y; }
			
		gdPointPtr as_gdPointPtr()
			{ return (gdPointPtr) this; }
	protected:
		int	_x, _y;
		};
	typedef Point * PointPtr;
	/**	This class GD::Size stores length in two dimensions.
		Giving the size of an area as width and height.
	*/
	class Size
		{
	public:
		// Constructors
		Size(int w, int h)
		:_w(w), _h(h) {}
		Size(const Size & p)
		:_w(p._w), _h(p._h) {}
		Size()
		:_w(0), _h(0) {}
		Size & operator=(const Size & p)
			{
			_w = p._w;
			_h = p._h;
			return (* this);
			}
		// Accessors
		int W() const
			{ return _w; }
		int H() const
			{ return _h; }
		// Updaters
		void W(int w)
			{ _w = w; }
		void H(int h)
			{ _h = h; }
		void set(int w, int h)
			{ _w = w; _h = h; }
		int & lhsW()
			{ return _w; }
		int & lhsH()
			{ return _h; }
	protected:
		int	_w, _h;
		};
	typedef Size * SizePtr;
	
	/**	This class GD::TrueColor stores a colour as an RGBA quadruplet.
		It can also be read as an integer, and in other colour formats.
	*/
	class TrueColor
		{
	public:
		union as_types
			{
			int as_int;
			struct uchars
				{
				unsigned char blue, green, red, alpha;
				} as_uchar;
			};
		TrueColor()
			{ internal.as_int = 0; }
		TrueColor(int c)
			{ internal.as_int = c; }
		TrueColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0)
			{
			internal.as_uchar.alpha = a; 
			internal.as_uchar.red = r; 
			internal.as_uchar.green = g; 
			internal.as_uchar.blue = b; 
			}
		// Accessors
		int Int() const
			{ return internal.as_int; }
		unsigned char Red() const
			{ return internal.as_uchar.red; }
		unsigned char Green() const
			{ return internal.as_uchar.green; }
		unsigned char Blue() const
			{ return internal.as_uchar.blue; }
		unsigned char Alpha() const
			{ return internal.as_uchar.alpha; }
		// Updaters
		void set(int c)
			{ internal.as_int = c; }
		void set(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0)
			{
			internal.as_uchar.alpha = a; 
			internal.as_uchar.red = r; 
			internal.as_uchar.green = g; 
			internal.as_uchar.blue = b; 
			}
		void Red(unsigned char c)
			{ internal.as_uchar.red = c; }
		void Green(unsigned char c)
			{ internal.as_uchar.green = c; }
		void Blue(unsigned char c)
			{ internal.as_uchar.blue = c; }
		void Alpha(unsigned char c)
			{ internal.as_uchar.alpha = c; }
	protected:
		as_types internal;
		};
	/*	The following tags are simply empty structures which are used
		to tell the compiler which constructor we want when we know
		the image file format.
	*/
	struct Png_tag {};
	struct Gif_tag {};
	struct WBMP_tag {};
	struct Jpeg_tag {};
	struct Gd_tag {};
	struct Gd2_tag {};
	struct Xbm_tag {};

	/**	This class GD::Image wraps all of the 'C' libgd functionality
		for the convenience of C++ users.  An instance of this class
		corresponds to a single image.
	*/
	class BGD_EXPORT_DATA_IMPL Image
		{
	public:
		/**	Construct a null image
		*/
		Image() 
		:im(0)
			{}
		/** Construct a blank image, of the given size and colour format type.
			\param[in] sx Width of the image
			\param[in] sy Height of the image
			\param[in] istruecolor Create a true colour image, defaults to false, i.e. create an indexed palette image.
		*/
		Image(int sx, int sy, bool istruecolor = false) 
		:im(0)
			{
			if (istruecolor)
				CreateTrueColor(sx, sy);
			else
				Create(sx, sy);
			}
		/** Construct a blank image, of the given size and colour format type.
			\param[in] s Width and height of the image
			\param[in] istruecolor Create a true colour image, defaults to false, i.e. create an indexed palette image.
		*/
		Image(const Size & s, bool istruecolor = false) 
		:im(0)
			{
			if (istruecolor)
				CreateTrueColor(s);
			else
				Create(s);
			}
		/** Construct an instance of the GD::Image class, given the internal gdImage poimter.
			Note that gdImageDestroy will be called on the image pointer in the destructor.
			\param[in] i Pointer to the internal gdImage
		*/
		Image(gdImagePtr i) 
		:im(i) {}
		/** Copy constructor. Construct an instance of the GD::Image class, 
			by making a copy of the GD::Image provided.
			\param[in] i Reference to the image to be copied
		*/
		Image(const GD::Image & i) 
		:im(0)
			{
			Copy(i); 
			}
		/** Construct an image by reading from \p in.  This constructor
			will first attempt to determine the file format.
			\param[in] in The stream containing the image data
		*/
		Image(std::istream & in) 
		:im(0) { CreateFrom(in); }
		/** Construct an image by reading from \p in.  This constructor
			will first attempt to determine the file format.
			\param[in] in An opened FILE * handle to a file containing the image data
		*/
		Image(FILE * in) 
		:im(0) { CreateFrom(in); }
		/** Construct an image by reading from memory block \p data.  This constructor
			will first attempt to determine the image formatting.
			\param[in] size The byte count of the memory block
			\param[in] data Pointer to the memory block
		*/
		Image(int size, void * data) 
		:im(0) { CreateFrom(size, data); }
#if HAVE_PNG
		/** Construct an image by reading from \p in.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(input, GD::Png_tag()); // read a png file from input
			\param[in] in The stream containing the image data
		*/
		Image(std::istream & in, Png_tag) 
		:im(0) { CreateFromPng(in); }
		/** Construct an image by reading from \p in.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(input, GD::Png_tag()); // read a png file from input
			\param[in] in An opened FILE * handle to a file containing the image data
		*/
		Image(FILE * in, Png_tag) 
		:im(0) { CreateFromPng(in); }
		/** Construct an image by reading from \p in.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(input, GD::Png_tag()); // read a png file from input
			\param[in] in The io context from which to read the image data
		*/
		Image(gdIOCtx * in, Png_tag) 
		:im(0) { CreateFromPng(in); }
		/** Construct an image by reading from memory block \p data.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(sz, dat, GD::Png_tag()); // read a png file from dat
			\param[in] size The byte count of the memory block
			\param[in] data Pointer to the memory block
		*/
		Image(int size, void * data, Png_tag) 
		:im(0) { CreateFromPng(size, data); }
#endif

		/** Construct an image by reading from \p in.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(input, GD::Gif_tag()); // read a gif file from input
			\param[in] in The stream containing the image data
		*/
		Image(std::istream & in, Gif_tag) 
		:im(0) { CreateFromGif(in); }
		/** Construct an image by reading from \p in.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(input, GD::Gif_tag()); // read a gif file from input
			\param[in] in An opened FILE * handle to a file containing the image data
		*/
		Image(FILE * in, Gif_tag) 
		:im(0) { CreateFromGif(in); }
		/** Construct an image by reading from \p in.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(input, GD::Gif_tag()); // read a gif file from input
			\param[in] in The io context from which to read the image data
		*/
		Image(gdIOCtx * in, Gif_tag) 
		:im(0) { CreateFromGif(in); }
		/** Construct an image by reading from memory block \p data.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(sz, dat, GD::Gif_tag()); // read a gif file from dat
			\param[in] size The byte count of the memory block
			\param[in] data Pointer to the memory block
		*/
		Image(int size, void * data, Gif_tag) 
		:im(0) { CreateFromGif(size, data); }

		/** Construct an image by reading from \p in.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(input, GD::WBMP_tag()); // read a monchrome WBMP file from input
			\param[in] in The stream containing the image data
		*/
		Image(std::istream & in, WBMP_tag) 
		:im(0) { CreateFromWBMP(in); }
		/** Construct an image by reading from \p in.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(input, GD::WBMP_tag()); // read a monchrome WBMP file from input
			\param[in] in An opened FILE * handle to a file containing the image data
		*/
		Image(FILE * in, WBMP_tag) 
		:im(0) { CreateFromWBMP(in); }
		/** Construct an image by reading from \p in.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(input, GD::WBMP_tag()); // read a monchrome WBMP file from input
			\param[in] in The io context from which to read the image data
		*/
		Image(gdIOCtx * in, WBMP_tag) 
		:im(0) { CreateFromWBMP(in); }
		/** Construct an image by reading from memory block \p data.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(sz, dat, GD::WBMP_tag()); // read a monchrome WBMP file from dat
			\param[in] size The byte count of the memory block
			\param[in] data Pointer to the memory block
		*/
		Image(int size, void * data, WBMP_tag) 
		:im(0) { CreateFromWBMP(size, data); }

#if HAVE_JPEG
		/** Construct an image by reading from \p in.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(input, GD::Jpeg_tag()); // read a jpeg file from input
			\param[in] in The stream containing the image data
		*/
		Image(std::istream & in, Jpeg_tag) 
		:im(0) { CreateFromJpeg(in); }
		/** Construct an image by reading from \p in.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(input, GD::Jpeg_tag()); // read a jpeg file from input
			\param[in] in An opened FILE * handle to a file containing the image data
		*/
		Image(FILE * in, Jpeg_tag) 
		:im(0) { CreateFromJpeg(in); }
		/** Construct an image by reading from \p in.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(input, GD::Jpeg_tag()); // read a jpeg file from input
			\param[in] in The io context from which to read the image data
		*/
		Image(gdIOCtx * in, Jpeg_tag) 
		:im(0) { CreateFromJpeg(in); }
		/** Construct an image by reading from memory block \p data.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(sz, dat, GD::Jpeg_tag()); // read a jpeg file from dat
			\param[in] size The byte count of the memory block
			\param[in] data Pointer to the memory block
		*/
		Image(int size, void * data, Jpeg_tag) 
		:im(0) { CreateFromJpeg(size, data); }
#endif

		/** Construct an image by reading from \p in.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(input, GD::Gd_tag()); // read a gd file from input
			\param[in] in The stream containing the image data
		*/
		Image(std::istream & in, Gd_tag) 
		:im(0) { CreateFromGd(in); }
		/** Construct an image by reading from \p in.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(input, GD::Gd_tag()); // read a gd file from input
			\param[in] in An opened FILE * handle to a file containing the image data
		*/
		Image(FILE * in, Gd_tag) 
		:im(0) { CreateFromGd(in); }
		/** Construct an image by reading from \p in.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(input, GD::Gd_tag()); // read a gd file from input
			\param[in] in The io context from which to read the image data
		*/
		Image(gdIOCtx * in, Gd_tag) 
		:im(0) { CreateFromGd(in); }
		/** Construct an image by reading from memory block \p data.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(sz, dat, GD::Gd_tag()); // read a gd file from dat
			\param[in] size The byte count of the memory block
			\param[in] data Pointer to the memory block
		*/
		Image(int size, void * data, Gd_tag) 
		:im(0) { CreateFromGd(size, data); }

		/** Construct an image by reading from \p in.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(input, GD::Gd2_tag()); // read a gd2 file from input
			\param[in] in The stream containing the image data
		*/
		Image(std::istream & in, Gd2_tag) 
		:im(0) { CreateFromGd2(in); }
		/** Construct an image by reading from \p in.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(input, GD::Png_tag()); // read a png file from input
			\param[in] in An opened FILE * handle to a file containing the image data
		*/
		Image(FILE * in, Gd2_tag) 
		:im(0) { CreateFromGd2(in); }
		/** Construct an image by reading from \p in.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(input, GD::Gd2_tag()); // read a gd2 file from input
			\param[in] in The io context from which to read the image data
		*/
		Image(gdIOCtx * in, Gd2_tag) 
		:im(0) { CreateFromGd2(in); }
		/** Construct an image by reading from memory block \p data.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(sz, dat, GD::Gd2_tag()); // read a gd2 file from dat
			\param[in] size The byte count of the memory block
			\param[in] data Pointer to the memory block
		*/
		Image(int size, void * data, Gd2_tag) 
		:im(0) { CreateFromGd2(size, data); }

		/** Construct an image by reading from \p in.
			The tag is an empty struct which simply tells the compiler which image read function to use.
			e.g. GD::Image img(input, GD::Xbm_tag()); // read an xbm file from input
			\param[in] in An opened FILE * handle to a file containing the image data
		*/
		Image(FILE * in, Xbm_tag) 
		:im(0) { CreateFromXbm(in); }

		~Image()
			{ clear(); }

		/** Assignment Operator. Make this a copy of the GD::Image provided.
			\param[in] src Reference to the image to be copied
		*/
		GD::Image & operator=(const GD::Image & src)
			{
			Copy(src);
			return (* this);
			}
		/** Make this an exact copy of the GD::Image provided.  Any existing iamge data is discarded.
			\param[in] src Reference to the image to be copied
		*/
		void Copy(const GD::Image & src)
			{
			int w = src.Width(), h = src.Height();
			if (src.IsTrueColor())
				CreateTrueColor(w, h);
			else
				{
				Create(w, h);
				PaletteCopy(src);
				}
			Copy(src, 0, 0, 0, 0, w, h);
			}
		/** Check to see if this appears to be a valid image
		*/	
		bool good() const
			{	return (im != 0); }
		// Creation:
		/**
			Create a palette-based image, with no more than 256 colors.
			\param sx Width of the desired image 
			\param sy Height of the desired image 
			\return true if it worked, else false
		*/
		bool Create(int sx, int sy)
			{
			clear();
			return ((im = gdImageCreate(sx, sy)) != 0);
			}
		/**
			Create a truecolor image.
			\param sx Width of the desired image 
			\param sy Height of the desired image 
			\return true if it worked, else false
		*/
		bool CreateTrueColor(int sx, int sy)
			{
			clear();
			return ((im = gdImageCreateTrueColor(sx, sy)) != 0);
			}
		/**
			Create a palette-based image, with no more than 256 colors.
			\param s Width and height of the desired image 
			\return true if it worked, else false
		*/
		bool Create(const Size & s)
			{ return Create(s.W(), s.H()); }
		/**
			Create a truecolor image.
			\param s Width and height of the desired image 
			\return true if it worked, else false
		*/
		bool CreateTrueColor(const Size & s)
			{ return CreateTrueColor(s.W(), s.H()); }
		// Create, determining the image format from the data
		/// Read an image from an open FILE * handle, after determining the image format
		bool CreateFrom(FILE * in);
		/// Read an image from an open standard library input stream, after determining the image format
		bool CreateFrom(std::istream & in);
		/// Read an image from a memory block, after determining the image format
		bool CreateFrom(int size, void * data);

#if HAVE_PNG
		// Png
		bool CreateFromPng(FILE * in)
			{
			clear();
			return ((im = gdImageCreateFromPng(in)) != 0);
			}
		bool CreateFromPng(gdIOCtx * in)
			{
			clear();
			return ((im = gdImageCreateFromPngCtx(in)) != 0);
			}
		bool CreateFromPng(int size, void * data)
			{
			clear();
			return ((im = gdImageCreateFromPngPtr(size, data)) != 0);
			}
		bool CreateFromPng(std::istream & in)
			{
			clear();
			istreamIOCtx _in_ctx(in);
			return ((im = gdImageCreateFromPngCtx( & _in_ctx)) != 0);
			}
#endif

		// Gif
		bool CreateFromGif(FILE * in)
			{
			clear();
			return ((im = gdImageCreateFromGif(in)) != 0);
			}
		bool CreateFromGif(gdIOCtx * in)
			{
			clear();
			return ((im = gdImageCreateFromGifCtx(in)) != 0);
			}
		bool CreateFromGif(int size, void * data)
			{
			clear();
			return ((im = gdImageCreateFromGifPtr(size, data)) != 0);
			}
		bool CreateFromGif(std::istream & in)
			{
			clear();
			istreamIOCtx _in_ctx(in);
			return ((im = gdImageCreateFromGifCtx( & _in_ctx)) != 0);
			}
		// WBMP
		bool CreateFromWBMP(FILE * in)
			{
			clear();
			return ((im = gdImageCreateFromWBMP(in)) != 0);
			}
		bool CreateFromWBMP(gdIOCtx * in)
			{
			clear();
			return ((im = gdImageCreateFromWBMPCtx(in)) != 0);
			}
		bool CreateFromWBMP(int size, void * data)
			{
			clear();
			return ((im = gdImageCreateFromWBMPPtr(size, data)) != 0);
			}
		bool CreateFromWBMP(std::istream & in)
			{
			clear();
			istreamIOCtx _in_ctx(in);
			return ((im = gdImageCreateFromWBMPCtx( & _in_ctx)) != 0);
			}

#if HAVE_JPEG
		// Jpeg
		/**
			Load a truecolor image from a JPEG format file. 
			Invoke CreateFromJpeg with an already opened
			pointer to a file containing the desired image. 
			CreateFromJpeg does not close the file.
			\return true for success, or false if unable to load the image (most often because 
			the file is corrupt or does not contain a JPEG image). 
			You can call Width() and Height() member functions of the image to determine its
			size. The returned image is always a truecolor image.
		*/
		bool CreateFromJpeg(FILE * in)
			{
			clear();
			return ((im = gdImageCreateFromJpeg(in)) != 0);
			}
		/**
			Load a truecolor image from a JPEG format file. 
			Invoke CreateFromJpeg with an already opened
			pointer to a file containing the desired image. 
			CreateFromJpeg does not close the file.
			\return true for success, or false if unable to load the image (most often because the file is corrupt or does not contain a JPEG image). 
			You can call Width() and Height() member functions of the image to determine its
			size. The returned image is always a truecolor image.
		*/
		bool CreateFromJpeg(gdIOCtx * in)
			{
			clear();
			return ((im = gdImageCreateFromJpegCtx(in)) != 0);
			}
		/**
			Load a truecolor image from a JPEG format file. 
			Invoke CreateFromJpeg with an already opened
			pointer to a file containing the desired image. 
			CreateFromJpeg does not close the file.
			\return true for success, or false if unable to load the image (most often because the file is corrupt or does not contain a JPEG image). 
			You can call Width() and Height() member functions of the image to determine its
			size. The returned image is always a truecolor image.
		*/
		bool CreateFromJpeg(int size, void * data)
			{
			clear();
			return ((im = gdImageCreateFromJpegPtr(size, data)) != 0);
			}
		/**
			Load a truecolor image from a JPEG format file. 
			Invoke CreateFromJpeg with an image file in memory.
			\return true for success, or false if unable to load the image (most often because the format is corrupt or does not contain a JPEG image). 
			You can call Width() and Height() member functions of the image to determine its
			size. The returned image is always a truecolor image.
		*/
		bool CreateFromJpeg(std::istream & in)
			{
			clear();
			istreamIOCtx _in_ctx(in);
			return ((im = gdImageCreateFromJpegCtx( & _in_ctx)) != 0);
			}
#endif

		// Gd
		bool CreateFromGd(FILE * in)
			{
			clear();
			return ((im = gdImageCreateFromGd(in)) != 0);
			}
		bool CreateFromGd(gdIOCtx * in)
			{
			clear();
			return ((im = gdImageCreateFromGdCtx(in)) != 0);
			}
		bool CreateFromGd(int size, void * data)
			{
			clear();
			return ((im = gdImageCreateFromGdPtr(size, data)) != 0);
			}
		bool CreateFromGd(std::istream & in)
			{
			clear();
			istreamIOCtx _in_ctx(in);
			return ((im = gdImageCreateFromGdCtx( & _in_ctx)) != 0);
			}
		// Gd2
		bool CreateFromGd2(FILE * in)
			{
			clear();
			return ((im = gdImageCreateFromGd2(in)) != 0);
			}
		bool CreateFromGd2(gdIOCtx * in)
			{
			clear();
			return ((im = gdImageCreateFromGd2Ctx(in)) != 0);
			}
		bool CreateFromGd2(int size, void * data)
			{
			clear();
			return ((im = gdImageCreateFromGd2Ptr(size, data)) != 0);
			}
		bool CreateFromGd2(std::istream & in)
			{
			clear();
			istreamIOCtx _in_ctx(in);
			return ((im = gdImageCreateFromGd2Ctx( & _in_ctx)) != 0);
			}
		// Gd2 Part
		bool CreateFromGd2Part(FILE * in, int srcx, int srcy, int w, int h)
			{
			clear();
			return ((im = gdImageCreateFromGd2Part(in, srcx, srcy, w, h)) != 0);
			}
		bool CreateFromGd2Part(gdIOCtx * in, int srcx, int srcy, int w, int h)
			{
			clear();
			return ((im = gdImageCreateFromGd2PartCtx(in, srcx, srcy, w, h)) != 0);
			}
		bool CreateFromGd2Part(int size, void * data, int srcx, int srcy, int w, int h)
			{
			clear();
			return ((im = gdImageCreateFromGd2PartPtr(size, data, srcx, srcy, w, h)) != 0);
			}
		bool CreateFromGd2Part(std::istream & in, int srcx, int srcy, int w, int h)
			{
			clear();
			istreamIOCtx _in_ctx(in);
			return ((im = gdImageCreateFromGd2PartCtx( & _in_ctx, srcx, srcy, w, h)) != 0);
			}
		bool CreateFromGd2Part(FILE * in, const Point & src, const Size & s)
			{ return CreateFromGd2Part(in, src.X(), src.Y(), s.W(), s.H()); }
		bool CreateFromGd2Part(gdIOCtx * in, const Point & src, const Size & s)
			{ return CreateFromGd2Part(in, src.X(), src.Y(), s.W(), s.H()); }
		bool CreateFromGd2Part(int size, void * data, const Point & src, const Size & s)
			{ return CreateFromGd2Part(size, data, src.X(), src.Y(), s.W(), s.H()); }
		bool CreateFromGd2Part(std::istream & in, const Point & src, const Size & s)
			{ return CreateFromGd2Part(in, src.X(), src.Y(), s.W(), s.H()); }
		// Xbm
		bool CreateFromXbm(FILE * in)
			{
			clear();
			return ((im = gdImageCreateFromXbm(in)) != 0);
			}
		// Xpm
		bool CreateFromXpm(char * filename)
			{
			clear();
			return ((im = gdImageCreateFromXpm(filename)) != 0);
			}
		bool CreateFromXpm(std::string & filename)
			{ return CreateFromXpm((char *)(filename.c_str())); }

		// Accessors, Updaters & Methods:
		void SetPixel(int x, int y, int color)
			{  gdImageSetPixel(im, x, y, color); }
		void SetPixel(const Point & p, int color)
			{  SetPixel(p.X(), p.Y(), color); }
		int GetPixel(int x, int y) const
			{  return gdImageGetPixel(im, x, y); }
		int GetPixel(const Point & p) const
			{  return GetPixel(p.X(), p.Y()); }
		int GetTrueColorPixel(int x, int y) const
			{  return gdImageGetTrueColorPixel(im, x, y); }
		int GetTrueColorPixel(const Point & p) const
			{  return GetTrueColorPixel(p.X(), p.Y()); }

		void SetPixel(int x, int y, TrueColor c)
			{  SetPixel(x, y, c.Int()); }
		void SetPixel(const Point & p, TrueColor c)
			{  SetPixel(p.X(), p.Y(), c.Int()); }
		void GetTrueColorPixel(TrueColor & c, int x, int y) const
			{  c.set(GetTrueColorPixel(x, y)); }
		void GetTrueColorPixel(TrueColor & c, const Point & p) const
			{  c.set(GetTrueColorPixel(p.X(), p.Y())); }

		void AABlend()
			{  gdImageAABlend(im); }

		void Line(int x1, int y1, int x2, int y2, int color)
			{  gdImageLine(im, x1, y1, x2, y2, color); }
		void Line(const Point & p1, const Point & p2, int color)
			{  Line(p1.X(), p1.Y(), p2.X(), p2.Y(), color); }
		void Rectangle(int x1, int y1, int x2, int y2, int color)
			{  gdImageRectangle(im, x1, y1, x2, y2, color); }
		void Rectangle(const Point & p1, const Point & p2, int color)
			{  Rectangle(p1.X(), p1.Y(), p2.X(), p2.Y(), color); }
		void Rectangle(const Point & p, const Size & s, int color)
			{  Rectangle(p.X(), p.Y(), p.X() + s.W(), p.Y() + s.H(), color); }
		void FilledRectangle(int x1, int y1, int x2, int y2, int color)
			{  gdImageFilledRectangle(im, x1, y1, x2, y2, color); }
		void FilledRectangle(const Point & p1, const Point & p2, int color)
			{  FilledRectangle(p1.X(), p1.Y(), p2.X(), p2.Y(), color); }
		void FilledRectangle(const Point & p, const Size & s, int color)
			{  FilledRectangle(p.X(), p.Y(), p.X() + s.W(), p.Y() + s.H(), color); }

		void SetClip(int x1, int y1, int x2, int y2)
			{  gdImageSetClip(im, x1, y1, x2, y2); }
		void SetClip(const Point & p1, const Point & p2)
			{  SetClip(p1.X(), p1.Y(), p2.X(), p2.Y()); }
		void SetClip(const Point & p, const Size & s)
			{  SetClip(p.X(), p.Y(), p.X() + s.W(), p.Y() + s.H()); }
		void GetClip(int & x1, int & y1, int & x2, int & y2) const
			{  gdImageGetClip(im, & x1, & y1, & x2, & y2); }
		void GetClip(Point & p1, Point & p2) const
			{  GetClip(p1.lhsX(), p1.lhsY(), p2.lhsX(), p2.lhsY()); }
		void GetClip(Point & p, Size & s) const
			{
			Point p2;
			GetClip(p.lhsX(), p.lhsY(), p2.lhsX(), p2.lhsY());
			s.set(p2.X() - p.X(), p2.Y() - p.Y());
			}

		bool BoundsSafe(int x, int y) const
			{  return (gdImageBoundsSafe(im, x, y)?true:false); }
		bool BoundsSafe(const Point & p) const
			{  return BoundsSafe(p.X(), p.Y()); }

		void Char(gdFontPtr f, int x, int y, int c, int color)
			{  gdImageChar(im, f, x, y, c, color); }
		void CharUp(gdFontPtr f, int x, int y, int c, int color)
			{  gdImageCharUp(im, f, x, y, c, color); }

		void Char(gdFontPtr f, const Point & p, int c, int color)
			{  Char(f, p.X(), p.Y(), c, color); }
		void CharUp(gdFontPtr f, const Point & p, int c, int color)
			{  CharUp(f, p.X(), p.Y(), c, color); }
			
		void String(gdFontPtr f, int x, int y, unsigned char * s, int color)
			{  gdImageString(im, f, x, y, (unsigned char *)s, color); }
		void StringUp(gdFontPtr f, int x, int y, unsigned char * s, int color)
			{  gdImageStringUp(im, f, x, y, (unsigned char *)s, color); }
		void String(gdFontPtr f, int x, int y, unsigned short * s, int color)
			{  gdImageString16(im, f, x, y, (unsigned short *)s, color); }
		void StringUp(gdFontPtr f, int x, int y, unsigned short * s, int color)
			{  gdImageStringUp16(im, f, x, y, (unsigned short *)s, color); }
		void String(gdFontPtr f, int x, int y, char * s, int color)
			{  gdImageString(im, f, x, y, (unsigned char *)s, color); }
		void StringUp(gdFontPtr f, int x, int y, char * s, int color)
			{  gdImageStringUp(im, f, x, y, (unsigned char *)s, color); }
		void String(gdFontPtr f, int x, int y, const std::string & s, int color)
			{  String(f, x, y, (char *)s.c_str(), color); }
		void StringUp(gdFontPtr f, int x, int y, const std::string & s, int color)
			{  StringUp(f, x, y, (char *)s.c_str(), color); }

		void String(gdFontPtr f, const Point & p, unsigned char * s, int color)
			{  String(f, p.X(), p.Y(), (unsigned char *)s, color); }
		void StringUp(gdFontPtr f, const Point & p, unsigned char * s, int color)
			{  StringUp(f, p.X(), p.Y(), (unsigned char *)s, color); }
		void String(gdFontPtr f, const Point & p, unsigned short * s, int color)
			{  String(f, p.X(), p.Y(), (unsigned short *)s, color); }
		void StringUp(gdFontPtr f, const Point & p, unsigned short * s, int color)
			{  StringUp(f, p.X(), p.Y(), (unsigned short *)s, color); }
		void String(gdFontPtr f, const Point & p, char * s, int color)
			{  String(f, p.X(), p.Y(), (unsigned char *)s, color); }
		void StringUp(gdFontPtr f, const Point & p, char * s, int color)
			{  StringUp(f, p.X(), p.Y(), (unsigned char *)s, color); }
		void String(gdFontPtr f, const Point & p, const std::string & s, int color)
			{  String(f, p, (char *)s.c_str(), color); }
		void StringUp(gdFontPtr f, const Point & p, const std::string & s, int color)
			{  StringUp(f, p, (char *)s.c_str(), color); }

		char * StringFT(int * brect, int fg, char * fontlist, double ptsize, double angle, 
				int x, int y, char * string)
			{  return gdImageStringFT(im, brect, fg, fontlist, ptsize, angle, x, y, string); }
		char * StringFT(int * brect, int fg, char * fontlist, double ptsize, double angle, 
				int x, int y, char * string, gdFTStringExtraPtr strex)
			{  return gdImageStringFTEx(im, brect, fg, fontlist, ptsize, angle, x, y, string, strex); }
		char * StringFT(int * brect, int fg, char * fontlist, double ptsize, double angle, 
				int x, int y, const std::string & string)
			{  return StringFT(brect, fg, fontlist, ptsize, angle, x, y, (char *)string.c_str()); }
		char * StringFT(int * brect, int fg, char * fontlist, double ptsize, double angle, 
				int x, int y, const std::string & string, gdFTStringExtraPtr strex)
			{  return StringFT(brect, fg, fontlist, ptsize, angle, x, y, (char *)string.c_str(), strex); }

		char * StringFT(int * brect, int fg, char * fontlist, double ptsize, double angle, 
				const Point & p, char * string)
			{  return StringFT(brect, fg, fontlist, ptsize, angle, p.X(), p.Y(), string); }
		char * StringFT(int * brect, int fg, char * fontlist, double ptsize, double angle, 
				const Point & p, char * string, gdFTStringExtraPtr strex)
			{  return StringFT(brect, fg, fontlist, ptsize, angle, p.X(), p.Y(), string, strex); }
		char * StringFT(int * brect, int fg, char * fontlist, double ptsize, double angle, 
				const Point & p, const std::string & string)
			{  return StringFT(brect, fg, fontlist, ptsize, angle, p, (char *)string.c_str()); }
		char * StringFT(int * brect, int fg, char * fontlist, double ptsize, double angle, 
				const Point & p, const std::string & string, gdFTStringExtraPtr strex)
			{  return StringFT(brect, fg, fontlist, ptsize, angle, p, (char *)string.c_str(), strex); }

		void Polygon(gdPointPtr p, int n, int c)
			{  gdImagePolygon(im, p, n, c); }
		void OpenPolygon(gdPointPtr p, int n, int c)
			{  gdImageOpenPolygon(im, p, n, c); }
		void FilledPolygon(gdPointPtr p, int n, int c)
			{  gdImageFilledPolygon(im, p, n, c); }

		void Polygon(PointPtr p, int n, int c)
			{  Polygon(p->as_gdPointPtr(), n, c); }
		void OpenPolygon(PointPtr p, int n, int c)
			{  OpenPolygon(p->as_gdPointPtr(), n, c); }
		void FilledPolygon(PointPtr p, int n, int c)
			{  FilledPolygon(p->as_gdPointPtr(), n, c); }

		int ColorAllocate(int r, int g, int b)
			{  return gdImageColorAllocate(im, r, g, b); }
		int ColorAllocate(int r, int g, int b, int a)
			{  return gdImageColorAllocateAlpha(im, r, g, b, a); }

		int ColorClosest(int r, int g, int b) const
			{  return gdImageColorClosest(im, r, g, b); }
		int ColorClosest(int r, int g, int b, int a) const
			{  return gdImageColorClosestAlpha(im, r, g, b, a); }
		int ColorClosestHWB(int r, int g, int b) const
			{  return gdImageColorClosestHWB(im, r, g, b); }
		int ColorExact(int r, int g, int b) const
			{  return gdImageColorExact(im, r, g, b); }
		int ColorExact(int r, int g, int b, int a) const
			{  return gdImageColorExactAlpha(im, r, g, b, a); }
		int ColorResolve(int r, int g, int b)
			{  return gdImageColorResolve(im, r, g, b); }
		int ColorResolve(int r, int g, int b, int a)
			{  return gdImageColorResolveAlpha(im, r, g, b, a); }

		void ColorDeallocate(int color)
			{  gdImageColorDeallocate(im, color); }

		void TrueColorToPalette(int ditherFlag, int colorsWanted)
			{  gdImageTrueColorToPalette(im, ditherFlag, colorsWanted); }

		void ColorTransparent(int color)
			{  gdImageColorTransparent(im, color); }

		void PaletteCopy(gdImagePtr src)
			{  gdImagePaletteCopy(im, src); }
		void PaletteCopy(const GD::Image & src)
			{  PaletteCopy(src.im); }

		/**
			Write out this image in GIF file format to \p out.
			\param out A FILE * handle 
		*/
		void Gif(FILE * out) const
			{ gdImageGif(im, out); }
		/**
			Write out this image in GIF file format to \p out.
			\param out A gdIOCtx * handle 
		*/
		void Gif(gdIOCtx * out) const
			{ gdImageGifCtx(im, out); }
		/**
			Allocate sufficient memory, and write this image, in GIF file format, to that memory.
			\param size A pointer for the allocated memory
			\return A pointer to the allocated memory, containing the image GIF file formatted.  Caller is responsible for freeing with gdFree(). 
		*/
		void * Gif(int * size) const
			{ return gdImageGifPtr(im, size); }
		/**
			Write out this image in GIF file format to \p out.
			\param out An output stream, already opened. 
		*/
		void Gif(std::ostream & out) const
			{ 
			ostreamIOCtx _out_ctx(out);
			gdImageGifCtx(im, & _out_ctx); 
			}

#if HAVE_PNG
		/**
			Write out this image in PNG file format to \p out.
			\param out A FILE * handle 
		*/
		void Png(FILE * out) const
			{ gdImagePng(im, out); }
		/**
			Write out this image in PNG file format to \p out.
			\param out A gdIOCtx * handle 
		*/
		void Png(gdIOCtx * out) const
			{ gdImagePngCtx(im, out); }
		/**
			Allocate sufficient memory, and write this image, in PNG file format, to that memory.
			\param size A pointer for the allocated memory
			\return A pointer to the allocated memory, containing the image PNG file formatted.  Caller is responsible for freeing with gdFree(). 
		*/
		void * Png(int * size) const
			{ return gdImagePngPtr(im, size); }
		/**
			Write out this image in PNG file format to \p out.
			\param out An output stream, already opened. 
		*/
		void Png(std::ostream & out) const
			{ 
			ostreamIOCtx _out_ctx(out);
			gdImagePngCtx(im, & _out_ctx); 
			}
		/**
			Write out this image in PNG file format to \p out.
			\param out A FILE * handle 
			\param level The level of compression: 0 == "no compression", 1 == "compressed as quickly as possible" --> 9 == "compressed as much as possible", -1 == zlib default compression level
		*/
		void Png(FILE * out, int level) const
			{ gdImagePngEx(im, out, level); }
		/**
			Write out this image in PNG file format to \p out.
			\param out A gdIOCtx * handle 
			\param level The level of compression: 0 == "no compression", 1 == "compressed as quickly as possible" --> 9 == "compressed as much as possible", -1 == zlib default compression level
		*/
		void Png(gdIOCtx * out, int level) const
			{ gdImagePngCtxEx(im, out, level); }
		/**
			Allocate sufficient memory, and write this image, in PNG file format, to that memory.
			\param size A pointer for the allocated memory
			\param level The level of compression: 0 == "no compression", 1 == "compressed as quickly as possible" --> 9 == "compressed as much as possible", -1 == zlib default compression level
			\return A pointer to the allocated memory, containing the image PNG file formatted.  Caller is responsible for freeing with gdFree(). 
		*/
		void * Png(int * size, int level) const
			{ return gdImagePngPtrEx(im, size, level); }
		/**
			Write out this image in PNG file format to \p out.
			\param out An output stream, already opened. 
			\param level The level of compression: 0 == "no compression", 1 == "compressed as quickly as possible" --> 9 == "compressed as much as possible", -1 == zlib default compression level
		*/
		void Png(std::ostream & out, int level) const
			{ 
			ostreamIOCtx _out_ctx(out);
			gdImagePngCtxEx(im, & _out_ctx, level); 
			}
#endif

		/**
			Write out this image in WBMP file format ( black and white only ) to \p out.
			\param fg The color index of the foreground. All other pixels considered background.
			\param out A FILE * handle 
		*/
		void WBMP(int fg, FILE * out) const
			{ gdImageWBMP(im, fg, out); }
		/**
			Write out this image in WBMP file format ( black and white only ) to \p out.
			\param fg The color index of the foreground. All other pixels considered background.
			\param out A gdIOCtx * handle 
		*/
		void WBMP(int fg, gdIOCtx * out) const
			{ gdImageWBMPCtx(im, fg, out); }
		/**
			Allocate sufficient memory, and write this image, in WBMP file format ( black and white only ), to that memory.
			\param size A pointer for the allocated memory
			\param fg The color index of the foreground. All other pixels considered background.
			\return A pointer to the allocated memory, containing the image WBMP file formatted.  Caller is responsible for freeing with gdFree(). 
		*/
		void * WBMP(int * size, int fg) const
			{ return gdImageWBMPPtr(im, size, fg); }
		/**
			Write out this image in WBMP file format ( black and white only ) to \p out.
			\param fg The color index of the foreground. All other pixels considered background.
			\param out An output stream, already opened. 
		*/
		void WBMP(int fg, std::ostream & out) const
			{ 
			ostreamIOCtx _out_ctx(out);
			gdImageWBMPCtx(im, fg, & _out_ctx); 
			}

#if HAVE_JPEG
		/**
			Write out this image in JPEG file format to \p out.
			\param out A FILE * handle
			\param quality Should be a value in the range 0-95, higher numbers imply both higher quality and larger image size.  Default value is -1, indicating "use a sensible default value". 
		*/
		void Jpeg(FILE * out, int quality = -1) const
			{ gdImageJpeg(im, out, quality); }
		/**
			Write out this image in JPEG file format to \p out.
			\param out A gdIOCtx * handle 
			\param quality Should be a value in the range 0-95, higher numbers imply both higher quality and larger image size.  Default value is -1, indicating "use a sensible default value". 
		*/
		void Jpeg(gdIOCtx * out, int quality = -1) const
			{ gdImageJpegCtx(im, out, quality); }
		/**
			Allocate sufficient memory, and write this image, in JPEG file format, to that memory.
			\param size A pointer for the allocated memory
			\param quality Should be a value in the range 0-95, higher numbers imply both higher quality and larger image size.  Default value is -1, indicating "use a sensible default value". 
			\return A pointer to the allocated memory, containing the image JPEG file formatted.  Caller is responsible for freeing with gdFree(). 
		*/
		void * Jpeg(int * size, int quality = -1) const
			{ return gdImageJpegPtr(im, size, quality); }
		/**
			Write out this image in JPEG file format to \p out.
			\param out An output stream, already opened. 
			\param quality Should be a value in the range 0-95, higher numbers imply both higher quality and larger image size.  Default value is -1, indicating "use a sensible default value". 
		*/
		void Jpeg(std::ostream & out, int quality = -1) const
			{ 
			ostreamIOCtx _out_ctx(out);
			gdImageJpegCtx(im, & _out_ctx, quality); 
			}
#endif
			
		void GifAnimBegin(FILE * out, int GlobalCM, int Loops) const
			{ gdImageGifAnimBegin(im, out, GlobalCM, Loops); }
		void GifAnimAdd(FILE * out, int LocalCM, int LeftOfs, int TopOfs, int Delay, int Disposal, gdImagePtr previm) const
			{ gdImageGifAnimAdd(im, out, LocalCM, LeftOfs, TopOfs, Delay, Disposal, previm); }
		void GifAnimAdd(FILE * out, int LocalCM, int LeftOfs, int TopOfs, int Delay, int Disposal, const GD::Image & previm) const
			{ GifAnimAdd(out, LocalCM, LeftOfs, TopOfs, Delay, Disposal, previm.im); }
		inline static void GifAnimEnd(FILE * out)
			{ gdImageGifAnimEnd(out); }
		void GifAnimBegin(gdIOCtx * out, int GlobalCM, int Loops) const
			{ gdImageGifAnimBeginCtx(im, out, GlobalCM, Loops); }
		void GifAnimAdd(gdIOCtx * out, int LocalCM, int LeftOfs, int TopOfs, int Delay, int Disposal, gdImagePtr previm) const
			{ gdImageGifAnimAddCtx(im, out, LocalCM, LeftOfs, TopOfs, Delay, Disposal, previm); }
		void GifAnimAdd(gdIOCtx * out, int LocalCM, int LeftOfs, int TopOfs, int Delay, int Disposal, const GD::Image & previm) const
			{ GifAnimAdd(out, LocalCM, LeftOfs, TopOfs, Delay, Disposal, previm.im); }
		inline static void GifAnimEnd(gdIOCtx * out)
			{ gdImageGifAnimEndCtx(out); }
		void * GifAnimBegin(int * size, int GlobalCM, int Loops) const
			{ return gdImageGifAnimBeginPtr(im, size, GlobalCM, Loops); }
		void * GifAnimAdd(int * size, int LocalCM, int LeftOfs, int TopOfs, int Delay, int Disposal, gdImagePtr previm) const
			{ return gdImageGifAnimAddPtr(im, size, LocalCM, LeftOfs, TopOfs, Delay, Disposal, previm); }
		void * GifAnimAdd(int * size, int LocalCM, int LeftOfs, int TopOfs, int Delay, int Disposal, const GD::Image & previm) const
			{ return GifAnimAdd(size, LocalCM, LeftOfs, TopOfs, Delay, Disposal, previm.im); }
		inline static void * GifAnimEnd(int * size)
			{ return gdImageGifAnimEndPtr(size); }

		void Gd(FILE * out) const
			{ gdImageGd(im, out); }
		void Gd(int * size) const
			{ gdImageGdPtr(im, size); }
		void Gd2(FILE * out, int cs, int fmt) const
			{ gdImageGd2(im, out, cs, fmt); }
		void Gd2(int cs, int fmt, int * size) const
			{ gdImageGd2Ptr(im, cs, fmt, size); }

		void Ellipse(int cx, int cy, int w, int h, int color)
			{ gdImageEllipse(im, cx, cy, w, h, color); }
		/**
			Draw a partial ellipse centered at the given point, with the specified width and height in pixels.
		*/
		void FilledArc(int cx, int cy, int w, int h, int s, int e, int color, int style)
			{ gdImageFilledArc(im, cx, cy, w, h, s, e, color, style); }
		void Arc(int cx, int cy, int w, int h, int s, int e, int color)
			{ gdImageArc(im, cx, cy, w, h, s, e, color); }
		void FilledEllipse(int cx, int cy, int w, int h, int color)
			{ gdImageFilledEllipse(im, cx, cy, w, h, color); }
		void FillToBorder(int x, int y, int border, int color)
			{ gdImageFillToBorder(im, x, y, border, color); }
		void Fill(int x, int y, int color)
			{ gdImageFill(im, x, y, color); }

		void Ellipse(const Point & c, const Size & s, int color)
			{ Ellipse(c.X(), c.Y(), s.W(), s.H(), color); }
		void FilledArc(const Point & c, const Size & si, int s, int e, int color, int style)
			{ FilledArc(c.X(), c.Y(), si.W(), si.H(), s, e, color, style); }
		void Arc(const Point & c, const Size & si, int s, int e, int color)
			{ Arc(c.X(), c.Y(), si.W(), si.H(), s, e, color); }
		void FilledEllipse(const Point & c, const Size & s, int color)
			{ FilledEllipse(c.X(), c.Y(), s.W(), s.H(), color); }
		void FillToBorder(const Point & p, int border, int color)
			{ FillToBorder(p.X(), p.Y(), border, color); }
		void Fill(const Point & p, int color)
			{ Fill(p.X(), p.Y(), color); }

		void Copy(const gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int w, int h)
			{ gdImageCopy(im, src, dstX, dstY, srcX, srcY, w, h); }
		void CopyMerge(const gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int w, int h, int pct)
			{ gdImageCopyMerge(im, src, dstX, dstY, srcX, srcY, w, h, pct); }
		void CopyMergeGray(const gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int w, int h, int pct)
			{ gdImageCopyMergeGray(im, src, dstX, dstY, srcX, srcY, w, h, pct); }

		void CopyResized(const gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int dstW, int dstH, int srcW, int srcH)
			{ gdImageCopyResized(im, src, dstX, dstY, srcX, srcY, dstW, dstH, srcW, srcH); }
		void CopyResampled(const gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int dstW, int dstH, int srcW, int srcH)
			{ gdImageCopyResampled(im, src, dstX, dstY, srcX, srcY, dstW, dstH, srcW, srcH); }
		void CopyRotated(const gdImagePtr src, double dstX, double dstY, int srcX, int srcY, int srcWidth, int srcHeight, int angle)
			{ gdImageCopyRotated(im, src, dstX, dstY, srcX, srcY, srcWidth, srcHeight, angle); }

		void Copy(const gdImagePtr src, const Point & dstP, const Point & srcP, const Size & s)
			{ Copy(src, dstP.X(), dstP.Y(), srcP.X(), srcP.Y(), s.W(), s.H()); }
		void CopyMerge(const gdImagePtr src, const Point & dstP, const Point & srcP, const Size & s, int pct)
			{ CopyMerge(src, dstP.X(), dstP.Y(), srcP.X(), srcP.Y(), s.W(), s.H(), pct); }
		void CopyMergeGray(const gdImagePtr src, const Point & dstP, const Point & srcP, const Size & s, int pct)
			{ CopyMergeGray(src, dstP.X(), dstP.Y(), srcP.X(), srcP.Y(), s.W(), s.H(), pct); }

		void CopyResized(const gdImagePtr src, const Point & dstP, const Point & srcP, const Size & dstS, const Size & srcS)
			{ CopyResized(src, dstP.X(), dstP.Y(), srcP.X(), srcP.Y(), dstS.W(), dstS.H(), srcS.W(), srcS.H()); }
		void CopyResampled(const gdImagePtr src, const Point & dstP, const Point & srcP, const Size & dstS, const Size & srcS)
			{ CopyResampled(src, dstP.X(), dstP.Y(), srcP.X(), srcP.Y(), dstS.W(), dstS.H(), srcS.W(), srcS.H()); }
		void CopyRotated(const gdImagePtr src, double dstX, double dstY, const Point & srcP, const Size & srcS, int angle)
			{ CopyRotated(src, dstX, dstY, srcP.X(), srcP.Y(), srcS.W(), srcS.H(), angle); }

		void Copy(const GD::Image & src, int dstX, int dstY, int srcX, int srcY, int w, int h)
			{ Copy(src.im, dstX, dstY, srcX, srcY, w, h); }
		void CopyMerge(const GD::Image & src, int dstX, int dstY, int srcX, int srcY, int w, int h, int pct)
			{ CopyMerge(src.im, dstX, dstY, srcX, srcY, w, h, pct); }
		void CopyMergeGray(const GD::Image & src, int dstX, int dstY, int srcX, int srcY, int w, int h, int pct)
			{ CopyMergeGray(src.im, dstX, dstY, srcX, srcY, w, h, pct); }

		void CopyResized(const GD::Image & src, int dstX, int dstY, int srcX, int srcY, int dstW, int dstH, int srcW, int srcH)
			{ CopyResized(src.im, dstX, dstY, srcX, srcY, dstW, dstH, srcW, srcH); }
		void CopyResampled(const GD::Image & src, int dstX, int dstY, int srcX, int srcY, int dstW, int dstH, int srcW, int srcH)
			{ CopyResampled(src.im, dstX, dstY, srcX, srcY, dstW, dstH, srcW, srcH); }
		void CopyRotated(const GD::Image & src, double dstX, double dstY, int srcX, int srcY, int srcWidth, int srcHeight, int angle)
			{ CopyRotated(src.im, dstX, dstY, srcX, srcY, srcWidth, srcHeight, angle); }

		void Copy(const GD::Image & src, const Point & dstP, const Point & srcP, const Size & s)
			{ Copy(src.im, dstP.X(), dstP.Y(), srcP.X(), srcP.Y(), s.W(), s.H()); }
		void CopyMerge(const GD::Image & src, const Point & dstP, const Point & srcP, const Size & s, int pct)
			{ CopyMerge(src.im, dstP.X(), dstP.Y(), srcP.X(), srcP.Y(), s.W(), s.H(), pct); }
		void CopyMergeGray(const GD::Image & src, const Point & dstP, const Point & srcP, const Size & s, int pct)
			{ CopyMergeGray(src.im, dstP.X(), dstP.Y(), srcP.X(), srcP.Y(), s.W(), s.H(), pct); }

		void CopyResized(const GD::Image & src, const Point & dstP, const Point & srcP, const Size & dstS, const Size & srcS)
			{ CopyResized(src.im, dstP.X(), dstP.Y(), srcP.X(), srcP.Y(), dstS.W(), dstS.H(), srcS.W(), srcS.H()); }
		void CopyResampled(const GD::Image & src, const Point & dstP, const Point & srcP, const Size & dstS, const Size & srcS)
			{ CopyResampled(src.im, dstP.X(), dstP.Y(), srcP.X(), srcP.Y(), dstS.W(), dstS.H(), srcS.W(), srcS.H()); }
		void CopyRotated(const GD::Image & src, double dstX, double dstY, const Point & srcP, const Size & srcS, int angle)
			{ CopyRotated(src.im, dstX, dstY, srcP.X(), srcP.Y(), srcS.W(), srcS.H(), angle); }

		void SetBrush(gdImagePtr brush)
			{ gdImageSetBrush(im, brush); }
		void SetBrush(const GD::Image & brush)
			{ SetBrush(brush.im); }
		void SetTile(gdImagePtr tile)
			{ gdImageSetTile(im, tile); }
		void SetTile(const GD::Image & tile)
			{ SetTile(tile.im); }
		void SetAntiAliased(int c)
			{ gdImageSetAntiAliased(im, c); }
		void SetAntiAliasedDontBlend(int c, int dont_blend)
			{ gdImageSetAntiAliasedDontBlend(im, c, dont_blend); }
		void SetStyle(int * style, int noOfPixels)
			{ gdImageSetStyle(im, style, noOfPixels); }
		void SetThickness(int thickness)
			{ gdImageSetThickness(im, thickness); }

		void Interlace(bool interlaceArg)
			{ gdImageInterlace(im, interlaceArg?1:0); }
		void AlphaBlending(bool alphaBlendingArg)
			{ gdImageAlphaBlending(im, alphaBlendingArg?1:0); }
		void SaveAlpha(bool saveAlphaArg)
			{ gdImageSaveAlpha(im, saveAlphaArg?1:0); }

		bool IsTrueColor() const
			{  return (gdImageTrueColor(im)?true:false); }
		int SX() const
			{  return gdImageSX(im); }
		int SY() const
			{  return gdImageSY(im); }
		int Width() const
			{  return SX(); }
		int Height() const
			{  return SY(); }
		void GetSize(Size & s) const
			{ s.set(SX(), SY()); }
		int ColorsTotal() const
			{  return gdImageColorsTotal(im); }
		int Red(int color) const
			{  return gdImageRed(im, color); }
		int Green(int color) const
			{  return gdImageGreen(im, color); }
		int Blue(int color) const
			{  return gdImageBlue(im, color); }
		int Alpha(int color) const
			{  return gdImageAlpha(im, color); }
		int GetTransparent() const
			{  return gdImageGetTransparent(im); }
		int GetInterlaced() const
			{  return gdImageGetInterlaced(im); }
		int PalettePixel(int x, int y) const
			{  return gdImagePalettePixel(im, x, y); }
		int TrueColorPixel(int x, int y) const
			{  return gdImageTrueColorPixel(im, x, y); }

		const gdImagePtr GetPtr() const
			{	return im; }

	protected:
		/// Free the internal image pointer 
		void	clear()
			{
			if (im)
				gdImageDestroy(im);
			im = 0;
			}
		gdImagePtr im;
		};
	} // namespace GD
/// Read in an image from a standard library input stream
std::istream & operator>> (std::istream & in, GD::Image & img);

#endif /* _gdpp_h */
#endif /* __cplusplus */
