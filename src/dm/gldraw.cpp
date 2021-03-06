/*****************************************************************************
 * DynaMechs: A Multibody Dynamic Simulation Library
 *
 * Copyright (C) 1994-2001  Scott McMillan   All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *****************************************************************************
 *     File: gldraw.cpp
 *   Author: Scott McMillan
 *  Created: 24 March 1997
 *  Summary:
 *****************************************************************************/

#include "dm.h"
#include "dmArticulation.hpp"
#include "dmLink.hpp"
#include "dmZScrewTxLink.hpp"
#include "dmStaticRootLink.hpp"
#include "dmMobileBaseLink.hpp"
#include "dmSphericalLink.hpp"
#include "dmQuaternionLink.hpp"
#include "dmMDHLink.hpp"
#include "dmRevoluteLink.hpp"
#include "dmPrismaticLink.hpp"
#include "dmContactModel.hpp"
#include "dmEnvironment.hpp"
#include "dmTreadmill.hpp"


#if defined(__APPLE__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

GLUquadricObj *quadric;

// yeah I know this is in other places but I really want to limit its scope
const float RADTODEG = (float)(180.0/M_PI);    // M_PI is defined in math.h

//----------------------------------------------------------------------------
inline void compute_face_normal(float v0[3], float v1[3], float v2[3],
                                float normal[3])
{
   float a[3], b[3];
   register int i;

   for (i=0; i<3; i++)
   {
      a[i] = v1[i] - v0[i];
      b[i] = v2[i] - v0[i];
   }

   //cross(a, b, normal);
   normal[0] = a[1]*b[2] - a[2]*b[1];
   normal[1] = a[2]*b[0] - a[0]*b[2];
   normal[2] = a[0]*b[1] - a[1]*b[0];

   //normalize(normal);
   float norm = sqrt(normal[0]*normal[0] +
                     normal[1]*normal[1] +
                     normal[2]*normal[2]);

   if (norm > 0.0)
   {
      normal[0] /= norm;
      normal[1] /= norm;
      normal[2] /= norm;
   }
}

//============================================================================
// Initialize DynaMechs/OpenGL drawing routines
//============================================================================

