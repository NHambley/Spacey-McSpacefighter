#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//Set the position and target of the camera
	(m_pCameraMngr->GetCamera(-1))->SetPositionTargetAndUpward(vector3(0.0f, 0.0f, 100.0f),
		vector3(0.0f, 0.0f, 99.0f),
		AXIS_Y);

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

#ifdef DEBUG
	uint uInstances = 900;
#else
	uint uInstances = 1849;
#endif
	crosshair = new Mesh();
	crosshair->GeneratePlane(.008f, C_WHITE);

	ground = new Mesh();
	ground->GeneratePlane(300, C_BROWN);

	uint uIndex = -1;
	SetTargets(uIndex, m_pEntityMngr);

	m_uOctantLevels = 1;
	m_pRoot = new MyOctant(m_uOctantLevels, 5);
	m_pEntityMngr->Update();
}
void Application::SetTargets(uint& index, MyEntityManager* entities)
{

	int numTargets = glm::linearRand(20, 45);

	for (size_t i = 0; i < numTargets; i++)
	{
		vector3 v3Position;
		matrix4 m4Position;
		// pick a type of target
		int targetType = glm::linearRand(1, 10);
		if (targetType % 2 == 0)
		{
			index++;
			entities->AddEntity("Minecraft\\Cube.fbx");
			v3Position = vector3(glm::linearRand(-30, 30), glm::linearRand(0, 16), glm::linearRand(60, 75));
			m4Position = glm::translate(v3Position);
			entities->SetModelMatrix(m4Position);
		}
		else if (targetType % 2 != 0)
		{
			index++;
			entities->AddEntity("Minecraft\\Steve.fbx");
			v3Position = vector3(glm::linearRand(-30, 30), glm::linearRand(0, 16), glm::linearRand(60, 75));
			m4Position = glm::translate(v3Position);
			entities->SetModelMatrix(m4Position);
		}
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Assign IDs to the dynamic entities and static entities
	if (octantActive)
	{
		m_pRoot->AssignIDtoOutEntity();
		m_pRoot->AssignIDtoEntity();
	}

	//Defaults the entites to the 
	if (!octantActive)
	{
		int sObjects = m_pEntityMngr->GetEntityCount();
		int dObjects = MyDynamicEntityManager::GetInstance()->GetEntityCount();

		m_pEntityMngr->ClearDimensionSetAll();
		MyDynamicEntityManager::GetInstance()->ClearDimensionSetAll();
	}



	//Update Entity Manager
	m_pEntityMngr->Update();
	MyDynamicEntityManager::GetInstance()->Update();

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
	MyDynamicEntityManager::GetInstance()->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//display octree
	if (octantDisplay)
	{
		if (m_uOctantID == -1)
			m_pRoot->Display();
		else
			m_pRoot->Display(m_uOctantID);
	}

	if (!octantActive)
		m_pRoot->Display(vector3(1.0f, 0.0f, 0.0f));

	//render cursor on camera plane
	matrix4 cameraPlane = m_pCameraMngr->GetCameraPlane();
	cameraPlane = glm::scale(cameraPlane, vector3(9.0f / 16.0f, 1, 1)); //divide the matrix by the aspect ratio to fix scaling on the x-axis
	crosshair->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), cameraPlane);

	//render ground
	matrix4 m4Position = glm::rotate(glm::translate(vector3(0, -5, 0)), glm::radians(-90.0f), AXIS_X);
	ground->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), m4Position);


	static uint clock = m_pSystem->GenClock();
	timer += m_pSystem->GetDeltaTime(clock);

	// it the timer is finished pause the game state
	if (timer > 30.0f)
	{
		m_pMeshMngr->ClearRenderList();
	}
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//draw gui,
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();

	
}
void Application::Release(void)
{
	SafeDelete(m_pRoot);
	//release GUI
	ShutdownGUI();
}
void Application::Restart()
{
	while (m_pEntityMngr->GetEntityCount() > 0)
		m_pEntityMngr->RemoveEntity(m_pEntityMngr->GetUniqueID());

	while (MyDynamicEntityManager::GetInstance()->GetEntityCount() > 0)
		MyDynamicEntityManager::GetInstance()->RemoveEntity(MyDynamicEntityManager::GetInstance()->GetUniqueID());

	MyDynamicEntityManager::GetInstance()->SetScore(0);

	uint uIndex = -1;
	uIndex++;

	m_pEntityMngr->AddEntity("Minecraft\\Cube.fbx");
	vector3 v3Position = vector3(rand() % 16 - 8, rand() % 16, rand() % 25 + 60);
	matrix4 m4Position = glm::translate(v3Position);
	m_pEntityMngr->SetModelMatrix(m4Position);

	uIndex++;
	m_pEntityMngr->AddEntity("Minecraft\\Cube.fbx");
	v3Position = vector3(rand() % 16 - 8, rand() % 16, rand() % 25 + 60);
	m4Position = glm::translate(v3Position);
	m_pEntityMngr->SetModelMatrix(m4Position);

	uIndex++;
	m_pEntityMngr->AddEntity("Minecraft\\Cube.fbx");
	v3Position = vector3(rand() % 16 - 8, rand() % 16, rand() % 25 + 60);
	m4Position = glm::translate(v3Position);
	m_pEntityMngr->SetModelMatrix(m4Position);

	uIndex++;
	m_pEntityMngr->AddEntity("Minecraft\\Cube.fbx");
	v3Position = vector3(rand() % 16 - 8, rand() % 16, rand() % 25 + 60);
	m4Position = glm::translate(v3Position);
	m_pEntityMngr->SetModelMatrix(m4Position);

	uIndex++;
	m_pEntityMngr->AddEntity("Minecraft\\Cube.fbx");
	v3Position = vector3(rand() % 16 - 8, rand() % 16, rand() % 25 + 60);
	m4Position = glm::translate(v3Position);
	m_pEntityMngr->SetModelMatrix(m4Position);

	uIndex++;
	m_pEntityMngr->AddEntity("Minecraft\\Cube.fbx");
	v3Position = vector3(rand() % 16 - 8, rand() % 16, rand() % 25 + 60);
	m4Position = glm::translate(v3Position);
	m_pEntityMngr->SetModelMatrix(m4Position);

	uIndex++;
	m_pEntityMngr->AddEntity("Minecraft\\Cube.fbx");
	v3Position = vector3(rand() % 16 - 8, rand() % 16, rand() % 25 + 60);
	m4Position = glm::translate(v3Position);
	m_pEntityMngr->SetModelMatrix(m4Position);

	uIndex++;
	m_pEntityMngr->AddEntity("Minecraft\\Cube.fbx");
	v3Position = vector3(rand() % 16 - 8, rand() % 16, rand() % 25 + 60);
	m4Position = glm::translate(v3Position);
	m_pEntityMngr->SetModelMatrix(m4Position);

	SafeDelete(m_pRoot);
	m_pRoot = new MyOctant(m_uOctantLevels, 5);
	m_pEntityMngr->Update();
}