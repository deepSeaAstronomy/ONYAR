#include <readDef.h>

uint8_t* read_file()
{
	uint8_t* retBuffer;
	FILE* sceneFile;
	int size;
	
	sceneFile = fopen("sceneDef", "rb");

	fseek(sceneFile, 0, SEEK_END); // getting the length of the file and resetting the stream's position indicator
	size = ftell(sceneFile);
	rewind(sceneFile);	

	retBuffer = (uint8_t*)malloc(sizeof(uint8_t) * size);
	
	fread(retBuffer, sizeof(uint8_t), size, sceneFile);

	fclose(sceneFile);

	return retBuffer;
}


