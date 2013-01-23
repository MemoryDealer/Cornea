//================================================//

#ifndef __APPSTATE_HPP__
#define __APPSTATE_HPP__

//================================================//

#include "Base.hpp"
#include "Profile.hpp"

//================================================//

class AppState;

//================================================//

class AppStateListener
{
public:
	AppStateListener(){};
	virtual ~AppStateListener(){};

	virtual void manageAppState(Ogre::String stateName, AppState* state) = 0;

	virtual AppState*		findByName(Ogre::String stateName) = 0;
	virtual void			changeAppState(AppState* state) = 0;
	virtual bool			pushAppState(AppState* state) = 0;
	virtual void			popAppState(void) = 0;
	virtual void			pauseAppState(void) = 0;
	virtual void			shutdown(void) = 0;
	//virtual void			popAllAndPushAppState(AppState* state) = 0;
};

//================================================//

/* Each game state will inherit this class */
class AppState : public OIS::KeyListener, public OIS::MouseListener
{
public:
	static void create(AppStateListener* parent, const Ogre::String name){}

	void destroy(void){ delete this; }

	virtual void enter(void) = 0;
	virtual void exit(void) = 0;
	virtual bool pause(void){ return true; }
	virtual void resume(void){};
	virtual void update(double elapsedTimeSec) = 0;	

protected:
	AppState(){};

	AppState*		findByName(Ogre::String stateName){ return m_pParent->findByName(stateName); }
	void			changeAppState(AppState* state){ m_pParent->changeAppState(state); }
	bool			pushAppState(AppState* state){ return m_pParent->pushAppState(state); }
	void			popAppState(void){ m_pParent->popAppState(); }
	void			shutdown(void){ m_pParent->shutdown(); }
	//void			popAllAndPushAppState(AppState* state){ m_pParent->popAllAndPushAppState(state); }

	AppStateListener*	m_pParent;

	Ogre::Camera*		m_pCamera;
	Ogre::SceneManager* m_pSceneMgr;
	Ogre::FrameEvent	m_FrameEvent;

	unsigned			m_subState;
};

//================================================//

#define DECLARE_APPSTATE_CLASS(T)\
static void create(AppStateListener* parent, const Ogre::String name)\
{\
	T* myAppState = new T();\
	myAppState->m_pParent = parent;\
	parent->manageAppState(name, myAppState);\
}

//================================================//

#endif

//================================================//