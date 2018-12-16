#pragma once
#include "MyEntity.h"

namespace Simplex {
	class MyDynamicEntity :
		public MyEntity
	{
	private:
		vector3 m_v3Velocity;
		vector3 m_v3Position;
		quaternion m_qRotation;
	public:
		MyDynamicEntity(vector3 a_v3Velocity, vector3 a_v3Position, quaternion a_qRotation, String a_sFileName, String a_sUniqueID = "NA");
		void Update(uint i);
		~MyDynamicEntity();
	};
}

