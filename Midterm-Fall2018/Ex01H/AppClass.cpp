#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	m_pEntityMngr = MyEntityManager::GetInstance();
#pragma region DOES NOT NEED CHANGES
	/*
		This part initialize the camera position so I can see the scene better; 
		the guide cube and the tip of the pencil so all solutions on the exam 
		look the same to the grader
	*/
	(m_pCameraMngr->GetCamera(-1))->SetPositionTargetAndUpward(AXIS_Z * 35.0f,
		ZERO_V3, 
		AXIS_Y);
	
	position = vector3(0.0f, 3.0f, 20.0f);
	target = vector3(0.0f, 3.0f, 19.0f);
	forward = glm::normalize(target - position);

	// IGNORE THESE TWO THINGS IF WE GET RID OF THEM THEY THROW AN EXCEPTION
	m_pGuideCube = new MyMesh();
	m_pGuideCube->GenerateCube(10.0f, C_WHITE);

	m_pMesh = new MyMesh();
	//m_pMesh->GenerateCone(1.0f, 2.0f, 8, vector3(0.0f, 0.0f, 0.0f));
	for (uint i = 0; i < 10; i++)
	{
		m_pEntityMngr->AddEntity("Minecraft\\Cube.obj");

		asteroidPos.push_back(vector3(rand() % 100 - 50, rand() % 100 - 50, rand() % 100 - 50));
		directions.push_back(rand() % 6 + 1);
		aster = new Model();
		aster->Load("Minecraft\\Cube.obj");
		asterRB.push_back(new MyRigidBody(aster->GetVertexList()));
	}
	
	pShip = new Model();
	pShip->Load("Minecraft\\Steve.obj");
	playerRB = new MyRigidBody(pShip->GetVertexList());

	//camera = new MyCamera();
	lazer = new Model();
	lazer->Load("Minecraft\\Steve.obj");

	xCam = 0.0f;
	yCam = 3.0f;
	zCam = 20.0f;
	//meshManager->SetCamera(camera);

#pragma endregion
	m_uOctantLevels = 1;
	m_pRoot = new MyOctant(m_uOctantLevels, 5);
	m_pEntityMngr->Update();
	m_pRoot->IsColliding();

	//Please change to your name and email
	m_sProgramer = "Nathan Hambley - nhambley6@gmail.com\nConor Lilley - conorqlilley@gmail.com\nFrederick DiTondo - fredsemail@gmail.com";
}
void Application::Update(void)
{
#pragma region DOES NOT NEED CHANGES
	/*
		This updates the internal clock of the Simplex library and process the Arcball
	*/
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	// Is the first person camera active?
	CameraRotation();
	shipPos = m_pCameraMngr->GetPosition();
	shipPos.z -= 1;
	shipPos.y -= 1;

	//track movement of asteroids **still needs octree
	for (uint i = 0; i < 10; i++)
	{
		if (directions[i] == 1)
		{
			asteroidPos[i].x -= .01;
		}
		else if (directions[i] == 2)
		{
			asteroidPos[i].x += .01;
		}
		else if (directions[i] == 3)
		{
			asteroidPos[i].z -= .01;
		}
		else if (directions[i] == 4)
		{
			asteroidPos[i].z += .01;
		}
		else if (directions[i] == 5)
		{
			asteroidPos[i].y -= .01;
		}
		else if (directions[i] == 6)
		{
			asteroidPos[i].y += .01;
		}
		matrix4 m4Pos = glm::translate(asteroidPos[i]);
		asterRB[i]->SetModelMatrix(m4Pos);
		m_pEntityMngr->SetModelMatrix(m4Pos);

		//Update Entity Manager
		m_pEntityMngr->Update();

		//Add objects to render list
		m_pEntityMngr->AddEntityToRenderList(-1, true);
	}


	// move any lazers if there are any in the scene
	for (uint i = 0; i < lazerPos.size(); i++)
	{
		lazerPos[i] += moveLazer[i];
		matrix4 m4Lazer = glm::translate(lazerPos[i]) * ToMatrix4(qLazer);
		lazer->SetModelMatrix(m4Lazer);
		lazerRB[i]->SetModelMatrix(m4Lazer);
		m_pMeshMngr->AddAxisToRenderList(m4Lazer);
		lazer->AddToRenderList();
		lazerRB[i]->AddToRenderList();
	}
	
	 //set player ship stuff
	matrix4 mPlayer = glm::translate(shipPos) * ToMatrix4(qShip) * ToMatrix4(m_qArcBall) * glm::rotate(IDENTITY_M4, glm::radians(-90.0f), AXIS_X);
	pShip->SetModelMatrix(mPlayer);
	playerRB->SetModelMatrix(mPlayer);
	m_pMeshMngr->AddAxisToRenderList(mPlayer);

	pShip->AddToRenderList();
	playerRB->AddToRenderList();

	bool bColliding = false;
	uint i = 0;
	while (!bColliding&&i<10)
	{
		bColliding = playerRB->IsColliding(asterRB[i]);
		i++;
	}
	m_pMeshMngr->Print("\nColliding: ");
	if (bColliding)
		m_pMeshMngr->PrintLine("YES!", C_RED);
	else
		m_pMeshMngr->PrintLine("no", C_YELLOW);
	
#pragma endregion
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	if (display)
	{
		m_pRoot->Display();
	}

	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Orientation;
	//std::vector<vector3> cubePoints = m_pGuideCube->GetVertexList();


	matrix4 m4Model = ToMatrix4(m_qArcBall) /* glm::translate(IDENTITY_M4, v3CurrentPos) */ * m4Orientation;

	m_pMesh->Render(m4Projection, m4View, m4Model);

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//draw gui
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}

void Application::Release(void)
{
	//release GUI
	ShutdownGUI();

	//I already deallocated the memory for the meshes if you allocate new memory please
	//deallocate it before ending the program
	SafeDelete(m_pMesh);
	SafeDelete(m_pGuideCube);
	SafeDelete(pShip);
	SafeDelete(aster);
}