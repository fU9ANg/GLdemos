
/*
 * Written 2011
 */

#ifdef _WIN32
    #include <windows.h>
    #include "GL/gl.h"
    #include "GL/glut.h"
#else
    #include <GL/gl.h>
    #include <GL/glut.h>
#endif

#include "d2_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

using namespace std;


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////                    BMP Image                       ////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/*
====================
d2BMPImage::LoadFile
====================
*/
int d2BMPImage::LoadFile (const char *filename)
{
    FILE   *fp;
    struct bmp_file_header_t bmfh;
    struct bmp_info_header_t bmih;
    struct bmp_core_header_t bmch;
    int os_type, colormapSize;
    unsigned int bitCount, compression;
    fpos_t bmhPos;
    GLubyte *colormap = NULL;

    fp = fopen (filename, "rb");
    if (!fp)
    {
        fprintf (stderr, "error: couldn't open \"%s\"!\n", filename);
        return (1);
    }

    fread (&bmfh, sizeof (struct bmp_file_header_t), 1, fp);
    fgetpos (fp, &bmhPos);
    if (strncmp ((char *)bmfh.bfType, "BM", 2) != 0)
    {
        fprintf (stderr, "%s is not a valid BMP file!\n", filename);
        fclose (fp);
        return (1);
    }

    _format = GL_RGB;
    _inter_format = 3;

    fread (&bmih, sizeof (struct bmp_info_header_t), 1, fp);
    if (bmih.biCompression > 3)
    {
        fsetpos (fp, &bmhPos);
        fread (&bmch, sizeof (struct bmp_core_header_t), 1, fp);
        os_type = BMP_OS2;
        compression = BI_RGB;
        bitCount = bmch.bcBitCount;

        _width = bmch.bcWidth;
        _height= bmch.bcHeight;
    }
    else
    {
        compression = bmih.biCompression;
        os_type = BMP_WIN;
        bitCount = bmih.biBitCount;

        _width = bmih.biWidth;
        _height= bmih.biHeight;
    }

    if (bitCount <= 8)
    {
        colormapSize = (1 << bitCount) * ((os_type == BMP_OS2) ? 3 : 4);
        colormap = (GLubyte *) malloc (colormapSize * sizeof (GLubyte));

        fread (colormap, sizeof (GLubyte), colormapSize, fp);
    }

    _texels = (GLubyte *) malloc (_width * _height * _inter_format);

    fseek (fp, bmfh.bfOffBits, SEEK_SET);

    switch (compression)
    {
        case BI_RGB:
            switch (bitCount)
            {
                case 1:
                    ReadBMP1bit (fp, colormap, os_type);
                    break;

                case 4:
                    ReadBMP4bits (fp, colormap, os_type);
                    break;

                case 8:
                    ReadBMP8bits (fp, colormap, os_type);
                    break;

                case 24:
                    ReadBMP24bits (fp);
                    break;

                case 32:
                    ReadBMP32bits (fp);
                    break;
            }
            break;

        case BI_RLE8:
            ReadBMP8bitsRLE (fp, colormap);
            break;

        case BI_RLE4:
            ReadBMP4bitsRLE (fp, colormap);
            break;

        case BI_BITFIELDS:
        default:
            fprintf (stderr, "unsupported bitmap type or bad file"
                     "compression type (%i)\n", compression);
            free (_texels);
            break;
    }

    if (colormap)
        free (colormap);

    fclose (fp);

    return (1);
}


/*
====================
d2BMPImage::ReadBMP1bit
====================
*/
void d2BMPImage::ReadBMP1bit (FILE *fp, const GLubyte *colormap, \
                              int os_type)
{
    int i, j, cmPixSize;
    GLubyte color, clrIndex;

    cmPixSize = (os_type == BMP_OS2) ? 3 : 4;

    for (i = 0; i < _width * _height; )
    {
        color = (GLubyte) fgetc (fp);

        for (j = 7; j >=0; --j, ++i)
        {
            clrIndex = ((color & (1 << j)) > 0);
            _texels[(i * 3) + 2] = colormap[(clrIndex * cmPixSize) + 0];
            _texels[(i * 3) + 1] = colormap[(clrIndex * cmPixSize) + 1];
            _texels[(i * 3) + 0] = colormap[(clrIndex * cmPixSize) + 2];
        }
    }
}


