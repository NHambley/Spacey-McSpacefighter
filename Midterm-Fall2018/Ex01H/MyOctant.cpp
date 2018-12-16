#include "MyOctant.h"
using namespace Simplex;
using namespace std;

//Setting things beforehand
uint MyOctant::m_uOctantCount = 0;
uint MyOctant::m_uMaxLevel = 3;
uint MyOctant::m_uIdealEntityCount = 5;
Simplex::MyOctant::MyOctant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	//Initialize defaults
	Init();

	//Set values accordingly
	m_uOctantCount = 0;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uID = m_uOctantCount;

	//Set root
	m_pRoot = this;

	//Clear the child list
	m_lChild.clear();

	std::vector<vector3> minMax;

	uint objectCount = m_pEntityMngr->GetEntityCount();//Get a count of the entities in the scene
	for (int i = 0; i < objectCount; i++)
	{
		MyEntity* pEntity = m_pEntityMngr->GetEntity(i);
		MyRigidBody* pRigidBody = pEntity->GetRigidBody();
		minMax.push_back(pRigidBody->GetMinGlobal());
		minMax.push_back(pRigidBody->GetMaxGlobal());
	}
	MyRigidBody* pRigidBody = new MyRigidBody(minMax);//Creating a RigidBody from the mins and maxes

	//Getting the halfwidth of the Rigidbody
	vector3 halfWidth = pRigidBody->GetHalfWidth();
	float maximum = halfWidth.x;//Maximum as a float to later become m_fSize!

	for (int i = 1; i < 3; i++)
	{
		if (maximum < halfWidth[i])
			maximum = halfWidth[i];
	}
	vector3 center = pRigidBody->GetCenterLocal();
	minMax.clear();//Clear the list
	SafeDelete(pRigidBody);//Clean up the pointers

	//Setting the bounding area of this octant - includes the center, and the maximum and minimum
	m_fSize = maximum * 2.0f;
	m_v3Center = center;
	m_v3Max = m_v3Center + (vector3(maximum));
	m_v3Min = m_v3Center - (vector3(maximum));

	m_uOctantCount++;//Octant count raised to 1

	ConstructTree(m_uMaxLevel);
}

Simplex::MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	//Initialize defaults
	Init();
	//Set equal to the given variables
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;

	//Set max and mins
	m_v3Max = m_v3Center + (vector3(m_fSize) / 2.0f);
	m_v3Min = m_v3Center - (vector3(m_fSize) / 2.0f);

	m_uOctantCount++;
}

//Copy Constructor
Simplex::MyOctant::MyOctant(MyOctant const & other)
{
	//Assigning primitives
	m_uChildren = other.m_uChildren;
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_fSize = other.m_fSize;

	//Assigning vector3 variables
	m_v3Center = other.m_v3Center;
	m_v3Max = other.m_v3Max;
	m_v3Min = other.m_v3Min;

	//Assigning pointers
	m_pParent = other.m_pParent;
	m_pRoot, other.m_pRoot;

	//Assigning lists of children
	m_lChild, other.m_lChild;

	//Setting the singleton pointers
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();

	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = other.m_pChild[i];
	}
}

//Copy Assignment Operator
MyOctant & Simplex::MyOctant::operator=(MyOctant const & other)
{
	if (this != &other)
	{
		Release();//Reset
		Init();//Initialize
		MyOctant temp(other);
		Swap(temp);
	}
	return *this;
}

//Destructor
Simplex::MyOctant::~MyOctant(void)
{
	Release();
}

//Changes object contents for the other object's
void Simplex::MyOctant::Swap(MyOctant & other)
{
	//Swapping primitive datatypes
	std::swap(m_uChildren, other.m_uChildren);
	std::swap(m_fSize, other.m_fSize);
	std::swap(m_uID, other.m_uID);
	std::swap(m_uLevel, other.m_uLevel);

	//Swapping pointer variables
	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_pParent, other.m_pParent);

	//Swapping the list of children
	std::swap(m_lChild, other.m_lChild);

	//Swapping vector3 variables
	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Max, other.m_v3Max);
	std::swap(m_v3Min, other.m_v3Min);

	//Reinstate singleton pointers
	m_pEntityMngr = MyEntityManager::GetInstance();
	m_pMeshMngr = MeshManager::GetInstance();

	//Swapping child arrays
	for (int i = 0; i < 8; i++)
	{
		std::swap(m_pChild[i], other.m_pChild[i]);
	}
}

//Accessor - Size
float Simplex::MyOctant::GetSize(void)
{
	return m_fSize;
}

//Accessor - center in global coordinates
vector3 Simplex::MyOctant::GetCenterGlobal(void)
{
	return m_v3Center;
}

//Accessor - get minimum in global coordinates
vector3 Simplex::MyOctant::GetMinGlobal(void)
{
	return m_v3Min;
}

//Accessor - get maximum in global coordinates
vector3 Simplex::MyOctant::GetMaxGlobal(void)
{
	return m_v3Max;
}

