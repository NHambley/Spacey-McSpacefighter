#include "LazerManager.h"



Simplex::LazerManager::LazerManager()
{
}


Simplex::LazerManager::~LazerManager()
{
}

Simplex::LazerManager::LazerManager(vector3 pos, vector3 move)
{
	model->Load("Sorted\\Lego.obj");
	position = pos;
	moveVec = move;
	rb = new MyRigidBody(model->GetVertexList());
}

void Simplex::LazerManager::Move()
{
}
