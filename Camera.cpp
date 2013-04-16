//================================================//

#include "Camera.hpp"

//================================================//

namespace Sparks
{

//================================================//

Camera::Camera(Ogre::SceneManager* mgr, Ogre::Real farClipDistance)
{
	Ogre::ConfigFile file;
	file.loadDirect("C:/cam.cfg");

	/* Current optimal settings:
	moveSpeed=60650.0
	maxVelocity=9.0
	mass=340.0
	grav=89.92
	*/

	m_mode = MODE_FIRST_PERSON;
	m_moveSpeed = Ogre::StringConverter::parseReal(file.getSetting("moveSpeed", "cam"));
	m_maxVelocity = Ogre::StringConverter::parseReal(file.getSetting("maxVelocity", "cam"));
	m_sprintVelocity = Ogre::StringConverter::parseReal(file.getSetting("sprintVelocity", "cam"));
	m_mass = Ogre::StringConverter::parseReal(file.getSetting("mass", "cam"));
	m_specMoveSpeed = 70.0;
	m_maxSpecVelocity = 10.0;
	m_rotateSpeed = 0.06;
	m_translateVector = Ogre::Vector3::ZERO;
	m_velocityVector = Ogre::Vector3::ZERO;
	m_acceleration = 0.3;
	m_jumping = false;
	m_jmpConstant = 500.0;
	m_cameraHeight = 22.0;
	m_capsuleRadius = 0.9;
	m_maxYOffset = 6.5;
	m_defGravity.setValue(0.0, -(Ogre::StringConverter::parseReal(file.getSetting("grav", "cam"))), 0.0);

	m_moveForwardsPressed = false;
	m_moveBackwardsPressed = false;
	m_moveLeftPressed = false;
	m_moveRightPressed = false;
	m_moveUpPressed = false;
	m_moveDownPressed = false;
	m_shiftPressed = false;
	m_spacePressed = false;

	// set the scene manager to point to the current one
	m_pSceneMgr = mgr;

	m_farClipDistance = farClipDistance;

	m_negativeYRayhit = nullptr;
	m_negativeZRayhit = nullptr;

	//init();
	// nodes
	m_pCameraNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode(NODE_CAMERA);
	m_pCameraYawNode = m_pCameraNode->createChildSceneNode(NODE_CAMERA_YAW);
	m_pCameraPitchNode = m_pCameraYawNode->createChildSceneNode(NODE_CAMERA_PITCH);
	m_pCameraRollNode = m_pCameraPitchNode->createChildSceneNode(NODE_CAMERA_ROLL);

	m_physicsData = new Physics::CAMERA_DATA();

	m_physicsData->heightOffset = &m_capsuleHeightOffset;
	m_physicsData->maxVelocity = &m_maxVelocity;
	m_physicsData->sprintVelocity = &m_sprintVelocity;
	m_physicsData->shift = &m_shiftPressed;
}

//================================================//

Camera::~Camera(void)
{
	if(m_mode == MODE_FIRST_PERSON)
		this->removeRigidBody();

	// Remove scene nodes and camera object
	m_pSceneMgr->destroySceneNode(m_pCameraNode);
	m_pSceneMgr->destroySceneNode(m_pCameraPitchNode);
	m_pSceneMgr->destroySceneNode(m_pCameraYawNode);
	m_pSceneMgr->destroySceneNode(m_pCameraRollNode);
	m_pSceneMgr->destroyCamera(m_pCamera);

	delete m_negativeYRayhit;
	delete m_negativeZRayhit;

	delete m_physicsData;
}

//================================================//

void Camera::init(Physics* physics)
{
	m_physics = physics;

	m_pSceneMgr->destroyCamera("PlayerCam");
	m_pCamera = m_pSceneMgr->createCamera("PlayerCam");
	m_pCamera->setAutoAspectRatio(true);

	m_pCamera->setNearClipDistance(0.1);
	m_pCamera->setFarClipDistance(m_farClipDistance);

	m_pCameraRollNode->attachObject(m_pCamera);
	//m_pCameraNode->attachObject(m_pCamera);

	// set the initial position
	m_pCameraNode->setPosition(0, 200, 0);

	this->createRigidBody();

	// Setup rayhits now to avoid nullptrs
	m_negativeYRayhit = new Rayhit();
	m_negativeYRayhit->node = m_pCameraNode;
	m_negativeYRayhit->distance = 0.0;

	m_negativeZRayhit = new Rayhit();
	m_negativeZRayhit->node = m_pCameraNode;
	m_negativeZRayhit->distance = 0.0;
}

//================================================//

void Camera::createRigidBody(void)
{
	// Add the camera to the physics world
	Ogre::Vector3 camPos = m_pCameraNode->getPosition();
	btScalar mass = m_mass;
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(camPos.x, camPos.y, camPos.z));