/*
====================
d2BMPImage::ReadBMP4bits
====================
*/
void d2BMPImage::ReadBMP4bits (FILE *fp, const GLubyte *colormap, \
                               int os_type)
{
    int i, cmPixSize;
    GLubyte color, clrIndex;

    cmPixSize = (os_type == BMP_OS2) ? 3 : 4;

    for (i = 0; i < _width * _height; i += 2)
    {
        color = (GLubyte) fgetc (fp);

        clrIndex = (color >> 4);
        _texels[(i * 3) + 2] = colormap[(clrIndex * cmPixSize) + 0];
        _texels[(i * 3) + 1] = colormap[(clrIndex * cmPixSize) + 1];
        _texels[(i * 3) + 0] = colormap[(clrIndex * cmPixSize) + 2];

        clrIndex = (color & 0x0f);
        _texels[(i * 3) + 5] = colormap[(clrIndex * cmPixSize) + 0];
        _texels[(i * 3) + 4] = colormap[(clrIndex * cmPixSize) + 1];
        _texels[(i * 3) + 3] = colormap[(clrIndex * cmPixSize) + 2];
    }
}


/*
====================
d2BMPImage::ReadBMP8bits
====================
*/
void d2BMPImage::ReadBMP8bits (FILE *fp, const GLubyte *colormap, \
                               int os_type)
{
    int i, cmPixSize;
    GLubyte color;

    cmPixSize = (os_type == BMP_OS2) ? 3 : 4;

    for (i = 0; i < _width * _height; ++i)
    {
        color = (GLubyte) fgetc (fp);

        _texels[(i * 3) + 2] = colormap[(color * cmPixSize) + 0];
        _texels[(i * 3) + 1] = colormap[(color * cmPixSize) + 1];
        _texels[(i * 3) + 0] = colormap[(color * cmPixSize) + 2];
    }
}


/*
====================
d2BMPImage::ReadBMP24bits
====================
*/
void d2BMPImage::ReadBMP24bits (FILE *fp)
{
    int i;

    for (i = 0; i < _width * _height; ++i)
    {
        _texels[(i * 3) + 2] = (GLubyte) fgetc (fp);
        _texels[(i * 3) + 1] = (GLubyte) fgetc (fp);
        _texels[(i * 3) + 0] = (GLubyte) fgetc (fp);
    }
}


/*
====================
d2BMPImage::ReadBMP32bit
====================
*/
void d2BMPImage::ReadBMP32bits (FILE *fp)
{
    int i;
    GLubyte skip;

    for (i = 0; i < _width * _height; ++i)
    {
        _texels[(i * 3) + 2] = (GLubyte) fgetc (fp);
        _texels[(i * 3) + 1] = (GLubyte) fgetc (fp);
        _texels[(i * 3) + 0] = (GLubyte) fgetc (fp);

        skip = (GLubyte) fgetc (fp);
    }
}


/*
====================
d2BMPImage::ReadBMP8bitsRLE
====================
*/
void d2BMPImage::ReadBMP8bitsRLE (FILE *fp, const GLubyte *colormap)
{
    int i;
    GLubyte color, skip;
    GLubyte byte1, byte2;
    GLubyte *pData = _texels;

    while (pData < _texels + (_width * _height) * 3)
    {
        byte1 = (GLubyte) fgetc (fp);
        byte2 = (GLubyte) fgetc (fp);

        if (byte1 == RLE_COMMAND)
        {
            for (i = 0; i < byte2; ++i, pData += 3)
            {
                color = (GLubyte) fgetc (fp);
                pData[0] = colormap[(color * 4) + 2];
                pData[1] = colormap[(color * 4) + 1];
                pData[2] = colormap[(color * 4) + 0];
            }

            if (byte2 % 2)
                skip = (GLubyte) fgetc (fp);
        }
        else
        {
            for (i = 0; i < byte1; ++i, pData += 3)
            {
                pData[0] = colormap[(byte2 * 4) + 2];
                pData[1] = colormap[(byte2 * 4) + 1];
                pData[2] = colormap[(byte2 * 4) + 0];
            }
        }
    }
}


