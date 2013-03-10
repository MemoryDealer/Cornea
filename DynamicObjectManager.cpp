//================================================//

#include "DynamicObjectManager.hpp"

//================================================//

DynamicObjectManager::DynamicObjectManager(Ogre::SceneManager* mgr, Sparks::Camera* camera)
{
	m_pSceneMgr = mgr;
	m_pCamera = camera;

	m_physics = m_pCamera->getPhysics();

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

int DynamicObjectManager::findType(Ogre::SceneNode* node)
{
	const char* name = node->getName().c_str();

	if(strstr(name, "_MovingObject_"))
		return DynamicObject::TYPE_MOVING_OBJECT;
	if(strstr(name, "_MovingKinematicObject_"))
		return DynamicObject::TYPE_MOVING_KINEMATIC_OBJECT;
	if(strstr(name, "_Elevator_"))
		return DynamicObject::TYPE_ELEVATOR;
	if(strstr(name, "_RDoor_") || strstr(name, "_Door_"))
		return DynamicObject::TYPE_ROTATING_DOOR;
	if(strstr(name, "_SDoor_"))
		return DynamicObject::TYPE_SLIDING_DOOR;
	if(strstr(name, "_Switch_"))
		return DynamicObject::TYPE_SWITCH;
	if(strstr(name, "_NPC_"))
		return DynamicObject::TYPE_NPC;

	return -1;
}

//================================================//

bool DynamicObjectManager::addObject(Ogre::SceneNode* node, btCollisionObject* obj, int tier)
{
	const Ogre::Any& any = node->getUserAny();
	DynamicObject::DYNAMIC_OBJECT_DATA* data = nullptr;

	switch(tier){
	case 1:
		switch(this->findType(node)){
		default:
			return false;

		case DynamicObject::TYPE_MOVING_OBJECT:
			m_objects.push_back(new MovingObject());
			m_objects.back()->init(m_pSceneMgr, m_physics, node, obj);

			// Fetch animation data
			if(!any.isEmpty()){
				//!! This can be simplified by retrieving the any data inside setupAnimation()
				data = Ogre::any_cast<DynamicObject::DYNAMIC_OBJECT_DATA*>(any);
				m_objects.back()->setupAnimation(data);
				//delete data; // you no longer need those <-- (Palpatine quote)
			}
			return true;
			
		case DynamicObject::TYPE_MOVING_KINEMATIC_OBJECT:
			m_objects.push_back(new MovingKinematicObject());
			m_objects.back()->init(m_pSceneMgr, m_physics, node, obj);

			if(!any.isEmpty()){
				data = Ogre::any_cast<DynamicObject::DYNAMIC_OBJECT_DATA*>(any);
				m_objects.back()->setupAnimation(data);
				//delete data;
			}
			return true;
			
		case DynamicObject::TYPE_ELEVATOR:
			m_objects.push_back(new Elevator());
			m_objects.back()->init(m_pSceneMgr, m_physics, node, obj);
			m_objects.back()->setState(DynamicObject::STATE_IDLE);

			if(!any.isEmpty()){
				data = Ogre::any_cast<DynamicObject::DYNAMIC_OBJECT_DATA*>(any);
				m_objects.back()->setupAnimation(data);
				//delete data;
			}
			return true;

		case DynamicObject::TYPE_ROTATING_DOOR:
			m_objects.push_back(new RotatingDoor());
			m_objects.back()->init(m_pSceneMgr, m_physics, node, obj);
			m_objects.back()->setState(DynamicObject::STATE_IDLE);

			if(!any.isEmpty()){
				// ...
			}
			return true;

		case DynamicObject::TYPE_SLIDING_DOOR:
			m_objects.push_back(new SlidingDoor());
			m_objects.back()->init(m_pSceneMgr, m_physics, node, obj);
			m_objects.back()->setState(DynamicObject::STATE_IDLE);

			if(!any.isEmpty()){
				// ...
			}
			return true;
		}
		break;

	case 2:
		switch(this->findType(node)){
		default:
			return false;

		case DynamicObject::TYPE_SWITCH:
			m_objects.push_back(new Switch());
			m_objects.back()->init(m_pSceneMgr, m_physics, node, obj);

			// Find the dynamic object it should be linked to
			if(!any.isEmpty()){
				Ogre::String name = Ogre::any_cast<Ogre::String>(any); // get the string

				// Acquire the link pointer
				DynamicObject* link = this->getObject(name);
				if(link != nullptr){
					m_objects.back()->setLinkedObject(link);
					link->attachSwitch(static_cast<Switch*>(m_objects.back()));
				}
			}
			return true;
		
		case DynamicObject::TYPE_NPC:
			m_pNPCManager->addNPC(node, obj);
			return true;
		}
		break;

	// Triggers
	case 3:
		{
			// A trigger string should look like "ObjectName.Trigger.Condition"
			Ogre::StringUtil strUtil;
			Ogre::StringVector tokens = strUtil.split(node->getName(), ".");

			if(tokens.size() > 1){

				if(strUtil.match(tokens[1], "Trigger", true)){

					if(tokens.size() > 2){

						// WalkOver trigger
						if(strUtil.match(tokens[2], "WalkOver", true)){
							printf("Adding trigger %s\n", node->getName().c_str());
							m_objects.push_back(new TriggerWalkOver());
							m_objects.back()->initTrigger(m_pSceneMgr, node, m_pCamera);

							// Set the trigger data
							if(!any.isEmpty()){
								data = Ogre::any_cast<DynamicObject::DYNAMIC_OBJECT_DATA*>(any);

								m_objects.back()->setTriggerData(data);
								this->registerTriggerAction(data);
							}
						}
						// LookAt trigger
						else if(strUtil.match(tokens[2], "LookAt", true)){
							m_objects.push_back(new TriggerLookAt());
							m_objects.back()->initTrigger(m_pSceneMgr, node, m_pCamera);

							if(!any.isEmpty()){
								data = Ogre::any_cast<DynamicObject::DYNAMIC_OBJECT_DATA*>(any);

								m_objects.back()->setTriggerData(data);
								this->registerTriggerAction(data);
							}
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
		btCollisionObjectArray& objects = m_physics->getWorld()->getCollisionObjectArray();
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