	// set up rigid body
	const Ogre::Real capsuleHeight = 5.0;
	m_capsuleHeightOffset = capsuleHeight / 1.7;
	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btCollisionShape* shape = new btCapsuleShape(m_capsuleRadius, capsuleHeight); // need changes
	btVector3 localInertia;
	shape->calculateLocalInertia(mass, localInertia);

	m_btCamera = new btRigidBody(mass, motionState, shape, localInertia);
	m_btCamera->setUserPointer((void*)m_pCameraNode);

	// optimize the body
	m_btCamera->setFriction(1.0);
	m_btCamera->setDamping(0.9, 0.9);
	m_btCamera->setAngularFactor(btVector3(0, 0, 0));

	// this will prevent the camera from falling through elevators when they "take off"
	m_btCamera->setActivationState(DISABLE_DEACTIVATION);

	m_btCamera->setHitFraction(0.0);
	m_btCamera->setContactProcessingThreshold(0.0);

	//m_btCamera->setGravity(m_defGravity);
	m_physics->getWorld()->setGravity(m_defGravity);

	// add the rigid body to the dynamics world
	//m_physics->setCameraBody(m_btCamera);
	m_physicsData->body = m_btCamera;
	m_physics->setCameraData(m_physicsData);
}

//================================================//

void Camera::createAltRigidBody(void)
{
	// Add the camera to the physics world
	Ogre::Vector3 camPos = m_pCameraNode->getPosition();
	btScalar mass = 12.0;
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(camPos.x, camPos.y, camPos.z));

	// set up rigid body
	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btCollisionShape* shape = new btBoxShape(btVector3(3.0, 5.0, 3.0)); // need changes
	btVector3 localInertia;
	shape->calculateLocalInertia(mass, localInertia);

	m_btCamera = new btRigidBody(mass, motionState, shape, localInertia);
	m_btCamera->setUserPointer((void*)m_pCameraNode);

	// optimize the body
	m_btCamera->setFriction(1.0);
	m_btCamera->setDamping(0.8, 0.8);
	m_btCamera->setAngularFactor(btVector3(0, 0, 0));

	m_btCamera->setHitFraction(0.0);
	m_btCamera->setContactProcessingThreshold(0.0);

	m_btCamera->setGravity(m_defGravity);

	// add the rigid body to the dynamics world
	//m_physics->setCameraBody(m_btCamera);
}

//================================================//

void Camera::removeRigidBody(void)
{
	// Remove Bullet objects
	m_physics->removeCameraBody();
 	delete m_btCamera->getMotionState();
	delete m_btCamera;
}

//================================================//

void Camera::update(double timeSinceLastFrame)
{
	switch(m_mode){
	case MODE_FIRST_PERSON:
		this->moveFirstPerson(timeSinceLastFrame);
		return;

	case MODE_SPECTATOR:
	default:
		this->moveSpectator(timeSinceLastFrame);
		return;
	}
}

//================================================//

void Camera::setPosition(Ogre::Vector3 pos)
{
	btTransform transform = m_btCamera->getWorldTransform();
	transform.setOrigin(btVector3(pos.x, pos.y, pos.z));
	m_btCamera->setWorldTransform(transform);
}

//================================================//

void Camera::setOrientation(Ogre::Quaternion q)
{
	btTransform transform = m_btCamera->getWorldTransform();
	transform.setRotation(btQuaternion(q.x, q.y, q.z, q.w));
	m_btCamera->setWorldTransform(transform);
}

//================================================//

