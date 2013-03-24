//================================================//

#include "DynamicObject.hpp"

//================================================//

DynamicObject::DynamicObject(void)
{
	m_position = Ogre::Vector3::ZERO;
	m_state = STATE_IDLE;
	m_needsUpdate = true;
	m_updateRange = 20000.0;
	
	m_hasSound = false;
}

//================================================//

DynamicObject::~DynamicObject(void)
{
	printf("Base Destructor called on %s\n", m_pSceneNode->getName().c_str());
}

//================================================//

void DynamicObject::init(Ogre::SceneNode* node, btCollisionObject* colObj)
{
	m_pSceneNode = node;
	m_collisionObject = colObj;

	// Set position vector
	m_position = m_pSceneNode->getPosition();
}

//================================================//

void DynamicObject::init(Ogre::SceneManager* mgr, Physics* physics, Ogre::SceneNode* node, btCollisionObject* colObj)
{
	m_pSceneMgr = mgr;
	m_physics = physics;
	m_pSceneNode = node;
	m_collisionObject = colObj;

	// Set position vector
	m_position = m_pSceneNode->getPosition();

	// Init
}

//================================================//

void DynamicObject::initSound(const char* file, bool loop)
{
	FMOD_RESULT result = Base::getSingletonPtr()->m_soundSystem->createSound(file, (loop == true) ? (FMOD_3D | FMOD_LOOP_NORMAL) : (FMOD_3D), 0, &m_sound);

	FMOD_VECTOR pos;
	pos.x = m_pSceneNode->getPosition().x;
	pos.y = m_pSceneNode->getPosition().y;
	pos.z = m_pSceneNode->getPosition().z;
	FMOD_VECTOR vel;
	memset(&vel, 0, sizeof(vel));
	m_soundChannel->set3DAttributes(&pos, &vel);

	m_hasSound = true;
}

//================================================//

unsigned DynamicObject::send(unsigned arg)
{
	switch(arg){
	case ARG_ACTION:
		m_state = !m_state;
		break;

	case ARG_ACTIVATE:
		m_state = STATE_ACTIVATED;
		break;

	case ARG_DEACTIVATE:
		m_state = STATE_IDLE;
		break;
	}

	return 0;
}

//================================================//

unsigned DynamicObject::recv(void)
{

	return 0;
}

//================================================//

unsigned DynamicObject::recv(unsigned arg)
{

	return 0;
}

//================================================//

bool DynamicObject::needsUpdate(void)
{
	// check distance

	return m_needsUpdate;
}

//================================================//

DynamicObjectData* DynamicObject::getData(void) const
{
	const Ogre::Any& any = m_pSceneNode->getUserAny();
	DynamicObjectData* data = nullptr;

	if(!any.isEmpty()){
		if(any.getType() == typeid(DynamicObjectData*))
			data = Ogre::any_cast<DynamicObjectData*>(any);
	}

	return data;
}

//================================================//

void DynamicObject::deleteData(void)
{
	const Ogre::Any& any = m_pSceneNode->getUserAny();

	if(!any.isEmpty()){
		if(any.getType() == typeid(DynamicObjectData*)){
			delete Ogre::any_cast<DynamicObjectData*>(any);
			printf("%s data deleted\n", m_pSceneNode->getName().c_str());
		}
	}
}

//================================================//

void DynamicObject::setUserData(int n, void* data)
{
	m_userData.data[n] = data;
}

//================================================//

int DynamicObject::findType(Ogre::SceneNode* node)
{
	const char* name = node->getName().c_str();

	if(strstr(name, "_MovingObject_"))
		return TYPE_MOVING_OBJECT;
	if(strstr(name, "_MovingKinematicObject_"))
		return TYPE_MOVING_KINEMATIC_OBJECT;
	if(strstr(name, "_Elevator_"))
		return TYPE_ELEVATOR;
	if(strstr(name, "_RDoor_") || strstr(name, "_Door_"))
		return TYPE_ROTATING_DOOR;
	if(strstr(name, "_SDoor_"))
		return TYPE_SLIDING_DOOR;
	if(strstr(name, "_Switch_"))
		return TYPE_SWITCH;
	if(strstr(name, "_NPC_"))
		return TYPE_NPC;

	if(strstr(name, "_Light_"))
		return TYPE_LIGHT;

	return -1;
}

//================================================//

void DynamicObject::update(double timeSinceLastFrame)
{
	// Update the world transform for the Bullet collision object
	btTransform transform(btQuaternion(m_pSceneNode->_getDerivedOrientation().x, m_pSceneNode->_getDerivedOrientation().y, m_pSceneNode->_getDerivedOrientation().z,
		m_pSceneNode->_getDerivedOrientation().w),
		btVector3(m_pSceneNode->_getDerivedPosition().x, m_pSceneNode->_getDerivedPosition().y, m_pSceneNode->_getDerivedPosition().z));
	m_collisionObject->setWorldTransform(transform);

	//Base::getSingletonPtr()->m_btWorld->updateSingleAabb(m_collisionObject);  // needed?
}

