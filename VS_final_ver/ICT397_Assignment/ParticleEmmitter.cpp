#include "ParticleEmmitter.h"

ParticleEmmitter::ParticleEmmitter()
{

}
ParticleEmmitter::~ParticleEmmitter()
{

}

void ParticleEmmitter::DrawParticles(ParticleEmmitter* particleSystem)
{
	int i;
	for (i = 1; i < particleSystem->getNumofParticles(); i++)
	{
		glPushMatrix();
		// set color and fade value (alpha) of current particle
		glColor4f(particleSystem->getR(i), particleSystem->getG(i), particleSystem->getB(i), particleSystem->getAlpha(i));
		
		// move the current particle to its new position
		//glTranslatef(particleSystem.getXPos(i), particleSystem.getYPos(i), particleSystem.getZPos(i));
		// rotate the particle (this is proof of concept for when proper smoke texture is added)
		//glRotatef(particleSystem.getDirection(i) - 90, 0, 0, 1);
		// scale the wurrent particle (only used for smoke)
		glScalef(particleSystem->getScale(i), particleSystem->getScale(i), particleSystem->getScale(i));

		//glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		//glBlendFunc(GL_DST_COLOR, GL_ZERO);
		//glBindTexture(GL_TEXTURE_2D, texture[0]);
		
		// clear the color draw b4
		//glColor4f(1.0, 0.0, 1.0, 0.0);

		glPointSize(15.0f);
		glBegin(GL_POINTS);
		//glColor3f(particleSystem->getR(i), particleSystem->getG(i), particleSystem->getB(i));
		glVertex3f(particleSystem->getXPos(i), particleSystem->getYPos(i), particleSystem->getZPos(i));
		
		glEnd();
		
		glColor4f(1.0, 1.0, 1.0, 0.0);
		/*
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3f(-1, -1, 0);
		glTexCoord2d(1, 0);
		glVertex3f(1, -1, 0);
		glTexCoord2d(1, 1);
		glVertex3f(1, 1, 0);
		glTexCoord2d(0, 1);
		glVertex3f(-1, 1, 0);
		glEnd();

		glBlendFunc(GL_ONE, GL_ONE);
		//glBindTexture(GL_TEXTURE_2D, texture[1]);

		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3f(-1, -1, 0);
		glTexCoord2d(1, 0);
		glVertex3f(1, -1, 0);
		glTexCoord2d(1, 1);
		glVertex3f(1, 1, 0);
		glTexCoord2d(0, 1);
		glVertex3f(-1, 1, 0);
		glEnd();
		*/
		glEnable(GL_DEPTH_TEST);

		glPopMatrix();
	}
}

void ParticleEmmitter::createParticle(Particle *p, float Xpos, float Ypos, float Zpos)
{
	//p->lifespan = 1000; //(((rand() % 10 + 1))) / 10.0f;
	//srand(time(0));
	p->lifespan = ((((randomness + rand()) % 10 + 1))) / 10.0f;
	p->age = 0.0f;
	p->scale = 0.25f;
	p->direction = 0;

	//srand(time(0));

	//the starting position of particles
	p->position[X] = Xpos;
	p->position[Y] = Ypos;
	p->position[Z] = Zpos;

	//the movement of particles
	//p->movement[X] = (((((((2) * rand() % 11) + 1)) * rand() % 11) + 1) * 0.0035) - (((((((2) * rand() % 11) + 1)) * rand() % 11) + 1) * 0.0035);
	//p->movement[Y] = (((((((2) * rand() % 11) + 1)) * rand() % 11) + 1) * 0.0035) - (((((((2) * rand() % 11) + 1)) * rand() % 11) + 1) * 0.0035);
	//p->movement[Y] = ((((((5) * rand() % 11) + 3)) * rand() % 11) + 7) * 0.015;
	//p->movement[Z] = (((((((2) * rand() % 11) + 1)) * rand() % 11) + 1) * 0.0035) - (((((((2) * rand() % 11) + 1)) * rand() % 11) + 1) * 0.0015);

	p->movement[X] = rand() % 10 - rand() % 10;
	p->movement[Y] = rand() % 10 - rand() % 10;
	p->movement[Z] = rand() % 10 - rand() % 10;

	//the colour of the particles
	p->color[X] = 1.0f;
	p->color[Y] = 1.0f;
	p->color[Z] = 1.0f;


	p->pull[X] = 0.0f;
	p->pull[Y] = 0.0f;
	p->pull[Z] = 0.0f;

}

