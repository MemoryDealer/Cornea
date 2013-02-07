//================================================//

#include "Inventory.hpp"

//================================================//

Inventory::Inventory(void)
{
	memset(m_weapons, 0, sizeof(m_weapons));

	this->setDefaults();	
}

//================================================//

Inventory::~Inventory(void)
{

}

//================================================//

void Inventory::setDefaults(void)
{
	m_weapons[0] = Weapon::NONE;
	m_weapons[1] = Weapon::TEST_GUN;
	m_weapons[2] = Weapon::TEST_SWORD;
	m_weapons[3] = Weapon::NONE;
}

//================================================//