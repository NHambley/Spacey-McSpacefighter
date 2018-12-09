//

#ifndef __MY_OCTANT_H_
#define __MY_OCTANT_H_

#include "Simplex\Simplex.h"
#include "MyEntityManager.h"

namespace Simplex
{
	class MyOctant
	{
		int m_nData = 0;
		std::vector<int> m_lData;
		MeshManager* m_pMeshMngr = nullptr;
		MyEntityManager* m_pEntityMngr = nullptr;
		MyRigidBody* m_pRigidBody = nullptr;
		uint m_iID = 0;
		static uint m_nCount;
		MyOctant* m_pParent = nullptr;
		MyOctant* m_pChild[8];
		uint m_nLevel = 0;
		uint m_nMaxChildren = 0;
	public:
		MyOctant(void);
		MyOctant(vector3 a_v3Center, float a_fSize);
		MyOctant(uint a_uOctantLevels, uint a_uEntityCount);
		MyOctant(MyOctant const& other);
		MyOctant& operator=(MyOctant const& other);
		~MyOctant(void);
		void Swap(MyOctant& other);
		int GetData(void);
		void SetData(int a_nData = 1);
		int& GetDataOnVector(int a_nIndex);
		void SetDataOnVector(int a_nData);
		void Display(void);
		void IsColliding(void);
		void Subdivide(uint a_nLevels = 1);
	private:
		void Release(void);
		void Init(void);
	}; //end class
} //end namespace

#endif