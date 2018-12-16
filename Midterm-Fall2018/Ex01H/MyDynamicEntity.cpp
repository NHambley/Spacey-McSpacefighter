#include "MyDynamicEntity.h"
#include "MyDynamicEntityManager.h"


Simplex::MyDynamicEntity::MyDynamicEntity(vector3 a_v3Velocity, vector3 a_v3Position, quaternion a_qRotation, String a_sFileName, String a_sUniqueID) : MyEntity(a_sFileName, a_sUniqueID)
{
	m_v3Velocity = a_v3Velocity;
	m_v3Position = a_v3Position;
	m_qRotation = a_qRotation;
}

void Simplex::MyDynamicEntity::Update(uint index) {
	matrix4 modelMatrix = this->GetModelMatrix();
	m_v3Position = m_v3Position + m_v3Velocity * .1f;
	matrix4 m4Position = glm::translate(m_v3Position);
	MyDynamicEntityManager::GetInstance()->SetModelMatrix(m4Position, index);
}

Simplex::MyDynamicEntity::~MyDynamicEntity()
{
}