void Camera::moveFirstPerson(double timeSinceLastFrame)
{
	// Limit the pitch
	Ogre::Real pitchAngle, pitchAngleSign;
	pitchAngle = (2 * Ogre::Degree(Ogre::Math::ACos(m_pCameraPitchNode->getOrientation().w)).valueDegrees());
	pitchAngleSign = m_pCameraPitchNode->getOrientation().x;

	if(pitchAngle > 80.0f){
		if(pitchAngleSign > 0){
			m_pCameraPitchNode->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f), Ogre::Math::Sqrt(0.5f) - 0.115f, 0, 0));
		}
		else{
			m_pCameraPitchNode->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f), -Ogre::Math::Sqrt(0.5f) + 0.115f, 0, 0));
		}
	}

	this->updateRays();

	// See if player is colliding against a wall in mid-air
	//  if so, stop the movement and let the player fall
	// ...

	m_translateVector = Ogre::Vector3::ZERO;

	// Apply movements based on keys pressed
	if(m_moveForwardsPressed){
		m_translateVector.z = -m_moveSpeed;
	}
	else if(!m_moveBackwardsPressed){
		m_translateVector.z = 0.0;
	}
	if(m_moveBackwardsPressed){
		m_translateVector.z = m_moveSpeed;
	}
	else if(!m_moveForwardsPressed){
		m_translateVector.z = 0.0;
	}
	if(m_moveLeftPressed){
		//m_translateVector.x = -(m_moveSpeed * 0.90); // limit sideways movement to 90%
		m_translateVector.x = -m_moveSpeed;
	}
	else if(!m_moveRightPressed){
		m_translateVector.x = 0.0;
	}
	if(m_moveRightPressed){
		//m_translateVector.x = (m_moveSpeed * 0.90);
		m_translateVector.x = m_moveSpeed;
	}
	else if(!m_moveLeftPressed){
		m_translateVector.x = 0.0;
	}

	m_translateVector = (m_pCameraYawNode->getOrientation() * m_pCameraPitchNode->getOrientation() * m_translateVector);

	// Calculate the forwards vector and use it to keep the camera going the same speed, despite the pitch
	// Note: this may allow wall climbing where not conceivable
	Ogre::Vector3 forwards = m_pCamera->getDerivedUp();
	forwards.crossProduct(m_pCamera->getDerivedRight());
	forwards.normalise();

	m_translateVector.x /= forwards.y;
	m_translateVector.z /= forwards.y;
	m_translateVector.y = 0.0;

	// Stop if the user is not moving
	if(m_translateVector == Ogre::Vector3::ZERO){
		return;
	}

	// Update the jump physics
	this->updateJump(timeSinceLastFrame);

	// Update the camera by applying an impulse to the Bullet rigid body
	btVector3 btTranslate(m_translateVector.x, m_translateVector.y, m_translateVector.z);
	
	m_btCamera->activate();
	m_btCamera->applyCentralForce(btTranslate);
}

//================================================//

