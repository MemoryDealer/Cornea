//================================================//

#include "Camera.hpp"

//================================================//

namespace Sparks
{

//================================================//

Camera::Camera(Ogre::SceneManager* mgr, Ogre::Real farClipDistance)
{
	m_mode = MODE_FIRST_PERSON;
	m_moveSpeed = 70.0;
	m_maxVelocity = 10.0;
	m_rotateSpeed = 0.06;
	m_translateVector = Ogre::Vector3::ZERO;
	m_velocityVector = Ogre::Vector3::ZERO;
	m_acceleration = 0.3;
	m_jumping = false;
	m_jmpConstant = 500.0;
	m_cameraHeight = 22.0;
	m_defGravity.setValue(0.0, -9.82, 0.0);

	m_moveForwardsPressed = false;
	m_moveBackwardsPressed = false;
	m_moveLeftPressed = false;
	m_moveRightPressed = false;
	m_moveUpPressed = false;
	m_moveDownPressed = false;
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
}

//================================================//

Camera::~Camera(void)
{
	if(m_mode == MODE_FIRST_PERSON)
		removeRigidBody();

	// Remove scene nodes and camera object
	m_pSceneMgr->destroySceneNode(m_pCameraNode);
	m_pSceneMgr->destroySceneNode(m_pCameraPitchNode);
	m_pSceneMgr->destroySceneNode(m_pCameraYawNode);
	m_pSceneMgr->destroySceneNode(m_pCameraRollNode);
	m_pSceneMgr->destroyCamera(m_pCamera);

	delete m_negativeYRayhit;
	delete m_negativeZRayhit;
}

//================================================//

void Camera::init(Physics* physics)
{
	m_physics = physics;

	m_pSceneMgr->destroyCamera("PlayerCam");
	m_pCamera = m_pSceneMgr->createCamera("PlayerCam");
	m_pCamera->setAutoAspectRatio(true);

	m_pCamera->setNearClipDistance(1);
	m_pCamera->setFarClipDistance(m_farClipDistance);

	m_pCameraRollNode->attachObject(m_pCamera);
	//m_pCameraNode->attachObject(m_pCamera);

	// set the initial position
	m_pCameraNode->setPosition(0, 200, 0);

	createRigidBody();

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
	btScalar mass = 12.0;
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(camPos.x, camPos.y, camPos.z));

	// set up rigid body
	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btCollisionShape* shape = new btCapsuleShape(7.0, 30.0); // need changes
	btVector3 localInertia;
	shape->calculateLocalInertia(mass, localInertia);

	m_btCamera = new btRigidBody(mass, motionState, shape, localInertia);
	m_btCamera->setUserPointer((void*)m_pCameraNode);

	// optimize the body
	m_btCamera->setFriction(1.0);
	m_btCamera->setDamping(0.4, 0.4);
	m_btCamera->setAngularFactor(btVector3(0, 0, 0));

	//m_btCamera->setCollisionFlags(m_btCamera->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);

	// this will prevent the camera from falling through elevators when they "take off"
	m_btCamera->setActivationState(DISABLE_DEACTIVATION);

	m_btCamera->setHitFraction(0.0);
	m_btCamera->setContactProcessingThreshold(0.0);

	m_btCamera->setGravity(m_defGravity);

	// add the rigid body to the dynamics world
	//Base::getSingletonPtr()->m_btWorld->addRigidBody(m_btCamera);
	m_physics->setCameraBody(m_btCamera);
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
	m_btCamera->setDamping(0.4, 0.4);
	m_btCamera->setAngularFactor(btVector3(0, 0, 0));

	m_btCamera->setHitFraction(0.0);
	m_btCamera->setContactProcessingThreshold(0.0);

	m_btCamera->setGravity(m_defGravity);

	// add the rigid body to the dynamics world
	m_physics->setCameraBody(m_btCamera);
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
		moveFirstPerson(timeSinceLastFrame);
		return;

	case MODE_SPECTATOR:
	default:
		moveSpectator(timeSinceLastFrame);
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

void Camera::moveFirstPerson(double timeSinceLastFrame)
{
	//updateFooting(timeSinceLastFrame);
	updateRays();

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
		m_translateVector.x = -(m_moveSpeed * 0.90); // limit sideways movement to 90%
	}
	else if(!m_moveRightPressed){
		m_translateVector.x = 0.0;
	}
	if(m_moveRightPressed){
		m_translateVector.x = (m_moveSpeed * 0.90);
	}
	else if(!m_moveLeftPressed){
		m_translateVector.x = 0.0;
	}

	m_translateVector = (m_pCameraYawNode->getOrientation() * m_pCameraPitchNode->getOrientation() * m_translateVector);
	//m_translateVector = (m_pCamera->getOrientationForViewUpdate() * m_translateVector);

	// Calculate the forwards vector and use it to keep the camera going the same speed, despite the pitch
	// Note: this may allow wall climbing where not conceivable
	Ogre::Vector3 forwards = m_pCamera->getDerivedUp();
	forwards.crossProduct(m_pCamera->getDerivedRight());
	forwards.normalise();

	m_translateVector.x /= forwards.y;
	m_translateVector.z /= forwards.y;

	// Update the jump physics
	updateJump(timeSinceLastFrame);

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

	// Limit the maximum velocity by magnitude
	if(m_btCamera->getLinearVelocity().length() > m_maxVelocity){
		return;
	}