//----------------------------------------------------------------------------
void dmEnvironment::drawInit()
{
   register int i, j;

   GLfloat vertex[3][3], normal[3];

   GLfloat vtex[4][3];//lyp

   // read in and allocate depth data

   m_terrain_model_index = glGenLists(1);

   if (m_terrain_model_index == 0)
   {
      cerr << "loadModel_grid: Error unable to allocate dlist index." << endl;
   }

   //////////////////////////////

// ****
// Currently only flat ground can have textures. If you want to work with uneven terrain, comment out the
// following line. That will switch back to the McMillan's old code. - yiping
 #define TEXTURED_FLAT_GROUND

    //NOTE: texture image has to be 16*16 or 64*64 or 128*128 or 256*256
#ifdef TEXTURED_FLAT_GROUND
	GLuint texture_ID_1;
	//glDisable(GL_BLEND);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glGenTextures( 1, &texture_ID_1 );
	//glBindTexture( GL_TEXTURE_2D, texture_ID_1 );

	FILE* f = fopen("checker1.bmp","rb");

	int tw;
	int th;
	unsigned char* data;
	//GLubyte **pixelArray;

	if (f != NULL)
	{
		unsigned char info[54];
		fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

		// extract texture image height and width from header
		tw = *(int*)&info[18];
		th = *(int*)&info[22];
		cout<< "texture file is "<< tw <<"x" <<th <<" ."<<endl;
		int size = 4 * tw * th;
		data = new unsigned char[size]; // allocate 4 bytes per pixel
		fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once!
			   //Size in bytes of each element to be read.
		fclose(f);

		for(i = 0; i < size; i += 4) // swap the first and third pixal, possibly necessary.
		{
			unsigned char tmp = data[i];
			data[i] = data[i+2];
			data[i+2] = tmp;
		}

		//pixelArray = new GLubyte * [tw*th];
		//for (int i=0; i<tw*th;i++)
		//{
		//	pixelArray[i]= new GLubyte [3];
		//	pixelArray[i][0]=(GLubyte) data[3*i];
		//	pixelArray[i][1]=(GLubyte) data[3*i+1];
		//	pixelArray[i][2]=(GLubyte) data[3*i+2];
		//}


	}
	else
	{
		cout<<"Texture file not opened. "<<endl;

	}

	// cout<<int(textureArray[0][15][0])<<endl;
	glBindTexture (GL_TEXTURE_2D, texture_ID_1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,
			  0, // level of detail 0-base level
			  GL_RGBA, // number of color components in texture
			  tw, // texture width
			  th, // texture height,
			  0, // border
			  GL_RGBA,// number of color components of image pixels
			  GL_UNSIGNED_BYTE,// image data type
			  data);



	glNewList(m_terrain_model_index, GL_COMPILE);
	{

		glEnable(GL_TEXTURE_2D);
		//glShadeModel(GL_SMOOTH);

		//glPolygonMode(GL_FRONT, GL_LINE);
		//glPolygonMode(GL_BACK, GL_LINE);  // wire frame terrain

		glPolygonMode(GL_FRONT, GL_FILL); //GL_LINE);
		glPolygonMode(GL_BACK, GL_FILL);

		for (i=0; i<m_x_dim; i++)//
		{
            for (j=0; j<m_y_dim; j++)//
            {
                vtex[0][0] = ((GLfloat) i)*m_grid_resolution;
                vtex[0][1] = ((GLfloat) j)*m_grid_resolution;
                vtex[0][2] = 0.0;

                vtex[1][0] = ((GLfloat) i+ 1.0)*m_grid_resolution;
                vtex[1][1] = ((GLfloat) j )*m_grid_resolution;
                vtex[1][2] = 0.0;

                vtex[2][0] = ((GLfloat) i + 1.0)*m_grid_resolution;
                vtex[2][1] = ((GLfloat) j + 1.0)*m_grid_resolution;
                vtex[2][2] = 0.0;

                vtex[3][0] = ((GLfloat) i )*m_grid_resolution;
                vtex[3][1] = ((GLfloat) j+ 1.0 )*m_grid_resolution;
                vtex[3][2] = 0.0;

                glBegin (GL_QUADS);
                glNormal3d(0, 0, 1);
                glTexCoord2f (0.0, 0.0);
                glVertex3fv (vtex[0]);
                glTexCoord2f (1.0, 0.0);
                glVertex3fv (vtex[1]);
                glTexCoord2f (1.0, 1.0);
                glVertex3fv (vtex[2]);
                glTexCoord2f (0.0, 1.0);
                glVertex3fv (vtex[3]);
                glEnd ();
            }
		}
		glDisable(GL_TEXTURE_2D);


	}
	glEndList();

#else  /////////////////////////////////////////////////



   glNewList(m_terrain_model_index, GL_COMPILE);
   {
      glPolygonMode(GL_FRONT, GL_FILL); //GL_LINE);
      glPolygonMode(GL_BACK, GL_FILL);

      //GLfloat color[4] = {0.8,0.5,0.5,1.0};
      //glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
      glColor4f(0.95,0.95,0.95,1.0);
      char buffer[200];



//
//
      for (j=0; j<m_y_dim-1; j++)
      {
         glBegin(GL_TRIANGLE_STRIP);
         {
            for (i=0; i<m_x_dim; i++)
            {
               vertex[0][0] = ((GLfloat) i)*m_grid_resolution;
               vertex[0][1] = ((GLfloat) j + 1.0)*m_grid_resolution;
               vertex[0][2] = m_depth[i][j+1];

               if (i > 0)
               {
                  vertex[1][0] = ((GLfloat) i - 1.0)*m_grid_resolution;
                  vertex[1][1] = ((GLfloat) j + 1.0)*m_grid_resolution;
                  vertex[1][2] = m_depth[i-1][j+1];

                  vertex[2][0] = ((GLfloat) i - 1.0)*m_grid_resolution;
                  vertex[2][1] = ((GLfloat) j)*m_grid_resolution;
                  vertex[2][2] = m_depth[i-1][j];

                  compute_face_normal(vertex[1], vertex[2], vertex[0], normal);
                  glNormal3fv(normal);
               }
               glVertex3fv(vertex[0]);

               vertex[1][0] = ((GLfloat) i)*m_grid_resolution;
               vertex[1][1] = ((GLfloat) j)*m_grid_resolution;
               vertex[1][2] = m_depth[i][j];

               if (i > 0)
               {
                  compute_face_normal(vertex[1], vertex[0], vertex[2], normal);
                  glNormal3fv(normal);
               }
               glVertex3fv(vertex[1]);
            }
         }
         glEnd();
      }

      //GLfloat color2[4] = {0.1,0.1,0.1,1.0};
      //glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color2);
      glColor4f(0.8,0.8,0.8,1.0);
      for (j=0; j<m_y_dim-1; j++)
      {
         for (i=0; i<m_x_dim-1; i++)
         {
            vtex[0][0] = ((GLfloat) i)*m_grid_resolution;
            vtex[0][1] = ((GLfloat) j)*m_grid_resolution;
            vtex[0][2] = m_depth[i][j]+0.0001;

            vtex[1][0] = ((GLfloat) i)*m_grid_resolution;
            vtex[1][1] = ((GLfloat) j+1)*m_grid_resolution;
            vtex[1][2] = m_depth[i][j+1]+0.0001;

            vtex[2][0] = ((GLfloat) i+1)*m_grid_resolution;
            vtex[2][1] = ((GLfloat) j)*m_grid_resolution;
            vtex[2][2] = m_depth[i+1][j]+0.0001;

            vtex[3][0] = ((GLfloat) i+1)*m_grid_resolution;
            vtex[3][1] = ((GLfloat) j+1)*m_grid_resolution;
            vtex[3][2] = m_depth[i+1][j+1]+0.0001;

	        glBegin(GL_LINES);
            {
			   if (j == 0)
               {
                  glVertex3fv(vtex[0]);
                  glVertex3fv(vtex[2]);
               }

			   if (i == 0)
               {
                  glVertex3fv(vtex[0]);
                  glVertex3fv(vtex[1]);
               }

               glVertex3fv(vtex[0]);
               glVertex3fv(vtex[3]);
               glVertex3fv(vtex[1]);
               glVertex3fv(vtex[3]);
               glVertex3fv(vtex[2]);
               glVertex3fv(vtex[3]);
            }
            glEnd();

	     }
      }
      glColor4f(1.0,1.0,1.0,1.0);

   }
   glEndList();
#endif

}

