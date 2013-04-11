//================================================//

#ifndef __INVENTORY_HPP__
#define __INVENTORY_HPP__

//================================================//

#include "Weapon.hpp"

//================================================//

class Inventory
{
public:
	Inventory(void);
	~Inventory(void);

	enum{
		MAGIC_CUBES = 0,

		END
	};

	void setDefaults(void);

	// Getter functions
	const unsigned	getWeapon(unsigned slot) const;
	

	// Setter functions
	void			setWeapon(unsigned weapon, unsigned slot);

	// Modifier functions
	void addMagicCube(void);

	const unsigned int getCount(int type);

private:
	void newXor(unsigned int index);

	unsigned int m_xor[10];

	// Collectible items
	unsigned int m_magicCubes;

	// Weapons
	int		m_weapons[4];
};

//================================================//

// Getter functions
inline const unsigned Inventory::getWeapon(unsigned slot) const
{ return m_weapons[slot]; }

// Setter functions
inline void Inventory::setWeapon(unsigned weapon, unsigned slot)
{ m_weapons[slot] = weapon; }

//================================================//

#endif

//================================================//