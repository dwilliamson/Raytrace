/*----------------------------------------------------------------------------
 * FILENAME: bmpwrite.c
 *
 * AUTHOR: Don Williamson
 *
 * CREATED: Sun 19 July, 1998
 *
 * NOTES: Writes both 8-bit and 24-bit bitmaps. 1-bit and 4-bit bitmaps have
 *        not been implemented. The implementation takes a buffer as input
 *        and simply outputs the .BMP to file. Any 32k and 64k 16-bit buffers
 *        will automatically be saved as 24-bit files since the .BMP file
 *        has no equivalent.
 *
 *		  18.08.99: DW: Added 32-bit image support (reduced to 24-bit)
 *----------------------------------------------------------------------------
 */

#include <malloc.h>
#include <memory.h>
#include <conio.h>
#include "ptc.h"
#include "types.h"
#include "classes.h"


/*============================================================================
>>>>>>>>>>>>>>>>>>>>>>>>> DEFINITIONS / STRUCTURES <<<<<<<<<<<<<<<<<<<<<<<<<<<
============================================================================*/
#define BMP_TYPE_8BIT           0       /* 256 colour, palettised */
#define BMP_TYPE_RGB565         1       /* 32k colour mode */
#define BMP_TYPE_RGB555         2       /* 64k colour mode */
#define BMP_TYPE_24BIT          3       /* True-colour */
#define BMP_TYPE_ALPHA_1555		4		/* Draw alpha map only for 32k RGB */
#define BMP_TYPE_32BIT			5		/* 32-bit reduced to 24-bit */

#define MAKE_WORD(x,y)          (unsigned short)(((unsigned short)y << 8) + (unsigned short)x)


/*============================================================================
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> FUNCTIONS <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
============================================================================*/
Bitmap::Bitmap(unsigned long width, unsigned long height, unsigned char type)
{
	if (type == BMP_TYPE_8BIT)
		buffer = new unsigned char[width * height];
	else if (type == BMP_TYPE_24BIT)
		buffer = new unsigned char[width * height * 3];
	else if (type == BMP_TYPE_32BIT)
		buffer = new unsigned char[width * height * 4];

	bmiHeader.biWidth = width;
	bmiHeader.biHeight = height;

	bmp_type = type;
}

Bitmap::~Bitmap(void)
{
	delete buffer;
}

bool Bitmap::WriteBuffer_BMP(unsigned char *filename)
{
	FILE *bmp_fp;
	bool retval = 1;

	/* Write file information */
	bfh.bfType = MAKE_WORD('B', 'M');
	bfh.bfReserved1 = bfh.bfReserved2 = 0;

	/* Write dimension information */
	bmiHeader.biSize = 40;
	bmiHeader.biPlanes = 1;
	bmiHeader.biCompression = 0;
	bmiHeader.biXPelsPerMeter = 1600;
	bmiHeader.biYPelsPerMeter = 960;
	bmiHeader.biClrUsed = 0;
	bmiHeader.biClrImportant = 0;

	/* Can only be two types of bit count above 4 bpp for BMP */
	if (bmp_type == BMP_TYPE_8BIT)
	{
		bfh.bfOffBits = sizeof(bfh) + sizeof(bmiHeader) + (256 << 2);
		bmiHeader.biBitCount = 8;
		bmiHeader.biSizeImage = bmiHeader.biWidth * bmiHeader.biWidth;
	}
	else
	{
		bfh.bfOffBits = sizeof(bfh) + sizeof(bmiHeader);
		bmiHeader.biBitCount = 24;
		bmiHeader.biSizeImage = bmiHeader.biWidth * bmiHeader.biHeight * 3;
	}

	/* Size of file is image size plus the offset from the beginning of the
	  file of the image */
	bfh.bfSize = bfh.bfOffBits + bmiHeader.biSizeImage;

	/* Attempt to open the file for writing */
	if (!(bmp_fp = fopen((const char *)filename, "wb"))) return (0);

	/* Write the image description depending upon image type */
	if (bmp_type == BMP_TYPE_8BIT) retval = WriteImage_8Bit(bmp_fp);
	if (bmp_type == BMP_TYPE_RGB565) retval = WriteImage_RGB565(bmp_fp);
	if (bmp_type == BMP_TYPE_RGB555) retval = WriteImage_RGB555(bmp_fp);
	if (bmp_type == BMP_TYPE_24BIT) retval = WriteImage_24Bit(bmp_fp);
	if (bmp_type == BMP_TYPE_ALPHA_1555) retval = WriteImage_Alpha_RGB1555(bmp_fp);
	if (bmp_type == BMP_TYPE_32BIT) retval = WriteImage_32Bit(bmp_fp);

	fclose(bmp_fp);

	return (retval);
}