//============================================================================
// draw environment and robot
//============================================================================


// functions to draw skeletons of the robot
// func 1.
void dmArticulation::drawSkeleton() const
{
   quadric = gluNewQuadric();
   glPushMatrix();

   glTranslatef(m_p_ICS[0], m_p_ICS[1], m_p_ICS[2]);

   Float len = sqrt(m_quat_ICS[0]*m_quat_ICS[0] +
                    m_quat_ICS[1]*m_quat_ICS[1] +
                    m_quat_ICS[2]*m_quat_ICS[2]);
   if (len > 1.0e-6)
   {
      float angle = 2.0*atan2(len, m_quat_ICS[3]);
      glRotatef(angle*RADTODEG,
                m_quat_ICS[0]/len, m_quat_ICS[1]/len, m_quat_ICS[2]/len);
   }

   // render some sort of base element
   if (getUserData())
      glCallList(*((GLuint *) getUserData()));

   for (unsigned int j=0; j<m_link_list.size(); j++)
   {
      if (m_link_list[j]->parent == NULL)
      {
         glPushMatrix();

         // draw base link
		 //cout<<"draw base link ... - "<<j<<endl;
         m_link_list[j]->link->drawSkeleton(0); 
         //cout<<"base link drawn."<<endl;

         // recurse through the children
         for (unsigned int i=0; i<m_link_list[j]->child_list.size(); i++)
         {
            glPushMatrix();
            drawSkeletonTraversal(m_link_list[j]->child_list[i]);
            glPopMatrix();
         }

         glPopMatrix();
      }
   }

   glPopMatrix();
}

// func 2.
void dmArticulation::drawSkeletonTraversal(LinkInfoStruct *node) const
{
   if (node && node->parent)
   {
      if (node->child_list.size())
         node->link->drawSkeleton(0);
      else
         node->link->drawSkeleton(1);

      for (unsigned int i=0; i<node->child_list.size(); i++)
      {
         if (node->child_list.size() > 1)
         {
            glPushMatrix();
            drawSkeletonTraversal(node->child_list[i]);
            glPopMatrix();
         }
         else
         {
            drawSkeletonTraversal(node->child_list[i]);
         }
      }
   }
}

// func 3.
void dmQuaternionLink::drawSkeleton(bool isTip) const
{

   glLineWidth (1.5);
   glColor4f(0.0,0.1,0.9,1.0);
   glBegin(GL_LINES);
   glVertex3f(0,0,0);
   glVertex3f(m_p[0], m_p[1], m_p[2]);
   glEnd();
   glColor4f(1.0,1.0,1.0,1.0);
   glLineWidth (1.5);

   glTranslatef(m_p[0], m_p[1], m_p[2]);
   glColor4f(0.0,0.0,1.0,1.0);
   gluSphere(quadric,.01f,16,16);
   glColor4f(1.0,1.0,1.0,1.0);

   Float len = sqrt(m_q[0]*m_q[0] + m_q[1]*m_q[1] + m_q[2]*m_q[2]);
   if (len > 1.0e-6)
   {
      float angle = 2.0*atan2(len, m_q[3]);
      glRotatef(angle*RADTODEG, m_q[0]/len, m_q[1]/len, m_q[2]/len);
   }

   // if the link is at the tip, draw an extra stub.
   if (isTip)
   {
	  glLineWidth (1.5);
	  glColor4f(0.0,0.0,1.0,1.0);
	  glBegin(GL_LINES);
	  glVertex3f(0,0,0);
	  glVertex3f(m_cg_pos[0], m_cg_pos[1], m_cg_pos[2] );
	  glEnd();
	  glLineWidth (1.5);     

	  glTranslatef(m_cg_pos[0], m_cg_pos[1], m_cg_pos[2]);
	  glColor4f(0.0,0.0,1.0,1.0);
	  gluSphere(quadric,.01f,16,16);
	  glColor4f(1.0,1.0,1.0,1.0); 
   }

}

