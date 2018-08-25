/*========================================================================
* COSC 363  Computer Graphics (2018)
* Assignment2 Ray Tracer
* Student name: Xiao Meng
* Due date: 23:55 31/05/2018
*=========================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include <GL/glut.h>
#include "Plane.h"
#include "TextureBMP.h"
#include "Triangle.h"
#include "Cylinder.h"
#include "Cone.h"

using namespace std;

const float WIDTH = 20.0;
const float HEIGHT = 20.0;
float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;

//-- global variables for map texture
TextureBMP texture_sphere, texture_ft, texture_bk, texture_lf, texture_rt, texture_up, texture_dn, texture_floor;
float x = 15.0, y = -25.0, z = -140.0, size = 10.0;  //size for the textured cube

glm::vec3 eye(0., 0., 0.);  //The eye position (source of primary rays) is the origin

vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene


//---The most important function in a ray tracer! ----------------------------------
//   Computes the colour value obtained by tracing a ray and finding its
//   closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)  //ray is the primary ray, the position and direction will define in display()
{
    glm::vec3 sumCol;
    glm::vec3 backgroundCol(0);
    glm::vec3 light1(10, 40, 15);  //Light source 1
    glm::vec3 light2(-40, 100, -100);  //Light source 2
    glm::vec3 ambientCol(0.2);   //Ambient color of light 1
    glm::vec3 ambientCol2(0.1);   //Ambient color of light 2

    glm::vec3 col1; //pixel color for light 1
    glm::vec3 col2; //pixel color for light 2

    ray.closestPt(sceneObjects);        //Compute the closest point of intersetion of objects with the primary ray

    if(ray.xindex == -1) return backgroundCol; //If there is no intersection return background colour
    
    glm::vec3 materialCol = sceneObjects[ray.xindex]->getColor(); //else return object's colour
    glm::vec3 proc_materialCol(0.7, 0., 0.);
    
    if (ray.xindex == 15)   //procudural pattern texture with cylinder
    {
		if (int(ray.xpt.y) % 3 == 0) materialCol = proc_materialCol;
	}

    glm::vec3 lightVector1 = light1 - ray.xpt;  //vector of light ray 1
    glm::vec3 l1 = normalize(lightVector1);
    glm::vec3 lightVector2 = light2 - ray.xpt;  //vector of light ray 2
    glm::vec3 l2 = normalize(lightVector2);

    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt); //normal vector on the point of intersection of primary ray and object
    glm::vec3 n = normalize(normalVector);
     //-- Texture of the cube made up by planes from index 3 to 8

    if(ray.xindex > 0 && ray.xindex < 14)
    {
        float texcoords_sphere = n.x /(2 * M_PI) + 0.5;
        float texcoordt_sphere = n.y /(2 * M_PI) + 0.5;
        switch(ray.xindex)
        {
            case(1):
            materialCol = texture_sphere.getColorAt(texcoords_sphere, texcoordt_sphere);
            break;

            case(5):
            {
            float texcoords = (ray.xpt.x - x) / size;
            float texcoordt = (ray.xpt.y - y) / size;
            materialCol = texture_ft.getColorAt(texcoords, texcoordt);
            break;
            }

            case(6):
            {
            float texcoords = (ray.xpt.x - (x + size)) / size;
            float texcoordt = (ray.xpt.y - y) / size;
            materialCol = texture_bk.getColorAt(texcoords, texcoordt);
            break;
            }

            case(7):
            {
            float texcoords = (ray.xpt.x - (x + size)) / size;
            float texcoordt = (ray.xpt.y - y) / size;
            materialCol = texture_lf.getColorAt(texcoords, texcoordt);
            break;
            }

            case(8):
            {
            float texcoords = (ray.xpt.x - x) / size;
            float texcoordt = (ray.xpt.y - y) / size;
            materialCol = texture_rt.getColorAt(texcoords, texcoordt);
            break;
            }

            case(9):
            {
            float texcoords = (ray.xpt.x - x) / size;
            float texcoordt = (ray.xpt.z - (z - size)) / size;
            materialCol = texture_up.getColorAt(texcoords, texcoordt);
            break;
            }

            case(10):
            {
            float texcoords = (ray.xpt.x - (x + size)) / size;
            float texcoordt = (ray.xpt.z - (z - size)) / size;
            materialCol = texture_dn.getColorAt(texcoords, texcoordt);
            break;
            }
        }
    }

    //-----------------diffuse reflection---------------------
    float lDotn1 = glm::dot(l1, n);
    float lDotn2 = glm::dot(l2, n);

    if (lDotn1 < 0) lDotn1 = 0.0; //lDotn < 0 means on the back side of light source
    col1 = ambientCol * materialCol + lDotn1 * materialCol;//return the color value of diffuse reflection for the point of intersection

    if (lDotn2 < 0) lDotn2 = 0.0; //lDotn < 0 means on the back side of light source
    col2 = ambientCol2 * materialCol + lDotn2 * materialCol;//return the color value of diffuse reflection for the point of intersection

    //-----------------specular reflection---------------------
    glm::vec3 viewVector = eye - ray.xpt;  //vector from the point of intersection to eye, constract to the real view ray
    glm::vec3 v = normalize(viewVector);

    glm::vec3 reflVector1 = glm::reflect(-lightVector1, normalVector);
    glm::vec3 r1 = normalize(reflVector1);
    glm::vec3 reflVector2 = glm::reflect(-lightVector2, normalVector);
    glm::vec3 r2 = normalize(reflVector2);

    float rDotv1 = glm::dot(r1, v);
    float rDotv2 = glm::dot(r2, v);
    float spec1 = pow(rDotv1, 10); //Phone constant
    float spec2 = pow(rDotv2, 10);

    if (rDotv1 < 0) spec1 = 0.0;
    col1 = col1 + spec1 * glm::vec3(1.0f);  //return the color value of specular reflection of the point of intersection
    if (rDotv2 < 0) spec2 = 0.0;
    col2 = col2 + spec2 * glm::vec3(1.0f);

    ////-- shadow ray
    Ray shadow1 = Ray(ray.xpt, l1); //generate a shadow ray form the point of intersection on blue ball
                                               //to the light source, check if there is any objects on the way
    shadow1.closestPt(sceneObjects);  //get all functions of the point of intersection for the shadow ray on another ball if found.

    Ray shadow2 = Ray(ray.xpt, l2);
    shadow2.closestPt(sceneObjects);

    float lightDist1 = glm::length(lightVector1);
    float lightDist2 = glm::length(lightVector2);

    //-- Shadow ray for the transparent object

    glm::vec3 t_materialCol = sceneObjects[ray.xindex == 3]->getColor(); //else return object's colour

    if (shadow1.xindex == 3 && shadow1.xdist < lightDist1)
    col1 = glm::vec3(0.4f) * t_materialCol;

    if (shadow1.xindex == 2 && shadow1.xdist < lightDist1)
    col1 = glm::vec3(0.4f) * materialCol;

    if ((shadow1.xindex > -1) && (shadow1.xdist < lightDist1) && (shadow1.xindex != 3 && shadow1.xindex != 2))
    col1 = ambientCol * materialCol;

    if (shadow2.xindex == 3 && shadow2.xdist < lightDist2)
    col2 = glm::vec3(0.4f) * t_materialCol;

    if (shadow2.xindex == 2 && shadow2.xdist < lightDist2)
    col1 = glm::vec3(0.4f) * materialCol;

    if ((shadow2.xindex > -1) && (shadow2.xdist < lightDist2) && (shadow2.xindex != 3 && shadow2.xindex != 2))
    col2 = ambientCol * materialCol;

    sumCol = col1 + col2;

    //-- Reflection of objects
    if((ray.xindex == 0 || ray.xindex == 3) && step < MAX_STEPS)  //reflective surface with sphere 0 and transparent sphere
    {
        glm::vec3 reflectedDir = glm::reflect(ray.dir, n);  //reflectedDir is the reflection vector for primary ray
        Ray reflectedRay(ray.xpt, reflectedDir);  //make the reflection vector to a ray
        glm::vec3 reflectedCol = trace(reflectedRay, step + 1);  //recurisively trace the reflection ray
        sumCol = sumCol + (1.5f * reflectedCol);  //add the portion of reflected color value to the sum color value
    }

    //-- Refraction objects
    if (ray.xindex == 2 && step < MAX_STEPS)
    {
        float eta = 1/1.5;  //refracted to a glass sphere
        glm::vec3 g = glm::refract(ray.dir, n, eta);
        Ray refrRay1(ray.xpt, g);
        refrRay1.closestPt(sceneObjects);
        glm::vec3 m = sceneObjects[refrRay1.xindex]->normal(refrRay1.xpt);
        glm::vec3 h = glm::refract(g, -m, 1.0f/eta);
        Ray refrRay2(refrRay1.xpt, h);
        glm::vec3 refractedCol = trace(refrRay2, step + 1);
        sumCol = 1.0f * refractedCol + spec1 * glm::vec3(1.0f) + spec2 * glm::vec3(1.0f);
    }

    //-- Transparency objects
    if (ray.xindex == 3 && step < MAX_STEPS)
    {
        Ray refrRay1(ray.xpt, ray.dir);
        refrRay1.closestPt(sceneObjects);

        Ray refrRay2(refrRay1.xpt, ray.dir);
        glm::vec3 refractedCol;
        if (refrRay1.xdist < 0.2)
        refractedCol = trace(refrRay1, step + 1);
        else
        refractedCol = trace(refrRay2, step + 1);

        sumCol += refractedCol + spec1 * glm::vec3(1.0f) + spec2 * glm::vec3(1.0f);
    }
    return sumCol;
}

void special(int key, int x, int y)
{
    if (key == GLUT_KEY_UP) EDIST += 25;
    else if (key == GLUT_KEY_DOWN) EDIST -= 25;

    glutPostRedisplay();
}

//---These parts of function create a series of objects-----------------------------
//   Specifically, they create scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//----------------------------------------------------------------------------------
void createSphere()
{
    //-- Create a pointer to a sphere object
    Sphere *sphere1 = new Sphere(glm::vec3(-5.0, 13.0, -180.0), 30.0, glm::vec3(0.17, 0.11, 0.72));//sphere(centre, radius, color)
    Sphere *sphere2 = new Sphere(glm::vec3(12.0, 20.0, -100.0), 5.0, glm::vec3(0, 0, 0.5));
    Sphere *sphere3 = new Sphere(glm::vec3(-22.0, -23.0, -120.0), 5.0, glm::vec3(0, 0.12, 0.11));
    Sphere *sphere4 = new Sphere(glm::vec3(10.0, -18.0, -100.0), 5.0, glm::vec3(0, 0, 0.5));

    //-- Add the above to the list of scene objects.
    sceneObjects.push_back(sphere1);//and the sphere to the last of the list
    sceneObjects.push_back(sphere2);
    sceneObjects.push_back(sphere3);
    sceneObjects.push_back(sphere4);
}


void createPlane()
{
    //-- Create a Plane as a floor
    Plane *plane1 = new Plane(glm::vec3(-50.0, -30.0, -40.0),  //point A
                              glm::vec3(50., -30, -40), //Point B
                              glm::vec3(50., -30, -350), //Point C
                              glm::vec3(-50., -30, -350), //Point D
                              glm::vec3(0.5, 0.5, 0.5)); //Colour

    //-- Add planes to the scene objects list
    sceneObjects.push_back(plane1);
}


void createBox()
{
    //-- Create a box using 6 planes
    Plane *box_ft = new Plane(glm::vec3(x, y, z), glm::vec3(x + size, y, z), glm::vec3(x + size, y + size, z), glm::vec3(x, y + size, z), glm::vec3(0)); // front
    Plane *box_bk = new Plane(glm::vec3(x + size, y, z - size), glm::vec3(x, y, z - size), glm::vec3(x, y + size, z - size), glm::vec3(x + size, y + size, z - size), glm::vec3(0));  // back
    Plane *box_lf = new Plane(glm::vec3(x + size, y, z), glm::vec3(x + size, y, z - size), glm::vec3(x + size, y + size, z - size), glm::vec3(x + size, y + size, z), glm::vec3(0)); //left
    Plane *box_rt = new Plane(glm::vec3(x, y, z - size), glm::vec3(x, y, z), glm::vec3(x, y + size, z), glm::vec3(x, y + size, z - size), glm::vec3(0)); // right
    Plane *box_up = new Plane(glm::vec3(x, y + size, z), glm::vec3(x + size, y + size, z), glm::vec3(x + size, y + size, z - size), glm::vec3(x, y + size, z - size), glm::vec3(0));  // up
    Plane *box_dn = new Plane(glm::vec3(x + size, y, z), glm::vec3(x, y, z), glm::vec3(x, y + size, z), glm::vec3(x, y, z - size), glm::vec3(0)); //down

    //-- Add the Planes to the list of scene objects
    sceneObjects.push_back(box_ft);
    sceneObjects.push_back(box_bk);
    sceneObjects.push_back(box_lf);
    sceneObjects.push_back(box_rt);
    sceneObjects.push_back(box_up);
    sceneObjects.push_back(box_dn);
}


void createTetrahedron()
{
    //-- Vertices of tetrahedron
    glm::vec3 pta(-28, 10, -120), ptb(-20, 7, -115), ptc(-16, 12, -125), ptd(-22, 24, -122);
    glm::vec3 col1(0.24, 0.85, 0.93);
    glm::vec3 col2(0.91, 0.56, 0.57);
    glm::vec3 col3(0.96, 0.73, 0.85);
    glm::vec3 col4(0.33, 0.46, 0.73);
    
    //-- Create a Tetrahedron by make four triangle plane together
    Triangle *triangle1 = new Triangle(pta, ptb, ptc, col3);
    Triangle *triangle2 = new Triangle(ptb, ptc, ptd, col2);
    Triangle *triangle3 = new Triangle(ptc, ptd, pta, col4);
    Triangle *triangle4 = new Triangle(ptd, pta, ptb, col1);

    //-- Add tetrahedron to the scene objects list
    sceneObjects.push_back(triangle1);
    sceneObjects.push_back(triangle2);
    sceneObjects.push_back(triangle3);
    sceneObjects.push_back(triangle4);
}


void createCylinder()
{
    //-- Draw the sides of cylinder
    Cylinder *cylinder = new Cylinder(glm::vec3(-3.0, -30.0, -130.0), 5.0, 14.5, glm::vec3(0., 0.6, 0.4));
    sceneObjects.push_back(cylinder);
}


void createCone()
{
    //-- Draw the sides of cone
    Cone *cone = new Cone(glm::vec3(15.0, -3.0, -120.0), 5.0, 12.0, glm::vec3(0.82, 0.16, 0.28));
    sceneObjects.push_back(cone);
}


//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
// Divided a quad into 4 for Anti-aliasing
//--------------------------------------------------------------------------------------
void display()
{
    float xp, yp;  //grid point
    float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
    float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glBegin(GL_QUADS);  //Each cell is a quad.

    for(int i = 0; i < NUMDIV; i++)     //For each grid point xp, yp
    {
        xp = XMIN + i * cellX;
        for(int j = 0; j < NUMDIV; j++)
        {
            yp = YMIN + j * cellY;

            //-- Anti-Aliasing generate 4 primary rays go through divided 4 pixel cells
            glm::vec3 dir1(xp + 0.25 * cellX, yp + 0.25 * cellY, -EDIST);   //direction of the primary ray
            glm::vec3 dir2(xp + 0.75 * cellX, yp + 0.25 * cellY, -EDIST);
            glm::vec3 dir3(xp + 0.25 * cellX, yp + 0.75 * cellY, -EDIST);
            glm::vec3 dir4(xp + 0.75 * cellX, yp + 0.75 * cellY, -EDIST);

            Ray ray1 = Ray(eye, dir1);      //Create a ray originating from the camera in the direction 'dir'
            Ray ray2 = Ray(eye, dir2);
            Ray ray3 = Ray(eye, dir3);
            Ray ray4 = Ray(eye, dir4);

            ray1.normalize();               //Normalize the direction of the ray to a unit vector
            ray2.normalize();
            ray3.normalize();
            ray4.normalize();

            glm::vec3 col1 = trace (ray1, 1); //Trace the primary ray and get the colour value
            glm::vec3 col2 = trace (ray2, 1);
            glm::vec3 col3 = trace (ray3, 1);
            glm::vec3 col4 = trace (ray4, 1);

            float col_r = (col1.r + col2.r + col3.r + col4.r) / 4;
            float col_g = (col1.g + col2.g + col3.g + col4.g) / 4;
            float col_b = (col1.b + col2.b + col3.b + col4.b) / 4;

            glColor3f(col_r, col_g, col_b);

            //-- Without anti-aliasing to generate graph faster for testing
            /*glm::vec3 dir(xp + 0.5 * cellX, yp + 0.5 * cellY, -EDIST);
            Ray ray = Ray(eye, dir);
            ray.normalize();
            glm::vec3 col = trace (ray, 1);
            glColor3f(col.r, col.g, col.b);*/

            //-- Draw each cell with its color value
            glVertex2f(xp, yp);
            glVertex2f(xp + cellX, yp);
            glVertex2f(xp + cellX, yp + cellY);
            glVertex2f(xp, yp + cellY);
        }
    }
    glEnd();
    glFlush();
}


//---This function initializes the scene -------------------------------------------
//
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);

    //-- bind pictures.bmp to textures
    texture_sphere = TextureBMP((char* const) "UFO.bmp");
    texture_ft = TextureBMP((char* const) "side.bmp");
    texture_bk = TextureBMP((char* const) "side.bmp");
    texture_lf = TextureBMP((char* const) "side.bmp");
    texture_rt = TextureBMP((char* const) "side.bmp");
    texture_up = TextureBMP((char* const) "up.bmp");
    texture_dn = TextureBMP((char* const) "dn.bmp");


    glClearColor(0, 0, 0, 1);
    createSphere();  // index 0 - 3
    createPlane();  // index 4
    createBox();  //index 5 - 10
    createTetrahedron();  // index 11 - 14
    createCylinder();  // index 15
    createCone();  // index 16
}


//--The main function of this program ----------------------------------------------
int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracer");

    glutSpecialFunc(special);
    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
