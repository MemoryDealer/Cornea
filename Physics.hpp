//================================================//

#ifndef __PHYSICS_HPP__
#define __PHYSICS_HPP__

//================================================//

#include "stdafx.h"
#include "BtOgre.hpp"

//================================================//

class Physics
{
public:
	Physics(void);
	~Physics(void);

	void init(void);
	void free(void);

	void initDebugDrawer(Ogre::SceneNode* root);

	// Collision
	void registerAllEntitiesInScene(Ogre::SceneManager* mgr);
	void setCameraBody(btRigidBody* body);
	void removeCameraBody(void);

	void flipDebugDrawer(void){ m_debugDrawer->setDebugMode(!m_debugDrawer->getDebugMode()); }

	// Getter functions
	btDiscreteDynamicsWorld* getWorld(void) const;

	void update(double timeSinceLastFrame);

private:
	btDiscreteDynamicsWorld*				m_world;
	btCollisionWorld*						m_collisionWorld;
	btSequentialImpulseConstraintSolver*	m_solver;
	btBroadphaseInterface*					m_broadphase;
	btDefaultCollisionConfiguration*		m_collisionConfiguration;
	btCollisionDispatcher*					m_collisionDispatcher;

	std::vector<btRigidBody*>				m_objects;
	int										m_objectCount;

	btRigidBody*							m_cameraBody;

	BtOgre::DebugDrawer*					m_debugDrawer;
};

//================================================//

inline btDiscreteDynamicsWorld* Physics::getWorld(void) const
{ return m_world; }

//================================================//

#endif

//================================================//