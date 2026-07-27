#include <primDef.h>

void point::set_point_coords(double x, double y, double z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

double dot_prod(point vec1, point vec2)
{
	return (vec1.x * vec2.x) + (vec1.y * vec2.y) + (vec1.z * vec2.z);
}

point cross_prod(point vec1, point vec2)
{
	point ret;

	ret.x = (vec1.y * vec2.z) - (vec1.z * vec2.y);
	ret.y = (vec1.z * vec2.x) - (vec1.x * vec2.z);
	ret.z = (vec1.x * vec2.y) - (vec1.y * vec2.x);

	return ret;
}

double vec_length(point vec)
{
	return sqrt(dot_prod(vec, vec));
}

point vec_proj(point a, point b)
{
	return b * (dot_prod(a, b) / dot_prod(b, b));
}

void normalize_vec(point* vec)
{
	double len = vec_length(*vec);

	vec->x = vec->x / len;
	vec->y = vec->y / len;
	vec->z = vec->z / len;
}

point operator*(point p, double scalar)	// operator overload for multiplying a vector with a scalar
{
	point retP;

	retP.x = scalar * p.x;
	retP.y = scalar * p.y;
	retP.z = scalar * p.z;

	return retP;
}

point operator+(point p1, point p2)	// operator overload for adding two vectors together 
{
	point retP;

	retP.x = p1.x + p2.x;
	retP.y = p1.y + p2.y;
	retP.z = p1.z + p2.z;

	return retP;
}

point operator-(point p1, point p2)	// operator overload for subtracting two vectors
{
	point retP;

	retP.x = p1.x - p2.x;
	retP.y = p1.y - p2.y;
	retP.z = p1.z - p2.z;

	return retP;
}

returnTrace sphere::intersect_with_line(line ray)
{
	static returnTrace sphereRet;

	if (sphereRet.normals == NULL)
		sphereRet.normals = (line*)malloc(sizeof(line) * 2);

	point coef;
	coef = ray.direction;

	point cons;
	cons = ray.origin - this->center;

	double a = dot_prod(coef, coef);
	double b = -2.0f * dot_prod(coef, cons);
	double c = dot_prod(cons, cons) - (this->radius * this->radius);

	double det = (b*b) - (4*a*c);

	if (det < 0)
	{
		sphereRet.numPoints = 0;
		return sphereRet;
	}
	
	sphereRet.numPoints = 2;

	det = sqrt(det);
	double t0 = (b - det) / (2.0f * a);
	double t1 = (b + det) / (2.0f * a);

	sphereRet.normals[0].origin = ray.origin + (ray.direction * t0);
	sphereRet.normals[0].direction = sphereRet.normals[0].origin - this->center;

	sphereRet.normals[1].origin = ray.origin + (ray.direction * t1);
	sphereRet.normals[1].direction = sphereRet.normals[1].origin - this->center;

	sphereRet.negativeT = (t0 < MIN_DIST) || (t1 < MIN_DIST);

// probably not needed	
/*
	if (abs(t1) < abs(t0))
		{
			point temp = sphereRet.points[1];
			sphereRet.points[1] = sphereRet.points[0];
			sphereRet.points[0] = temp;
		}
*/

	return sphereRet;
}

bool sphere::point_is_in_volume(point pos)
{
	return (vec_length(pos - this->center) < this->radius);
}

sphere::sphere(double radius, point center, matDef material)
{
	this->primType = SPHERE;
	this->radius = radius;
	this->center = center;
	this->material = material;
}


plane::plane(line normalLine, matDef material)
{
	this->normal = normalLine;
	this->primType = PLANE;
	this->material = material;
}

returnTrace plane::intersect_with_line(line ray)
{
	static returnTrace planeRet;

	if (planeRet.normals == NULL)
	{
		planeRet.normals = (line*)malloc(sizeof(line) * 1);
		planeRet.numPoints = 1;
	}
	
	double t = (dot_prod(this->normal.origin, this->normal.direction) - dot_prod(this->normal.direction, ray.origin)) / dot_prod(this->normal.direction, ray.direction);
	
	planeRet.negativeT = (t < MIN_DIST);

	planeRet.normals[0].origin = ray.origin + (ray.direction * t);
	planeRet.normals[0].direction = this->normal.direction;

	return planeRet;
}

bool plane::point_is_in_volume(point pos)
{
	return true;
}

cone::cone(line ax, double ra, matDef material)
{
	this->ratio = ra;
	this->axis = ax;
	this->primType = CONE;
	this->material = material;
}

returnTrace cone::intersect_with_line(line ray)
{
	static returnTrace coneRet;

	if (coneRet.normals == NULL)
		coneRet.normals = (line*)malloc(sizeof(line) * 2);

	double axisDirProd = dot_prod(this->axis.direction, this->axis.direction);
	double coeff0 = (2.0f + ((this->ratio * this->ratio) - 1)) / axisDirProd; 
	point cons = ray.origin - this->axis.origin;

	point coeff1; 
	coeff1.x = 1 - ((this->axis.direction.x * this->axis.direction.x) * coeff0); 
	coeff1.y = 1 - ((this->axis.direction.y * this->axis.direction.y) * coeff0); 
	coeff1.z = 1 - ((this->axis.direction.z * this->axis.direction.z) * coeff0); 

	point coeff2;
	coeff2.x = 2.0f * coeff0 * (this->axis.direction.x * this->axis.direction.y);
	coeff2.y = 2.0f * coeff0 * (this->axis.direction.x * this->axis.direction.z);
	coeff2.z = 2.0f * coeff0 * (this->axis.direction.y * this->axis.direction.z);



	point coeff3;
	coeff3.x = ray.direction.x * ray.direction.x;
	coeff3.y = ray.direction.y * ray.direction.y;
	coeff3.z = ray.direction.z * ray.direction.z;

	point coeff4;
	coeff4.x = ray.direction.x * ray.direction.y; 
	coeff4.y = ray.direction.x * ray.direction.z; 
	coeff4.z = ray.direction.y * ray.direction.z; 

	double a = dot_prod(coeff1, coeff3) - dot_prod(coeff2, coeff4);



	coeff4.x = (cons.x * ray.direction.y) + (cons.y * ray.direction.x);
	coeff4.y = (cons.x * ray.direction.z) + (cons.z * ray.direction.x);
	coeff4.z = (cons.z * ray.direction.y) + (cons.y * ray.direction.z);

	coeff3.x = 2.0f * ray.direction.x * cons.x;
	coeff3.y = 2.0f * ray.direction.y * cons.y;
	coeff3.z = 2.0f * ray.direction.z * cons.z;

	double b = dot_prod(coeff1, coeff3) - dot_prod(coeff2, coeff4);

	
	
	coeff3.x = cons.x * cons.x;
	coeff3.y = cons.y * cons.y;
	coeff3.z = cons.z * cons.z;

	coeff4.x = cons.x * cons.y;
	coeff4.y = cons.x * cons.z;
	coeff4.z = cons.z * cons.y;

	double c = dot_prod(coeff1, coeff3) - dot_prod(coeff2, coeff4);



	double det = (b*b) - (4*a*c);

	if (det < 0)
	{
		coneRet.numPoints = 0;
		return coneRet;
	}
	
	coneRet.numPoints = 2;

	det = sqrt(det);
	double t0 = ((-1.0f * b) - det) / (2.0f * a);
	double t1 = ((-1.0f * b) + det) / (2.0f * a);

	coneRet.normals[0].origin = ray.origin + (ray.direction * t0);
	point axisRelativeIntersection = coneRet.normals[0].origin - this->axis.origin;
	point projection1 = vec_proj(axisRelativeIntersection, this->axis.direction);
	coneRet.normals[0].direction = vec_proj(projection1, axisRelativeIntersection) - projection1;

	coneRet.normals[1].origin = ray.origin + (ray.direction * t1);
	axisRelativeIntersection = coneRet.normals[1].origin - this->axis.origin;
	projection1 = vec_proj(axisRelativeIntersection, this->axis.direction);
	coneRet.normals[1].direction = vec_proj(projection1, axisRelativeIntersection) - projection1;

	coneRet.negativeT = (t0 < MIN_DIST) && (t1 < MIN_DIST);

	if ((t0 < 1) && (!coneRet.negativeT))
	{
		line temp = coneRet.normals[0];
		coneRet.normals[0] = coneRet.normals[1];
		coneRet.normals[1] = temp;
	}

	return coneRet;
}

bool cone::point_is_in_volume(point pos)
{
	return true;
}

bool compoundPrim::point_is_in_volume(point pos)
{
	//PLACEHOLDER	
	return true;
}

returnTrace compoundPrim::intersect_with_line(line ray) // only supports the NOT case for now
{
	static returnTrace compRet;

	if (compRet.normals == NULL)
		compRet.normals = (line*)malloc(sizeof(line) * 1); // placeholder

	returnTrace traceMain = this->mainPrim->intersect_with_line(ray); 
	int sizeMain = sizeof(line) * traceMain.numPoints;
	line* normalMain = (line*)malloc(sizeMain); 
	memcpy(normalMain, traceMain.normals, sizeMain);
	

	returnTrace traceSec = this->secPrim->intersect_with_line(ray);
	int sizeSec = sizeof(line) * traceSec.numPoints;
	line* normalSec = (line*)malloc(sizeSec); 
	memcpy(normalSec, traceSec.normals, sizeSec);

	int volIndexMain = -1;
	for (int curP = 0; curP < traceMain.numPoints; curP++)
	{
		if (this->secPrim->point_is_in_volume(normalMain[curP].origin))
			volIndexMain = curP;
	}

	// the below check is irrelevant if we're *inside* the primitives doing a secondary trace
	if ((traceMain.numPoints == 0) || (traceSec.numPoints == 0) || ((volIndexMain == -1) && (traceMain.numPoints != 0) && (traceSec.numPoints != 0)))
	{
		this->material = this->mainPrim->material;
		
		compRet = traceMain;
		compRet.normals[0] = normalMain[0];
		return compRet; 
	}
	
	this->material = this->secPrim->material;

	int volIndexSec = -1;
	for (int curP = 0; curP < traceSec.numPoints; curP++)
	{
		if (this->mainPrim->point_is_in_volume(normalSec[curP].origin)) // assumes traceSec's normals are sorted
			volIndexSec = curP;
	}

	this->material = this->secPrim->material;
 
	compRet.normals[0].origin = traceSec.normals[volIndexSec].origin;
	compRet.normals[0].direction = traceSec.normals[volIndexSec].direction * -1.0f;
	compRet.numPoints = (volIndexSec != -1);
						 
	point rayTrunc = compRet.normals[0].origin - ray.origin;

	compRet.negativeT = (dot_prod(rayTrunc, ray.direction) < 0.0f) || (vec_length(rayTrunc) < MIN_DIST);

	free(normalMain);
	free(normalSec);
	return compRet;
}

compoundPrim::compoundPrim(basePrim* a, basePrim* b, compDef type)
{
	this->compType = type;
	this->mainPrim = a;
	this->secPrim = b;
	this->primType = COMPOUND;
	this->material = a->material;
}





