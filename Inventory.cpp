//================================================//

#include "Inventory.hpp"

//================================================//

Inventory::Inventory(void)
{
	memset(m_weapons, 0, sizeof(m_weapons));

	m_weapons[1] = Weapon::TEST_GUN;
	m_weapons[2] = Weapon::TEST_SWORD;
}

//================================================//

Inventory::~Inventory(void)
{

}

//================================================//