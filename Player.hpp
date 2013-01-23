//================================================//

#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__

//================================================//

#include "Base.hpp"
#include "Camera.hpp"

// Game headers
#include "Boots.hpp"
#include "EventManager.hpp"
#include "DynamicObject.hpp"

// Weapon headers
#include "Weapon.hpp"
#include "MLP.hpp"
#include "WarpGun.hpp"
#include "MeleeWeapons.hpp"

//================================================//

class Player
{
public:
	Player(Ogre::SceneManager* mgr);
	~Player(void);

	void action(EventManager* eventManager);

	// Getter functions
	Weapon* getWeapon(void) const;
	const Ogre::Vector3 getPosition(void) const;
	Sparks::Camera* getCamera(void) const;

	// Setter functions
	void setWeapon(unsigned weapon, EventManager* pEventManager);

	void update(double timeSinceLastFrame);

	// --- //

	enum{
		WEAPON_NONE = 0,
		WEAPON_DEFAULT,
		WEAPON_WARPGUN
	};

private:
	// Ogre
	Ogre::SceneManager*		m_pSceneMgr;

	// Camera
	Sparks::Camera*			m_pCamera;

	Weapon*			m_pWeapon;
	unsigned		m_nWeapon;
	Ogre::Real		m_actionRange;
};

//================================================//

inline Weapon* Player::getWeapon(void) const
{ return m_pWeapon; }

inline const Ogre::Vector3 Player::getPosition(void) const
{ return m_pCamera->getSceneNode()->getPosition(); }

inline Sparks::Camera* Player::getCamera(void) const
{ return m_pCamera; }

//================================================//

#endif

//================================================//