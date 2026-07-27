#include <writeDef.h>

#define HEADER_SIZE 54

int numberOfPaddingBytes = (4 - ((XRES * 3) % 4)) & 0x03;
int bytesPerRow = (XRES * 3) + numberOfPaddingBytes;
int imageSize = bytesPerRow * YRES;
uint8_t* buffer;
int currentOffset = 0;

void int_to_bytes(int valueToConvert, int numberOfBytes)
{
	for (int byte = 0; byte < numberOfBytes; byte++)
		buffer[currentOffset + byte] = (uint8_t)((valueToConvert >> (byte << 3)) & 0xff); // little endian btw

	currentOffset += numberOfBytes;
}

void write_pixel(color pixelToWrite)
{
	int_to_bytes(pixelToWrite.b,1);
	int_to_bytes(pixelToWrite.g,1);
	int_to_bytes(pixelToWrite.r,1);
}

void generate_header()
{ 	
	buffer[0] = 0x42; // the "magic number"
	buffer[1] = 0x4d;
	
	currentOffset = 2;
	
	int_to_bytes(HEADER_SIZE + imageSize,4);

	int_to_bytes(0,4); // reserved

	int_to_bytes(HEADER_SIZE,4); // offset to data

	int_to_bytes(40,4); // DIB header size

	int_to_bytes(XRES,4);
	int_to_bytes(YRES,4);

	int_to_bytes(1,2); // number of color planes
	int_to_bytes(24,2); // bits per pixel

	int_to_bytes(0,4); // compression type

	int_to_bytes(imageSize,4);
	
	int_to_bytes(0,4); // pixels per meter x
	int_to_bytes(0,4); // pixels per meter y
	
	int_to_bytes(0,4); // number of colors
	int_to_bytes(0,4); // important colors
}

void save_to_bmp()
{
	FILE* image;
	
	image = fopen("out.bmp", "wb");
	fwrite(buffer, sizeof(uint8_t), imageSize + HEADER_SIZE, image);
	fclose(image);
}

void generate_image(color (*color_func)(int, int))
{
	buffer = (uint8_t*)malloc(imageSize * sizeof(uint8_t));
	
	generate_header();
	
	for (int y = 0; y < YRES; y++)
	{
		for (int x = 0; x < XRES; x++)
		{
			write_pixel(color_func(x, y));
		}
		int_to_bytes(0,numberOfPaddingBytes);
	}

	save_to_bmp();

	free(buffer);
}
