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

	// Getter functions
	const unsigned	getWeapon(unsigned slot) const;

	// Setter functions
	void			setWeapon(unsigned weapon, unsigned slot);

private:


	// Weapons
	unsigned		m_weapons[4];
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