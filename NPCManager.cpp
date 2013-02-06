//================================================//

#include "NPCManager.hpp"

//================================================//

NPCManager::NPCManager(Ogre::SceneManager* mgr, Sparks::Camera* camera)
{
	m_pSceneMgr = mgr;
	m_pCamera = camera;
	m_physics = camera->getPhysics();
}

//================================================//

NPCManager::~NPCManager(void)
{

}

//================================================//

void NPCManager::addNPC(Ogre::SceneNode* node, btCollisionObject* obj)
{
	Ogre::String name = node->getName();
	Ogre::StringUtil strUtil;

	if(strUtil.startsWith(name, "NPC_Monster_", false)){
		m_NPCs.push_back(new Monster());
		m_NPCs.back()->init(m_pSceneMgr, node, obj, m_pCamera);

	}
	else{

	}
}

//================================================//

NPC* NPCManager::getNPC(Ogre::String name)
{
	Ogre::StringUtil strUtil;

	for(std::vector<NPC*>::iterator itr = m_NPCs.begin();
		itr != m_NPCs.end();
		++itr){
		if(strUtil.match(name, (*itr)->getSceneNode()->getName())){
			return *itr;
		}
	}

	return nullptr;
}

//================================================//

void NPCManager::update(double timeSinceLastFrame)
{
	for(std::vector<NPC*>::iterator itr = m_NPCs.begin();
		itr != m_NPCs.end();){
			if(!(*itr)->isAlive()){
				m_physics->getWorld()->removeCollisionObject((*itr)->getCollisionObject());
				delete *itr;
				itr = m_NPCs.erase(itr); // remove dead NPC and move to next one
			}
			else{
				(*itr)->update(timeSinceLastFrame);

				++itr;
			}
	}
}

//================================================//