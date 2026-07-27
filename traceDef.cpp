#include <traceDef.h>

//PLACEHOLDER, each bounding volume will have its own vector<>
std::vector<basePrim*> global;

envParam env;
double fovScaleFactor = tan(FOV * 0.0087266f);

struct traceStatus
{
	bool success;
	int nearestPrimIndex; // maybe remove
	line nearestNormal;
	double dist;
	matDef mat;
};

void parse_scene()
{
//PLACEHOLDER

	srand(time(NULL));

	point c;

	color col; col.r = 240; col.g = 240; col.b = 240;
	env.sunColor = col;
	col.r = 255; col.g = 255; col.b = 255;
	env.sunSpecular = col;
	col.r = 0; col.g = 7; col.b = 18;
	env.shadowColor = col;
	col.r = 255; col.g = 152; col.b = 25;
	env.skyColor = col;
	env.sunDirection.set_point_coords(5.0f, -1.0f, -1.0f);
	normalize_vec(&env.sunDirection);
	env.sunDirection = (env.sunDirection * -1.0f);


	matDef matty; matty.baseColor.r = 250; matty.baseColor.g = 0; matty.baseColor.b = 0;
	matDef matty2; matty2.baseColor.r = 0; matty2.baseColor.g = 250; matty2.baseColor.b = 0;
	c.set_point_coords(0.0f, 6.0f, 0.0f);
	point c2; c2.set_point_coords(0.0f, 5.0f, 1.0f);
	global.push_back(new compoundPrim(new sphere(1.5f, c, matty), new sphere(1.5f, c2, matty2), NOT));

	matty.baseColor.r = 0; matty.baseColor.g = 250; matty.baseColor.b = 250;
	c.set_point_coords(0.0f, 6.0f, 0.0f);
	global.push_back(new sphere(0.3f,c, matty));


/*
	matDef matty; matty.baseColor.r = 250; matty.baseColor.g = 0; matty.baseColor.b = 0;
	c.set_point_coords(-1.5f, 6.0f, -2.0f);
	global.push_back(new sphere(1.5f,c, matty));

	matty.baseColor.r = 0; matty.baseColor.g = 250; matty.baseColor.b = 0;
	c.set_point_coords(-0.5f, 5.0f, -0.21f);
	global.push_back(new sphere(0.3f,c, matty));

	matty.baseColor.r = 0; matty.baseColor.g = 0; matty.baseColor.b = 250;
	c.set_point_coords(1.5f, 6.8f, 0.0f);
	global.push_back(new sphere(2.5f,c, matty));

	matty.baseColor.r = 250; matty.baseColor.g = 250; matty.baseColor.b = 250;
	line normal; normal.origin.set_point_coords(0.0f, 0.0f, -2.0f); normal.direction.set_point_coords(0.0f, 0.0f, 1.0f);
	global.push_back(new plane(normal, matty));

	matty.baseColor.r = 250; matty.baseColor.g = 250; matty.baseColor.b = 0;
	line axis; axis.origin.set_point_coords(-0.5f, 5.0f, -0.21f); axis.direction.set_point_coords(0.0f, 1.0f, 1.0f);
	global.push_back(new cone(axis, 0.2f, matty));
*/
}

traceStatus find_shortest_trace(line ray)
{
	traceStatus retStat; 
	int primIndex = 0;
	double currentLowest = 1337.0f;
	bool firstIteration = true;
	double currentDist;
	returnTrace currentTrace;

	while (primIndex < global.size())
	{
		currentTrace = global.at(primIndex)->intersect_with_line(ray);

		if ((currentTrace.numPoints != 0) && (!currentTrace.negativeT))
		{
			currentDist = vec_length(currentTrace.normals[0].origin - ray.origin);

			if ((currentLowest > currentDist) || firstIteration)
			{
				currentLowest = currentDist;
				firstIteration = false;
				retStat.nearestPrimIndex = primIndex;
				retStat.nearestNormal = currentTrace.normals[0];
				retStat.dist = currentDist;

				retStat.mat = global.at(primIndex)->material;
			}
		}
		primIndex++;
	}

	retStat.success = !firstIteration;

	return retStat;
}	