void Camera::updateJump(double timeSinceLastFrame)
{
	// Process falling
	//printf("Y: %.2f\n", m_negativeYRayhit->distance);
	if(m_negativeYRayhit->distance > m_maxYOffset){
		if(m_btCamera->getLinearVelocity().length() > (m_maxVelocity / 2.0))
			m_translateVector *= 0.001;

		return;
		// Test to see if the player is actually falling, otherwise the player will be stuck if moving along the edge of a ramp, 
		// due to the sides of the capsule still hanging on the ledge. 
		//if(m_btCamera->getLinearVelocity().getY() < -1.0){
		//	//m_translateVector *= 0.9;
		//	m_translateVector *= 0.0;
		//	m_translateVector.y = -(m_moveSpeed * 6.0);
		//}
		//else if(m_btCamera->getLinearVelocity().length() > m_maxVelocity){
		//	printf("***Blocking %.2f\n", m_btCamera->getLinearVelocity().length());
		//	m_translateVector *= 0.0;
		//	m_translateVector.y = -(m_moveSpeed * 6.0); // make 6.0 const
		//}

		// Expanding raycasting outside of capsule to avoid hitting self
		const Ogre::Real offset = m_capsuleRadius + 0.4; 

		// Cast four rays down around the radius of the capsule
		Rayhit ray;
		const Ogre::Real down = -10000.0;
		Ogre::Vector3 from = m_pCameraNode->getPosition();
		Ogre::Vector3 to = m_pCameraNode->getPosition();
		to.y = down;

		// Right edge
		from.x += offset;
		to.x += offset;
		this->getRayhit(from, to, &ray);
		if(ray.hasHit){
			printf("%.2f\tRIGHT\n", ray.distance);
			if(ray.distance < m_maxYOffset)
				return;
		}

		// Top edge
		from = m_pCameraNode->getPosition();
		to = m_pCameraNode->getPosition();
		from.z -= offset;
		to.z -= offset;
		to.y = down;
		this->getRayhit(from, to, &ray);
		if(ray.hasHit){
			printf("%.2f\tTOP\n", ray.distance);
			if(ray.distance < m_maxYOffset){
				
				return;
			}
		}

		// Left edge
		from = m_pCameraNode->getPosition();
		to = m_pCameraNode->getPosition();
		from.x -= offset;
		to.x -= offset;
		to.y = down;
		this->getRayhit(from, to, &ray);
		if(ray.hasHit){
			printf("%.2f\tLEFT\n", ray.distance);
			if(ray.distance < m_maxYOffset){
				
				return;
			}
		}

		// Lower edge
		from = m_pCameraNode->getPosition();
		to = m_pCameraNode->getPosition();
		from.z += offset;
		to.z += offset;
		to.y = down;
		this->getRayhit(from, to, &ray);
		if(ray.hasHit){
			printf("%.2f\tLOWER\n", ray.distance);
			if(ray.distance < m_maxYOffset){
				
				return;
			}
		}

		printf("STOPPING!\n");
		
		//m_translateVector.y = -(m_moveSpeed * 6.0);
		return;
	}

	return;

	static bool allowAir = false;
	static Ogre::Real jmpHeight = 0.0;

	if(m_spacePressed){
		if(!m_jumping){
			// Prevent jumping if the player is falling
			if(m_negativeYRayhit->distance > 1.0){ // 1.0 allows for leniency when climbing inclines, rather then only 90 degrees on a flat plane
				return; 
			}

			btVector3 jmp(m_translateVector.x * Boots::getSingletonPtr()->getXMultiplier(), 
				Boots::getSingletonPtr()->getYMultiplier(), 
				m_translateVector.z * Boots::getSingletonPtr()->getZMultiplier());

			// set whether or not to allow movement in the air after jumping
			if((Ogre::Math::Abs(jmp.getX()) > 0.0) || (Ogre::Math::Abs(jmp.getZ()) > 0.0))
				allowAir = true;
			else
				allowAir = false;

			// Save the height at which the player jumped
			jmpHeight = m_pCameraNode->getPosition().y - m_capsuleHeightOffset;

			// modify this variable if needed
			switch(Boots::getSingletonPtr()->getEquippedType()){
			case Boots::BOOTS_TYPE_NORMAL:
			default:
				//jmpHeight /= 1.5;
				break;
			}

			m_btCamera->setGravity(Boots::getSingletonPtr()->getGravity());
			
			m_btCamera->activate();
			m_btCamera->applyCentralImpulse(jmp);
			m_jumping = true;
			return;
		}
	}
	
	// Test if the player is on the ground, if so set jumping to false
	if((m_negativeYRayhit->distance <= 1.0) && (m_jumping)){
		m_btCamera->setGravity(m_defGravity);
		m_jumping = false;

		// TODO: Test here the difference between the camPos.y and original jump height for death upon impact of the ground

		return;
	}

	// Apply jumping forces if still in the air--basically allows the player to move while in the air (most games do this to some extent)
	if(m_jumping){
		// stop movement in the air if the player just jumped straight up by multiplying by allowAir
		btVector3 jmp(m_translateVector.x * Boots::getSingletonPtr()->getXAirMultiplier() * Ogre::Real(allowAir), 
			0.0, 
			m_translateVector.z * Boots::getSingletonPtr()->getZAirMultiplier() * Ogre::Real(allowAir)); 

		//! OPTIMIZE! slow down movement after player has fallen below original jump height
		if((m_pCameraNode->getPosition().y - m_capsuleHeightOffset) <= jmpHeight){
			jmp /= 1.5;
			
			// decrement the gravity until it's normal
			btVector3 gravity = m_btCamera->getGravity();
			if(gravity.getY() > m_defGravity.getY()){
				gravity.setY(gravity.getY() - 0.01);
				m_btCamera->setGravity(gravity);
			}
		}

		m_btCamera->activate();
		m_btCamera->applyCentralImpulse(jmp * timeSinceLastFrame / 10.0); // it perplexes me on why this must be frame-rate independent when the engine's time step already is
	}
	// Test if the player is falling and not jumping, to allow for some movement
	else if(m_negativeYRayhit->distance > 0.1){
		btVector3 move(m_translateVector.x * 0.01, -0.1, m_translateVector.z * 0.01);

		if(m_btCamera->getGravity() != m_defGravity)
				m_btCamera->setGravity(m_defGravity);

		m_btCamera->activate();
		m_btCamera->applyCentralImpulse(move / 10.0); // ^ Same as above call
	}
}

