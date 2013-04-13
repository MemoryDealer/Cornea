//================================================//

#include "RetrievableItems.hpp"

//================================================//

MagicCube::MagicCube(void)
	: DynamicObject()
{
	m_retrievable = true;
}

//================================================//

void MagicCube::retrieve(void)
{
	DynamicObject::retrieve();
}

//================================================//

void MagicCube::update(double timeSinceLastFrame)
{

}

//================================================//