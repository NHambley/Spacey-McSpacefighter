#pragma once

#include "Definitions.h"

#include "ControllerConfiguration.h"
#include "imgui\ImGuiObject.h"

#include "MyMesh.h"
#include "MyRigidBody.h"
#include "MyMeshManager.h"

namespace Simplex
{
	class LazerManager
	{
	public:
		LazerManager();
		~LazerManager();

		LazerManager(vector3 pos, vector3 move);
		void Move();
		vector3 position;
		vector3 moveVec;
		Model* model = nullptr;
		quaternion quat;
		MyRigidBody* rb;
	};
}



