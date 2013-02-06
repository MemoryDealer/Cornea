//================================================//

#include "Physics.hpp"

//================================================//

Physics::Physics(void)
{
	m_cameraBody = nullptr;
}

//================================================//

Physics::~Physics(void)
{

}

//================================================//

void Physics::init(void)
{
	const int sweep = 1000;
	m_broadphase = new btAxisSweep3(btVector3(-sweep, -sweep, -sweep), btVector3(sweep, sweep, sweep));
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_collisionDispatcher = new btCollisionDispatcher(m_collisionConfiguration);

	m_solver = new btSequentialImpulseConstraintSolver();
	m_world = new btDiscreteDynamicsWorld(m_collisionDispatcher, m_broadphase, m_solver, m_collisionConfiguration);
	m_collisionWorld = new btCollisionWorld(m_collisionDispatcher, m_broadphase, m_collisionConfiguration);
	m_objectCount = 0;
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

void Physics::setCameraBody(btRigidBody* body)
{
	m_cameraBody = body;
	m_world->addRigidBody(m_cameraBody);
	
}

//================================================//

void Physics::removeCameraBody(void)
{
	m_world->removeRigidBody(m_cameraBody);
}

//================================================//

void Physics::update(double timeSinceLastFrame)
{
	// Step the world
	m_world->stepSimulation(timeSinceLastFrame * 0.015f, 60);

	// Update debug drawer
	m_debugDrawer->step();

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