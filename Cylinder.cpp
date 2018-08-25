/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cylinder class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cylinder.h"
#include <math.h>

/**
* Cylinder's intersection method.  The input is a ray (pos, dir). 
*/
float Cylinder::intersect(glm::vec3 posn, glm::vec3 dir)
{
	//As the intersection equation of cylinder from slides of lecture 9,
	//get the value of t for the ray by solving a quadratic equation
	float vx = posn.x - center.x;
	float vz = posn.z - center.z;
	
	float a = pow(dir.x, 2) + pow(dir.z, 2);
	float b = 2 * (dir.x * vx + dir.z * vz);
	float c = pow(vx, 2) + pow(vz, 2) - pow(radius, 2);
	float delta = pow(b, 2) - 4 * a * c;
	
	if(fabs(delta) < 0.001) return -1.0; 
    if(delta < 0.0) return -1.0;

    float t1 = (-b - sqrt(delta)) / (2 * a);
    float t2 = (-b + sqrt(delta)) / (2 * a);
    
    float t;
    
    if(fabs(t1) < 0.001)
    {
        if (t2 > 0) t = t2;
        else t1 = -1.0;
    }
    if(fabs(t2) < 0.001) t2 = -1.0;
    
    t = fmin(t1, t2);
    float s = fmax(t1, t2);
    
    float m1 = posn.y + dir.y * t;  // the y value of closer point of intersection
    float m2 = posn.y + dir.y * s;  // the y value of further point of intersection
    
    float cap_t1 = (center.y + height - posn.y) / dir.y;
    float cap_t2 = (center.y - posn.y) / dir.y;

    if (center.y < m1 && m1 < (height + center.y)) return t;
    
    else if (center.y < m2 && m2 < (height + center.y) && m1 > (center.y + height)) return cap_t1;
	else if (center.y < m2 && m2 < (height + center.y) && m1 < center.y) return cap_t2;
	else return -1.0;
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the sphere.
*/
glm::vec3 Cylinder::normal(glm::vec3 p)
{
	glm::vec3 n;
	float inte_y;
	if (p.y == height + center.y) 
	inte_y = height;
	else if (p.y == center.y)
	inte_y = -height;
	
	n = glm::vec3 (p.x - center.x, 0, p.z - center.z);
    n = glm::normalize(n);
    
    //glm::vec3 n(p.x - center.x, 0, p.z - center.z);
    //n = glm::normalize(n);
    return n;
}