/*
====================
d2BMPImage::ReadBMP4bitsRLE
====================
*/
void d2BMPImage::ReadBMP4bitsRLE (FILE *fp, const GLubyte *colormap)
{
    int i, bytesRead = 0;
    GLubyte color, datatype, skip;
    GLubyte byte1, byte2;
    GLubyte *pData = _texels;

    while (pData < _texels + (_width * _height) * 3)
    {
        byte1 = (GLubyte) fgetc (fp);
        byte2 = (GLubyte) fgetc (fp);
        bytesRead += 2;

        if (byte1 == RLE_COMMAND)
        {
            datatype = 0;

            for (i = 0; i < byte2; ++i, pData += 3)
            {
                if (i % 2)
                    color = (datatype & 0x0f);
                else
                {
                    datatype = (GLubyte) fgetc (fp);
                    ++bytesRead;
                    color = (datatype >> 4);
                }

                pData[0] = colormap[(color * 4) + 2];
                pData[1] = colormap[(color * 4) + 1];
                pData[2] = colormap[(color * 4) + 0];
            }

            if (bytesRead % 2)
            {
                skip = (GLubyte) fgetc (fp);
                ++bytesRead;
            }
        }
        else
        {
            for (i = 0; i < byte1; ++i, pData += 3)
            {
                if (i % 2)
                    color = (byte2 & 0x0f);
                else
                    color = (byte2 >> 4);

                pData[0] = colormap[(color * 4) + 2];
                pData[1] = colormap[(color * 4) + 1];
                pData[2] = colormap[(color * 4) + 0];
            }
        }
    }
}


/*
====================
d2BMPImage::~d2BMPImage
====================
*/
d2BMPImage::~d2BMPImage ()
{
	if (_texels)
		free (_texels);

	_texels = NULL;
}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////                    TGA Image                       ////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/*
====================
d2TGAImage::LoadFile
====================
*/
int d2TGAImage::LoadFile (const char *filename)
{
	FILE *fp;
	struct tga_header_t header;
	GLubyte *colormap = NULL;

	fp = fopen (filename, "rb");
	if (!fp)
	{
		fprintf (stderr, "error: couldn't open \"%s\"!\n", filename);
		return (1);
	}

	fread (&header, sizeof (struct tga_header_t), 1, fp);

#if 1
	_width = header.width;
	_height = header.height;

	switch (header.image_type)
	{
		case 3:
		case 11:
		{
			if (header.pixel_depth == 8)
			{
				_format = GL_LUMINANCE;
				_inter_format = 1;
			}
			else
			{
				_format = GL_LUMINANCE_ALPHA;
				_inter_format = 2;
			}
			break;
		}

		case 1:
		case 2:
		case 9:
		case 10:
		{
			if (header.pixel_depth <= 24)
			{
				_format = GL_RGB;
				_inter_format = 3;
			}
			else
			{
				_format = GL_RGBA;
				_inter_format = 4;
			}
			break;
		}
	}
#endif
	fseek (fp, header.id_lenght, SEEK_CUR);

	_texels = (GLubyte *) malloc (sizeof (GLubyte) * \
		                          _width * _height * _inter_format);
	if (!_texels)
	{
		fprintf (stderr, "out of memory!\n");
		return (1);
	}

	if (header.colormap_type)
	{
		colormap = (GLubyte *) malloc (sizeof (GLubyte) * \
			                           header.cm_length * (header.cm_size >> 3));
		fread (colormap, sizeof (GLubyte), header.cm_length * \
			   (header.cm_size >> 3), fp);
	}

	switch (header.image_type)
	{
		case 0:
			break;

		case 1:
			ReadTGA8bits (fp, colormap);
			break;

		case 2:
			switch (header.pixel_depth)
			{
				case 16:
					ReadTGA16bits (fp);
					break;
				case 24:
					ReadTGA24bits (fp);
					break;
				case 32:
					ReadTGA32bits (fp);
					break;
			}
			break;

		case 3:
			if (header.pixel_depth == 8)
				ReadTGAgray8bits (fp);
			else
				ReadTGAgray16bits (fp);
			break;

		case 9:
			ReadTGA8bitsRLE (fp, colormap);
			break;

		case 10:
			switch (header.pixel_depth)
			{
				case 16:
					ReadTGA16bitsRLE (fp);
					break;
				case 24:
					ReadTGA24bitsRLE (fp);
					break;
				case 32:
					ReadTGA32bitsRLE (fp);
					break;
			}
			break;

		case 11:
			if (header.pixel_depth == 8)
				ReadTGAgray8bitsRLE (fp);
			else
				ReadTGAgray16bitsRLE (fp);
			break;

		default:
			fprintf (stderr, "error: unknown TGA image type %i!\n", \
				     header.image_type);
			free (_texels);
			_texels = NULL;
			break;
	}

	if (colormap)
		free (colormap);

	fclose (fp);
	return (0);
}


