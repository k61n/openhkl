#include <ctime>
#include <cstdlib>
#include <iostream>
#include <set>
#include <stdlib.h>
#include <unordered_map>
#include <vector>

#include <GL/glut.h>

#include <Eigen/Dense>

#include "zpr.h"

#include "AABB.h"
#include "Blob3D.h"
#include "Blob3DFinder.h"
#include "MMILLAsciiReader.h"
#include "NDTree.h"

#define VIEWING_DISTANCE_MIN  3.0

using namespace std;
using namespace SX::Geometry;

static bool g_bButton1Down = false;
static GLfloat g_fViewDistance = 3 * VIEWING_DISTANCE_MIN;
static GLfloat g_nearPlane = 1;
static GLfloat g_farPlane = 8000;
static int g_Width = 600;                          // Initial window width
static int g_Height = 600;                         // Initial window height
static int g_yClick = 0;

typedef AABB<double,3> AABB3D;
typedef Eigen::Matrix<double,3,1> vector3d;
typedef Eigen::Matrix<double,3,3> matrix3d;
typedef Eigen::Matrix<double,4,4> HomMatrix;

struct Ellipsoid : public AABB3D
{
	Ellipsoid(const vector3d& center, const vector3d& extents, const matrix3d& axis)
	{
		// Setup the 4x4 matrix, Ellipsoid  is plotted at 3 sigma
		_trans(0,3)=center[0];
		_trans(1,3)=center[1];
		_trans(2,3)=center[2];
		_trans(3,3)=1.0;
		_trans(3,0)=_trans(3,1)=_trans(3,2)=0.0;
		//
		_trans(0,0)=axis(0,0)*extents(0)*3.0;
		_trans(1,0)=axis(1,0)*extents(0)*3.0;
		_trans(2,0)=axis(2,0)*extents(0)*3.0;
		//
		_trans(0,1)=axis(0,1)*extents(1)*3.0;
		_trans(1,1)=axis(1,1)*extents(1)*3.0;
		_trans(2,1)=axis(2,1)*extents(1)*3.0;

		_trans(0,2)=axis(0,2)*extents(2)*3.0;
		_trans(1,2)=axis(1,2)*extents(2)*3.0;
		_trans(2,2)=axis(2,2)*extents(2)*3.0;

		// Calculate the bounding boxes
		vector3d hw;
		double w=std::pow(extents[0]*axis(0,0),2)+std::pow(extents[1]*axis(0,1),2)+std::pow(extents[2]*axis(0,2),2);
		double h=std::pow(extents[0]*axis(1,0),2)+std::pow(extents[1]*axis(1,1),2)+std::pow(extents[2]*axis(1,2),2);
		double d=std::pow(extents[0]*axis(2,0),2)+std::pow(extents[1]*axis(2,1),2)+std::pow(extents[2]*axis(2,2),2);
		hw<<3.0*sqrt(w),3.0*sqrt(h),3.0*sqrt(d);
		vector3d low=center-hw;
		vector3d high=center+hw;

		// Setup the AABB

		_lowerBound(0)=low[0];_lowerBound(1)=low[1];_lowerBound(2)=low[2];
		_upperBound(0)=high[0];_upperBound(1)=high[1];_upperBound(2)=high[2];
		_intercept=false;
	}
	Ellipsoid(const Ellipsoid& rhs)
	{
		_trans=rhs._trans;
		_lowerBound=rhs._lowerBound;
		_upperBound=rhs._upperBound;
		_intercept=rhs._intercept;

	}
	Ellipsoid& operator=(const Ellipsoid& rhs)
	{
		if (this!=&rhs)
		{
			_trans=rhs._trans;
			_lowerBound=rhs._lowerBound;
			_upperBound=rhs._upperBound;
			_intercept=rhs._intercept;
		}
		return *this;
	}
	// Plot to OpenGL
	void plot(GLuint dlist1, GLuint dlist2=0, bool bounding_box_on=true)
	{
		glPushMatrix();
		glMultMatrixd((GLdouble*)(&(_trans(0,0))));
		if (!_intercept)
			glColor3f(1.0f,0.0f,0.0f);
		else
			glColor3f(1.0f,1.0f,1.0f);
		glCallList(dlist1);
		glPopMatrix();
		if (bounding_box_on)
		{
			glPushMatrix();
			vector3d center=this->getCenter();
			vector3d dim=this->getBoxExtents();
			glColor3f(0.0f,1.0f,1.0f);
			glTranslated(center(0),center(1),center(2));
			glScaled(dim(0),dim(1),dim(2));
			glCallList(dlist2);
			glPopMatrix();
		}
		return ;
	}

	void setIntercept(bool intercept)
	{
		_intercept=intercept;
	}

	HomMatrix _trans;
	bool _intercept;
};

//
typedef std::vector<int> vint;

struct Data
{
	Data()
	{}
	void fromFile(const std::string& filename)
	{
	SX::Data::MMILLAsciiReader mm;
	mm.mapFile(filename.c_str());

	_frames.resize(mm.nBlocks());
	#pragma omp parallel for
	for (std::size_t i=0;i<mm.nBlocks();++i)
		_frames[i]=std::move(mm.readBlock(i));
	}
	blob3DCollection getEllipsoids(double threshold, double confidence)
	{
		std::vector<int*> ptr;
		for (unsigned int i=0;i<_frames.size();++i)
		{
			vint& m=_frames[i];
			ptr.push_back(&m[0]);
		}
		blob3DCollection blobs=findBlobs3D<int>(ptr,256,640,threshold,5,100000000,confidence,0);
		return blobs;
	}
std::vector<vint> _frames;

};


