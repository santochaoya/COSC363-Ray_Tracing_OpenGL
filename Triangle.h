/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Triangle class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#ifndef H_TRIANGLE
#define H_TRIANGLE

#include <glm/glm.hpp>
#include "SceneObject.h"

class Triangle : public SceneObject
{
private:
    glm::vec3 a, b, c, d;  //The four vertices

public:	
	Triangle(void);
	
    Triangle(glm::vec3 pa, glm::vec3 pb, glm::vec3 pc, glm::vec3 col)
		: a(pa), b(pb), c(pc)
	{
		color = col;
	};

	bool isInside(glm::vec3 pt);
	
	float intersect(glm::vec3 posn, glm::vec3 dir);
	
	glm::vec3 normal(glm::vec3 pt);

};

#endif //!H_TRIANGLE
