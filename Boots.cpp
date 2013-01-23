//================================================//

#include "Boots.hpp"

//================================================//

template<> Boots* Ogre::Singleton<Boots>::msSingleton = 0;

//================================================//

Boots::Boots(void)
{
	equipType(BOOTS_TYPE_DEBUG);
}

//================================================//

Boots::~Boots(void)
{

}

//================================================//

void Boots::equipType(unsigned type)
{
	if(type == m_equippedType)
		return;

	m_equippedType = type;

	switch(type){
	case BOOTS_TYPE_NORMAL:
	case BOOTS_TYPE_DEBUG:
	default:
		m_xMultiplier = m_zMultiplier = 2.50;
		m_yMultiplier = 450.0;
		m_xAirMultiplier = m_zAirMultiplier = 0.15;
		m_gravity.setValue(0.0, -5.1, 0.0);
		break;

	case BOOTS_TYPE_VELOCITY:
		m_xMultiplier = m_zMultiplier = 3.50;
		m_yMultiplier = 500.0;
		m_xAirMultiplier = m_zAirMultiplier = 0.60;
		m_gravity.setValue(0.0, -4.3, 0.0);
		break;

	case BOOTS_TYPE_HIGH:
		m_xMultiplier = m_zMultiplier = 2.50;
		m_yMultiplier = 2500.0;
		m_xAirMultiplier = m_zAirMultiplier = 0.5;
		m_gravity.setValue(0.0, -8.80, 0.0);
		break;
	}
}

//================================================//