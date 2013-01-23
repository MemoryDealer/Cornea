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
	m_nWeapon = WEAPON_NONE;
	m_pWeapon = 0;

	// How close a player should be to trigger an action
	m_actionRange = 45.0;
}

//================================================//

Player::~Player(void)
{
	if(m_pCamera) delete m_pCamera;
	if(m_pWeapon) delete m_pWeapon;
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

//void Player::action(EventManager* eventManager)
//{
//	// Cast a ray forward from the player and test it against the object hit
//	Ogre::Vector3 pos = m_pCamera->getSceneNode()->getPosition();
//	Ogre::Vector3 direction = pos + (m_pCamera->getDirection() * 5000.0);
//
//	btVector3 from(pos.x, pos.y, pos.z);
//	btVector3 to(direction.x, direction.y, direction.z);
//
//	btCollisionWorld::ClosestRayResultCallback res(from, to);
//	Base::getSingletonPtr()->m_btWorld->rayTest(from, to, res);
//
//	if(res.hasHit()){
//		Ogre::SceneNode* node = static_cast<Ogre::SceneNode*>(res.m_collisionObject->getUserPointer());
//		if(node){
//			Ogre::Vector3 hit(res.m_hitPointWorld.getX(), res.m_hitPointWorld.getY(), res.m_hitPointWorld.getZ());
//			Ogre::Real distance = pos.distance(hit);
//
//			// retrieve the container of dynamic objects
//			std::vector<DynamicObject*> objects = eventManager->getDynamicObjectManager()->getObjects();
//
//			printf("Object hit %s\nDistance: %.2f\n", node->getName().c_str(), distance);
//
//			// If the object is within range, see if it's dynamic, and send an action command to it
//			for(std::vector<DynamicObject*>::iterator itr = objects.begin();
//				itr != objects.end();){
//					if((*itr)->getSceneNode() == node){
//						
//						// The node is a dynamic object, send it the action command if it's in range
//						if(distance <= m_actionRange){
//							(*itr)->send(DynamicObject::ARG_ACTION);
//							break;
//						}
//					}
//
//					++itr;
//			}
//		}
//	}
//}

//================================================//

void Player::setWeapon(unsigned weapon, EventManager* pEventManager)
{
	if(m_nWeapon == weapon){
		return;
	}

	// Set new weapon and delete the old class
	m_nWeapon = weapon;
	if(m_pWeapon != NULL)
		delete m_pWeapon;

	// Load the new derived class for the weapon
	switch(m_nWeapon){
	case WEAPON_NONE:
	default:
		return;

	case WEAPON_DEFAULT:
		m_pWeapon = new Sword(m_pSceneMgr);
		break;

	case WEAPON_WARPGUN:
		m_pWeapon = new MLP(m_pSceneMgr);
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
	if(m_nWeapon != WEAPON_NONE)
		m_pWeapon->update(timeSinceLastFrame);
}

//================================================//