/*
====================
d2TGAImage::ReadTGA8bits
====================
*/
void d2TGAImage::ReadTGA8bits  (FILE *fp, const GLubyte *colormap)
{
	int i;
	GLubyte color;

	for (i = 0; i < _width * _height; ++i)
	{
		color = (GLubyte) fgetc (fp);

		_texels[(i * 3) + 2] = colormap[(color * 3) + 0];
		_texels[(i * 3) + 1] = colormap[(color * 3) + 1];
		_texels[(i * 3) + 0] = colormap[(color * 3) + 2];
	}
}


/*
====================
d2TGAImage::ReadTGA16bits
====================
*/
void d2TGAImage::ReadTGA16bits (FILE *fp)
{
	int i;
	unsigned short color;

	for (i = 0; i < _width * _height; ++i)
	{
		color = fgetc (fp) + (fgetc (fp) << 8);

		_texels[(i * 3) + 0] = (GLubyte) (((color & 0x7c00) >> 10) << 3);
		_texels[(i * 3) + 1] = (GLubyte) (((color & 0x03e0) >>  5) << 3);
		_texels[(i * 3) + 2] = (GLubyte) (((color & 0x001f) >>  0) << 3);
	}
}


/*
====================
d2TGAImage::ReadTGA24bits
====================
*/
void d2TGAImage::ReadTGA24bits (FILE *fp)
{
	int i;

	for (i = 0; i < _width * _height; ++i)
	{
		_texels[(i * 3) + 2] = (GLubyte) fgetc (fp);
		_texels[(i * 3) + 1] = (GLubyte) fgetc (fp);
		_texels[(i * 3) + 0] = (GLubyte) fgetc (fp);
	}
}

/*
====================
d2TGAImage::ReadTGA32bits
====================
*/
void d2TGAImage::ReadTGA32bits (FILE *fp)
{
	int i;

	for (i = 0; i < _width * _height; ++i)
	{
		_texels[(i * 4) + 2] = (GLubyte) fgetc (fp);
		_texels[(i * 4) + 1] = (GLubyte) fgetc (fp);
		_texels[(i * 4) + 0] = (GLubyte) fgetc (fp);
		_texels[(i * 4) + 3] = (GLubyte) fgetc (fp);
	}
}

/*
====================
d2TGAImage::ReadTGAgray8bits
====================
*/
void d2TGAImage::ReadTGAgray8bits  (FILE *fp)
{
	int i;

	for (i = 0; i < _width * _height; ++i)
	{
		_texels[i] = (GLubyte) fgetc (fp);
	}
}

/*
====================
d2TGAImage::ReadTGAgray16bits
====================
*/
void d2TGAImage::ReadTGAgray16bits (FILE *fp)
{
	int i;

	for (i = 0; i < _width * _height; ++i)
	{
		_texels[(i * 2) + 0] = (GLubyte) fgetc (fp);
		_texels[(i * 2) + 1] = (GLubyte) fgetc (fp);
	}
}

/*
====================
d2TGAImage::ReadTGA8bitsRLE
====================
*/
void d2TGAImage::ReadTGA8bitsRLE  (FILE *fp, const GLubyte *colormap)
{
	int i, size;
	GLubyte color;
	GLubyte packet_header;
	GLubyte *ptr = _texels;

	while (ptr < _texels + (_width * _height) * 3)
	{
		packet_header = (GLubyte) fgetc (fp);
		size = 1 + (packet_header & 0x7f);

		if (packet_header & 0x80)
		{
			color = (GLubyte) fgetc (fp);

			for (i = 0; i < size; ++i, ptr += 3)
			{
				ptr[0] = colormap[(color * 3) + 2];
				ptr[1] = colormap[(color * 3) + 1];
				ptr[2] = colormap[(color * 3) + 0];
			}
		}
		else
		{
			for (i = 0; i < size; ++i, ptr += 3)
			{
				color = (GLubyte) fgetc (fp);

				ptr[0] = colormap[(color * 3) + 2];
				ptr[1] = colormap[(color * 3) + 1];
				ptr[2] = colormap[(color * 3) + 0];
			}
		}
    }
}

