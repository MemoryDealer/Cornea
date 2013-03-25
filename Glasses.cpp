//================================================//

#include "Glasses.hpp"

//================================================//

Glasses::Glasses(Sparks::Compositor* compositor)
{
	m_pCompositor = compositor;
	this->unequip();
}

//================================================//

Glasses::~Glasses(void)
{
	
}

//================================================//

void Glasses::flipState(void)
{
	if(m_equipped)
		this->unequip();
	else
		this->equip();
}

//================================================//

void Glasses::equip(void)
{
	m_pCompositor->setEnabled(COMPOSITOR_GAUSSIAN_BLUR, false);
	m_equipped = true;
}

//================================================//

void Glasses::unequip(void)
{
	m_pCompositor->setEnabled(COMPOSITOR_GAUSSIAN_BLUR, true);
	m_equipped = false;
}

//================================================//