//================================================//

#include "Physics.hpp"

//================================================//

// Tick callback 
static void tickCallback(btDynamicsWorld* world, btScalar timeStep)
{
	Physics* physics = static_cast<Physics*>(world->getWorldUserInfo());
	physics->processTickCallback(timeStep);
}

//================================================//

Physics::Physics(void)
{
	m_cameraActive = false;
}

//================================================//

Physics::~Physics(void)
{

}

//================================================//

void Physics::init(void)
{
	const int sweep = 10000;
	m_broadphase = new btAxisSweep3(btVector3(-sweep, -sweep, -sweep), btVector3(sweep, sweep, sweep));
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_collisionDispatcher = new btCollisionDispatcher(m_collisionConfiguration);

	m_solver = new btSequentialImpulseConstraintSolver();
	m_world = new btDiscreteDynamicsWorld(m_collisionDispatcher, m_broadphase, m_solver, m_collisionConfiguration);
	m_collisionWorld = new btCollisionWorld(m_collisionDispatcher, m_broadphase, m_collisionConfiguration);
	m_objectCount = 0;

	// Init tick callback
	m_world->setInternalTickCallback(tickCallback, static_cast<void*>(this), false);
}

//================================================//

void Physics::free(void)
{
	btCollisionObjectArray& objects = m_world->getCollisionObjectArray();
	int size = objects.size();
	for(int i=(size - 1); i>=0; --i){
		printf("Removing %s...\n", static_cast<Ogre::SceneNode*>(objects.at(i)->getUserPointer())->getName().c_str());
		m_world->removeCollisionObject(objects.at(i));
	}

	m_objects.clear();
	delete m_world;
	delete m_solver;
	delete m_collisionDispatcher;
	delete m_collisionConfiguration;
	delete m_broadphase;
}

//================================================//

void Physics::initDebugDrawer(Ogre::SceneNode* root)
{
	m_debugDrawer = new BtOgre::DebugDrawer(root, m_world);
	m_world->setDebugDrawer(m_debugDrawer);
	m_debugDrawer->setDebugMode(0);
}

//================================================//

void Physics::registerAllEntitiesInScene(Ogre::SceneManager* mgr)
{
	BtOgre::registerAllEntitiesAsColliders(mgr, m_world);
}

//================================================//

void Physics::setCameraData(CAMERA_DATA* data)
{
	m_pCameraData = data;
	m_world->addRigidBody(m_pCameraData->body);
	m_cameraActive = true;
}

//================================================//

void Physics::removeCameraBody(void)
{
	m_world->removeRigidBody(m_pCameraData->body);
	m_cameraActive = false;
}

//================================================//

void Physics::update(double timeSinceLastFrame)
{
	const float rate = 1.0f/240.0f;		// More efficient, suggested by Bullet creator Erwin
	float physicsTime = timeSinceLastFrame / 1000.0f;
	int nMaxSteps = physicsTime/(rate)+1;
	m_world->stepSimulation(physicsTime, nMaxSteps, rate);

	// Step the world
	/*const double timeStep = 1.0f/60.0f;
	static double accumulator = 0.0f;

	accumulator += timeSinceLastFrame;
	while(accumulator >= timeStep){
		m_world->stepSimulation(timeStep);
		accumulator -= timeStep;
	}*/

	// Update camera rigid body
	if(m_cameraActive){
		btTransform transform;
		Ogre::SceneNode* node = static_cast<Ogre::SceneNode*>(m_pCameraData->body->getUserPointer());
		m_pCameraData->body->getMotionState()->getWorldTransform(transform);

		btVector3 pos = transform.getOrigin(); //m_pCameraData->body->getCenterOfMassPosition();
		node->setPosition(
			Ogre::Vector3((float)pos[0], (float)pos[1] + *(m_pCameraData->heightOffset), (float)pos[2]));

		btQuaternion orientation = transform.getRotation();
		node->setOrientation(Ogre::Quaternion(orientation.w(), orientation.x(), orientation.y(), orientation.z()));
	}

	// Update debug drawer
	m_debugDrawer->step();

	// Update other rigid bodies
	btRigidBody* body;

	for(std::vector<btRigidBody*>::iterator itr = m_objects.begin(); itr != m_objects.end(); ++itr){
		Ogre::SceneNode* node = static_cast<Ogre::SceneNode*>((*itr)->getUserPointer());
		body = *itr;

		btVector3 pos = body->getCenterOfMassPosition();
		node->setPosition((float)pos[0], (float)pos[1], (float)pos[2]);

		btQuaternion orientation = body->getOrientation();
		node->setOrientation(Ogre::Quaternion(orientation.w(), orientation.x(), orientation.y(), orientation.z()));
	}
}

//================================================//

void Physics::processTickCallback(btScalar timeStep)
{
	// Limit the maximum speed of the player
	if(m_cameraActive){
		btVector3 velocity = m_pCameraData->body->getLinearVelocity();
		btScalar speed = velocity.length();
		btScalar max = *(m_pCameraData->shift) ? *(m_pCameraData->sprintVelocity) : *(m_pCameraData->maxVelocity);
		if(speed > max){
			velocity *= max/speed;
			m_pCameraData->body->setLinearVelocity(velocity);
		}
	}
}

//================================================//