bool Bitmap::WriteImage_8Bit(FILE *fp)
{
	unsigned char	*scanline, *tbuf;
	unsigned long	x;

	/* Write BMP file headers */
	if (fwrite(&bfh, sizeof(bfh), 1, fp) != 1) return (0);
	if (fwrite(&bmiHeader, sizeof(bmiHeader), 1, fp) != 1)
	  return (0);

	/* Since this is 8-bit, need to write the current palette RAM to file */
	if (!WriteImagePalette(fp)) return (0);

	/* Allocate memory for a temporary scanline */
	if (!(scanline = (unsigned char *)malloc(bmiHeader.biWidth))) return (0);

	/* Locate the beginning of the last scanline */
	tbuf = buffer + (bmiHeader.biHeight - 1) * bmiHeader.biWidth;

	/* Write each scanline at a time, starting at the bottom of the screen
	  (logical top) and working to the top of the screen */
	for (x = 0; x < bmiHeader.biHeight; x++)
	{
		/* Copy scanline */
		memcpy(scanline, tbuf, bmiHeader.biWidth);

		/* Attempt to write it to file */
		if (fwrite(scanline, bmiHeader.biWidth, 1, fp) != 1)
			{ free(scanline); return (0); }

		/* Advance to the next scanline */
		tbuf -= bmiHeader.biWidth;
	}

	free(scanline);
	return (1);
}

bool Bitmap::WriteImagePalette(FILE *fp)
{
	unsigned char	rgb[4];
	unsigned short	x;

	/* Fill in reserved byte */
	rgb[3] = 0;

	/* Write the 256 colours of an 8-bit palette */
	for (x = 0; x < 256; x++)
	{
		rgb[2] = (unsigned char)x;
		rgb[1] = (unsigned char)x;
		rgb[0] = (unsigned char)x;

		/* Output to file */
		if (fwrite(rgb, 4, 1, fp) != 1) return (0);
	}

	return (1);
}

bool Bitmap::WriteImage_RGB565(FILE *fp)
{
	unsigned char	*scanline, *tbuf, rgb[3];
	unsigned long	x, scanlength, z;

	/* Write BMP file headers */
	fwrite(&bfh, sizeof(bfh), 1, fp);
	fwrite(&bmiHeader, sizeof(bmiHeader), 1, fp);

	/* Allocate memory for a temporary scanline */
	scanlength = (bmiHeader.biWidth << 1);
	if (!(scanline = (unsigned char *)malloc(scanlength))) return (0);

	/* Locate the beginning of the last scanline */
	tbuf = buffer + (bmiHeader.biHeight - 1) * scanlength;

	/* Write each scanline at a time, starting at the bottom of the screen
	  (logical top) and working to the top of the screen */
	for (x = 0; x < bmiHeader.biHeight; x++)
	{
		/* Copy scanline */
		memcpy(scanline, tbuf, scanlength);

		/* Attempt to write it to file */
		for (z = 0; z < scanlength; z += 2)
		{
			/* Format: BBBBBGGG GGGRRRRR */

			/* Get blue pigment intensity (scaled for 0-255) */
			rgb[2] = (((scanline[z + 1]) >> 3) & 31) << 3;

			/* Get green pigment intensity (scaled for 0-255) */
			rgb[1] = scanline[z] >> 5;
			rgb[1] += ((scanline[z + 1] & 7) << 3);
			rgb[1] <<= 2;

			/* Get red pigment intensity (scaled for 0-255) */
			rgb[0] = (scanline[z] & 31) << 3;

			/* Write the RGB triplet */
			fwrite(rgb, 3, 1, fp);
		}

		/* Advance to the next scanline */
		tbuf -= scanlength;
	}

	free(scanline);
	return (1);
}

bool Bitmap::WriteImage_RGB555(FILE *fp)
{
	unsigned char	*scanline, *tbuf, rgb[3];
	unsigned long	x, scanlength, z;

	/* Write BMP file headers */
	fwrite(&bfh, sizeof(bfh), 1, fp);
	fwrite(&bmiHeader, sizeof(bmiHeader), 1, fp);

	/* Allocate memory for a temporary scanline */
	scanlength = (bmiHeader.biWidth << 1);
	if (!(scanline = (unsigned char *)malloc(scanlength))) return (0);

	/* Locate the beginning of the last scanline */
	tbuf = buffer + (bmiHeader.biHeight - 1) * scanlength;

	/* Write each scanline at a time, starting at the bottom of the screen
	  (logical top) and working to the top of the screen */
	for (x = 0; x < bmiHeader.biHeight; x++)
	{
		/* Copy scanline */
		memcpy(scanline, tbuf, scanlength);

		/* Attempt to write it to file */
		for (z = 0; z < scanlength; z += 2)
		{
			/* Format: BBBBBGGG GGRRRRRr (r = reserved) */

			/* Get blue pigment intensity (scaled for 0-255) */
			rgb[0] = (scanline[z] & 31) << 3;

			/* Get green pigment intensity (scaled for 0-255) */
			rgb[1] = scanline[z] >> 5;
			rgb[1] += ((scanline[z + 1] & 3) << 3);
			rgb[1] <<= 3;

			/* Get red pigment intensity (scaled for 0-255) */
			rgb[2] = ((scanline[z + 1] >> 2) & 31) << 3;

			/* Write the RGB triplet */
			fwrite(rgb, 3, 1, fp);
		}

		/* Advance to the next scanline */
		tbuf -= scanlength;
	}

	free(scanline);
	return (1);
}

