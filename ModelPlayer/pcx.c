/*
 * pcx.c -- pcx texture loader
 * last modification: aug. 14, 2007
 *
 * Copyright (c) 2005-2007 David HENRY
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * gcc -Wall -ansi -lGL -lGLU -lglut pcx.c -o pcx
 */

#include "GL/glut.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* OpenGL texture info */
struct gl_texture_t
{
  GLsizei width;
  GLsizei height;

  GLenum format;
  GLint internalFormat;
  GLuint id;

  GLubyte *texels;
};

#pragma pack(1)
/* PCX header */
struct pcx_header_t
{
  GLubyte manufacturer;
  GLubyte version;
  GLubyte encoding;
  GLubyte bitsPerPixel;

  GLushort xmin, ymin;
  GLushort xmax, ymax;
  GLushort horzRes, vertRes;

  GLubyte palette[48];
  GLubyte reserved;
  GLubyte numColorPlanes;

  GLushort bytesPerScanLine;
  GLushort paletteType;
  GLushort horzSize, vertSize;

  GLubyte padding[54];
};
#pragma pack(4)

/* Texture id for the demo */
GLuint texId;


static void
ReadPCX1bit (FILE *fp, const struct pcx_header_t *hdr,
	     struct gl_texture_t *texinfo)
{
  int y, i, bytes;
  int colorIndex;
  int rle_count = 0, rle_value = 0;
  GLubyte *ptr = texinfo->texels;