/*
====================
d2TGAImage::ReadTGA16bitsRLE
====================
*/
void d2TGAImage::ReadTGA16bitsRLE (FILE *fp)
{
	int i, size;
	unsigned short color;
	GLubyte packet_header;
	GLubyte *ptr = _texels;

	while (ptr < _texels + (_width * _height) * 3)
	{
		packet_header = fgetc (fp);
		size = 1 + (packet_header & 0x7f);

		if (packet_header & 0x80)
		{
			color = fgetc (fp) + (fgetc (fp) << 8);

			for (i = 0; i < size; ++i, ptr += 3)
			{
				ptr[0] = (GLubyte) (((color & 0x7C00) >> 10) << 3);
				ptr[1] = (GLubyte) (((color & 0x03E0) >>  5) << 3);
				ptr[2] = (GLubyte) (((color & 0x001F) >>  0) << 3);
			}
		}
		else
		{
			for (i = 0; i < size; ++i, ptr += 3)
			{
				color = fgetc (fp) + (fgetc (fp) << 8);

				ptr[0] = (GLubyte) (((color & 0x7C00) >> 10) << 3);
				ptr[1] = (GLubyte) (((color & 0x03E0) >>  5) << 3);
				ptr[2] = (GLubyte) (((color & 0x001F) >>  0) << 3);
			}
		}
    }
}

/*
====================
d2TGAImage::ReadTGA24bitsRLE
====================
*/
void d2TGAImage::ReadTGA24bitsRLE (FILE *fp)
{
	int i, size;
	GLubyte rgb[3];
	GLubyte packet_header;
	GLubyte *ptr = _texels;

	while (ptr < _texels + (_width * _height) * 3)
	{
		packet_header = (GLubyte) fgetc (fp);
		size = 1 + (packet_header & 0x7f);

		if (packet_header & 0x80)
		{
			fread (rgb, sizeof (GLubyte), 3, fp);

			for (i = 0; i < size; ++i, ptr += 3)
			{
				ptr[0] = rgb[2];
				ptr[1] = rgb[1];
				ptr[2] = rgb[0];
			}
		}
		else
		{
			for (i = 0; i < size; ++i, ptr += 3)
			{
				ptr[2] = (GLubyte) fgetc (fp);
				ptr[1] = (GLubyte) fgetc (fp);
				ptr[0] = (GLubyte) fgetc (fp);
			}
		}
    }
}

/*
====================
d2TGAImage::ReadTGA32bitsRLE
====================
*/
void d2TGAImage::ReadTGA32bitsRLE (FILE *fp)
{
	int i, size;
	GLubyte rgba[4];
	GLubyte packet_header;
	GLubyte *ptr = _texels;

	while (ptr < _texels + (_width * _height) * 4)
    {
		packet_header = (GLubyte) fgetc (fp);
		size = 1 + (packet_header & 0x7f);

		if (packet_header & 0x80)
		{
			fread (rgba, sizeof (GLubyte), 4, fp);

			for (i = 0; i < size; ++i, ptr += 4)
			{
				ptr[0] = rgba[2];
				ptr[1] = rgba[1];
				ptr[2] = rgba[0];
				ptr[3] = rgba[3];
			}
		}
		else
		{
			for (i = 0; i < size; ++i, ptr += 4)
			{
				ptr[2] = (GLubyte) fgetc (fp);
				ptr[1] = (GLubyte) fgetc (fp);
				ptr[0] = (GLubyte) fgetc (fp);
				ptr[3] = (GLubyte) fgetc (fp);
			}
		}
    }
}

/*
====================
d2TGAImage::ReadTGAgray8bitsRLE
====================
*/
void d2TGAImage::ReadTGAgray8bitsRLE  (FILE *fp)
{
	int i, size;
	GLubyte color;
	GLubyte packet_header;
	GLubyte *ptr = _texels;

	while (ptr < _texels + (_width * _height))
	{
		packet_header = (GLubyte) fgetc (fp);
		size = 1 + (packet_header & 0x7f);

		if (packet_header & 0x80)
		{
			color = (GLubyte) fgetc (fp);

			for (i = 0; i < size; ++i, ptr++)
				*ptr = color;
		}
		else
		{
			for (i = 0; i < size; ++i, ptr++)
				*ptr = (GLubyte) fgetc (fp);
		}
    }
}

/*
====================
d2TGAImage::ReadTGAgray16bitsRLE
====================
*/
void d2TGAImage::ReadTGAgray16bitsRLE (FILE *fp)
{
	int i, size;
	GLubyte color, alpha;
	GLubyte packet_header;
	GLubyte *ptr = _texels;

	while (ptr < _texels + (_width * _height) * 2)
    {
		packet_header = (GLubyte) fgetc (fp);
		size = 1 + (packet_header & 0x7f);

		if (packet_header & 0x80)
		{
			color = (GLubyte) fgetc (fp);
			alpha = (GLubyte) fgetc (fp);

			for (i = 0; i < size; ++i, ptr += 2)
			{
				ptr[0] = color;
				ptr[1] = alpha;
			}
		}
		else
		{
			for (i = 0; i < size; ++i, ptr += 2)
			{
				ptr[0] = (GLubyte) fgetc (fp);
				ptr[1] = (GLubyte) fgetc (fp);
			}
		}
    }
}

