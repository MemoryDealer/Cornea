//================================================//

#ifndef __BOOTS_HPP__
#define __BOOTS_HPP__

//================================================//

#include "stdafx.h"

//================================================//

class Boots : public Ogre::Singleton<Boots>
{
public:
	Boots(void);
	~Boots(void);

	void equipType(unsigned type);

	// Getter functions
	const unsigned		getEquippedType(void) const;
	const Ogre::Real	getXMultiplier(void) const;
	const Ogre::Real	getYMultiplier(void) const;
	const Ogre::Real	getZMultiplier(void) const;
	const Ogre::Real	getXAirMultiplier(void) const;
	const Ogre::Real	getZAirMultiplier(void) const;
	const btVector3		getGravity(void) const;

	// Setter functions
	void setXMultiplier(const Ogre::Real x);
	void setYMultiplier(const Ogre::Real y);
	void setZMultiplier(const Ogre::Real z);
	void setXAirMultiplier(const Ogre::Real x);
	void setZAirMultiplier(const Ogre::Real z);
	void setGravity(const btVector3& gravity);

	// --- //

	enum Types{
		BOOTS_TYPE_NORMAL = 0,
		BOOTS_TYPE_VELOCITY,
		BOOTS_TYPE_HIGH,

		BOOTS_TYPE_DEBUG
	};
private:

	// --- //

	unsigned		m_equippedType;

	// Movement constants
	Ogre::Real		m_xMultiplier;
	Ogre::Real		m_yMultiplier;
	Ogre::Real		m_zMultiplier;
	Ogre::Real		m_xAirMultiplier;
	Ogre::Real		m_zAirMultiplier;
	btVector3		m_gravity;
};

//================================================//

// Getter functions

inline const unsigned Boots::getEquippedType(void) const
{ return m_equippedType; }

inline const Ogre::Real Boots::getXMultiplier(void) const
{ return m_xMultiplier; }

inline const Ogre::Real Boots::getYMultiplier(void) const
{ return m_yMultiplier; }

inline const Ogre::Real Boots::getZMultiplier(void) const
{ return m_zMultiplier; }

inline const Ogre::Real Boots::getXAirMultiplier(void) const
{ return m_xAirMultiplier; }

inline const Ogre::Real Boots::getZAirMultiplier(void) const
{ return m_zAirMultiplier; }

inline const btVector3 Boots::getGravity(void) const
{ return m_gravity; }

// Setter functions

inline void Boots::setXMultiplier(const Ogre::Real x)
{ m_xMultiplier = x; }

inline void Boots::setYMultiplier(const Ogre::Real y)
{ m_yMultiplier = y; }

inline void Boots::setZMultiplier(const Ogre::Real z)
{ m_zMultiplier = z; }

inline void Boots::setXAirMultiplier(const Ogre::Real x)
{ m_xAirMultiplier = x; }

inline void Boots::setZAirMultiplier(const Ogre::Real z)
{ m_zAirMultiplier = z; }

inline void Boots::setGravity(const btVector3& gravity)
{ m_gravity = gravity; }

//================================================//

#endif

//================================================//