//Checks if this object is colliding with the object in the EntityManager 
bool Simplex::MyOctant::IsColliding(uint a_uRBIndex)
{
	int objects = m_pEntityMngr->GetEntityCount();//To check the index given
	if (objects <= a_uRBIndex)
		return false;

	//Use AABB collision to check collisions
	MyEntity* pEntity = m_pEntityMngr->GetEntity(a_uRBIndex);
	MyRigidBody* pRigidBody = pEntity->GetRigidBody();
	//Getting the min and max to check against the octant
	vector3 maxCheck = pRigidBody->GetMaxGlobal();
	vector3 minCheck = pRigidBody->GetMinGlobal();

	//Checking X
	if (m_v3Max.x < minCheck.x)
		return false;
	if (m_v3Min.x > maxCheck.x)
		return false;

	//Checking y
	if (m_v3Max.y < minCheck.y)
		return false;
	if (m_v3Min.y > maxCheck.y)
		return false;

	//Checking Z
	if (m_v3Max.x < minCheck.x)
		return false;
	if (m_v3Min.x > maxCheck.x)
		return false;

	//If it got through all these checks they are colliding
	return true;
}

bool Simplex::MyOctant::IsOutColliding(uint a_uRBIndex)
{
	int objects = m_pDynamEntityMngr->GetEntityCount();//To check the index given
	if (objects <= a_uRBIndex)
		return false;

	//Use AABB collision to check collisions
	MyEntity* pEntity = m_pDynamEntityMngr->GetEntity(a_uRBIndex);
	MyRigidBody* pRigidBody = pEntity->GetRigidBody();
	//Getting the min and max to check against the octant
	vector3 maxCheck = pRigidBody->GetMaxGlobal();
	vector3 minCheck = pRigidBody->GetMinGlobal();

	//Checking X
	if (m_v3Max.x < minCheck.x)
		return false;
	if (m_v3Min.x > maxCheck.x)
		return false;

	//Checking y
	if (m_v3Max.y < minCheck.y)
		return false;
	if (m_v3Min.y > maxCheck.y)
		return false;

	//Checking Z
	if (m_v3Max.x < minCheck.x)
		return false;
	if (m_v3Min.x > maxCheck.x)
		return false;

	//If it got through all these checks they are colliding
	return true;
}

//Display the octant based on index
void Simplex::MyOctant::Display(uint a_nIndex, vector3 a_v3Color)
{
	if (m_uID == a_nIndex)
	{
		m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
		return;
	}
	for (uint i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->Display(a_v3Color);
	}
}

//Display all octants with this color
void Simplex::MyOctant::Display(vector3 a_v3Color)
{
	for (int i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->Display(a_v3Color);
	}
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}

//Display only leaf octants with this color
void Simplex::MyOctant::DisplayLeafs(vector3 a_v3Color)
{
	int leafs = m_lChild.size();
	for (int i = 0; i < leafs; i++)
		m_lChild[i]->DisplayLeafs(a_v3Color);
	m_pMeshMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) * glm::scale(vector3(m_fSize)), a_v3Color, RENDER_WIRE);
}

//Clear the list of every octant
void Simplex::MyOctant::ClearEntityList(void)
{
	for (int i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->ClearEntityList();
	}
	m_EntityList.clear();
}

void Simplex::MyOctant::Subdivide(void)
{
	//If this is the maximum it should go down, return without changing things
	if (m_uLevel >= m_uMaxLevel)
		return;

	//If there are children already, return without changing things
	if (m_uChildren != 0)
		return;

	m_uChildren = 8;

	float childSize = m_fSize / 4.0f;
	float childDiam = m_fSize / 2.0f;
	vector3 center;

	//Bottom Left Back Octant
	center = m_v3Center;
	center.x -= childSize;
	center.y -= childSize;
	center.z -= childSize;
	m_pChild[0] = new MyOctant(center, childDiam);

	//Bottom Right Back Octant
	center.x += childDiam;
	m_pChild[1] = new MyOctant(center, childDiam);

	//Bottom Right Front Octant
	center.z += childDiam;
	m_pChild[2] = new MyOctant(center, childDiam);

	//Bottom Left Front Octant
	center.x -= childDiam;
	m_pChild[3] = new MyOctant(center, childDiam);

	//Top Left Front Octant
	center.y += childDiam;
	m_pChild[4] = new MyOctant(center, childDiam);

	//Top Left Back Octant
	center.z -= childDiam;
	m_pChild[5] = new MyOctant(center, childDiam);

	//Top Right Back Octant
	center.x += childDiam;
	m_pChild[6] = new MyOctant(center, childDiam);

	//Top Right Front Octant
	center.z += childDiam;
	m_pChild[7] = new MyOctant(center, childDiam);

	for (int i = 0; i < 8; i++)
	{
		m_pChild[i]->m_pRoot = m_pRoot;
		m_pChild[i]->m_pParent = this;
		m_pChild[i]->m_uLevel = m_uLevel + 1;
		if (m_pChild[i]->ContainsMoreThan(m_uIdealEntityCount))
		{
			m_pChild[i]->Subdivide();
		}
	}
}
//Accessor of child by index
MyOctant* Simplex::MyOctant::GetChild(uint a_nChild)
{
	if (a_nChild > 7)
		return nullptr;

	return m_pChild[a_nChild];
}