  for (y = 0; y < texinfo->height; ++y)
    {
      ptr = &texinfo->texels[(texinfo->height - (y + 1)) * texinfo->width * 3];
      bytes = hdr->bytesPerScanLine;

      /* Decode line number y */
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

static void
ReadPCX4bits (FILE *fp, const struct pcx_header_t *hdr,
	      struct gl_texture_t *texinfo)
{
  GLubyte *colorIndex, *line;
  GLubyte *pLine, *ptr;
  int rle_count = 0, rle_value = 0;
  int x, y, c;
  int bytes;

  colorIndex = (GLubyte *)malloc (sizeof (GLubyte) * texinfo->width);
  line = (GLubyte *)malloc (sizeof (GLubyte) * hdr->bytesPerScanLine);

  for (y = 0; y < texinfo->height; ++y)
    {
      ptr = &texinfo->texels[(texinfo->height - (y + 1)) * texinfo->width * 3];

      memset (colorIndex, 0, texinfo->width * sizeof (GLubyte));

      for (c = 0; c < 4; ++c)
	{
	  pLine = line;
	  bytes = hdr->bytesPerScanLine;

	  /* Decode line number y */
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

	  /* Compute line's color indexes */
	  for (x = 0; x < texinfo->width; ++x)
	    {
	      if (line[x / 8] & (128 >> (x % 8)))
		colorIndex[x] += (1 << c);
	    }
	}

      /* Decode scan line.  color index => rgb  */
      for (x = 0; x < texinfo->width; ++x, ptr += 3)
	{
	  ptr[0] = hdr->palette[colorIndex[x] * 3 + 0];
	  ptr[1] = hdr->palette[colorIndex[x] * 3 + 1];
	  ptr[2] = hdr->palette[colorIndex[x] * 3 + 2];
	}
    }

  /* Release memory */
  free (colorIndex);
  free (line);
}

static void
ReadPCX8bits (FILE *fp, const struct pcx_header_t *hdr,
	      struct gl_texture_t *texinfo)
{
  int rle_count = 0, rle_value = 0;
  GLubyte palette[768];
  GLubyte magic;
  GLubyte *ptr;
  fpos_t curpos;
  int y, bytes;

  /* The palette is contained in the last 769 bytes of the file */
  fgetpos (fp, &curpos);
  fseek (fp, -769, SEEK_END);
  magic = fgetc (fp);

  /* First byte must be equal to 0x0c (12) */
  if (magic != 0x0c)
    {
      fprintf (stderr, "error: colormap's first byte must be 0x0c! "
	       "(%#x)\n", magic);

      free (texinfo->texels);
      texinfo->texels = NULL;
      return;
    }

  /* Read palette */
  fread (palette, sizeof (GLubyte), 768, fp);
  fsetpos (fp, &curpos);

  /* Read pixel data */
  for (y = 0; y < texinfo->height; ++y)
    {
      ptr = &texinfo->texels[(texinfo->height - (y + 1)) * texinfo->width * 3];
      bytes = hdr->bytesPerScanLine;

      /* Decode line number y */
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

static void
ReadPCX24bits (FILE *fp, const struct pcx_header_t *hdr,
	       struct gl_texture_t *texinfo)
{
  GLubyte *ptr = texinfo->texels;
  int rle_count = 0, rle_value = 0;
  int y, c;
  int bytes;

  for (y = 0; y < texinfo->height; ++y)
    {
      /* For each color plane */
      for (c = 0; c < 3; ++c)
	{
	  ptr = &texinfo->texels[(texinfo->height - (y + 1)) * texinfo->width * 3];
	  bytes = hdr->bytesPerScanLine;

	  /* Decode line number y */
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
	      ptr[c] = (GLubyte)rle_value;
	      ptr += 3;
	    }
	}
    }
}

static struct gl_texture_t *
ReadPCXFile (const char *filename)
{
  struct gl_texture_t *texinfo;
  struct pcx_header_t header;
  FILE *fp = NULL;
  int bitcount;

  /* Open image file */
  fp = fopen (filename, "rb");
  if (!fp)
    {
      fprintf (stderr, "error: couldn't open \"%s\"!\n", filename);
      return NULL;
    }

  /* Read header file */
  fread (&header, sizeof (struct pcx_header_t), 1, fp);
  if (header.manufacturer != 0x0a)
    {
      fprintf (stderr, "error: bad version number! (%i)\n",
	       header.manufacturer);
      return NULL;
    }

  /* Initialize texture parameters */
  texinfo = (struct gl_texture_t *)
    malloc (sizeof (struct gl_texture_t));
  texinfo->width = header.xmax - header.xmin + 1;
  texinfo->height = header.ymax - header.ymin + 1;
  texinfo->format = GL_RGB;
  texinfo->internalFormat = 3;
  texinfo->texels = (GLubyte *)
    malloc (sizeof (GLubyte) * texinfo->width
	    * texinfo->height * texinfo->internalFormat);

  bitcount = header.bitsPerPixel * header.numColorPlanes;

  /* Read image data */
  switch (bitcount)
    {
    case 1:
      /* 1 bit color index */
      ReadPCX1bit (fp, &header, texinfo);
      break;

    case 4:
      /* 4 bits color index */
      ReadPCX4bits (fp, &header, texinfo);
      break;

    case 8:
      /* 8 bits color index */
      ReadPCX8bits (fp, &header, texinfo);
      break;

    case 24:
      /* 24 bits */
      ReadPCX24bits (fp, &header, texinfo);
      break;

    default:
      /* Unsupported */
      fprintf (stderr, "error: unknown %i bitcount pcx files\n", bitcount);
      free (texinfo->texels);
      free (texinfo);
      texinfo = NULL;
      break;
    }

  fclose (fp);
  return texinfo;
}

GLuint
loadPCXTexture (const char *filename)
{
  struct gl_texture_t *pcx_tex = NULL;
  GLuint tex_id = 0;
  GLint alignment;

  pcx_tex = ReadPCXFile (filename);

  if (pcx_tex && pcx_tex->texels)
    {
      /* Generate texture */
      glGenTextures (1, &pcx_tex->id);
      glBindTexture (GL_TEXTURE_2D, pcx_tex->id);

      /* Setup some parameters for texture filters and mipmapping */
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glGetIntegerv (GL_UNPACK_ALIGNMENT, &alignment);
      glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

#if 0
      glTexImage2D (GL_TEXTURE_2D, 0, pcx_tex->internalFormat,
		    pcx_tex->width, pcx_tex->height, 0, pcx_tex->format,
		    GL_UNSIGNED_BYTE, pcx_tex->texels);
#else
      gluBuild2DMipmaps (GL_TEXTURE_2D, pcx_tex->internalFormat,
			 pcx_tex->width, pcx_tex->height,
			 pcx_tex->format, GL_UNSIGNED_BYTE, pcx_tex->texels);
#endif

      glPixelStorei (GL_UNPACK_ALIGNMENT, alignment);

      tex_id = pcx_tex->id;

      /* OpenGL has its own copy of texture data */
      free (pcx_tex->texels);
      free (pcx_tex);
    }

  return tex_id;
}

static void
cleanup ()
{
  glDeleteTextures (1, &texId);
}

static void
init (const char *filename)
{
  /* Initialize OpenGL */
  glClearColor (0.5f, 0.5f, 0.5f, 1.0f);
  glShadeModel (GL_SMOOTH);

  glEnable (GL_DEPTH_TEST);

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  /* Load PCX texture from file */
  texId = loadPCXTexture (filename);
  if (!texId)
    exit (EXIT_FAILURE);
}

static void
reshape (int w, int h)
{
  if (h == 0)
    h = 1;

  glViewport (0, 0, (GLsizei)w, (GLsizei)h);

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (45.0, w/(GLdouble)h, 0.1, 1000.0);

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

  glutPostRedisplay ();
}

static void
display ()
{
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity ();

  glEnable (GL_TEXTURE_2D);
  glBindTexture (GL_TEXTURE_2D, texId);

  /* Draw textured quad */
  glTranslatef (0.0, 0.0, -5.0);
  glBegin (GL_QUADS);
    glTexCoord2f (0.0f, 0.0f);
    glVertex3f (-1.0f, -1.0f, 0.0f);

    glTexCoord2f (1.0f, 0.0f);
    glVertex3f (1.0f, -1.0f, 0.0f);

    glTexCoord2f (1.0f, 1.0f);
    glVertex3f (1.0f, 1.0f, 0.0f);

    glTexCoord2f (0.0f, 1.0f);
    glVertex3f (-1.0f, 1.0f, 0.0f);
  glEnd  ();

  glDisable (GL_TEXTURE_2D);

  glutSwapBuffers ();
}

static void
keyboard (unsigned char key, int x, int y)
{
  /* Escape */
  if (key == 27)
    exit (0);
}

int
main (int argc, char *argv[])
{
  if (argc < 2)
    {
      fprintf (stderr, "usage: %s filename.pcx\n", argv[0]);
      return -1;
    }

  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowSize (640, 480);
  glutCreateWindow ("PCX Texture Demo");

  atexit (cleanup);
  init (argv[1]);

  glutReshapeFunc (reshape);
  glutDisplayFunc (display);
  glutKeyboardFunc (keyboard);

  glutMainLoop ();

  return 0;
}