/*
====================
d2TGAImage::~d2TGAImage
====================
*/
d2TGAImage::~d2TGAImage()
{
	if (_texels)
		free (_texels);

	_texels = NULL;
}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////                    PCX Image                       ////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/*
====================
d2PCXImage::LoadFile
====================
*/
int
d2PCXImage::LoadFile (const char *filename)
{
	struct pcx_header_t header;
	FILE *fp = NULL;
	int bitcount;

	fp = fopen (filename, "rb");
	if (!fp)
	{
		fprintf (stderr, "error: couldn't open \"%s\"!\n", filename);
		return (1);
	}

	fread (&header, sizeof (struct pcx_header_t), 1, fp);
	if (header.manufacturer != 0x0a)
	{
		fprintf (stderr, "error: bad version number! (%i)\n", \
			     header.manufacturer);
		return (1);
	}

	_width  = header.xmax - header.xmin + 1;
	_height = header.ymax - header.ymin + 1;
	_format = GL_RGB;
	_inter_format = 3;
	_texels = (GLubyte *) \
		       malloc (sizeof (GLubyte) * _width * _height * _inter_format);
	bitcount = header.bitsPerPixel * header.numColorPlanes;
	switch (bitcount)
	{
		case 1:
			ReadPCX1bit  (fp, &header);
			break;

		case 4:
			ReadPCX4bits (fp, &header);
			break;

		case 8:
			ReadPCX8bits (fp, &header);
			break;

		case 24:
			ReadPCX24bits (fp, &header);
			break;

		default:
			fprintf (stderr, "error: unknown %i bitcount pcx files\n", bitcount);
			free (_texels);
			break;
	}

	fclose (fp);
	return (0);
}

/*
====================
d2PCXImage::ReadPCX1bit
====================
*/
void d2PCXImage::ReadPCX1bit  (FILE *fp, const struct pcx_header_t *hdr)
{
	int y, i, bytes;
	int colorIndex;
	int rle_count = 0, rle_value = 0;
	GLubyte *ptr  = _texels;

	for (y = 0; y < _height; ++y)
    {
		ptr = &_texels[(_height - (y + 1)) * _width * 3];
		bytes = hdr->bytesPerScanLine;

		while (bytes--)
		{
			if (rle_count == 0)
			{
				if ( (rle_value = fgetc (fp)) < 0xc0)
				{
					rle_count = 1;
				}
				else
				{
					rle_count = rle_value - 0xc0;
					rle_value = fgetc (fp);
				}
			}

			rle_count--;

			for (i = 7; i >= 0; --i, ptr += 3)
			{
				colorIndex = ((rle_value & (1 << i)) > 0);

				ptr[0] = hdr->palette[colorIndex * 3 + 0];
				ptr[1] = hdr->palette[colorIndex * 3 + 1];
				ptr[2] = hdr->palette[colorIndex * 3 + 2];
			}
		}
    }
}

/*
====================
d2PCXImage::ReadPCX4bits
====================
*/
void d2PCXImage::ReadPCX4bits (FILE *fp, const struct pcx_header_t *hdr)
{
	GLubyte *colorIndex, *line;
	GLubyte *pLine, *ptr;
	int rle_count = 0, rle_value = 0;
	int x, y, c;
	int bytes;

	colorIndex = (GLubyte *) malloc (sizeof (GLubyte) * _width);
	line = (GLubyte *) malloc (sizeof (GLubyte) * hdr->bytesPerScanLine);

	for (y = 0; y < _height; ++y)
    {
		ptr = &_texels[(_height - (y + 1)) * _width * 3];

		memset (colorIndex, 0, _width * sizeof (GLubyte));

		for (c = 0; c < 4; ++c)
		{
			pLine = line;
			bytes = hdr->bytesPerScanLine;

			while (bytes--)
			{
				if (rle_count == 0)
				{
					if ( (rle_value = fgetc (fp)) < 0xc0)
					{
						rle_count = 1;
					}
					else
					{
						rle_count = rle_value - 0xc0;
						rle_value = fgetc (fp);
					}
				}

				rle_count--;
				*(pLine++) = rle_value;
			}

			for (x = 0; x < _width; ++x)
			{
				if (line[x / 8] & (128 >> (x % 8)))
					colorIndex[x] += (1 << c);
			}
		}

		for (x = 0; x < _width; ++x, ptr += 3)
		{
			ptr[0] = hdr->palette[colorIndex[x] * 3 + 0];
			ptr[1] = hdr->palette[colorIndex[x] * 3 + 1];
			ptr[2] = hdr->palette[colorIndex[x] * 3 + 2];
		}
    }

	free (colorIndex);
	free (line);
}

