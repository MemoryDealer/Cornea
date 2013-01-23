//================================================//

#include "DynamicObjectManager.hpp"

//================================================//

DynamicObjectManager::DynamicObjectManager(Ogre::SceneManager* mgr, Sparks::Camera* camera)
{
	m_pSceneMgr = mgr;
	m_pCamera = camera;

	// Register object hierarchy
	firstTierObjects.push_back("MovingObject_");
	firstTierObjects.push_back("MovingKinematicObject_");
	firstTierObjects.push_back("Elevator_");

	secondTierObjects.push_back("Switch_");

	thirdTierObjects.push_back("_Trigger");
}

//================================================//

DynamicObjectManager::~DynamicObjectManager(void)
{

}

//================================================//

bool DynamicObjectManager::addObject(Ogre::SceneNode* node, btCollisionObject* obj, int tier)
{
	Ogre::String name = node->getName();

	// Determine the type of dynamic object
	Ogre::StringUtil strUtil;

	switch(tier){
	case 1:
		if(strUtil.startsWith(name, "MovingObject_", false)){
			m_objects.push_back(new MovingObject());
			m_objects.back()->init(m_pSceneMgr, node, obj);

			// Fetch animation data
			const Ogre::Any& any = node->getUserAny();
			if(!any.isEmpty()){
				DynamicObject::DYNAMIC_OBJECT_DATA* data = Ogre::any_cast<DynamicObject::DYNAMIC_OBJECT_DATA*>(any);
				m_objects.back()->setupAnimation(data);
				//delete data; // you no longer need those <-- (Palpatine quote)

				return true;
			}
		}
		else if(strUtil.startsWith(name, "MovingKinematicObject_", false)){
			m_objects.push_back(new MovingKinematicObject());
			m_objects.back()->init(m_pSceneMgr, node, obj);

			// Fetch animation data
			const Ogre::Any& any = node->getUserAny();
			if(!any.isEmpty()){
				DynamicObject::DYNAMIC_OBJECT_DATA* data = Ogre::any_cast<DynamicObject::DYNAMIC_OBJECT_DATA*>(any);
				m_objects.back()->setupAnimation(data);
				//delete data;

				return true;
			}
		}
		else if(strUtil.startsWith(name, "Elevator_", false)){
			m_objects.push_back(new Elevator());
			m_objects.back()->init(m_pSceneMgr, node, obj);
			m_objects.back()->setState(DynamicObject::STATE_IDLE);

			// Fetch animation data
			const Ogre::Any& any = node->getUserAny();
			if(!any.isEmpty()){
				DynamicObject::DYNAMIC_OBJECT_DATA* data = Ogre::any_cast<DynamicObject::DYNAMIC_OBJECT_DATA*>(any);
				m_objects.back()->setupAnimation(data);
				//delete data;

				return true;
			}
		}
		break;

	case 2:
		if(strUtil.startsWith(name, "Switch_", false)){
			m_objects.push_back(new Switch());
			m_objects.back()->init(m_pSceneMgr, node, obj);

			// Find the dynamic object it should be linked to
			const Ogre::Any& any = node->getUserAny();
			if(!any.isEmpty()){
				Ogre::String name = Ogre::any_cast<Ogre::String>(any); // get the string

				// Acquire the link pointer
				DynamicObject* link = this->getObject(name);
				if(link != nullptr){
					m_objects.back()->setLinkedObject(link);
					link->attachSwitch(static_cast<Switch*>(m_objects.back()));
				}
			}
		}
		else if(strUtil.startsWith(name, "NPC_", false)){
			m_pNPCManager->addNPC(node, obj);
			return true;
		}
		break;

	// Triggers
	case 3:
		{
			// A trigger string should look like "ObjectName.Trigger.Condition"

			Ogre::StringVector tokens = strUtil.split(name, ".");

			if(tokens.size() > 1){

				if(strUtil.match(tokens[1], "Trigger", true)){

					if(tokens.size() > 2){

						// WalkOver trigger
						if(strUtil.match(tokens[2], "WalkOver", true)){
							printf("Adding trigger %s\n", node->getName().c_str());
							m_objects.push_back(new TriggerWalkOver());
							m_objects.back()->initTrigger(m_pSceneMgr, node, m_pCamera);

							// Set the trigger data
							const Ogre::Any& any = node->getUserAny();
							DynamicObject::DYNAMIC_OBJECT_DATA* data = Ogre::any_cast<DynamicObject::DYNAMIC_OBJECT_DATA*>(any);

							m_objects.back()->setTriggerData(data);
							this->registerTriggerAction(data);
						}
						// LookAt trigger
						else if(strUtil.match(tokens[2], "LookAt", true)){
							m_objects.push_back(new TriggerLookAt());
							m_objects.back()->initTrigger(m_pSceneMgr, node, m_pCamera);

							const Ogre::Any& any = node->getUserAny();
							DynamicObject::DYNAMIC_OBJECT_DATA* data = Ogre::any_cast<DynamicObject::DYNAMIC_OBJECT_DATA*>(any);

							m_objects.back()->setTriggerData(data);
							this->registerTriggerAction(data);
						}

					}

				}
			}
		}
		break;
		
	default:
		break;
	}

	return false;
}