//Get the parent of a child
MyOctant * Simplex::MyOctant::GetParent(void)
{
	return m_pParent;
}

//Check if it's a leaf; has no children, like a milennial married couple.
bool Simplex::MyOctant::IsLeaf(void)
{
	if (m_uChildren == 0)
		return true;
	return false;
}

//Checks the Entity list size against the given number
bool Simplex::MyOctant::ContainsMoreThan(uint a_nEntities)
{
	//Initialize some variables to get a count of objects in this octant
	int objectCount = 0;
	int objects = m_pEntityMngr->GetEntityCount();
	for (int i = 0; i < objects; i++)
	{
		if (IsColliding(i))//If they're colliding, add to the count of objects in this octant
			objectCount++;
		if (objectCount > a_nEntities)//If it ever exceeds the number given, return true
			return true;
	}
	return false;
}

//Deletes all octants of this tree except for the root
void Simplex::MyOctant::KillBranches(void)
{
	/*Travel down this tree until a child with no children of itself is found
	  Then, recursively delete until it reaches the node that called the method*/
	for (int i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->KillBranches();
		delete m_pChild[i];
		m_pChild[i] = nullptr;
	}
	m_uChildren = 0;
}

//Construct the tree from the beginning
void Simplex::MyOctant::ConstructTree(uint a_nMaxLevel)
{
	//Only the root should be able to call this method
	if (m_uLevel != 0)
		return;

	//Setting the max level and the octant count
	m_uMaxLevel = a_nMaxLevel;
	m_uOctantCount = 1;

	//Clearing entity list
	m_EntityList.clear();

	//Clear the tree
	KillBranches();
	m_lChild.clear();

	//Start subdividing now
	if (ContainsMoreThan(m_uIdealEntityCount))
	{
		Subdivide();
	}

	//Add octant ID to Entities
	AssignIDtoEntity();

	//Contruct the list of objects
	ConstructList();
}

void Simplex::MyOctant::AssignIDtoEntity(void)
{
	//Go until a leaf is reached
	for (int i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->AssignIDtoEntity();
	}

	//If it's a leaf, move forward
	if (m_uChildren == 0)
	{
		int entityCount = m_pEntityMngr->GetEntityCount();
		for (int i = 0; i < entityCount; i++)
		{
			if (IsColliding(i))
			{
				m_EntityList.push_back(i);//Add this object to the EntityList
				m_pEntityMngr->AddDimension(i, m_uID);//Add this dimension to the entity
			}
		}
	}
}

void Simplex::MyOctant::AssignIDtoOutEntity(void)
{
	//Go until a leaf is reached
	for (int i = 0; i < m_uChildren; i++)
	{
		m_pChild[i]->AssignIDtoOutEntity();
	}

	if (m_uChildren == 0)
	{
		int entityCount = m_pDynamEntityMngr->GetEntityCount();//Sets up the count for the entities
		for (int i = 0; i < entityCount; i++)
		{
			if (IsOutColliding(i))
			{
				m_pDynamEntityMngr->AddDimension(i, m_uID);//Add this dimension to the entity
			}
		}
	}
}

//Accessor - Gives count of octants currently existing
uint Simplex::MyOctant::GetOctantCount(void)
{
	return m_uOctantCount;
}

void Simplex::MyOctant::Release(void)
{
	//Kill branches if this is the root
	if (m_uLevel == 0)
	{
		KillBranches();
	}

	m_uChildren = 0;
	m_fSize = 0.0f;
	m_EntityList.clear();
	m_lChild.clear();
}

void Simplex::MyOctant::Init(void)
{
	//Setting variables
	m_uChildren = 0;
	m_fSize = 0.0f;

	m_uID = m_uOctantCount;
	m_uLevel = 0;

	m_v3Center = vector3(0.0f);
	m_v3Max = vector3(0.0f);
	m_v3Min = vector3(0.0f);

	//Setting the singleton pointers
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();
	m_pDynamEntityMngr = MyDynamicEntityManager::GetInstance();

	//Setting pointers to nullptr to start
	m_pRoot = nullptr;
	m_pParent = nullptr;
	for (int i = 0; i < 8; i++)
		m_pChild[i] = nullptr;
}

//Constructs the list of children that are leaves that have entities within them
void Simplex::MyOctant::ConstructList(void)
{
	//Call this method for every node of the tree
	for (int i = 0; i < m_uChildren; i++)
		m_pChild[i]->ConstructList();

	//If this object has an object in it, add it to the list
	if (m_EntityList.size() > 0)
		m_pParent->m_lChild.push_back(this);
}