/*
====================
d2PCXImage::ReadPCX8bits
====================
*/
void d2PCXImage::ReadPCX8bits (FILE *fp, const struct pcx_header_t *hdr)
{
	int rle_count = 0, rle_value = 0;
	GLubyte palette[768];
	GLubyte magic;
	GLubyte *ptr;
	fpos_t curpos;
	int y, bytes;

	fgetpos (fp, &curpos);
	fseek (fp, -769, SEEK_END);
	magic = fgetc (fp);

	if (magic != 0x0c)
    {
		fprintf (stderr, "error: colormap's first byte must be 0x0c! "
				"(%#x)\n", magic);

		free (_texels);
		_texels = NULL;
		return;
    }

	fread (palette, sizeof (GLubyte), 768, fp);
	fsetpos (fp, &curpos);

	for (y = 0; y < _height; ++y)
    {
		ptr = &_texels[(_height - (y + 1)) * _width * 3];
		bytes = hdr->bytesPerScanLine;

		while (bytes--)
		{
			if (rle_count == 0)
			{
				if( (rle_value = fgetc (fp)) < 0xc0)
				{
					rle_count = 1;
				}
				else
				{
					rle_count = rle_value - 0xc0;
					rle_value = fgetc (fp);
				}
			}

			rle_count--;

			ptr[0] = palette[rle_value * 3 + 0];
			ptr[1] = palette[rle_value * 3 + 1];
			ptr[2] = palette[rle_value * 3 + 2];
			ptr += 3;
		}
    }
}

/*
====================
d2PCXImage::ReadPCX24bits
====================
*/
void d2PCXImage::ReadPCX24bits(FILE *fp, const struct pcx_header_t *hdr)
{
	GLubyte *ptr = _texels;
	int rle_count = 0, rle_value = 0;
	int y, c;
	int bytes;

	for (y = 0; y < _height; ++y)
    {
		for (c = 0; c < 3; ++c)
		{
			ptr = &_texels[(_height - (y + 1)) * _width * 3];
			bytes = hdr->bytesPerScanLine;

			while (bytes--)
			{
				if (rle_count == 0)
				{
					if( (rle_value = fgetc (fp)) < 0xc0)
					{
						rle_count = 1;
					}
					else
					{
						rle_count = rle_value - 0xc0;
						rle_value = fgetc (fp);
					}
				}

				rle_count--;
				ptr[c] = (GLubyte) rle_value;
				ptr += 3;
			}
		}
    }
}

/*
====================
d2PCXImage::~d2PCXImage
====================
*/
d2PCXImage::~d2PCXImage ()
{
	if (_texels)
		free (_texels);

	_texels = NULL;
}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////                    JPG Image                       ////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/*
====================
d2JPGImage::LoadFile
====================
*/
int
d2JPGImage::LoadFile (const char *filename)
{
#if 0
	FILE *fp = NULL;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW j;
	int i;

	fp = fopen (filename, "rb");
	if (!fp)
	{
		fprintf (stderr, "error: couldn't not open \"%s\"!\n", filename);
		return (1);
	}

	jpeg_create_decompress (&cinfo);
	cinfo.err = jpeg_std_error (&jerr);
	jpeg_stdio_src (&cinfo, fp);

	jpeg_read_header (&cinfo, TRUE);
	jpeg_start_decompress (&cinfo);

	_width = cinfo.image_width;
	_height= cinfo.image_height;
	_inter_format = cinfo.num_components;

	if (cinfo.num_components == 1)
		_format = GL_LUMINANCE;
	else
		_format = GL_RGB;

	_texels = (GLubyte *) malloc (sizeof (GLubyte) * _width * _height * _inter_format);

	for (i = 0; i < _height; ++i)
	{
		j = _texels + ((_height - (i + 1)) * _width * _inter_format);
		jpeg_read_scanlines (&cinfo, &j, 1);
	}

	jpeg_finish_decompress  (&cinfo);
	jpeg_destroy_decompress (&cinfo);

	fclose (fp);
#endif
	return (0);
}