//================================================//

void Camera::moveSpectator(double timeSinceLastFrame)
{
	m_translateVector = Ogre::Vector3::ZERO;

	// Apply movements based on keys pressed

	// forwards
	if(m_moveForwardsPressed){
		if(m_velocityVector.z < m_maxSpecVelocity){
			m_velocityVector.z += m_acceleration;
		}
		else if(m_velocityVector.z > m_maxSpecVelocity){
			m_velocityVector.z -= m_acceleration;
		}

		m_translateVector.z = -(m_specMoveSpeed * m_velocityVector.z);
	}
	else if(!m_moveBackwardsPressed && m_translateVector.z < 0){
		if(m_velocityVector.z > 0){
			m_velocityVector.z -= (m_acceleration * 2);
			if(m_velocityVector.z < 0){
				m_velocityVector.z = 0;
			}
		}

		m_translateVector.z = -(m_specMoveSpeed * m_velocityVector.z);
	}

	// backwards
	if(m_moveBackwardsPressed){
		if(m_velocityVector.z < m_maxSpecVelocity){
			m_velocityVector.z += m_acceleration;
		}
		else if(m_velocityVector.z > m_maxSpecVelocity){
			m_velocityVector.z -= m_acceleration;
		}

		m_translateVector.z = (m_specMoveSpeed * m_velocityVector.z);
	}
	else if(!m_moveForwardsPressed && m_translateVector.z > 0){
		if(m_velocityVector.z > 0){
			m_velocityVector.z -= (m_acceleration * 2);
			if(m_velocityVector.z < 0){
				m_velocityVector.z = 0;
			}
		}

		m_translateVector.z = (m_specMoveSpeed * m_velocityVector.z);
	}

	// left
	if(m_moveLeftPressed){
		if(m_velocityVector.x < m_maxSpecVelocity){
			m_velocityVector.x += m_acceleration;
		}
		else if(m_velocityVector.x > m_maxSpecVelocity){
			m_velocityVector.x -= m_acceleration;
		}

		m_translateVector.x = -(m_specMoveSpeed * m_velocityVector.x / 2); // make left/right movement slower for realism
	}
	else if(!m_moveRightPressed && m_translateVector.x < 0){
		if(m_velocityVector.x > 0){
			m_velocityVector.x -= (m_acceleration * 2);
			if(m_velocityVector.x < 0){
				m_velocityVector.x = 0;
			}
		}

		m_translateVector.x = -(m_specMoveSpeed * m_velocityVector.x / 2);
	}

	// right
	if(m_moveRightPressed){
		if(m_velocityVector.x < m_maxSpecVelocity){
			m_velocityVector.x += m_acceleration;
		}
		else if(m_velocityVector.x > m_maxSpecVelocity){
			m_velocityVector.x -= m_acceleration;
		}

		m_translateVector.x = (m_specMoveSpeed * m_velocityVector.x / 2);
	}
	else if(!m_moveLeftPressed && m_translateVector.x > 0){
		if(m_velocityVector.x > 0){
			m_velocityVector.x -= (m_acceleration * 2);
			if(m_velocityVector.x < 0){
				m_velocityVector.x = 0;
			}
		}

		m_translateVector.x = (m_specMoveSpeed * m_velocityVector.x / 2);
	}

	// translate the camera node

	m_translateVector = (m_pCameraYawNode->getOrientation() * m_pCameraPitchNode->getOrientation() * m_translateVector * timeSinceLastFrame / 240.0);
	//m_translateVector = (m_pCamera->getOrientationForViewUpdate() * m_translateVector * timeSinceLastFrame / 300.0);

	m_pCameraNode->translate(m_translateVector);
}

//================================================//

void Camera::pitch(Ogre::Degree x)
{
	Ogre::Radian amount(x * -m_rotateSpeed); // * Profile::sensitivity * Profile::inverted

	m_pCameraPitchNode->pitch(amount);
}

//================================================//

void Camera::yaw(Ogre::Degree y)
{
	Ogre::Radian amount(y * -m_rotateSpeed);

	m_pCameraYawNode->yaw(amount);
}

