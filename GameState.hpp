//================================================//

#ifndef __GAMESTATE_HPP__
#define __GAMESTATE_HPP__

//================================================//

#include "AppState.hpp"
#include "Camera.hpp"
#include "Sparks.hpp"
#include "DotSceneLoader.hpp"
#include "Physics.hpp"
#include "GUILayerGameState.hpp"

#include "EventManager.hpp"
#include "Boots.hpp"
#include "Profile.hpp"
#include "Player.hpp"
#include "NPC.hpp"
#include "SharedData.hpp"

//================================================//

class GameState : public AppState
{
public:
	GameState(void);

	DECLARE_APPSTATE_CLASS(GameState);

	virtual void enter(void);
	virtual void exit(void);
	virtual bool pause(void);
	virtual void resume(void);
	virtual void createScene(void);
	virtual void destroyScene(void);
	virtual void createGUI(void);
	void createLoadingGUI(void);
	virtual void destroyGUI(void);

	virtual void preloadMeshData(void);

	// OIS::KeyListener
	bool keyPressed(const OIS::KeyEvent& arg);
	bool keyReleased(const OIS::KeyEvent& arg);

	// OIS::MouseListener
	bool mouseMoved(const OIS::MouseEvent& arg);
	bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id);

	// GUI callbacks

	// Updaters
	virtual void update(double timeSinceLastFrame);
	void updateUI(void);
	void updateBullet(double timeSinceLastFrame);

protected:
	enum{
		STATE_ACTIVE = 0,
		STATE_LOADING_FIRST_ENTRY,
		STATE_LOADING_NEXT_STAGE
	};

	enum{
		STEP_FIRST = 0,
		STEP_CREATE_PHYSICS_WORLD,
		STEP_INIT_EVENT_MANAGER,
		STEP_PRELOAD_MESH_DATA,
		STEP_CREATE_SCENE,
		STEP_SETUP_COLLISION_WORLD,
		STEP_CREATE_GUI,

		STEP_FINAL
	};

	// Session
	int						m_state;

	// Loading
	int						m_loadingStep;

	// Environment
	EventManager*			m_pEventManager;
	Ogre::Light*			m_pSunlight;
	
	// Player 
	Player*					m_player;
	Profile*				m_profile;

	// Physics
	Physics*				m_physics;

	// SkyX
	SkyX::SkyX*				m_skyX;
	SkyX::BasicController*	m_skyXController;
	bool					m_skyXInitialised;

	// Hydrax
	Hydrax::Hydrax*			m_hydrax;
	bool					m_hydraxInitialised;
	Ogre::Camera*			m_hydraxCamera;

	// GUI elements
	GUILayer*				m_GUIHudLayer;
	GUILayer*				m_GUILoadingLayer;
	int						m_GUIEventId;

	bool					m_bQuit;
};

//================================================//

#endif

//================================================//