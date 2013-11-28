#include "MMILLAsciiReader.h"
#include "Blob3D.h"
#include "Matrix33.h"
#include "Blob3DFinder.h"
#include "V3D.h"
#include "AABB.h"
#include "NDTree.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <set>
#include <stdlib.h>
#include <vector>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <GL/glut.h>
#include <unordered_map>
#include "zpr.h"

#define VIEWING_DISTANCE_MIN  3.0

using namespace std;
using namespace boost::numeric::ublas;
using namespace SX::Geometry;


static bool g_bButton1Down = false;
static GLfloat g_fViewDistance = 3 * VIEWING_DISTANCE_MIN;
static GLfloat g_nearPlane = 1;
static GLfloat g_farPlane = 8000;
static int g_Width = 600;                          // Initial window width
static int g_Height = 600;                         // Initial window height
static int g_yClick = 0;

typedef AABB<double,3> AABB3D;
//
struct Ellipsoid : public AABB3D
{
	Ellipsoid(const V3D& center, const V3D& semi_Axes, const V3D& v0, const V3D& v1, const V3D& v2)
	{
		// Setup the 4x4 matrix, Ellipsoid  is plotted at 3 sigma
		_trans(0,3)=center[0];
		_trans(1,3)=center[1];
		_trans(2,3)=center[2];
		_trans(3,3)=1.0;
		_trans(3,0)=_trans(3,1)=_trans(3,2)=0.0;
		//
		_trans(0,0)=v0[0]*semi_Axes[0]*3.0;
		_trans(1,0)=v0[1]*semi_Axes[0]*3.0;
		_trans(2,0)=v0[2]*semi_Axes[0]*3.0;
		//
		_trans(0,1)=v1[0]*semi_Axes[1]*3.0;
		_trans(1,1)=v1[1]*semi_Axes[1]*3.0;
		_trans(2,1)=v1[2]*semi_Axes[1]*3.0;

		_trans(0,2)=v2[0]*semi_Axes[2]*3.0;
		_trans(1,2)=v2[1]*semi_Axes[2]*3.0;
		_trans(2,2)=v2[2]*semi_Axes[2]*3.0;

		// Calculate the bounding boxes
		V3D hw;
		double w=std::pow(semi_Axes[0]*v0[0],2)+std::pow(semi_Axes[1]*v1[0],2)+std::pow(semi_Axes[2]*v2[0],2);
		hw[0]=3.0*sqrt(w);
		double h=std::pow(semi_Axes[0]*v0[1],2)+std::pow(semi_Axes[1]*v1[1],2)+std::pow(semi_Axes[2]*v2[1],2);
		hw[1]=3.0*sqrt(h);
		double d=std::pow(semi_Axes[0]*v0[2],2)+std::pow(semi_Axes[1]*v1[2],2)+std::pow(semi_Axes[2]*v2[2],2);
		hw[2]=3.0*sqrt(d);
		V3D low=center-hw;
		V3D high=center+hw;

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
		glMultMatrixd((GLdouble*)(&(*(_trans.data().begin()))));
		if (!_intercept)
			glColor3f(1.0f,0.0f,0.0f);
		else
			glColor3f(1.0f,1.0f,1.0f);
		glCallList(dlist1);
		glPopMatrix();
		if (bounding_box_on)
		{
			glPushMatrix();
			bounded_vector<double,3> center=this->getCenter();
			bounded_vector<double,3> dim=this->getExtents();
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

	bounded_matrix<double,4,4,column_major> _trans;
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
	SX::Data::MMILLAsciiReader mm(filename.c_str());
	mm.readMetaDataBlock();
	_frames.resize(mm.nBlocks());
	#pragma omp parallel for
	for (std::size_t i=0;i<mm.nBlocks();++i)
		_frames[i]=std::move(mm.readBlock(i));
	}
	blob3DCollection getEllipsoids(double threashold)
	{
		std::vector<int*> ptr;
		for (unsigned int i=0;i<_frames.size();++i)
		{
			vint& m=_frames[i];
			ptr.push_back(&m[0]);
		}
		blob3DCollection blobs=findBlobs3D<int>(ptr,256,640,threashold,5,100000000,0);
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
	blobs=d.getEllipsoids(10.0);
	std::cout << "Found : " << blobs.size() << std::endl;
	// Transform blobs to Ellipsoids
	V3D center;
	V3D sa;
	V3D v0,v1,v2;
	for (auto& it : blobs)
	{
		it.second.toEllipsoid(center,sa,v0,v1,v2);
		v2=v0.cross_prod(v1);
		ell.push_back(Ellipsoid(center,sa,v0,v1,v2));
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
			bounded_vector<double,3> center=it->getCenter();
			bounded_vector<double,3> dim=it->getExtents();
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
