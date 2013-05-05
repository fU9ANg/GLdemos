#ifndef _D2_IMAGE_H
#define _D2_IMAGE_H

#ifdef _WIN32
    #include <windows.h>
    #include "GL/gl.h"
    #include "GL/glut.h"
    #include "jpeglib.h"
    //#include "include/png.h"
#else
    #include <GL/gl.h>
    #include <GL/glut.h>
    //#include <jpeglib.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#ifdef _MSC_VER
#pragma comment (lib, "jpeg.lib")
#pragma comment (lib, "libpng.lib")
#pragma comment (lib, "zlib.lib")
#pragma comment (linker, "/nodefaultlib:libc")
#endif // _MSC_VER

using namespace std;


/*
====================
  class d2Image
====================
*/
class d2Image
{

public:
    d2Image () {}
    //d2Image (const string &filename, GLsizei w, GLsizei h, GLenum format, GLint inter_format, const GLubyte *pixels);
    //virtual ~d2Image ();

    virtual int LoadFile (const char *filename) { return 0; }

    GLsizei width () const { return _width; }
	//GLuint  id () const { return _id; }
    GLsizei height() const { return _height; }
    GLenum format () const { return _format; }
    GLint inter_format () const { return _inter_format; }
	const GLubyte *texels () const { return _texels; }
    //const GLubyte *pixels () const { return _pixels; }
    //const string& name () const {return _name; }
    
protected:
    GLsizei  _width;
    GLsizei  _height;
    GLenum   _format;
    GLint    _inter_format;
	//GLuint   _id;

	GLubyte* _texels;
    //GLubyte* _pixels;
    //string   _name;
};


/*
====================
  class d2BMPImage
====================
*/
class d2BMPImage : public d2Image
{
private:
    #define RLE_COMMAND         0
    #define RLE_ENDOFLINE       0
    #define RLE_ENDOFBITMAP     1
    #define RLE_DELTA           2

    #define BMP_WIN             0
    #define BMP_OS2             1

    #ifndef _WIN32
        #define BI_RGB          0
        #define BI_RLE8         1
        #define BI_RLE4         2
        #define BI_BITFIELDS    3
    #endif

    #pragma pack(push, 2)
    struct bmp_file_header_t            
    {
        unsigned char   bfType[2];
        unsigned int    bfSize;
        unsigned short  bfReserved1;
        unsigned short  bfReserved2;
        unsigned int    bfOffBits;
    };
    #pragma pack(pop)

    struct bmp_info_header_t
    {
        unsigned int    biSize;
        int             biWidth;
        int             biHeight;
        unsigned short  biPlanes;
        unsigned short  biBitCount;
        unsigned int    biCompression;
        unsigned int    biSizeImage;
        int             biXPelsPerMeter;
        int             biYPelsPerMeter;
        unsigned int    biClrUsed;
        unsigned int    biClrImportant;
    };

    struct bmp_core_header_t
    {
        unsigned int    bcSize;
        unsigned short  bcWidth;
        unsigned short  bcHeight;
        unsigned short  bcPlanes;
        unsigned short  bcBitCount;
    };

    struct bmp_core_info_t
    {
        struct bmp_core_header_t bmciHeader;
        unsigned char   bmciColors[3];
    };


public:
    int  LoadFile        (const char *filename);

    void ReadBMP1bit     (FILE *, const GLubyte *, int);
    void ReadBMP4bits    (FILE *, const GLubyte *, int);
    void ReadBMP8bits    (FILE *, const GLubyte *, int);
    void ReadBMP24bits   (FILE *);
    void ReadBMP32bits   (FILE *);
    void ReadBMP8bitsRLE (FILE *, const GLubyte *);
    void ReadBMP4bitsRLE (FILE *, const GLubyte *);

    ~d2BMPImage ();
};


/*
====================
  class d2TGAImage
====================
*/
class d2TGAImage : public d2Image
{
private:
    #pragma pack(push, 1)
    struct tga_header_t
    {
        GLubyte id_lenght;          // size of image id
        GLubyte colormap_type;      // 1 is has a colormap
        GLubyte image_type;         // compression type

        short   cm_first_entry;     // colormap origin
        short   cm_length;          // colormap length
        GLubyte cm_size;            // colormap size

        short   x_origin;           // bottom left x coord origin
        short   y_origin;           // bottom left y coord origin

        short   width;              // picture width (in pixels)
        short   height;             // picture height (in pixels)

        GLubyte pixel_depth;        // bits per pixel: 8, 16, 24 or 32
        GLubyte image_descriptor;   // 24 bits = 0x00; 32 bits = 0x80
    };
    #pragma pack(pop)


public:
    int  LoadFile (const char *filename);


    void ReadTGA8bitsRLE  (FILE *, const GLubyte *);
    void ReadTGA16bitsRLE (FILE *);
    void ReadTGA24bitsRLE (FILE *);
    void ReadTGA32bitsRLE (FILE *);
    void ReadTGAgray8bitsRLE  (FILE *);
    void ReadTGAgray16bitsRLE (FILE *);

	void ReadTGA8bits  (FILE *, const GLubyte *);
	void ReadTGA16bits (FILE *);
	void ReadTGA24bits (FILE *);
	void ReadTGA32bits (FILE *);
	void ReadTGAgray8bits  (FILE *);
	void ReadTGAgray16bits (FILE *);


    ~d2TGAImage ();
};


/*
====================
  class d2PCXImage
====================
*/
class d2PCXImage : public d2Image
{
private:
    #pragma pack(1)
    struct pcx_header_t
    {
        GLubyte  manufacturer;
        GLubyte  version;
        GLubyte  encoding;
        GLubyte  bitsPerPixel;

        GLushort xmin, ymin;
        GLushort xmax, ymax;
        GLushort horzRes, vertRes;

        GLubyte  palette[48];
        GLubyte  reserved;
        GLubyte  numColorPlanes;

        GLushort bytesPerScanLine;
        GLushort paletteType;
        GLushort horzSize, vertSize;

        GLubyte  padding[54];
    };
    #pragma pack(4)


public:
    int  LoadFile (const char *filename);


	void ReadPCX1bit  (FILE *, const struct pcx_header_t *);
	void ReadPCX4bits (FILE *, const struct pcx_header_t *);
	void ReadPCX8bits (FILE *, const struct pcx_header_t *);
	void ReadPCX24bits(FILE *, const struct pcx_header_t *);


    ~d2PCXImage ();
};


/*
====================
  class d2JPGImage
====================
*/
class d2JPGImage : public d2Image
{
public:
    int  LoadFile (const char *filename);

    ~d2JPGImage ();
};


/*
====================
  class d2PNGImage
====================
*/
class d2PNGImage : public d2Image
{
private:
	void GetPNGInfo (int color_type);


public:
    int  LoadFile (const char *filename);

    ~d2PNGImage ();
};


#endif  //_D2_IMAGE_H