point apply_random_rotation(point vec)	// this looks pretty bad but it's preferable to the glut of 
{					// trig operations and multiplications inherent in using a rotation matrix
	point retPoint;
					
	do 
	{
		retPoint.x = (rand() % 20) - 10;
		retPoint.y = (rand() % 20) - 10;
		retPoint.z = (rand() % 20) - 10;
	}
	while(!(dot_prod(vec, retPoint) > 0.0f));

	return retPoint;
}

color get_color (line ray, traceStatus stat, int tracesLeft)
{
	bool shadow;
	double diffuseDot;
	double specularDot;
	double directLight = 1.0f;
	double indirectLight = 1.0f;
	point surfNormal;
	point incident;
	point reflected;
	color ambients[MAX_SAMPLES];
	color globalColor;
	color retCol;
	traceStatus currTrace;



	surfNormal = stat.nearestNormal.direction;
	normalize_vec(&surfNormal);
	diffuseDot = dot_prod(surfNormal, env.sunDirection);

	incident = ray.direction * -1.0f;
	reflected = (vec_proj(env.sunDirection, surfNormal) * 2.0f) - env.sunDirection;
	normalize_vec(&incident); normalize_vec(&reflected);
	specularDot = dot_prod(incident, reflected);

	line newTrace; newTrace.origin = stat.nearestNormal.origin; newTrace.direction = env.sunDirection;
	shadow = find_shortest_trace(newTrace).success || (dot_prod(surfNormal, newTrace.direction) < 0.0f);



	if (diffuseDot < 0.0f) diffuseDot = 0.0f;
	if (specularDot < 0.0f) specularDot = 0.0f;
	specularDot = pow(specularDot, 20) * 0.4f;

	color diffCol = env.sunColor * stat.mat.baseColor;
	color specCol = env.sunSpecular * stat.mat.baseColor;

	double specularAmount = 1.0f - (shadow * 0.15f);
	double shadowAmount = shadow * 0.15f;
	double diffuseAmount = 1.0f - (shadow * 0.15f);

//	if (tracesLeft != MAX_TRACES)
	retCol = clamp_color((env.shadowColor * (shadowAmount)) + ((diffCol * diffuseDot) * diffuseAmount) + ((specCol * specularDot) * specularAmount));
//	else
//		retCol.r = 0; retCol.g = 0; retCol.b = 0;

	if (false) //(tracesLeft > 0)
	{
		for (int traces = 0; traces < MAX_SAMPLES; traces++)
		{
			stat.nearestNormal.direction = apply_random_rotation(surfNormal);
			currTrace = find_shortest_trace(stat.nearestNormal);

			if (currTrace.success)
				ambients[traces] = get_color(stat.nearestNormal, currTrace, tracesLeft - 1) * (1.0f / pow(currTrace.dist + 1, 2));
			else
				ambients[traces] = get_color(ray, stat, 0);
		}
		
//		globalColor = average_color(ambients, MAX_SAMPLES);

//		retCol = clamp_color((retCol * 0.60) + (globalColor * 0.40f));
		
		retCol = clamp_color((retCol + (average_color(ambients, MAX_SAMPLES) * 2.0f)) * (1 / 2.0f));

	}

	return retCol;
}

color trace_ray(int x, int y)
{
// PLACEHOLDER
	line ray;
	ray.origin.set_point_coords(0.0f, 0.0f, 0.0f);

	double deltaZ;
	double deltaX;

	if (XRES > YRES)
	{
		deltaZ = (2.0f * (double(y) / XRES)) - (double(YRES) / XRES);
		deltaX = 2.0f * (double(x) / XRES) - 1.0f;
	}
	else if (YRES > XRES)
	{
		deltaX = (2.0f * (double(x) / YRES)) - (double(XRES) / YRES);
		deltaZ = 2.0f * (double(y) / YRES) - 1.0f;
	}
	else
	{
		deltaX = 2.0f * (double(x) / XRES) - 1.0f;
		deltaZ = 2.0f * (double(y) / YRES) - 1.0f;
	}
	
	deltaX *= fovScaleFactor; deltaZ *= fovScaleFactor;

	ray.direction.set_point_coords(deltaX, 1.0f, deltaZ);

	traceStatus firstPassStatus = find_shortest_trace(ray);

	if (!firstPassStatus.success)
		return env.skyColor;
	else
		return get_color(ray, firstPassStatus, MAX_TRACES);	
}