	// Stop if the user is not moving
	if(m_translateVector == Ogre::Vector3::ZERO){
		return;
	}

	// Update the camera by applying an impulse to the Bullet rigid body
	btVector3 btTranslate(m_translateVector.x, 0.0, m_translateVector.z);
	
	m_btCamera->activate();
	m_btCamera->applyCentralImpulse(btTranslate);
}

//================================================//

void Camera::updateJump(double timeSinceLastFrame)
{
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
			jmpHeight = m_pCameraNode->getPosition().y;

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
		if(m_pCameraNode->getPosition().y <= jmpHeight){
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
	else if(m_negativeYRayhit->distance > 2.5){
		btVector3 move(m_translateVector.x * 0.04, 0.0, m_translateVector.z * 0.04);

		if(m_btCamera->getGravity() != m_defGravity)
				m_btCamera->setGravity(m_defGravity);

		m_btCamera->activate();
		m_btCamera->applyCentralImpulse(move * timeSinceLastFrame / 10.0); // ^ Same as above call
	}
}

//================================================//

void Camera::moveSpectator(double timeSinceLastFrame)
{
	m_translateVector = Ogre::Vector3::ZERO;

	// Apply movements based on keys pressed

	// forwards
	if(m_moveForwardsPressed){
		if(m_velocityVector.z < m_maxVelocity){
			m_velocityVector.z += m_acceleration;
		}
		else if(m_velocityVector.z > m_maxVelocity){
			m_velocityVector.z -= m_acceleration;
		}

		m_translateVector.z = -(m_moveSpeed * m_velocityVector.z);
	}
	else if(!m_moveBackwardsPressed && m_translateVector.z < 0){
		if(m_velocityVector.z > 0){
			m_velocityVector.z -= (m_acceleration * 2);
			if(m_velocityVector.z < 0){
				m_velocityVector.z = 0;
			}
		}

		m_translateVector.z = -(m_moveSpeed * m_velocityVector.z);
	}

	// backwards
	if(m_moveBackwardsPressed){
		if(m_velocityVector.z < m_maxVelocity){
			m_velocityVector.z += m_acceleration;
		}
		else if(m_velocityVector.z > m_maxVelocity){
			m_velocityVector.z -= m_acceleration;
		}

		m_translateVector.z = (m_moveSpeed * m_velocityVector.z);
	}
	else if(!m_moveForwardsPressed && m_translateVector.z > 0){
		if(m_velocityVector.z > 0){
			m_velocityVector.z -= (m_acceleration * 2);
			if(m_velocityVector.z < 0){
				m_velocityVector.z = 0;
			}
		}

		m_translateVector.z = (m_moveSpeed * m_velocityVector.z);
	}

	// left
	if(m_moveLeftPressed){
		if(m_velocityVector.x < m_maxVelocity){
			m_velocityVector.x += m_acceleration;
		}
		else if(m_velocityVector.x > m_maxVelocity){
			m_velocityVector.x -= m_acceleration;
		}

		m_translateVector.x = -(m_moveSpeed * m_velocityVector.x / 2); // make left/right movement slower for realism
	}
	else if(!m_moveRightPressed && m_translateVector.x < 0){
		if(m_velocityVector.x > 0){
			m_velocityVector.x -= (m_acceleration * 2);
			if(m_velocityVector.x < 0){
				m_velocityVector.x = 0;
			}
		}

		m_translateVector.x = -(m_moveSpeed * m_velocityVector.x / 2);
	}

	// right
	if(m_moveRightPressed){
		if(m_velocityVector.x < m_maxVelocity){
			m_velocityVector.x += m_acceleration;
		}
		else if(m_velocityVector.x > m_maxVelocity){
			m_velocityVector.x -= m_acceleration;
		}

		m_translateVector.x = (m_moveSpeed * m_velocityVector.x / 2);
	}
	else if(!m_moveLeftPressed && m_translateVector.x > 0){
		if(m_velocityVector.x > 0){
			m_velocityVector.x -= (m_acceleration * 2);
			if(m_velocityVector.x < 0){
				m_velocityVector.x = 0;
			}
		}

		m_translateVector.x = (m_moveSpeed * m_velocityVector.x / 2);
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

	//m_pCamera->pitch(amount);

	//m_pCameraPitchNode->pitch(-amount);
	//m_pCameraNode->setOrientation(m_pCamera->getOrientationForViewUpdate());
}

//================================================//

void Camera::yaw(Ogre::Degree y)
{
	Ogre::Radian amount(y * -m_rotateSpeed);

	m_pCameraYawNode->yaw(amount);

	//m_pCamera->yaw(amount);
	//m_pCameraNode->yaw(amount, Ogre::Node::TS_PARENT);

	//m_pCameraYawNode->yaw(-amount);
	//m_pCameraNode->setOrientation(m_pCamera->getOrientationForViewUpdate());

	// try simple grid instead
	// or move camera to one node
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

void Camera::updateRays(void)
{
	// Update rayhits

	// Negative Y
	this->getRayhit(Ogre::Vector3(m_pCameraNode->getPosition().x, -5000.0, m_pCameraNode->getPosition().z), m_negativeYRayhit);
	m_negativeYRayhit->distance = (m_negativeYRayhit->distance - m_cameraHeight);

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