//================================================//
//================================================//

/* Moving Object */

MovingObject::MovingObject(void)
	: DynamicObject()
{

}

//================================================//

MovingObject::~MovingObject(void)
{

}

//================================================//

void MovingObject::setupAnimation(void)
{
	DynamicObjectData* data = this->getData();
	if(data == nullptr)
		return;

	m_loop = data->animation.loop;

	// Create animation
	Ogre::Animation* anim = m_pSceneMgr->createAnimation(m_pSceneNode->getName() + "_Animation", data->animation.length);
	anim->setInterpolationMode((data->animation.spline == true) ? Ogre::Animation::IM_SPLINE : Ogre::Animation::IM_LINEAR);

	// Create node track
	Ogre::NodeAnimationTrack* track = anim->createNodeTrack(0, m_pSceneNode);
	Ogre::Real timePos = 0.0;
	for(std::vector<Ogre::Vector3>::iterator itr = data->animation.vectors.begin();
		itr != data->animation.vectors.end();
		++itr, timePos += data->animation.step){
			/*(*itr).x /= m_pSceneNode->_getDerivedScale().x;
			(*itr).y /= m_pSceneNode->_getDerivedScale().y;
			(*itr).z /= m_pSceneNode->_getDerivedScale().z;*/
			track->createNodeKeyFrame(timePos)->setTranslate((*itr));
	}

	m_pSceneNode->setInitialState();

	// Set up animation state for updating
	m_pAnimState = m_pSceneMgr->createAnimationState(m_pSceneNode->getName() + "_Animation");
	m_pAnimState->setEnabled(true);

	if(data->animation.active)
		m_state = STATE_ACTIVATED;
}

//================================================//

void MovingObject::update(double timeSinceLastFrame)
{
	if(m_state == STATE_ACTIVATED){
		DynamicObject::update(timeSinceLastFrame);

		m_pAnimState->addTime(timeSinceLastFrame);
	}
}

//================================================//
//================================================//

/* Moving Kinematic Object */

MovingKinematicObject::MovingKinematicObject(void)
	: MovingObject()
{

}

//================================================//

void MovingKinematicObject::setupAnimation(void)
{
	DynamicObjectData* data = this->getData();
	if(data == nullptr)
		return;

	// Remove the collision object
	m_physics->getWorld()->removeCollisionObject(m_collisionObject);

	// Add rigid body
	const float modifier = 0.98f; // Ogre's AABBs are slightly larger than Bullet's
	Ogre::AxisAlignedBox bb = m_pSceneNode->getAttachedObject(0)->getBoundingBox();
	Ogre::Vector3 bbSize;
	Ogre::Vector3 pos = m_pSceneNode->_getDerivedPosition();
	Ogre::Vector3 scale = m_pSceneNode->_getDerivedScale();
	bbSize.x = bb.getSize().x * scale.x * modifier;
	bbSize.y = bb.getSize().y * scale.y * modifier;
	bbSize.z = bb.getSize().z * scale.z * modifier;

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(pos.x, pos.y, pos.z));

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btVector3 halfExtents(bbSize.x * 0.5f, bbSize.y * 0.5f, bbSize.z * 0.5f);
	btCollisionShape* shape = new btBoxShape(halfExtents);

	btVector3 inertia;
	shape->calculateLocalInertia(0.0, inertia);
	//inertia.setValue(0.7f, 0.7f, 0.7f);
	
	m_rigidBody = new btRigidBody(0.0, motionState, shape, inertia);
	m_rigidBody->setCollisionFlags(m_rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	m_rigidBody->setActivationState(DISABLE_DEACTIVATION);
	if(data->animation.friction)
		m_rigidBody->setFriction(data->animation.friction);
	m_rigidBody->setUserPointer((void*)m_pSceneNode);

	/*m_rigidBody->setHitFraction(0.0);
	m_rigidBody->setContactProcessingThreshold(0.0);*/

	m_physics->getWorld()->addRigidBody(m_rigidBody);

	// Now set the animation track
	MovingObject::setupAnimation();
}

//================================================//

