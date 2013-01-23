//================================================//

#ifndef __BASE_HPP__
#define __BASE_HPP__

#include "stdafx.h"

//================================================//

class Base : public Ogre::FrameListener, public Ogre::WindowEventListener, public Ogre::Singleton<Base>, public OIS::KeyListener, public OIS::MouseListener
{
public:
	Base(void);
	~Base(void);

	bool init(OIS::KeyListener* pKeyListener, OIS::MouseListener* pMouseListener);
	void initResources(void);

	void createPhysicsWorld(void);
	void destroyPhysicsWorld(void);

	void updateOgre(double timeSinceLastFrame);

	// Internal game clock
	const float	getClockSpeed(void);
	void		setClockSpeed(const float speed = 1.0f);

	// OIS::KeyListener
	bool keyPressed(const OIS::KeyEvent& arg);
	bool keyReleased(const OIS::KeyEvent& arg);

	// OIS::MouseListener
	bool mouseMoved(const OIS::MouseEvent& arg);
	bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id);

	// Ogre::WindowEventListener
	void windowResized(Ogre::RenderWindow* rw);

	// --- //

	// Ogre
	Ogre::Root*				m_pRoot;
	Ogre::RenderWindow*		m_pRenderWindow;
	Ogre::SceneManager*		m_pSceneMgr;
	Ogre::Viewport*			m_pViewport;
	Ogre::String		    m_resourcesCfg, m_pluginsCfg;
	Ogre::Timer*			m_pTimer;
	Ogre::Log*				m_pLog;

	// OIS
	OIS::InputManager*		m_pInputMgr;
	OIS::Mouse*				m_pMouse;
	OIS::Keyboard*			m_pKeyboard;

	// MyGUI
	MyGUI::Gui*				m_GUI;
	MyGUI::OgrePlatform*	m_GUI_Platform;
	MyGUI::PointerManager*  m_pointerManager;
	bool					m_injectGUI;

	// SkyX
	SkyX::SkyX*				m_skyX;
	SkyX::BasicController*	m_skyController;
	bool					m_useSkyX;

	// Bullet
	bool									m_useBullet;
	btDiscreteDynamicsWorld*				m_btWorld;
	btCollisionWorld*						m_btColWorld;
	std::vector<btRigidBody*>				m_btObjects;
	int										m_btObjectCount;
	btBroadphaseInterface*					m_btBroadphase;
	btDefaultCollisionConfiguration*		m_btCollisionConfiguration;
	btCollisionDispatcher*					m_btCollisionDispatcher;
	btSequentialImpulseConstraintSolver*	m_btSolver;

	// FMOD
	FMOD::System*							m_soundSystem;

	/*OgreBulletDynamics::DynamicsWorld*			m_world;
	OgreBulletCollisions::DebugDrawer*			m_debugDrawer;
	int											m_numEntitiesInstanced;
	std::deque<OgreBulletDynamics::RigidBody*>	m_bodies;
	std::deque<OgreBulletCollisions::CollisionShape *> m_shapes;*/


protected:
	float GetElapsedTimeInSeconds(void);
	void createResourceListener(void);

	Base(const Base&);
	Base& operator=(const Base&);

	// --- //

	float					m_elapsedTimeSec;
	float					m_clockSpeed;		// speed of game (percentage)
};

//================================================//

inline const float Base::getClockSpeed(void)
{
	return m_clockSpeed;
}

//================================================//

inline void Base::setClockSpeed(const float speed)
{
	m_clockSpeed = speed;
}

//================================================//

#define FMOD_ERRCHECK(result)\
	if(result != FMOD_OK){\
		printf("FMOD Error %d: %s\n", result, FMOD_ErrorString(result));\
	}\

//================================================//

#endif

//================================================//