//================================================//

void Camera::roll(Ogre::Degree z)
{
	m_pCameraRollNode->roll(z * -m_rotateSpeed);
}

//================================================//

void Camera::getRayhit(Ogre::Vector3& to, Rayhit* rayhit)
{
	btVector3 btFrom(m_pCameraNode->getPosition().x, m_pCameraNode->getPosition().y, m_pCameraNode->getPosition().z);
	btVector3 btTo(to.x, to.y, to.z);

	btCollisionWorld::ClosestRayResultCallback res(btFrom, btTo);

	m_physics->getWorld()->rayTest(btFrom, btTo, res);

	if(res.hasHit()){
		// Get scene node
		rayhit->node = static_cast<Ogre::SceneNode*>(res.m_collisionObject->getUserPointer());

		// Calculate distance
		rayhit->hitPoint = Sparks::bulletToOgreVector3(res.m_hitPointWorld);
		rayhit->distance = m_pCameraNode->getPosition().distance(rayhit->hitPoint);

		rayhit->hasHit = true;
	}
	else{
		rayhit->hasHit = false;
	}
}

//================================================//

void Camera::getRayhit(Ogre::Vector3& from, Ogre::Vector3& to, Rayhit* rayhit)
{
	btVector3 btFrom(from.x, from.y, from.z);
	btVector3 btTo(to.x, to.y, to.z);

	btCollisionWorld::ClosestRayResultCallback res(btFrom, btTo);

	m_physics->getWorld()->rayTest(btFrom, btTo, res);

	if(res.hasHit()){
		// Get scene node
		rayhit->node = static_cast<Ogre::SceneNode*>(res.m_collisionObject->getUserPointer());

		// Calculate distance
		rayhit->hitPoint = Sparks::bulletToOgreVector3(res.m_hitPointWorld);
		rayhit->distance = m_pCameraNode->getPosition().distance(rayhit->hitPoint);

		rayhit->hasHit = true;
	}
	else{
		rayhit->hasHit = false;
	}
}

//================================================//

void Camera::updateRays(void)
{
	// Update rayhits

	// Negative Y
	this->getRayhit(Ogre::Vector3(m_pCameraNode->getPosition().x, -10000.0, m_pCameraNode->getPosition().z), m_negativeYRayhit);

	// Negative Z															Varies
	this->getRayhit((m_pCameraNode->getPosition() + (this->getDirection() * 10000.0)), m_negativeZRayhit);
}

//================================================//

//void Camera::updateFooting(double timeSinceLastFrame)
//{
//	// Cast a ray straight down from the camera's position
//	btVector3 from(m_pCameraNode->getPosition().x, m_pCameraNode->getPosition().y, m_pCameraNode->getPosition().z);
//	btVector3 to(m_pCameraNode->getPosition().x, -5000.0, m_pCameraNode->getPosition().z);
//
//	btCollisionWorld::ClosestRayResultCallback res(from, to);
//
//	Base::getSingletonPtr()->m_btWorld->rayTest(from, to, res);
//
//	if(res.hasHit()){
//		// update the y difference from the camera to the ground
//		m_negativeYRayhit->distance = (m_pCameraNode->getPosition().y - Ogre::Math::Abs(res.m_hitPointWorld.getY()) - m_cameraHeight);
//
//
//		// get the rigid body pointer of the object hit
///// * This may be causing the world to disappear, because V: #! or something is displayed when it happens
//		// use isKinematicObject()
//		//btRigidBody* body = static_cast<btRigidBody*>(res.m_collisionObject);
//		//if(body){
//		//	btVector3 velocity = body->getLinearVelocity();
//
//		//	// adjust the movement speed to compensate for the movement of the object below
//		//	if(m_btCamera->getLinearVelocity().length() < velocity.length()){
//		//		m_btCamera->applyCentralImpulse(velocity);
//		//		printf("V: %.2f\n", velocity.length());
//		//	}
//		//}
//	}
//}

//================================================//

void Camera::setMode(unsigned mode)
{
	if(mode != m_mode){
		m_mode = mode;

		if(m_mode == MODE_FIRST_PERSON){
			createRigidBody();
		}
		else if(m_mode == MODE_SPECTATOR){
			removeRigidBody();
		}
	}
}

//================================================//

} // namespace Sparks

//================================================//