//================================================//

#include "Projectile.hpp"

//================================================//

Projectile::~Projectile(void)
{
	
}

//================================================//

void Projectile::init(Ogre::SceneManager* mgr, PROJECTILE_DATA& data, NPCManager* npcManager)
{
	// Set NPCManager pointer
	m_pNPCManager = npcManager;

	// Set particle data
	m_origin = data.origin;
	m_direction = data.direction;
	m_speed = data.speed;
	m_damage = data.damage;
	m_range = data.range;
	m_colour = data.colour;

	// Set scene manager pointer
	m_pSceneMgr = mgr;

	// Create the light and billboard
	m_pLight = m_pSceneMgr->createLight();
	m_pLight->setDiffuseColour(m_colour);
	m_pLight->setSpecularColour(m_colour);
	m_pLight->setAttenuation(1000, 1, 0.0005, 0);

	m_pBbSet = m_pSceneMgr->createBillboardSet(1);
	// TODO: Use switch statement here to get the right texture/model from projectile data
	m_pBbSet->setMaterialName("Examples/Flare3");
	m_pBb = m_pBbSet->createBillboard(0, 0, 0, m_colour);

	// Create the scene node
	m_pSceneNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	m_pSceneNode->attachObject(m_pLight);
	m_pSceneNode->attachObject(m_pBbSet);

	m_pSceneNode->setPosition(m_origin);

	m_alive = true;
	m_distanceMoved = 0.0;

	m_lastPosition = m_origin;
}

//================================================//

void Projectile::cleanup(void)
{
	m_alive = false;

	m_pSceneNode->detachAllObjects();

	m_pBbSet->removeBillboard(m_pBb);
	m_pSceneMgr->destroyBillboardSet(m_pBbSet);
	m_pSceneMgr->destroyLight(m_pLight);
}

//================================================//

bool Projectile::isColliding(void)
{
	btVector3 from(m_lastPosition.x, m_lastPosition.y, m_lastPosition.z);
	btVector3 to(m_pSceneNode->getPosition().x, m_pSceneNode->getPosition().y, m_pSceneNode->getPosition().z);

	btCollisionWorld::ClosestRayResultCallback res(from, to);

	Base::getSingletonPtr()->m_btWorld->rayTest(from, to, res);

	// set the new "last" position
	m_lastPosition = m_pSceneNode->getPosition();
	
	if(res.hasHit()){
		Ogre::SceneNode* node = static_cast<Ogre::SceneNode*>(res.m_collisionObject->getUserPointer());

		// Impact effect
		// ...

		if(node){
			Ogre::String name = node->getName();
			Ogre::StringUtil strUtil;

			// Test if projectile hit an NPC
			if(strUtil.startsWith(name, "NPC_", false)){
				std::vector<NPC*> NPCs = m_pNPCManager->getNPCs();

				if(NPCs.size() > 0){
					for(std::vector<NPC*>::iterator itr = NPCs.begin();
						itr != NPCs.end();
						++itr){
						Ogre::String npcName = static_cast<Ogre::String>((*itr)->getSceneNode()->getName());

						if(strUtil.match(name, npcName, true)){
							printf("Projectile hit %s\n", npcName.c_str());

							// Send damage to NPC
							(*itr)->takeDamage(m_damage);
						}
					}
				}
			}
		}

		return true;
	}

	return false;
}

//================================================//

void Projectile::update(double timeSinceLastFrame)
{
	Ogre::Vector3 translate = m_direction * m_speed * timeSinceLastFrame;

	// Translate the projectile node
	m_pSceneNode->translate(translate);

	// Test for collision
	if(isColliding()){
		cleanup();
	}

	// Test for range
	m_distanceMoved += translate.length();
	if(m_distanceMoved > m_range){
		cleanup();
	}
}

//================================================//

// *** //

//================================================//

ProjectileManager::ProjectileManager(Ogre::SceneManager* mgr)
{
	m_pSceneMgr = mgr;
}

//================================================//

ProjectileManager::~ProjectileManager(void)
{

}

//================================================//

void ProjectileManager::addProjectile(Projectile::PROJECTILE_DATA& data)
{
	m_projectiles.push_back(new Projectile()); // memory leak?
	m_projectiles.back()->init(m_pSceneMgr, data, m_pNPCManager);
}

//================================================//

void ProjectileManager::clearAll(void)
{
	for(std::vector<Projectile*>::iterator itr = m_projectiles.begin();
		itr != m_projectiles.end();){
			if((*itr)->isAlive()){
				(*itr)->cleanup();
			}

			delete *itr;
			itr = m_projectiles.erase(itr);
	}
}

//================================================//

void ProjectileManager::update(double timeSinceLastFrame)
{
	// Update all the projectiles and remove dead ones
	for(std::vector<Projectile*>::iterator itr = m_projectiles.begin();
		itr != m_projectiles.end();){
			if((*itr)->isAlive()){
				(*itr)->update(timeSinceLastFrame);
				++itr;
			}
			else{
				delete *itr;
				itr = m_projectiles.erase(itr);
			}
	}

	// Update the player projectiles
	//for(std::vector<Projectile*>::iterator itr = m_projectiles.begin();
	//	itr != m_projectiles.end();
	//	++itr){
	//		// prevent the iterator from going out of bounds
	//		if(m_projectiles.empty())
	//			break;
	//		
	//		// update it if it's alive
	//		if((*itr)->isAlive()){
	//			(*itr)->update(timeSinceLastFrame);
	//			numProjectiles++;
	//		}	
	//}

	//// Clear vector when player is not firing
	//if(numProjectiles == 0)
	//	m_projectiles.clear();

	
}

//================================================//