// func 4.
void dmRevoluteLink::drawSkeleton(bool isTip) const
{
   // rendering method 1
   /*Matrix3F Rot;
   Rot<< 1, 0, 0,
         0, cos(m_alphaMDH), -sin(m_alphaMDH),
         0, sin(m_alphaMDH), cos(m_alphaMDH);
   Vector3F p, p1;
   p<< m_aMDH, 0.0, m_dMDH;
   //p1 = Rot.transpose() * p; 
   p1 = Rot * p;

   glLineWidth (1.5);
   glColor4f(0.0,0.1,0.9,1.0);
   glBegin(GL_LINES);
   glVertex3f(0,0,0);
   glVertex3f(p1(0), p1(1), p1(2) );
   glEnd();
   glColor4f(1.0,1.0,1.0,1.0);
   glLineWidth (1.5);*/

   // rendering method 2
   Matrix3F Rot;
   Rot<< 1, 0, 0,
         0, cos(m_alphaMDH), -sin(m_alphaMDH),
         0, sin(m_alphaMDH), cos(m_alphaMDH);
   Vector3F p, p1;
   p<< m_aMDH, 0.0, 0.0;
   p1<<m_aMDH, 0.0, m_dMDH;
   p1 = Rot * p1;

   glLineWidth (1.5);
   glColor4f(0.0,0.4,0.9,1.0);
   glBegin(GL_LINES);
   glVertex3f(0,0,0);
   glVertex3f(p(0), p(1), p(2) );
   glVertex3f(p(0), p(1), p(2) );
   glVertex3f(p1(0), p1(1), p1(2) );
   glEnd();
   glColor4f(1.0,1.0,1.0,1.0);
   //glLineWidth (1.5);



   // set static portion of the MDH transformation.

   if (m_alphaMDH != 0.0)
   {
      glRotatef(m_alphaMDH*RADTODEG, 1.0, 0.0, 0.0);
   }

   if ((m_aMDH != 0.0) || (m_dMDH != 0.0))
   {
      glTranslatef(m_aMDH, 0.0, m_dMDH);
   }

   // set dynamic z-axis transformation.
   glRotatef(m_thetaMDH*RADTODEG, 0.0, 0.0, 1.0);

   glColor4f(0.0,0.0,1.0,1.0);
   //gluSphere(quadric,.01f,16,16);
   glPushMatrix();
   glTranslatef(0.0, 0.0, 0.005);
   gluDisk(quadric, 0.0, 0.015, 16, 8);
   glTranslatef(0.0, 0.0, -0.01);
   gluCylinder(quadric, 0.015, 0.015, 0.01, 16, 2);
   glTranslatef(0.0, 0.0, 0.005);
   glRotatef(180, 1.0, 0.0, 0.0);
   glTranslatef(0.0, 0.0, 0.005);
   gluDisk(quadric, 0.0, 0.015, 16, 8);
   glPopMatrix();
   glColor4f(1.0,1.0,1.0,1.0);

   // if the link is at the tip, draw an extra stub.
   /*if (isTip)
   {
	  glLineWidth (1.5);
	  glColor4f(0.0,0.0,1.0,1.0);
	  glBegin(GL_LINES);
	  glVertex3f(0,0,0);
	  glVertex3f(m_cg_pos[0], m_cg_pos[1], m_cg_pos[2] );
	  glEnd();
	  glLineWidth (1.5);

	  glTranslatef(m_cg_pos[0], m_cg_pos[1], m_cg_pos[2]);
	  glColor4f(0.0,0.0,1.0,1.0);
	  gluSphere(quadric,.01f,16,16);
	  glColor4f(1.0,1.0,1.0,1.0);
   }*/


   // If there are contact points, render them too.
   if (getNumForces())
   {
      //dmContactModel *cm;
      //cm = dynamic_cast<dmContactModel *>(getForce(0));
      //for (int i=0; i<cm->getNumContactPoints(); i++)
      //{
      //   CartesianVector pos;
      //   cm->getContactPoint(i, pos);
      //   glLineWidth (1.5);
      //   glColor4f(0.0,1.0,0.0,1.0);
      //   glBegin(GL_LINES);
      //   glVertex3f(0,0,0);
      //   glVertex3f(pos[0], pos[1], pos[2] );
      //   glEnd();
      //   glColor4f(1.0,1.0,1.0,1.0);
      //}

      dmContactModel *cm;
      cm = dynamic_cast<dmContactModel *>(getForce(0));
      int num_cp = cm->getNumContactPoints();
      for (int i=0; i<num_cp; i++)
      {
         CartesianVector pos, pos1;
         cm->getContactPoint(i, pos);
         cm->getContactPoint((i+1)%num_cp, pos1);
         // glLineWidth (1.5);
         glColor4f(1.0,0.0,1.0,1.0);
         glBegin(GL_LINES);
         glVertex3f(0,0,0);
         glVertex3f(pos[0], pos[1], pos[2] );
         glVertex3f(pos[0], pos[1], pos[2] );
         glVertex3f(pos1[0], pos1[1], pos1[2] );
         glEnd();
      }
      glColor4f(1.0,1.0,1.0,1.0);
   }
}


// func 5.
void dmMobileBaseLink::drawSkeleton(bool isTip) const
{
   //cout<<"draw mobile base skeleton"<<endl;
   glTranslatef(m_p[0], m_p[1], m_p[2]);

   Float len = sqrt(m_quat[0]*m_quat[0] +
                    m_quat[1]*m_quat[1] +
                    m_quat[2]*m_quat[2]);
   if (len > 1.0e-6)
   {
      float angle = 2.0*atan2(len, m_quat[3]);
      glRotatef(angle*RADTODEG, m_quat[0]/len, m_quat[1]/len, m_quat[2]/len);
   }

   glColor4f(0.0,0.0,1.0,1.0);
   gluSphere(quadric,.01f,32,32);
   glColor4f(1.0,1.0,1.0,1.0);

}


