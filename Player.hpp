//================================================//

#ifndef __PLAYER_HPP__
#define __PLAYER_HPP__

//================================================//

#include "stdafx.h"
#include "Base.hpp"
#include "Camera.hpp"

// Game headers
#include "Inventory.hpp"
#include "Boots.hpp"
#include "EventManager.hpp"
#include "DynamicObject.hpp"
#include "TextRenderer.hpp"
#include "Flashlight.hpp"
#include "Glasses.hpp"
#include "Profile.hpp"
#include "Sound.hpp"

// Weapon/Item headers
#include "Weapon.hpp"
#include "MLP.hpp"
#include "WarpGun.hpp"
#include "MeleeWeapons.hpp"

//================================================//

class Player
{
public:
	Player(Ogre::SceneManager* mgr, Profile* profile);
	~Player(void);

	bool init(void);
	void spawn(Ogre::SceneNode* node);
	void action(EventManager* eventManager);
	void processRetrieval(DynamicObject* object);

	void initFlashlight(void);
	void initGlasses(Sparks::Compositor* compositor);

	// Getter functions
	Weapon* getWeapon(void) const;
	const unsigned getWeaponValue(void) const;		
	const Ogre::Vector3 getPosition(void) const;
	Sparks::Camera* getCamera(void) const;
	Flashlight* getFlashlight(void) const;
	Glasses* getGlasses(void) const;

	// Setter functions
	void setWeapon(unsigned weapon, EventManager* pEventManager);
	void setPosition(const Ogre::Vector3 pos);
	void setOrientation(const Ogre::Quaternion q);

	void update(double timeSinceLastFrame);

	// --- //

protected:
	// Ogre
	Ogre::SceneManager*		m_pSceneMgr;

	// Camera
	Sparks::Camera*			m_pCamera;

	Profile*				m_pProfile;

	Flashlight*				m_pFlashlight;
	Glasses*				m_pGlasses;

	Weapon*					m_pWeapon;
	unsigned				m_nWeapon;
	Ogre::Real				m_actionRange;

	// Sounds
	Sound*					m_pSoundRetrieve;
};

//================================================//

inline Weapon* Player::getWeapon(void) const
{ return m_pWeapon; }

inline const unsigned Player::getWeaponValue(void) const
{ return m_nWeapon; }

inline const Ogre::Vector3 Player::getPosition(void) const
{ return m_pCamera->getSceneNode()->getPosition(); }

inline Sparks::Camera* Player::getCamera(void) const
{ return m_pCamera; }

inline Flashlight* Player::getFlashlight(void) const
{ return m_pFlashlight; }

inline Glasses* Player::getGlasses(void) const
{ return m_pGlasses; }

inline void Player::setPosition(const Ogre::Vector3 pos)
{ m_pCamera->setPosition(pos); }

inline void Player::setOrientation(const Ogre::Quaternion q)
{ m_pCamera->setOrientation(q); }

//================================================//

#endif

//================================================//