bool Bitmap::WriteImage_Alpha_RGB1555(FILE *fp)
{
	unsigned char	*scanline, *tbuf, rgb[3];
	unsigned long	x, scanlength, z;

	/* Write BMP file headers */
	fwrite(&bfh, sizeof(bfh), 1, fp);
	fwrite(&bmiHeader, sizeof(bmiHeader), 1, fp);

	/* Allocate memory for a temporary scanline */
	scanlength = (bmiHeader.biWidth << 1);
	if (!(scanline = (unsigned char *)malloc(scanlength))) return (0);

	/* Locate the beginning of the last scanline */
	tbuf = buffer + (bmiHeader.biHeight - 1) * scanlength;

	/* Write each scanline at a time, starting at the bottom of the screen
	  (logical top) and working to the top of the screen */
	for (x = 0; x < bmiHeader.biHeight; x++)
	{
		/* Copy scanline */
		memcpy(scanline, tbuf, scanlength);

		/* Attempt to write it to file */
		for (z = 0; z < scanlength; z += 2)
		{
			if (scanline[z + 1] & 0x80)
				rgb[0] = rgb[1] = rgb[2] = 255;
			else
				rgb[0] = rgb[1] = rgb[2] = 0;

			/* Write the RGB triplet */
			fwrite(rgb, 3, 1, fp);
		}

		/* Advance to the next scanline */
		tbuf -= scanlength;
	}

	free(scanline);
	return (1);
}

bool Bitmap::WriteImage_24Bit(FILE *fp)
{
	unsigned char	*scanline, *tbuf;
	unsigned long	x, scanlength;

	/* Write BMP file headers */
	if (fwrite(&bfh, sizeof(bfh), 1, fp) != 1) return (0);
	if (fwrite(&bmiHeader, sizeof(bmiHeader), 1, fp) != 1)
		return (0);

	/* Allocate memory for a temporary scanline */
	scanlength = (bmiHeader.biWidth * 3);
	if (!(scanline = (unsigned char *)malloc(scanlength))) return (0);

	/* Locate the beginning of the last scanline */
	tbuf = buffer + (bmiHeader.biHeight - 1) * scanlength;

	/* Write each scanline at a time, starting at the bottom of the screen
	  (logical top) and working to the top of the screen */
	for (x = 0; x < bmiHeader.biHeight; x++)
	{
		/* Copy scanline */
		memcpy(scanline, tbuf, scanlength);

		/* Attempt to write it to file */
		if (fwrite(scanline, scanlength, 1, fp) != 1)
			{ free(scanline); return (0); }

		/* Advance to the next scanline */
		tbuf -= scanlength;
	}

	free(scanline);
	return (1);
}

bool Bitmap::WriteImage_32Bit(FILE *fp)
{
	unsigned long	*scanline, *tbuf;
	unsigned long	x, scanlength, z;
	unsigned char	rgb[3];

	/* Write BMP file headers */
	fwrite(&bfh, sizeof(bfh), 1, fp);
	fwrite(&bmiHeader, sizeof(bmiHeader), 1, fp);

	/* Allocate memory for a temporary scanline */
	scanlength = bmiHeader.biWidth;
	if (!(scanline = (unsigned long *)malloc(scanlength * 4))) return (0);

	/* Locate the beginning of the last scanline */
	tbuf = (unsigned long *)buffer;
	tbuf += (bmiHeader.biHeight - 1) * scanlength;

	/* Write each scanline at a time, starting at the bottom of the screen
	  (logical top) and working to the top of the screen */
	for (x = 0; x < bmiHeader.biHeight; x++)
	{
		/* Copy scanline */
		memcpy(scanline, tbuf, scanlength * 4);

		/* Attempt to write it to file */
		for (z = 0; z < scanlength; z++)
		{
			rgb[2] = (unsigned char)((scanline[z] >> 16) & 255);
			rgb[1] = (unsigned char)((scanline[z] >> 8) & 255);
			rgb[0] = (unsigned char)(scanline[z] & 255);

			/* Write the RGB triplet */
			fwrite(rgb, 3, 1, fp);
		}

		/* Advance to the next scanline */
		tbuf -= scanlength;
	}

	free(scanline);
	return (1);
}
