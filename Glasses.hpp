//================================================//

#ifndef __GLASSES_HPP__
#define __GLASSES_HPP__

//================================================//

#include "stdafx.h"
#include "Compositor.hpp"

//================================================//

class Glasses
{
public:
	Glasses(Sparks::Compositor* compositor);
	~Glasses(void);

	void flipState(void);
	void equip(void);
	void unequip(void);

	// Getter functions
	bool equipped(void) const;

private:
	Sparks::Compositor* m_pCompositor;

	bool	m_equipped;
};

//================================================//

inline bool Glasses::equipped(void) const
{ return m_equipped; }

//================================================//

#endif

//================================================//