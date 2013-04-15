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

	typedef struct{
		btRigidBody*	body;
		Ogre::Real*		heightOffset;
		Ogre::Real*		maxVelocity;
		Ogre::Real*		sprintVelocity;
		bool*			shift;
	} CAMERA_DATA;

	void init(void);
	void free(void);

	void initDebugDrawer(Ogre::SceneNode* root);

	// Collision
	void registerAllEntitiesInScene(Ogre::SceneManager* mgr);
	void setCameraData(CAMERA_DATA* data);
	void removeCameraBody(void);

	void flipDebugDrawer(void){ m_debugDrawer->setDebugMode(!m_debugDrawer->getDebugMode()); }

	// Getter functions
	btDiscreteDynamicsWorld* getWorld(void) const;

	void update(double timeSinceLastFrame);
	void processTickCallback(btScalar timeStep);

private:
	btDiscreteDynamicsWorld*				m_world;
	btCollisionWorld*						m_collisionWorld;
	btSequentialImpulseConstraintSolver*	m_solver;
	btBroadphaseInterface*					m_broadphase;
	btDefaultCollisionConfiguration*		m_collisionConfiguration;
	btCollisionDispatcher*					m_collisionDispatcher;

	std::vector<btRigidBody*>				m_objects;
	int										m_objectCount;

	CAMERA_DATA*							m_pCameraData;
	bool									m_cameraActive;

	BtOgre::DebugDrawer*					m_debugDrawer;
};

//================================================//

inline btDiscreteDynamicsWorld* Physics::getWorld(void) const
{ return m_world; }

//================================================//

#endif

//================================================//