// func 6.
void dmPrismaticLink::drawSkeleton(bool isTip) const
{

   Matrix3F Rot;
   Rot<< 1, 0, 0,
         0, cos(m_alphaMDH), -sin(m_alphaMDH),
         0, sin(m_alphaMDH), cos(m_alphaMDH);
   Vector3F p, p1;
   p<< m_aMDH, 0.0, m_dMDH;
   //p1 = Rot.transpose() * p; 
   p1 = Rot * p;

   glLineWidth (1.5);
   glColor4f(0.0,0.1,0.9,1.0);
   glBegin(GL_LINES);
   glVertex3f(0,0,0);
   glVertex3f(p1(0), p1(1), p1(2) );
   glEnd();
   glColor4f(1.0,1.0,1.0,1.0);
   glLineWidth (1.5);

   // set static portion of the MDH transformation.
   if (m_alphaMDH != 0.0)
   {
      glRotatef(m_alphaMDH*RADTODEG, 1.0, 0.0, 0.0);
   }

   if ((m_aMDH != 0.0) || (m_dMDH != 0.0))
   {
      glTranslatef(m_aMDH, 0.0, m_dMDH);
   }

   if (m_thetaMDH != 0.0)
   {
      glRotatef(m_thetaMDH*RADTODEG, 0.0, 0.0, 1.0);
   }

   glColor4f(0.0,0.0,1.0,1.0);
   gluSphere(quadric,.01f,16,16);
   glColor4f(1.0,1.0,1.0,1.0);

   // if the link is at the tip, draw an extra stub.
   if (isTip)
   {
	  glLineWidth (1.5);
	  glColor4f(0.0,0.0,1.0,1.0);
	  glBegin(GL_LINES);
	  glVertex3f(0,0,0);
	  glVertex3f(m_cg_pos[0], m_cg_pos[1], m_cg_pos[2] );
	  glEnd();
	  glLineWidth (1.5);

	  glTranslatef(m_cg_pos[0], m_cg_pos[1], m_cg_pos[2]);
	  glColor4f(0.0,0.0,1.0,1.0);
	  gluSphere(quadric,.01f,16,16);
	  glColor4f(1.0,1.0,1.0,1.0);
   }
}


// func 7.
void dmZScrewTxLink::drawSkeleton(bool isTip) const
{
   Vector3F p;
   p<< 0.0, 0.0, m_dMDH;

   glLineWidth (1.5);
   glColor4f(1.0,0.1,0.1,1.0);
   glBegin(GL_LINES);
   glVertex3f(0,0,0);
   glVertex3f( p(0), p(1), p(2) );
   glEnd();
   glColor4f(1.0,1.0,1.0,1.0);


   glTranslatef(0.0, 0.0, m_dMDH);
   glRotatef(m_thetaMDH*RADTODEG, 0.0, 0.0, 1.0);
}

// func 8.
void dmStaticRootLink::drawSkeleton(bool isTip) const
{
   cout<<"dmStaticRootLink::drawSkeleton() not implemented !"<<endl;
   exit(1);
}

// func 9.
void dmSphericalLink::drawSkeleton(bool isTip) const
{
   glLineWidth (1.5);

   glColor4f(0.0,0.0,1.0,1.0);
   glBegin(GL_LINES);
   glVertex3f(0,0,0);
   glVertex3f(m_p[0], m_p[1], m_p[2]);
   glEnd();
   glColor4f(1.0,1.0,1.0,1.0);
   glLineWidth (1.5);

   glTranslatef(m_p[0], m_p[1], m_p[2]);
   glRotatef(m_q[2]*RADTODEG, 0.0, 0.0, 1.0);
   glRotatef(m_q[1]*RADTODEG, 0.0, 1.0, 0.0);
   glRotatef(m_q[0]*RADTODEG, 1.0, 0.0, 0.0);

   // if the link is at the tip, draw an extra stub.
   if (isTip)
   {
	  glLineWidth (1.5);
	  glColor4f(0.0,0.0,1.0,1.0);
	  glBegin(GL_LINES);
	  glVertex3f(0,0,0);
	  glVertex3f(m_cg_pos[0], m_cg_pos[1], m_cg_pos[2] );
	  glEnd();
	  glLineWidth (1.5);     

	  glTranslatef(m_cg_pos[0], m_cg_pos[1], m_cg_pos[2]);
	  glColor4f(0.0,0.0,1.0,1.0);
	  gluSphere(quadric,.01f,16,16);
	  glColor4f(1.0,1.0,1.0,1.0); 
   }
}






//----------------------------------------------------------------------------
void dmArticulation::draw() const
{
   
   glPushMatrix();

   glTranslatef(m_p_ICS[0], m_p_ICS[1], m_p_ICS[2]);

   Float len = sqrt(m_quat_ICS[0]*m_quat_ICS[0] +
                    m_quat_ICS[1]*m_quat_ICS[1] +
                    m_quat_ICS[2]*m_quat_ICS[2]);
   if (len > 1.0e-6)
   {
      float angle = 2.0*atan2(len, m_quat_ICS[3]);
      glRotatef(angle*RADTODEG,
                m_quat_ICS[0]/len, m_quat_ICS[1]/len, m_quat_ICS[2]/len);
   }

   // render some sort of base element
   if (getUserData())
      glCallList(*((GLuint *) getUserData()));

   for (unsigned int j=0; j<m_link_list.size(); j++)
   {
      if (m_link_list[j]->parent == NULL)
      {
         glPushMatrix();

         // draw base link
         m_link_list[j]->link->draw();

         // recurse through the children
         for (unsigned int i=0; i<m_link_list[j]->child_list.size(); i++)
         {
            glPushMatrix();
            drawTraversal(m_link_list[j]->child_list[i]);
            glPopMatrix();
         }

         glPopMatrix();
      }
   }

   glPopMatrix();
}

