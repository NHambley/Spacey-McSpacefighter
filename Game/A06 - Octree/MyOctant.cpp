#include "MyOctant.h"
using namespace Simplex;
uint MyOctant::m_nCount = 0;

MyOctant::MyOctant()
{
	Init();
	MyEntity** entityArray = m_pEntityMngr->GetEntityArray();
	uint entityCount = m_pEntityMngr->GetEntityCount();
	std::vector<vector3> v3MaxMinList;
	for (int i = 0; i < entityCount; i++)
	{
		MyRigidBody* pRG = entityArray[i]->GetRigidBody();
		vector3 v3Min = pRG->GetMinGlobal();
		vector3 v3Max = pRG->GetMaxGlobal();
		v3MaxMinList.push_back(v3Min);
		v3MaxMinList.push_back(v3Max);
	}

	m_pRigidBody = new MyRigidBody(v3MaxMinList);
	m_pRigidBody->MakeCubic();
	m_iID = m_nCount;
	Subdivide();
}

MyOctant::MyOctant(vector3 a_v3Center, float a_fSize)
{
	Init();
	std::vector<vector3> v3MaxMinList;
	v3MaxMinList.push_back(a_v3Center - vector3(a_fSize));
	v3MaxMinList.push_back(a_v3Center + vector3(a_fSize));
	m_pRigidBody = new MyRigidBody(v3MaxMinList);
	m_nCount++;
	m_iID = m_nCount;
}

MyOctant::MyOctant(uint a_uOctantLevels, uint a_uEntityCount)
{
	Init();
	MyEntity** entityArray = m_pEntityMngr->GetEntityArray();
	uint entityCount = m_pEntityMngr->GetEntityCount();
	std::vector<vector3> v3MaxMinList;
	for (int i = 0; i < entityCount; i++)
	{
		MyRigidBody* pRG = entityArray[i]->GetRigidBody();
		vector3 v3Min = pRG->GetMinGlobal();
		vector3 v3Max = pRG->GetMaxGlobal();
		v3MaxMinList.push_back(v3Min);
		v3MaxMinList.push_back(v3Max);
	}

	m_pRigidBody = new MyRigidBody(v3MaxMinList);
	m_pRigidBody->MakeCubic();
	m_iID = m_nCount;
	Subdivide(a_uOctantLevels);
}

MyOctant::MyOctant(MyOctant const& other)
{
	m_nData = other.m_nData;
	m_lData = other.m_lData;
}

MyOctant& MyOctant::operator=(MyOctant const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyOctant temp(other);
		Swap(temp);
	}
	return *this;
}

MyOctant::~MyOctant(void)
{
	Release();
}

void MyOctant::Swap(MyOctant& other)
{
	std::swap(m_nData, other.m_nData);
	std::swap(m_lData, other.m_lData);
}

int MyOctant::GetData(void)
{
	return m_nData;
}

void MyOctant::SetData(int a_nData)
{
	m_nData = a_nData;
}

int& MyOctant::GetDataOnVector(int a_nIndex)
{
	int nIndex = static_cast<int>(m_lData.size());
	assert(a_nIndex >= 0 && a_nIndex < nIndex);
	return m_lData[a_nIndex];
}

void MyOctant::SetDataOnVector(int a_nData)
{
	m_lData.push_back(a_nData);
}

void MyOctant::Display(void)
{
	m_pRigidBody->AddToRenderList();
	for (uint i = 0; i < 8; i++)
	{
		if (m_pChild[i])
			m_pChild[i]->Display();
	}
}

void MyOctant::IsColliding(void)
{
	for (int i = 0; i < 8; i++)
	{
		if (m_pChild[i])
			m_pChild[i]->IsColliding();
		else
		{
			MyEntity** entityArray = m_pEntityMngr->GetEntityArray();
			uint iEntityCount = m_pEntityMngr->GetEntityCount();
			for (uint j = 0; j < iEntityCount; j++)
			{
				MyRigidBody* pRB = entityArray[j]->GetRigidBody();
				if (pRB->IsColliding(m_pRigidBody))
				{
					entityArray[j]->AddDimension(m_iID);
				}
			}
		}
	}
}

void MyOctant::Subdivide(uint a_nLevels)
{
	if (m_nLevel >= a_nLevels)
		return;

	vector3 v3Center = m_pRigidBody->GetCenterLocal();
	vector3 v3HalfWidth = m_pRigidBody->GetHalfWidth();
	float fSize = (v3HalfWidth.x) / 2.0f;
	float fCenters = fSize;

	m_pChild[0] = new MyOctant(v3Center + vector3(fCenters, fCenters, fCenters), fSize);
	m_pChild[1] = new MyOctant(v3Center + vector3(-fCenters, fCenters, fCenters), fSize);
	m_pChild[2] = new MyOctant(v3Center + vector3(-fCenters, -fCenters, fCenters), fSize);
	m_pChild[3] = new MyOctant(v3Center + vector3(fCenters, -fCenters, fCenters), fSize);

	m_pChild[4] = new MyOctant(v3Center + vector3(fCenters, fCenters, -fCenters), fSize);
	m_pChild[5] = new MyOctant(v3Center + vector3(-fCenters, fCenters, -fCenters), fSize);
	m_pChild[6] = new MyOctant(v3Center + vector3(-fCenters, -fCenters, -fCenters), fSize);
	m_pChild[7] = new MyOctant(v3Center + vector3(fCenters, -fCenters, -fCenters), fSize);

	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i]->m_nLevel = m_nLevel + 1;
		m_pChild[i]->m_pParent = this;
		m_pChild[i]->Subdivide(a_nLevels);
	}
}

//private methods

void MyOctant::Release(void)
{
	m_lData.clear();
}

void MyOctant::Init(void)
{
	m_nData = 0;
	m_pMeshMngr = MeshManager::GetInstance();
	m_pEntityMngr = MyEntityManager::GetInstance();
	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = nullptr;
	}
}