/*
====================
d2JPGImage::~d2JPGImage
====================
*/
d2JPGImage::~d2JPGImage ()
{
	if (_texels)	// write on base class.
		free (_texels);

	_texels = NULL;
}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////                    PNG Image                       ////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

/*
====================
d2PNGImage::LoadFile
====================
*/
int d2PNGImage::LoadFile (const char *filename)
{
#if 0
	png_byte	magic[8];
	png_structp png_ptr;
	png_infop   info_ptr;
	int bit_depth, color_type;
	FILE *fp = NULL;
	png_bytep *row_pointers = NULL;
	png_uint_32 w, h;
	int i;

	fp = fopen (filename, "rb");
	if (!fp)
	{
		fprintf (stderr, "error: couldn't open \"%s\"!\n", filename);
		return (1);
	}

	fread (magic, 1, sizeof (magic), fp);

	if (!png_check_sig (magic, sizeof (magic)))
	{
		fprintf (stderr, "error: \"%s\" is not a valid PNG image!\n", filename);
		fclose (fp);
		return (1);
	}

	png_ptr = png_create_read_struct
		(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		fclose (fp);
		return (1);
	}

	info_ptr = png_create_info_struct (png_ptr);
	if (!info_ptr)
	{
		fclose (fp);
		png_destroy_read_struct (&png_ptr, NULL, NULL);
		return (1);
	}

	if (setjmp (png_jmpbuf (png_ptr)))
	{
		fclose (fp);
		png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
		if (row_pointers)
			free (row_pointers);

		if (_texels)
			free (_texels);

		return (1);
	}

	png_init_io (png_ptr, fp);
	png_set_sig_bytes (png_ptr, sizeof (magic));
	png_read_info (png_ptr, info_ptr);

	bit_depth = png_get_bit_depth (png_ptr, info_ptr);
	color_type = png_get_color_type (png_ptr, info_ptr);

	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb (png_ptr);

	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_gray_1_2_4_to_8 (png_ptr);

	if (png_get_valid (png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha (png_ptr);

	if (bit_depth == 16)
		png_set_strip_16 (png_ptr);
	else if (bit_depth < 8)
		png_set_packing (png_ptr);

	png_read_update_info (png_ptr, info_ptr);

	png_get_IHDR (png_ptr, info_ptr, &w, &h, &bit_depth, \
		&color_type, NULL, NULL, NULL);
	_width  = w;
	_height = h;

	GetPNGInfo (color_type);

	_texels = (GLubyte *) malloc (sizeof (GLubyte) \
		* _width * _height * _inter_format);
	row_pointers = (png_bytep *) malloc (sizeof (png_bytep) \
		* _height);

	for (i = 0; i < _height; ++i)
	{
	#if 1
		row_pointers[i] = (png_bytep) (_texels + \
			((_height - (i + 1)) * _width * _inter_format));
	#else
		row_pointers[i] = (png_bytep) (_texels + \
			(_width * i * _inter_format));
	#endif
	}

	png_read_image (png_ptr, row_pointers);
	png_read_end (png_ptr, NULL);
	png_destroy_read_struct (&png_ptr, &info_ptr, NULL);

	free (row_pointers);

	fclose (fp);
#endif
	return (0);
}

/*
====================
d2PNGImage::GetPNGInfo
====================
*/
void d2PNGImage::GetPNGInfo (int color_type)
{
#if 0
	switch (color_type)
	{
		case PNG_COLOR_TYPE_GRAY:
			_format = GL_LUMINANCE;
			_inter_format = 1;
			break;

		case PNG_COLOR_TYPE_GRAY_ALPHA:
			_format = GL_LUMINANCE_ALPHA;
			_inter_format = 2;
			break;

		case PNG_COLOR_TYPE_RGB:
			_format = GL_RGB;
			_inter_format = 3;
			break;

		case PNG_COLOR_TYPE_RGB_ALPHA:
			_format = GL_RGBA;
			_inter_format = 4;
			break;

		default:
			break;

	}
    #endif   
}

/*
====================
d2PNGImage::~d2PNGImage
====================
*/
d2PNGImage::~d2PNGImage ()
{
	if (_texels)	// write on base class.
		free (_texels);

	_texels = NULL;
}
