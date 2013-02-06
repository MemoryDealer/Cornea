//================================================//

#include "Weapon.hpp"

//================================================//

Weapon::Weapon(Ogre::SceneManager* mgr)
{
	m_pSceneMgr = mgr;
	m_defPosition = m_offset = m_projectileOffset = Ogre::Vector3::ZERO;

	m_state = STATE_IDLE;

	m_debugRays = false;

	// Default reticule
	m_reticuleDefault = "Examples/Crosshair";
	m_reticuleEnemy = "Examples/CrosshairRed";
	m_reticuleFriendly = "Examples/CrosshairGreen";
	m_reticuleRange = 6000.0;

	// Muzzle flash
	m_muzzleFlashType = MuzzleFlash::TYPE_NULL;

	// Sound
	m_pSoundAttack = new Sound();
}

//================================================//

Weapon::~Weapon(void)
{
	if(m_isLoaded){
		m_pEntity->detachFromParent();
		m_pSceneMgr->destroyEntity(m_pEntity);

		m_pSceneMgr->destroySceneNode(m_pSceneNode);
	}

	delete m_pSoundAttack;
}

//================================================//

void Weapon::init(Ogre::SceneNode* parentNode)
{
	m_pParentNode = parentNode;

	// Load the mesh and create the child scene node
	m_pEntity = m_pSceneMgr->createEntity(m_filename);
	m_pSceneNode = parentNode->createChildSceneNode("$PlayerWeapon");
	m_pSceneNode->attachObject(m_pEntity);

	m_pSceneNode->translate(m_offset);

	m_defPosition = m_pSceneNode->getPosition();

	// Init impact manager
	m_impactManager = new ImpactManager();

	// Init muzzle flash
	m_muzzleFlash = new MuzzleFlash(m_pSceneMgr);
}

//================================================//

void Weapon::setOffset(Ogre::Vector3 offset)
{
	m_offset = offset;

	m_pSceneNode->setPosition(m_pParentNode->getPosition());
	m_pSceneNode->translate(offset);
}

//================================================//

void Weapon::attack(Sparks::Camera* camera)
{
	/*if(m_state == STATE_IDLE){
		m_state = STATE_ATTACKING;

	}*/
	
	
}

//================================================//

bool Weapon::rayAttack(Sparks::Camera* camera)
{
	Ogre::Vector3 pos = camera->getSceneNode()->getPosition();
	Ogre::Vector3 forwards = pos + (camera->getDirection() * m_range);

	// Draw a debug line to show the bullet path
	if(m_debugRays){
		drawDebugRay(pos, forwards);
	}

	// Sound
	m_pSoundAttack->play();

	// Muzzle flash
	m_muzzleFlash->create(m_pSceneNode, m_muzzleFlashType, m_muzzleFlashPos);

	// Cast a ray
	btVector3 from(pos.x, pos.y, pos.z);
	btVector3 to(forwards.x, forwards.y, forwards.z);
	btCollisionWorld::ClosestRayResultCallback res(from, to);

	//Base::getSingletonPtr()->m_btWorld->rayTest(from, to, res);
	// GET CAMERA RAY

	if(res.hasHit()){
		Ogre::SceneNode* node = static_cast<Ogre::SceneNode*>(res.m_collisionObject->getUserPointer());

		// Impact effect
		// ...

		if(node){
			Ogre::String name = node->getName();
			Ogre::StringUtil strUtil;

			// Test if the ray hit an NPC
			if(strUtil.startsWith(name, "NPC_", false)){
				std::vector<NPC*> NPCs = m_pNPCManager->getNPCs();

				if(NPCs.size() > 0){
					for(std::vector<NPC*>::iterator itr = NPCs.begin();
						itr != NPCs.end();
						++itr){
						Ogre::String npcName = static_cast<Ogre::String>((*itr)->getSceneNode()->getName());
						
						if(strUtil.match(name, npcName, true)){
							printf("Hit %s\n", npcName.c_str());

							// Send damage to the NPC
							(*itr)->takeDamage(m_damage);

							return true;
						}
					}
				}
			}
			else if(0){

			}
		}
	}

	return false;
}

//================================================//

