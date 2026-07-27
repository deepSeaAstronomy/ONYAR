#ifndef PRIMDEF
#define PRIMDEF

#include <readDef.h>
#include <vector>

#define MIN_DIST 0.000001f

// XOR was not deemed very important for conventional scene construction and thus omitted
enum compDef {NOT, AND, OR}; 
enum primDef {SPHERE, PLANE, CONE, COMPOUND};
enum renderDef {SPECUAR, DIFFUSE, TEXTURE};

class point
{
    public:
	double x, y, z;
	void set_point_coords(double, double, double);
};

struct line
{
	point direction;
	point origin;
};

struct matDef
{
	color baseColor;
	renderDef renderType;
//	string normalMap;
};

struct returnTrace
{
	int numPoints;
	line* normals;
	bool negativeT; // for convex shapes, this means at least one point of intersection has a negative t
};			// for non-convex shapes (ie. compounds prims and toruses) this means all points of intersection have 
			// a negative t
class basePrim
{
	public:
		matDef material;
		primDef primType; // arguably not needed
		virtual bool point_is_in_volume(point) {return true;};
		virtual returnTrace intersect_with_line(line) {return *(new returnTrace);}; // these functions get immediately overridden
};								      	         	    // by classes derived from basePrim but are still
											    // included here to make g++ shut up in compilation
class sphere : public basePrim
{
	private:
		double radius;
		point center;
	public:
		bool point_is_in_volume(point);
		returnTrace intersect_with_line(line);
		sphere(double, point, matDef);
};

class plane : public basePrim
{
	private:
		line normal;
	public:
		bool point_is_in_volume(point);
		returnTrace intersect_with_line(line);
		plane(line, matDef);
};

class cone : public basePrim
{
	private:
		line axis;
		double ratio;
	public:
		bool point_is_in_volume(point);
		returnTrace intersect_with_line(line);
		cone(line, double, matDef);
};

class pointLight
{
	public:
		point origin;
		color lightColor;
		double radius;
		double intensity;

		pointLight(point, color, double);
};

class envParam
{
	public:
		color sunColor;
		color sunSpecular;
		color shadowColor;
		color skyColor;
		point sunDirection;
};

class compoundPrim : public basePrim
{
	private:
		compDef compType;
		basePrim* mainPrim;
		basePrim* secPrim; 
	public:
		bool point_is_in_volume(point);
		returnTrace intersect_with_line(line);
		compoundPrim(basePrim*, basePrim*, compDef);
};


double dot_prod(point, point); // points have all the info they need to pass for vectors
point cross_prod(point, point);
void normalize_vec(point*);
double vec_length(point);
point vec_proj(point, point);
point operator*(point, double);
point operator+(point, point);
point operator-(point, point);

#endif