//================================================//

void DynamicObjectManager::registerAllObjectsInScene(void)
{
	int tier = 1;
	const int MAX_TIER = 3;

	// Iterate through collision world objects and add them to the manager based on tier
	for(;tier<=MAX_TIER; ++tier){
		btCollisionObjectArray& objects = Base::getSingletonPtr()->m_btWorld->getCollisionObjectArray();
		int size = objects.size();
		for(int i=(size - 1); i>=0; --i){

			if(addObject(static_cast<Ogre::SceneNode*>(objects.at(i)->getUserPointer()), objects.at(i), tier)){
				printf("Added object: %s\n", static_cast<Ogre::SceneNode*>(objects.at(i)->getUserPointer())->getName().c_str());
			}
			else{
				printf("Skipped object: %s\n", static_cast<Ogre::SceneNode*>(objects.at(i)->getUserPointer())->getName().c_str());
			}

			////! Redundant!
			//Ogre::Entity* entity = static_cast<Ogre::Entity*>(objects.at(i)->getUserPointer());
			//Ogre::StringUtil strUtil;
			//
			//if(!strUtil.startsWith(static_cast<Ogre::SceneNode*>(objects.at(i)->getUserPointer())->getName(), "MofoPlane", false)){
			//	entity->setMaterialName("blue");
			//	entity->setCastShadows(true);
			//}
		}
	}

	// Then remove all extra garbage from the BtOgre function
}

//================================================//

void DynamicObjectManager::registerTriggerAction(DynamicObject::DYNAMIC_OBJECT_DATA* data)
{
	switch(data->trigger.actionCode){
	default:
		break;

	case TRIGGER_ACTION_CODE::ACTION_NPC_ENABLE:
		m_objects.back()->setLinkedObject(m_pNPCManager->getNPC(data->trigger.buffer));
		break;

	case TRIGGER_ACTION_CODE::ACTION_DYNAMIC_OBJECT_ACTIVATE:
	case TRIGGER_ACTION_CODE::ACTION_DYNAMIC_OBJECT_DEACTIVATE:
		m_objects.back()->setLinkedObject(this->getObject(data->trigger.buffer));
		break;
	}
}

//================================================//

DynamicObject* DynamicObjectManager::getObject(Ogre::String& name)
{
	Ogre::StringUtil strUtil;

	for(std::vector<DynamicObject*>::iterator itr = m_objects.begin();
		itr != m_objects.end();
		++itr){
		Ogre::String str = static_cast<Ogre::String>((*itr)->getSceneNode()->getName());
		
		if(strUtil.match(name, str, true)){
			return *itr;
		}
	}

	return nullptr;
}

//================================================//

void DynamicObjectManager::update(double timeSinceLastFrame)
{
	for(std::vector<DynamicObject*>::iterator itr = m_objects.begin();
		itr != m_objects.end();){
		if((*itr)->needsUpdate()){
			(*itr)->update(timeSinceLastFrame);
		}
		++itr;
	}
}

//================================================//