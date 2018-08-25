/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cone class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cone.h"
#include <math.h>

/**
* Cone's intersection method.  The input is a ray (pos, dir). 
*/
float Cone::intersect(glm::vec3 posn, glm::vec3 dir)
{
	//As the intersection equation of cone from slides of lecture 9,
	//get the value of t for the ray by solving a quadratic equation
	float vx = posn.x - center.x;
	float vy = height - posn.y + center.y;
	float vz = posn.z - center.z;
	float tan = radius / height;
	
	float a = pow(dir.x, 2) + pow(dir.z, 2) - pow(tan, 2) * pow(dir.y, 2);
	float b = 2 * (dir.x * vx + dir.z * vz) + 2 * pow(tan, 2) * dir.y * vy;
	float c = pow(vx, 2) + pow(vz, 2) - pow(tan, 2) * pow(vy, 2);
	float delta = pow(b, 2) - 4 * a * c;
   
	if(fabs(delta) < 0.001) return -1.0; 
    if(delta < 0.0) return -1.0;

    float t1 = (-b - sqrt(delta)) / (2 * a);
    float t2 = (-b + sqrt(delta)) / (2 * a);
    float t;
    
    if(fabs(t1) < 0.001)
    {
        if (t2 > 0) t = t2;
        else t = -1.0;
    }
    if(fabs(t2) < 0.001 ) t = -1.0;
	t = (t1 < t2)? t1: t2;
	
	float m = posn.y + dir.y * t;
	
	return (center.y <= m) && (m <= height + center.y)? t : -1.0;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the sphere.
*/
glm::vec3 Cone::normal(glm::vec3 p)
{
	float r = sqrt(pow((p.x - center.x), 2) + pow((p.z - center.z), 2));
    glm::vec3 n(p.x - center.x, r * (radius / height), p.z - center.z);
    n = glm::normalize(n);
    return n;
}