//----------------------------------------------------------------------------
void dmArticulation::drawTraversal(LinkInfoStruct *node) const
{
   if (node && node->parent)
   {
      node->link->draw();

      for (unsigned int i=0; i<node->child_list.size(); i++)
      {
         if (node->child_list.size() > 1)
         {
            glPushMatrix();
            drawTraversal(node->child_list[i]);
            glPopMatrix();
         }
         else
         {
            drawTraversal(node->child_list[i]);
         }
      }
   }
}


//----------------------------------------------------------------------------
void dmZScrewTxLink::draw() const
{
   glTranslatef(0.0, 0.0, m_dMDH);
   glRotatef(m_thetaMDH*RADTODEG, 0.0, 0.0, 1.0);
}

//----------------------------------------------------------------------------
void dmStaticRootLink::draw() const
{
   glCallList(*((GLuint *) getUserData()));
}

//----------------------------------------------------------------------------
void dmRevoluteLink::draw() const
{


   // set static portion of the MDH transformation.
   if (m_alphaMDH != 0.0)
   {
      glRotatef(m_alphaMDH*RADTODEG, 1.0, 0.0, 0.0);
   }

   if ((m_aMDH != 0.0) || (m_dMDH != 0.0))
   {
      glTranslatef(m_aMDH, 0.0, m_dMDH);
   }

   // set dynamic z-axis transformation.
   glRotatef(m_thetaMDH*RADTODEG, 0.0, 0.0, 1.0);

   glCallList(*((GLuint *) getUserData()));
   glCallList(*((GLuint *) getUserData2()));
}

//----------------------------------------------------------------------------
void dmPrismaticLink::draw() const
{
   // set static portion of the MDH transformation.
   if (m_alphaMDH != 0.0)
   {
      glRotatef(m_alphaMDH*RADTODEG, 1.0, 0.0, 0.0);
   }

   if ((m_aMDH != 0.0) || (m_dMDH != 0.0))
   {
      glTranslatef(m_aMDH, 0.0, m_dMDH);
   }

   if (m_thetaMDH != 0.0)
   {
      glRotatef(m_thetaMDH*RADTODEG, 0.0, 0.0, 1.0);
   }

   glCallList(*((GLuint *) getUserData()));
}

//----------------------------------------------------------------------------
void dmSphericalLink::draw() const
{
   glTranslatef(m_p[0], m_p[1], m_p[2]);
   glRotatef(m_q[2]*RADTODEG, 0.0, 0.0, 1.0);
   glRotatef(m_q[1]*RADTODEG, 0.0, 1.0, 0.0);
   glRotatef(m_q[0]*RADTODEG, 1.0, 0.0, 0.0);

   glCallList(*((GLuint *) getUserData()));
}

//----------------------------------------------------------------------------
void dmQuaternionLink::draw() const
{
   glTranslatef(m_p[0], m_p[1], m_p[2]);

   Float len = sqrt(m_q[0]*m_q[0] + m_q[1]*m_q[1] + m_q[2]*m_q[2]);
   if (len > 1.0e-6)
   {
      float angle = 2.0*atan2(len, m_q[3]);
      glRotatef(angle*RADTODEG, m_q[0]/len, m_q[1]/len, m_q[2]/len);
   }

   glCallList(*((GLuint *) getUserData()));



}

//----------------------------------------------------------------------------
void dmMobileBaseLink::draw() const
{
   glTranslatef(m_p[0], m_p[1], m_p[2]);

   Float len = sqrt(m_quat[0]*m_quat[0] +
                    m_quat[1]*m_quat[1] +
                    m_quat[2]*m_quat[2]);
   if (len > 1.0e-6)
   {
      float angle = 2.0*atan2(len, m_quat[3]);
      glRotatef(angle*RADTODEG, m_quat[0]/len, m_quat[1]/len, m_quat[2]/len);
   }

   glCallList(*((GLuint *) getUserData()));
   glCallList(*((GLuint *) getUserData2()));
}

//----------------------------------------------------------------------------
void dmContactModel::draw() const
{
}

//----------------------------------------------------------------------------
void dmEnvironment::draw() const
{
   glCallList(m_terrain_model_index);
}