void ParticleEmmitter::createParticles(float Xpos, float Ypos, float Zpos)
{
	systemPull[Y] = -0.0105;
	systemPull[X] = systemPull[Z] = -0.001f;
	srand(time(0));
	randomness = rand();

	initialPos[0] = Xpos;
	initialPos[1] = Ypos;
	initialPos[2] = Zpos;

	int i = 0;
	for (i; i < MAX_PARTICLES; i++)
	{
		createParticle(&particles[i], Xpos, Ypos, Zpos);
	}
}

void ParticleEmmitter::updateParticles()
{
	int i = 0;
	for (i; i < MAX_PARTICLES; i++)
	{
		particles[i].age = particles[i].age + 0.02;
		if (i==1)
		{
			//cout << "particle 1 location: " << particles[i].position[Y] << endl;
			//cout << "particle oldness" << particles[i].lifespan << endl;
		}

		particles[i].scale = particles[i].scale + 0.001;

		particles[i].direction = particles[i].direction + ((((((int)(0.5) * rand() % 11) + 1)) * rand() % 11) + 1);

		particles[i].position[X] = particles[i].position[X] + particles[i].movement[X] + particles[i].pull[X];
		particles[i].position[Y] = particles[i].position[Y] + particles[i].movement[Y] + particles[i].pull[Y];
		particles[i].position[Z] = particles[i].position[Z] + particles[i].movement[Z] + particles[i].pull[Z];

		particles[i].pull[X] = particles[i].pull[X] + systemPull[X];
		particles[i].pull[Y] = particles[i].pull[Y] + systemPull[Y]; // acleration due to gravity
		particles[i].pull[Z] = particles[i].pull[Z] + systemPull[Z];

		if (particles[i].age > particles[i].lifespan /5)
		{//gold
			particles[i].color[X] = 1.0f;
			particles[i].color[Y] = 1.0f;
			particles[i].color[Z] = 0.0f;
		}
		if (particles[i].age > (particles[i].lifespan / 3) )
		{//gold
			particles[i].color[X] = 1.0f;
			particles[i].color[Y] = 0.5f;
			particles[i].color[Z] = 0.0f;
		}
		if (particles[i].age > particles[i].lifespan/2)
		{//gold
			particles[i].color[X] = 0.5f;
			particles[i].color[Y] = 0.0f;
			particles[i].color[Z] = 0.0f;
		}
		if (particles[i].age > particles[i].lifespan / 1.5)
		{//gold
			particles[i].color[X] = 0.0f;
			particles[i].color[Y] = 0.0f;
			particles[i].color[Z] = 0.0f;
		}

		//if (particles[i].position[Y] < initialPos[1])
		//{
		//	createParticle(&particles[i], initialPos[0], initialPos[1], initialPos[2]);


		//	//cout << "getting here";

		//}
		if (particles[i].age > particles[i].lifespan)
		{
			//createParticle(&particles[i], initialPos[0], initialPos[1], initialPos[2]);
			
		}

		/*
		if (particles[i].position[X] > (initialPos[0] + 200) || particles[i].position[Y] < (initialPos[1] - 20) || particles[i].position[Z] > (initialPos[2] + 200))
		{
			createParticle(&particles[i], initialPos[0], initialPos[1], initialPos[2]);


			//cout << "getting here";

		}*/

	}	

}


int ParticleEmmitter::getNumofParticles()
{
	return MAX_PARTICLES;
}

float ParticleEmmitter::getXPos(int i)
{
	return particles[i].position[X];
}

float ParticleEmmitter::getYPos(int i)
{
	return particles[i].position[Y];
}
float ParticleEmmitter::getZPos(int i)
{
	return particles[i].position[Z];
}

float ParticleEmmitter::getR(int i)
{
	return particles[i].color[X];
}

float ParticleEmmitter::getG(int i)
{
	return particles[i].color[Y];
}
float ParticleEmmitter::getB(int i)
{
	return particles[i].color[Z];
}

float ParticleEmmitter::getScale(int i)
{
	return particles[i].scale;
}

float ParticleEmmitter::getDirection(int i)
{
	return particles[i].direction;
}

float ParticleEmmitter::getAlpha(int i)
{
	return (1 - particles[i].age / particles[i].lifespan);
}

void ParticleEmmitter::modifySystemPull(float x, float y, float z)
{
	systemPull[X] += x;
	systemPull[Y] += y;
	systemPull[Z] += z;
}