GLuint createUnitSphere(bool fill=true)
{
	GLuint myobject=glGenLists(1);
	GLUquadric* quad=gluNewQuadric();
	gluQuadricNormals(quad,GLU_SMOOTH);
	if (fill)
		gluQuadricDrawStyle(quad,GLU_FILL);
	else
		gluQuadricDrawStyle(quad,GLU_LINE);
	glNewList(myobject,GL_COMPILE);
	gluQuadricOrientation(quad,GLU_OUTSIDE);
	gluSphere(quad,1.0,10,10);
	glEndList();
	return myobject;
}

GLuint createUnitCube()
{


	GLuint myobject=glGenLists(1);

	glNewList(myobject,GL_COMPILE);
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glPushMatrix();
	glBegin(GL_QUADS);
	float minus=-0.5;
	float plus=0.5;
	glVertex3f(minus,minus,minus);
	glVertex3f(minus,plus,minus);
	glVertex3f(plus,plus,minus);
	glVertex3f(plus,minus,minus);
	glVertex3f(minus,minus,plus);
	glVertex3f(minus,plus,plus);
	glVertex3f(plus,plus,plus);
	glVertex3f(plus,minus,plus);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(minus,minus,minus);
	glVertex3f(minus,minus,plus);
	glVertex3f(minus,plus,minus);
	glVertex3f(minus,plus,plus);
	glVertex3f(plus,plus,minus);
	glVertex3f(plus,plus,plus);
	glVertex3f(plus,minus,minus);
	glVertex3f(plus,minus,plus);
	glEnd();

	glPopMatrix();
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glEndList();

	return myobject;
}



void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
}



typedef NDTree<double,3> Octree;
Data d;
blob3DCollection blobs;
std::vector<Ellipsoid> ell;
//Create an octree
Octree tree({0.0,0.0,0.0},{640.0,256.0,1172.0});
GLuint ball, ballwire;
GLuint cube;
std::vector<AABB3D*> treeAABBs;
std::set<std::pair<AABB3D*,AABB3D*> > collisions;
bool show_tree=false;
bool show_bb=false;
double threshold=10.0;
double confidence=0.99;

void HandleKeys(unsigned char key, int x, int y)
{
	if (key=='t')
	{
		show_tree=!show_tree;
	}
	if (key=='b')
	{
		show_bb=!show_bb;
	}
	glutPostRedisplay();
	return;

}
void initData(const char* file)
{
	// Load display list for sphere and  cube
	ball=createUnitSphere(true);
	ballwire=createUnitSphere(false);
	cube=createUnitCube();
	// Read the data and search for blobs
	d.fromFile(file);
	blobs=d.getEllipsoids(threshold,confidence);
	std::cout << "Found : " << blobs.size() << std::endl;
	// Transform blobs to Ellipsoids
	vector3d center;
	vector3d extents;
	matrix3d axis;
	vector3d v0,v1,v2;
	for (auto it=blobs.begin();it!=blobs.end();++it)
	{
		it->second.toEllipsoid(confidence,center,extents,axis);
		ell.push_back(Ellipsoid(center,extents,axis));
	}
	// Populate the Octree
	tree.setMaxDepth(10);
	tree.setMaxStorage(6);
	for (auto it=ell.begin();it!=ell.end();++it)
	{
		//AABB<double,3>* tmp=dynamic_cast<AABB<double,3>*>(&(*it));
		tree.addData(&(*it));
	}
	// Get the octreeAABBs
	tree.getVoxels(treeAABBs);
	tree.getPossibleCollisions(collisions);
	for(auto& it : collisions)
	{
		Ellipsoid* temp=dynamic_cast<Ellipsoid*>(it.first);
		if (temp)
			temp->setIntercept(true);
		temp=dynamic_cast<Ellipsoid*>(it.second);
		if (temp)
			temp->setIntercept(true);
	}
}

void drawScene() {



	glMatrixMode(GL_MODELVIEW);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Move the scene to center of the D19 frame
	glPushMatrix();
    glTranslatef(-320.f,-128.0f,-586.0f);

	GLfloat ambientColor[] = {0.5f, 0.5f, 0.5f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

	GLfloat lightColor[] = {0.7f, 0.7f, 0.7f, 1.0f};
	GLfloat lightPos[] = {1.0f, 0.2f, 0.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);


	glShadeModel(GL_SMOOTH);
	glDisable(GL_TEXTURE_2D);

	for (auto& it : ell )
	{
		if (it._intercept)
			it.plot(ballwire,cube,show_bb);
		else
			it.plot(ball,cube,show_bb);
	}
	// To vizualize the tree
	if (show_tree)
	{
		for (auto& it : treeAABBs )
		{
			glPushMatrix();
			vector3d center=it->getCenter();
			vector3d dim=it->getBoxExtents();
			glColor3f(1.0f,1.0f,0.0f);
			glTranslated(center(0),center(1),center(2));
			glScaled(dim(0),dim(1),dim(2));
			glCallList(cube);
			glPopMatrix();
		}
	}

	glPushMatrix();
	glColor3f(0.0f,1.0f,0.0f);
	glTranslated(320.0f,128.0f,586.0f);
	glScalef(640.0f,256.0f,1172.0f);
	glCallList(cube);
	glPopMatrix();
	glPopMatrix();

	glutSwapBuffers();
}


int main(int argc, char** argv)
{
	if (argc!=2)
		exit(1);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Collision Detection - Test NDTree");
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(HandleKeys);
	initData(argv[1]);
	glScalef(0.004f,0.004f,0.004f);
	zprInit();

	initRendering();

	glutMainLoop();
	return 0;
}