//----------------------------------------------------------------------------
void dmTreadmill::draw() const
{
   dmEnvironment::draw();

   glBegin(GL_QUAD_STRIP);
   float dxl = m_half_width*m_left[0];
   float dyl = m_half_width*m_left[1];
   float dzl = m_half_width*m_left[2];

   float dxf = m_half_length*m_forward[0];
   float dyf = m_half_length*m_forward[1];
   float dzf = m_half_length*m_forward[2];

   glVertex3f(m_position[0] + dxl - dxf,
              m_position[1] + dyl - dyf,
              m_position[2] + dzl - dzf);
   glVertex3f(m_position[0] - dxl - dxf,
              m_position[1] - dyl - dyf,
              m_position[2] - dzl - dzf);

   float step = 2.0f*m_half_length/10.f;
   float offset = m_q - ((int)(m_q/step))*step;
   if (offset < 0) offset += step;

   for (unsigned int ix = 0; ix < 10; ++ix)
   {
      float distance = offset + ix*step - m_half_length;
      float dx = distance*m_forward[0];
      float dy = distance*m_forward[1];
      float dz = distance*m_forward[2];
      glVertex3f(m_position[0] + dxl + dx,
                 m_position[1] + dyl + dy,
                 m_position[2] + dzl + dz);
      glVertex3f(m_position[0] - dxl + dx,
                 m_position[1] - dyl + dy,
                 m_position[2] - dzl + dz);
   }

   glVertex3f(m_position[0] + dxl + dxf,
              m_position[1] + dyl + dyf,
              m_position[2] + dzl + dzf);
   glVertex3f(m_position[0] - dxl + dxf,
              m_position[1] - dyl + dyf,
              m_position[2] - dzl + dzf);
   glEnd();
}







