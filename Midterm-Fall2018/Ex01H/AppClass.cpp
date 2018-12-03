#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
#pragma region DOES NOT NEED CHANGES
	/*
		This part initialize the camera position so I can see the scene better; 
		the guide cube and the tip of the pencil so all solutions on the exam 
		look the same to the grader
	*/
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 35.0f, ZERO_V3, AXIS_Y);
	
	// IGNORE THESE TWO THINGS IF WE GET RID OF THEM THEY THROW AN EXCEPTION
	m_pGuideCube = new MyMesh();
	m_pGuideCube->GenerateCube(10.0f, C_WHITE);

	m_pMesh = new MyMesh();
	//m_pMesh->GenerateCone(1.0f, 2.0f, 8, vector3(0.0f, 0.0f, 0.0f));

	InitAster(aster, 50);
	
	pShip = new Model();
	pShip->Load("Minecraft\\Steve.obj");
	playerRB = new MyRigidBody(pShip->GetVertexList());
	
#pragma endregion

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
	shipPos.z -= 0.5f;
	shipPos.y -=1;

	//track movement of asteroids **still needs octree
	for (uint i = 0; i < 50; i++)
	{
		aster = new Model();
		aster->Load("Minecraft\\Cube.obj");
		asterRB.push_back(new MyRigidBody(aster->GetVertexList()));
		asteroidPos.push_back(vector3(rand() % 100-50, rand() % 100-50, rand() % 100-50));
		matrix4 mAster = glm::translate(asteroidPos[i]) * ToMatrix4(qAster);//glm::translate(vector3(2.25f, 0.0f, 0.0f)) * glm::rotate(IDENTITY_M4, glm::radians(-55.0f), AXIS_Z);
		aster->SetModelMatrix(mAster);
		asterRB[i]->SetModelMatrix(mAster);
		m_pMeshMngr->AddAxisToRenderList(mAster);
		aster->AddToRenderList();
		asterRB[i]->AddToRenderList();
	}

	// move any lazers that are in the scene
	for (uint i = 0; i < lazerRB.size(); i++)
	{
		matrix4 mLazer = glm::translate(lazerPos[i]) * ToMatrix4(qLazer);
	}
	 //set player ship stuff
	matrix4 mPlayer = glm::translate(shipPos) * ToMatrix4(qShip) * ToMatrix4(m_qArcBall) * glm::rotate(m_pCameraMngr->GetProjectionMatrix(), glm::radians(-90.0f), AXIS_X);
	pShip->SetModelMatrix(mPlayer);
	playerRB->SetModelMatrix(mPlayer);
	m_pMeshMngr->AddAxisToRenderList(mPlayer);

	pShip->AddToRenderList();
	playerRB->AddToRenderList();
	
#pragma endregion
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

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
}
void Application::InitAster(Model *aster, uint index)
{
	for (uint i = 0; i < index; i++)
	{
		
	}
}