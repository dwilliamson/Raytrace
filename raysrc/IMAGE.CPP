
#include <stdio.h>
#include "ptc.h"
#include "types.h"
#include "classes.h"


#pragma pack(push, 1)

typedef struct
{
	unsigned char	Manufacturer;
	unsigned char	Version;
	unsigned char	Encoding;
	unsigned char	BitsPerPixel;
	unsigned short	XMin, YMin;
	unsigned short	XMax, YMax;
	unsigned short	HRes, VRes;
	unsigned char	ColourMap[48];
	unsigned char	Reserved;
	unsigned char	NPlanes;
	unsigned short	BytesPerLine;
	unsigned short	PaletteInfo;
	unsigned char	Filler[58];
} PCXHeader;


int Image::Load(char *filename)
{
	FILE			*fp;
	unsigned char	*file_data, data;
	unsigned long	size;
	PCXHeader		*header;
	unsigned char	*image_data, *line_store, *bls;
	int				current_line, total_bytes, sub_total, len, x;
	int32			*pt;
	int32			r, g, b;

	if ((fp = fopen(filename, "rb")) == NULL)
		return (0);

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if ((file_data = new unsigned char[size]) == NULL)
	{
		fclose(fp);
		return (0);
	}

	if (fread(file_data, 1, size, fp) != size)
	{
		delete file_data;
		fclose(fp);
		return (0);
	}

	header = (PCXHeader *)file_data;
	image_data = file_data + 128;

	if (header->Manufacturer != 10)
	{
		delete file_data;
		fclose(fp);
		return (0);
	}

	if (header->BitsPerPixel * header->NPlanes != 24)
	{
		delete file_data;
		fclose(fp);
		return (0);
	}

	width = header->XMax - header->XMin + 1;
	height = header->YMax - header->YMin + 1;

	current_line = height;
	total_bytes = header->NPlanes * header->BytesPerLine;
	bls = new unsigned char[total_bytes];

	pt = buffer = new int32[width * height];

	while (current_line)
	{
		sub_total = 0;
		line_store = bls;

		while (sub_total < total_bytes)
		{
			data = *image_data++;

			if (data >= 192)
			{
				len = data & 0x3F;
				data = *image_data++;
			}
			else len = 1;

			while (len--) line_store[sub_total++] = data;
		}

		for (x = 0; x < header->BytesPerLine; x++)
		{
			r = bls[x];
			g = bls[header->BytesPerLine + x];
			b = bls[(header->BytesPerLine << 1) + x];

			*pt++ = b | (g << 8) | (r << 16);
		}

		pt -= header->BytesPerLine;
		pt += 256;

		current_line--;
	}

	delete file_data;
	delete bls;

	fclose(fp);

	return (1);
}


void Image::Release(void)
{
	delete buffer;
	buffer = NULL;
}