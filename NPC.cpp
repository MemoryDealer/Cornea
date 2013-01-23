//================================================//

#include "NPC.hpp"

//================================================//

NPC::NPC(void)
{
	m_pHitTimer = new Ogre::Timer();
}

//================================================//

void NPC::takeDamage(Ogre::Real amount)
{
	m_settings.health -= amount;

	if(m_settings.health <= 0.0){
		m_state = STATE_DEAD;
		die();
	}

	// Hit reaction
	m_pHitTimer->reset();
	m_pEntity->setMaterialName("red");
	m_hit = true;
}

//================================================//

void NPC::die(void)
{
	m_pSceneNode->detachAllObjects();
	Base::getSingletonPtr()->m_btWorld->removeCollisionObject(m_collisionObject);
}

//================================================//

void NPC::setAnimationState(Ogre::String str)
{
	m_pAnimationState = m_pEntity->getAnimationState(str);
	m_pAnimationState->setTimePosition(0.0);
	m_pAnimationState->setLoop(true);
	m_pAnimationState->setEnabled(true);
}

//================================================//

void NPC::update(double timeSinceLastFrame)
{
	// Activate/Deactivate as needed
	switch(m_settings.activationMethod){
	default:
		break;

	case ACTIVATE_BY_RANGE:
		if(m_pSceneNode->_getDerivedPosition().distance(m_pPlayerNode->getPosition()) < m_settings.activationRange){
			m_state = STATE_ACTIVE;

		}
		break;

	case ACTIVATE_BY_LOOKAT:
		{
			Rayhit* rayhit = m_pCamera->getNegativeZRayhit();
			if(rayhit->hasHit){
				if(rayhit->node == m_pSceneNode){
					if(rayhit->distance <= m_settings.activationRange){
						m_state = STATE_ACTIVE;
					}
				}
			}
		}
		break;
	}

	if(m_settings.animate){
		m_pAnimationState->addTime(timeSinceLastFrame * m_settings.animationSpeed);
	}

	// Update hit reaction
	if(m_hit){
		if(m_pHitTimer->getMillisecondsCPU() >= 150){
			m_pEntity->setMaterialName("blue");
			m_hit = false;
		}
	}
}

//================================================//
//================================================//

Monster::Monster(void) : NPC()
{
	m_friendly = false;
	m_state = STATE_IDLE;
}

//================================================//

void Monster::init(Ogre::SceneManager* mgr, Ogre::SceneNode* node, btCollisionObject* obj, Sparks::Camera* camera)
{
	// Set pointers
	m_pSceneMgr = mgr;
	m_pSceneNode = node;
	m_collisionObject = obj;

	m_pCamera = camera;
	m_pPlayerNode = m_pCamera->getSceneNode();
	m_pSceneNode->setFixedYawAxis(true);

	// Fetch user data and load
	const Ogre::Any& any = m_pSceneNode->getUserAny();
	if(!any.isEmpty()){
		NPC::NPC_DATA* data = Ogre::any_cast<NPC::NPC_DATA*>(any);

		memcpy(&m_settings, data, sizeof(NPC_DATA));
	}

	// Determine forwards vector
	switch(m_settings.forwards){
	case FORWARDS_NEGATIVE_UNIT_Z:
	default:
		m_forwards = Ogre::Vector3::NEGATIVE_UNIT_Z;
		break;

	case FORWARDS_NEGATIVE_UNIT_Y:
		m_forwards = Ogre::Vector3::NEGATIVE_UNIT_Y;
		break;

	case FORWARDS_NEGATIVE_UNIT_X:
		m_forwards = Ogre::Vector3::NEGATIVE_UNIT_X;
		break;

	case FORWARDS_UNIT_Z:
		m_forwards = Ogre::Vector3::UNIT_Z;
		break;

	case FORWARDS_UNIT_Y:
		m_forwards = Ogre::Vector3::UNIT_Y;
		break;

	case FORWARDS_UNIT_X:
		m_forwards = Ogre::Vector3::UNIT_X;
		break;
	}

	// Setup animation 
	m_pEntity = static_cast<Ogre::Entity*>(m_pSceneNode->getAttachedObject(0));
	if(m_pEntity->hasAnimationState("Idle")){
		setAnimationState("Idle");
	}

	// Create scene node
	/*m_pEntity = m_pSceneMgr->createEntity(data.name, "cylinder.mesh");

	m_pSceneNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode(data.name + "_Node");
	m_pSceneNode->attachObject(m_pEntity);
	m_pSceneNode->setPosition(data.pos);
	m_pSceneNode->scale(10.0, 10.0, 10.0);*/

	// Create the Bullet Physics rigid body
	/*btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(data.pos.x, data.pos.y, data.pos.z));

	data.shape.radius = m_pEntity->getBoundingBox().getSize().z;
	data.shape.height = m_pEntity->getBoundingBox().getSize().y;

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btCollisionShape* shape = new btCapsuleShape(data.shape.radius, data.shape.height);
	btVector3 localInertia;
	shape->calculateLocalInertia(data.mass, localInertia);

	m_btBody = new btRigidBody(data.mass, motionState, shape, localInertia);
	m_btBody->setUserPointer((void*)m_pSceneNode);

	m_btBody->setFriction(0.5);
	m_btBody->setDamping(0.4, 0.4);
	m_btBody->setAngularFactor(btVector3(0, 0, 0));
	m_btBody->setGravity(data.gravity);

	Base::getSingletonPtr()->m_btWorld->addRigidBody(m_btBody);*/
	//Base::getSingletonPtr()->m_btObjects.push_back(m_btBody);
	//Base::getSingletonPtr()->m_btObjectCount++;
}

//================================================//

void Monster::update(double timeSinceLastFrame)
{
	NPC::update(timeSinceLastFrame);

	switch(m_state){
	case STATE_IDLE:
	default:
		return;

	case STATE_ACTIVE:
		updateAI(timeSinceLastFrame);
		return;
	}
}

//================================================//

void Monster::updateAI(double timeSinceLastFrame)
{
	Ogre::Vector3 pos = m_pSceneNode->_getDerivedPosition();
	Ogre::Quaternion rot = m_pSceneNode->_getDerivedOrientation();

	// ...add ability for shooting the NPC and making it disappear
	m_pSceneNode->lookAt(m_pPlayerNode->getPosition(), Ogre::Node::TS_WORLD, m_forwards); 

	Ogre::Vector3 dir = m_pPlayerNode->getPosition() - pos;
	dir.normalise();
	dir /= 250.0;
	m_pSceneNode->translate(dir * timeSinceLastFrame);

	// Update Bullet transforms
	btTransform transform = m_collisionObject->getWorldTransform();
	transform.setOrigin(btVector3(pos.x, pos.y, pos.z));
	transform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
	m_collisionObject->setWorldTransform(transform);
}

//================================================//