bool Weapon::projectileAttack(Sparks::Camera* camera)
{
	// Sound
	m_pSoundAttack->play();

	// Muzzle flash
	m_muzzleFlash->create(m_pSceneNode, m_muzzleFlashType, m_muzzleFlashPos);

	// Add a projectile to the manager
	m_projectileData.origin = getPosition() + m_projectileOffset;
	m_projectileData.direction = camera->getDirection();

	m_projectileManager->addProjectile(m_projectileData);

	return true;
}

//================================================//

bool Weapon::meleeAttack(Sparks::Camera* camera)
{
	Ogre::Vector3 pos = camera->getSceneNode()->getPosition();
	Ogre::Vector3 forwards = pos + (camera->getDirection() * m_range);

	// Draw debug line if needed
	if(m_debugRays){
		drawDebugRay(pos, forwards);
	}

	// Cast a ray
	btVector3 from(pos.x, pos.y, pos.z);
	btVector3 to(forwards.x, forwards.y, forwards.z);
	btCollisionWorld::ClosestRayResultCallback res(from, to);

	//Base::getSingletonPtr()->m_btWorld->rayTest(from, to, res);
	// GET CAMERA RAY

	if(res.hasHit()){
		Ogre::SceneNode* node = static_cast<Ogre::SceneNode*>(res.m_collisionObject->getUserPointer());

		// Impact effect
		// ...

		if(node){
			Ogre::String name = node->getName();
			Ogre::StringUtil strUtil;

			// Test if the ray hit an NPC
			if(strUtil.startsWith(name, "NPC_", false)){
				std::vector<NPC*> NPCs = m_pNPCManager->getNPCs();

				if(NPCs.size() > 0){
					for(std::vector<NPC*>::iterator itr = NPCs.begin();
						itr != NPCs.end();
						++itr){
						Ogre::String npcName = static_cast<Ogre::String>((*itr)->getSceneNode()->getName());

						if(strUtil.match(name, npcName, true)){
							printf("Melee hit %s\n", npcName.c_str());

							// Send damage to NPC
							(*itr)->takeDamage(m_damage);

							return true;
						}
					}
				}
			}
			else if(0){

			}
		}
	}

	return false;
}

//================================================//

void Weapon::drawDebugRay(Ogre::Vector3 from, Ogre::Vector3 to)
{
	static int ctr = 0;

	m_manualObject = m_pSceneMgr->createManualObject("LineObject" + Ogre::StringConverter::toString(ctr++));
	m_lineNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("LineNode" + Ogre::StringConverter::toString(ctr));
		
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create("linemat", "General");
	material->setReceiveShadows(false);
	material->getTechnique(0)->setLightingEnabled(0);
	material->getTechnique(0)->getPass(0)->setDiffuse(0, 0, 1, 0);
	material->getTechnique(0)->getPass(0)->setAmbient(0, 0, 1);
	material->getTechnique(0)->getPass(0)->setSelfIllumination(0, 0, 1);
	material->unload();

	m_manualObject->begin("linemat", Ogre::RenderOperation::OT_LINE_LIST);
	m_manualObject->position(from);
	m_manualObject->position(to);

	m_manualObject->end();
	m_lineNode->attachObject(m_manualObject);
}

//================================================//

void Weapon::update(double timeSinceLastFrame)
{
	// Update impact manager
	m_impactManager->update(timeSinceLastFrame);

	// Update muzzle flash
	if(m_muzzleFlash->isAlive())
		m_muzzleFlash->update(timeSinceLastFrame);

	// Update main animation
	if(m_pAnimState->getEnabled()){
		m_pAnimState->addTime(timeSinceLastFrame);

		if(m_pAnimState->hasEnded()){

			// Disable and reset animation
			m_pAnimState->setEnabled(false);
			m_pAnimState->setTimePosition(0.0);

			m_state = STATE_IDLE;
		}
	}
}

//================================================//
//================================================//

WeaponNone::WeaponNone(Ogre::SceneManager* mgr) : Weapon(mgr)
{
	m_isLoaded = false;
}

//================================================//

void WeaponNone::init(Ogre::SceneNode* node)
{
	
}

//================================================//

void WeaponNone::createAnimations(void)
{

}

//================================================//

void WeaponNone::update(double timeSinceLastFrame)
{
	
}

//================================================//



//================================================//

