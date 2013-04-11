//================================================//

#include "Inventory.hpp"

//================================================//

Inventory::Inventory(void)
{
	memset(m_weapons, 0, sizeof(m_weapons));

	// Generate our first xor values
	this->newXor(MAGIC_CUBES);

	this->setDefaults();	
}

//================================================//

Inventory::~Inventory(void)
{

}

//================================================//

void Inventory::newXor(unsigned int index)
{
	// This is a simple hack to prevent easy run-time hacking
	// by preventing someone from just searching for the value in memory.
	m_xor[index] = Ogre::Math::RangeRandom(0, 10000);
}

//================================================//

void Inventory::setDefaults(void)
{
	m_magicCubes = 0;
	m_magicCubes ^= m_xor[MAGIC_CUBES];

	m_weapons[0] = Weapon::NONE;
	m_weapons[1] = Weapon::TEST_GUN;
	m_weapons[2] = Weapon::TEST_SWORD;
	m_weapons[3] = Weapon::NONE;
}

//================================================//

void Inventory::addMagicCube(void)
{
	// Decrypt with current key
	m_magicCubes ^= m_xor[MAGIC_CUBES];

	m_magicCubes++;

	// Generate new key and encrypt
	this->newXor(MAGIC_CUBES);
	m_magicCubes ^= m_xor[MAGIC_CUBES];
}

//================================================//

const unsigned int Inventory::getCount(int type)
{
	// Decrypt the right value and return it, after encrypting it back
	unsigned int count = 0;

	switch(type){
	default:
		break;

	case MAGIC_CUBES:
		m_magicCubes ^= m_xor[MAGIC_CUBES];
		count = m_magicCubes;

		this->newXor(MAGIC_CUBES);
		m_magicCubes ^= m_xor[MAGIC_CUBES];
		break;
	}

	return count;
}

//================================================//