void MovingKinematicObject::update(double timeSinceLastFrame)
{
	if(m_state == STATE_ACTIVATED){
		// Must use the derived position here due to the initial state being set in setupAnimation()
		Ogre::Vector3 pos = m_pSceneNode->_getDerivedPosition();

		btTransform trans = m_rigidBody->getWorldTransform();
		trans.setOrigin(btVector3(pos.x, pos.y, pos.z));
		m_rigidBody->setWorldTransform(trans);
		btMotionState* motion = m_rigidBody->getMotionState();
		motion->getWorldTransform(trans);
		trans.setOrigin(btVector3(pos.x, pos.y, pos.z));
		motion->setWorldTransform(trans);

		m_pAnimState->addTime(timeSinceLastFrame);

		/*m_rigidBody->setInterpolationWorldTransform(m_rigidBody->getWorldTransform());
		m_rigidBody->setInterpolationLinearVelocity(btVector3(0, 0, 0));
		m_rigidBody->setInterpolationAngularVelocity(btVector3(0, 0, 0));*/
	}
}

//================================================//
//================================================//

/* Elevator */

Elevator::Elevator(void)
{

}

//================================================//

unsigned Elevator::send(unsigned arg)
{
	switch(arg){
	case ARG_OVERRIDE: // If player activated switch to the elevator
		DynamicObject::send(ARG_ACTION);
		break;

	case ARG_ACTION: // If player activated elevator itself
	default:
		break;
	}

	return 0;
}

//================================================//

void Elevator::update(double timeSinceLastFrame)
{
	MovingObject::update(timeSinceLastFrame);
}

//================================================//
//================================================//

/* Switch */

Switch::Switch(void)
	: DynamicObject()
{
	
}

//================================================//

void Switch::init(Ogre::SceneManager* mgr, Physics* physics, Ogre::SceneNode* node, btCollisionObject* colObj)
{
	DynamicObject::init(mgr, physics, node, colObj);

	//DynamicObject::initSound("C:/test.wav");

	m_linked = false;
}

//================================================//

unsigned Switch::send(unsigned arg)
{
	if(m_linked){
		m_linkedObject->send(ARG_OVERRIDE); 

		if(m_hasSound)
			Base::getSingletonPtr()->m_soundSystem->playSound(FMOD_CHANNEL_FREE, m_sound, false, &m_soundChannel);
	}

	return 0;
}

//================================================//

void Switch::deleteData(void)
{
	// no data on the heap here
	return;
}

//================================================//

void Switch::update(double timeSinceLastFrame)
{

}

//================================================//
//================================================//

Light::Light(void)
	: DynamicObject()
{

}

//================================================//

void Light::initLight(Ogre::SceneManager* mgr, Ogre::SceneNode* node)
{
	m_pSceneMgr = mgr;
	m_pSceneNode = node;

	LightData* data = nullptr;
	bool dataLoaded = false;

	const Ogre::Any& any = m_pSceneNode->getUserAny();
	if(!any.isEmpty()){
		if(any.getType() == typeid(LightData*)){
			data = Ogre::any_cast<LightData*>(any);
			dataLoaded = true;
		}
	}

	if(!dataLoaded){
		// Setup default light settings
		printf("No light data found on %s\n", m_pSceneNode->getName().c_str());
		data = new LightData();
		data->type == TYPE_SPOT;
		data->range = 600.0;
		data->inner = 5.0;
		data->outer = 21.0;
		data->shadows = false;
	}

	m_pLight = m_pSceneMgr->createLight("Dyn_" + m_pSceneNode->getName());
	m_pLight->setType((data->type == TYPE_SPOT) ? Ogre::Light::LT_SPOTLIGHT : Ogre::Light::LT_POINT);
	m_pLight->setPosition(m_pSceneNode->_getDerivedPosition());
	
	Ogre::Vector3 dir = m_pSceneNode->_getDerivedOrientation() * Ogre::Vector3::UNIT_Y;
	m_pLight->setDirection(dir);
	
	// parse these...
	m_pLight->setDiffuseColour(1.0, 1.0, 1.0);
	m_pLight->setSpecularColour(0.8, 0.8, 0.8);
	
	if(data->type == TYPE_SPOT){
		m_pLight->setAttenuation(data->range, 0.5, 0.0, 0.0);

		m_pLight->setSpotlightInnerAngle(Ogre::Radian(data->inner));
		m_pLight->setSpotlightOuterAngle(Ogre::Radian(data->outer));

		m_pLight->setSpotlightFalloff(5.0);
	}

	m_pLight->setCastShadows(data->shadows);

	if(data->hideNode)
		m_pSceneNode->setVisible(false);
}

//================================================//

void Light::deleteData(void)
{
	const Ogre::Any& any = m_pSceneNode->getUserAny();
	if(!any.isEmpty()){
		if(any.getType() == typeid(LightData*))
			delete Ogre::any_cast<LightData*>(any);
	}
}

//================================================//

void Light::update(double timeSinceLastFrame)
{
	// useful for flickering etc.
}

//================================================//