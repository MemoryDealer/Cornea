//================================================//

#include "Player.hpp"

//================================================//

Player::Player(Ogre::SceneManager* mgr)
{
	// Set the scene manager pointer
	m_pSceneMgr = mgr;

	// Create the camera
	m_pCamera = new Sparks::Camera(m_pSceneMgr, 30000);
	m_pCamera->setMode(Sparks::Camera::MODE_FIRST_PERSON);

	Base::getSingletonPtr()->m_pViewport->setCamera(m_pCamera->getOgreCamera());

	// Set weapon to NULL for now
	m_nWeapon = Weapon::NONE;
	m_pWeapon = nullptr;

	// How close a player should be to trigger an action
	m_actionRange = 45.0;
}

//================================================//

Player::~Player(void)
{
	if(m_pCamera) delete m_pCamera;
	if(m_pWeapon != nullptr) delete m_pWeapon;
	if(m_pFlashlight != nullptr) delete m_pFlashlight;
}

//================================================//

void Player::action(EventManager* eventManager)
{
	Rayhit* rayhit = m_pCamera->getNegativeZRayhit();

	if(rayhit->hasHit){
		//printf("Object hit %s\nDistance: %.2f\n", rayhit->node->getName().c_str(), rayhit->distance);

		// Iterate through dynamic objects
		std::vector<DynamicObject*> objects = eventManager->getDynamicObjectManager()->getObjects();

		for(std::vector<DynamicObject*>::iterator itr = objects.begin();
			itr != objects.end();
			++itr){
			if((*itr)->getSceneNode() == rayhit->node){
				if(rayhit->distance <= m_actionRange){
					(*itr)->send(DynamicObject::ARG_ACTION);
					break;
				}
			}
		}
	}
}

//================================================//

void Player::initFlashlight(void)
{
	m_pFlashlight = new Flashlight(m_pSceneMgr, m_pCamera->getRollNode());
}

//================================================//

void Player::setWeapon(unsigned weapon, EventManager* pEventManager)
{
	if(weapon != Weapon::NONE){
		if(m_nWeapon == weapon){
			return;
		}
	}

	// Set new weapon and delete the old class
	m_nWeapon = weapon;
	if(m_pWeapon != nullptr)
		delete m_pWeapon;

	// Load the new derived class for the weapon
	switch(m_nWeapon){
	case Weapon::NONE:
	default:
		m_pWeapon = new WeaponNone(m_pSceneMgr);
		break;

	case Weapon::TEST_GUN:
		m_pWeapon = new MLP(m_pSceneMgr);
		break;

	case Weapon::TEST_SWORD:
		m_pWeapon = new Sword(m_pSceneMgr);
		break;
	}

	// Initialise the weapon
	m_pWeapon->init(m_pCamera->getRollNode());

	// Set manager pointers
	m_pWeapon->setNPCManager(pEventManager->getNPCManager());
	m_pWeapon->setProjectileManager(pEventManager->getProjectileManager());
}

//================================================//

void Player::update(double timeSinceLastFrame)
{
	// Update the camera
	m_pCamera->update(timeSinceLastFrame);

	// Update the player's weapon
	m_pWeapon->update(timeSinceLastFrame);

	// Update player's flashlight
	m_pFlashlight->update(timeSinceLastFrame);
}

//================================================//