//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// A special drawInit for demo purpose
void dmEnvironment::drawInitSpecial()
{
	cout<<"entering drawInitSpecial"<<endl;
	register int i, j;

	GLfloat vertex[3][3], normal[3];
	GLfloat vtex[4][3];

	m_terrain_model_index = glGenLists(1);
	if (m_terrain_model_index == 0)
	{
	  cerr << "loadModel_grid: Error unable to allocate dlist index." << endl;
	}



	// Load texture [texture image has to be 16*16 or 64*64 or 128*128 or 256*256]

	GLuint texture_ID_1;
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glGenTextures( 1, &texture_ID_1 );

	FILE* f = fopen("checker1.bmp","rb");

	int tw;
	int th;
	unsigned char* data;

	if (f != NULL)
	{
		unsigned char info[54];
		fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

		// extract texture image height and width from header
		tw = *(int*)&info[18];
		th = *(int*)&info[22];
		cout<< "texture file is "<< tw <<"x" <<th <<" ."<<endl;
		int size = 4 * tw * th;
		data = new unsigned char[size]; // allocate 4 bytes per pixel
		fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once!
			   //Size in bytes of each element to be read.
		fclose(f);

		for(i = 0; i < size; i += 4) // swap the first and third pixal, possibly necessary.
		{
			unsigned char tmp = data[i];
			data[i] = data[i+2];
			data[i+2] = tmp;
		}

	}
	else
	{
		cout<<"Texture not loaded! "<<endl;
	}

	glBindTexture (GL_TEXTURE_2D, texture_ID_1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,
			  0, // level of detail 0-base level
			  GL_RGBA, // number of color components in texture
			  tw, // texture width
			  th, // texture height,
			  0, // border
			  GL_RGBA,// number of color components of image pixels
			  GL_UNSIGNED_BYTE,// image data type
			  data);


	cout<<"covering flat ground with texture"<<endl;
	// cover flat terrain patches with texture
	glNewList(m_terrain_model_index, GL_COMPILE);
	{

		glEnable(GL_TEXTURE_2D);
		//glShadeModel(GL_SMOOTH);

		glPolygonMode(GL_FRONT, GL_FILL); //GL_LINE
		glPolygonMode(GL_BACK, GL_FILL);

		cout<<m_x_dim<<" "<<m_y_dim<<endl;
		for (i=0; i<m_x_dim-1; i++)//
		{
            for (j=0; j<m_y_dim-1; j++)//
            {
				cout<<"["<<i<<","<<j<<"]: "<<m_depth[i][j]<<" "<<m_depth[i+1][j]<<" "<<m_depth[i+1][j+1]<<" "<<m_depth[i][j+1]<<endl;
				if ( (abs(m_depth[i][j])    <0.000001) & 
                     (abs(m_depth[i+1][j])  <0.000001) &
                     (abs(m_depth[i+1][j+1])<0.000001) &
                     (abs(m_depth[i][j+1])  <0.000001)   )
				{
		            vtex[0][0] = ((GLfloat) i)*m_grid_resolution;
		            vtex[0][1] = ((GLfloat) j)*m_grid_resolution;
		            vtex[0][2] = 0.0001;

		            vtex[1][0] = ((GLfloat) i+ 1.0)*m_grid_resolution;
		            vtex[1][1] = ((GLfloat) j )*m_grid_resolution;
		            vtex[1][2] = 0.0001;

		            vtex[2][0] = ((GLfloat) i + 1.0)*m_grid_resolution;
		            vtex[2][1] = ((GLfloat) j + 1.0)*m_grid_resolution;
		            vtex[2][2] = 0.0001;

		            vtex[3][0] = ((GLfloat) i )*m_grid_resolution;
		            vtex[3][1] = ((GLfloat) j+ 1.0 )*m_grid_resolution;
		            vtex[3][2] = 0.0001;

					if ( (i+j)%2 ==  0 )
					{
				        glBegin (GL_QUADS);
				        glNormal3d(0, 0, 1);
				        glTexCoord2f (0.0, 0.0);
				        glVertex3fv (vtex[0]);
				        glTexCoord2f (0.5, 0.0);
				        glVertex3fv (vtex[1]);
				        glTexCoord2f (0.5, 0.5);
				        glVertex3fv (vtex[2]);
				        glTexCoord2f (0.0, 0.5);
				        glVertex3fv (vtex[3]);
				        glEnd ();
					}
					else
					{
					    glBegin (GL_QUADS);
					    glNormal3d(0, 0, 1);
					    glTexCoord2f (0.5, 0.0);
					    glVertex3fv (vtex[0]);
					    glTexCoord2f (1.0, 0.0);
					    glVertex3fv (vtex[1]);
					    glTexCoord2f (1.0, 0.5);
					    glVertex3fv (vtex[2]);
					    glTexCoord2f (0.5, 0.5);
					    glVertex3fv (vtex[3]);
					    glEnd ();
					}
				}
            }
		}
		glDisable(GL_TEXTURE_2D);

	
		


		// render terrain triangles (single color)

		glPolygonMode(GL_FRONT, GL_FILL); //GL_LINE
		glPolygonMode(GL_BACK, GL_FILL);

		glColor4f(0.95,0.95,0.95,1.0);
		char buffer[200];

		for (j=0; j<m_y_dim-1; j++)
		{
			glBegin(GL_TRIANGLE_STRIP);
			{
				for (i=0; i<m_x_dim; i++)
				{
					vertex[0][0] = ((GLfloat) i)*m_grid_resolution;
					vertex[0][1] = ((GLfloat) j + 1.0)*m_grid_resolution;
					vertex[0][2] = m_depth[i][j+1];

					if (i > 0)
					{
						vertex[1][0] = ((GLfloat) i - 1.0)*m_grid_resolution;
						vertex[1][1] = ((GLfloat) j + 1.0)*m_grid_resolution;
						vertex[1][2] = m_depth[i-1][j+1];

						vertex[2][0] = ((GLfloat) i - 1.0)*m_grid_resolution;
						vertex[2][1] = ((GLfloat) j)*m_grid_resolution;
						vertex[2][2] = m_depth[i-1][j];

						compute_face_normal(vertex[1], vertex[2], vertex[0], normal);
						glNormal3fv(normal);
					}
					glVertex3fv(vertex[0]);

					vertex[1][0] = ((GLfloat) i)*m_grid_resolution;
					vertex[1][1] = ((GLfloat) j)*m_grid_resolution;
					vertex[1][2] = m_depth[i][j];

					if (i > 0)
					{
						compute_face_normal(vertex[1], vertex[0], vertex[2], normal);
						glNormal3fv(normal);
					}
					glVertex3fv(vertex[1]);
				}
			}
			glEnd();
		}

		// render terrain wireframe 
		glColor4f(0.8,0.8,0.8,1.0);
		for (j=0; j<m_y_dim-1; j++)
		{
			for (i=0; i<m_x_dim-1; i++)
			{

				if ( !( (abs(m_depth[i][j])  <0.000001) & 
                      (abs(m_depth[i+1][j])  <0.000001) &
                      (abs(m_depth[i+1][j+1])<0.000001) &
                      (abs(m_depth[i][j+1])  <0.000001)  )   )
				{
					vtex[0][0] = ((GLfloat) i)*m_grid_resolution;
					vtex[0][1] = ((GLfloat) j)*m_grid_resolution;
					vtex[0][2] = m_depth[i][j]+0.0001;

					vtex[1][0] = ((GLfloat) i)*m_grid_resolution;
					vtex[1][1] = ((GLfloat) j+1)*m_grid_resolution;
					vtex[1][2] = m_depth[i][j+1]+0.0001;

					vtex[2][0] = ((GLfloat) i+1)*m_grid_resolution;
					vtex[2][1] = ((GLfloat) j)*m_grid_resolution;
					vtex[2][2] = m_depth[i+1][j]+0.0001;

					vtex[3][0] = ((GLfloat) i+1)*m_grid_resolution;
					vtex[3][1] = ((GLfloat) j+1)*m_grid_resolution;
					vtex[3][2] = m_depth[i+1][j+1]+0.0001;

					glBegin(GL_LINES);
					{
						if (j == 0)
						{
							glVertex3fv(vtex[0]);
							glVertex3fv(vtex[2]);
						}

						if (i == 0)
						{
							glVertex3fv(vtex[0]);
							glVertex3fv(vtex[1]);
						}

						glVertex3fv(vtex[0]);
						glVertex3fv(vtex[3]);
						glVertex3fv(vtex[1]);
						glVertex3fv(vtex[3]);
						glVertex3fv(vtex[2]);
						glVertex3fv(vtex[3]);
					}
					glEnd();
				}
			}
		}
		glColor4f(1.0,1.0,1.0,1.0);
	}
	glEndList();



	cout<<"exiting drawInitSpecial"<<endl;
}
