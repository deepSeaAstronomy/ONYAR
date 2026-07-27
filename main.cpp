#include <writeDef.h>
#include <traceDef.h>

int main()
{
	parse_scene();

	generate_image(trace_ray);




	uint8_t* pointer = read_file();
	printf("\n");
	for(int i = 0; i < 5; i++)
		printf("%c", (char)pointer[i]);
	printf("\n");
}


