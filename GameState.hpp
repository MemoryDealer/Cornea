//================================================//

#ifndef __GAMESTATE_HPP__
#define __GAMESTATE_HPP__

//================================================//

#include "AppState.hpp"
#include "Camera.hpp"
#include "Sparks.hpp"
#include "DotSceneLoader.hpp"
#include "BtOgre.hpp"
#include "GUILayerGameState.hpp"

#include "EventManager.hpp"
#include "Boots.hpp"
#include "Profile.hpp"
#include "Player.hpp"
#include "NPC.hpp"

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
	virtual void createGUI(void);
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
		STATE_LOADING
	};

	// Environment
	EventManager*			m_pEventManager;
	
	// Player 
	Player*					m_player;

	// Physics
	BtOgre::DebugDrawer*	m_debugDrawer;

	// GUI elements
	GUILayer*				m_GUIHudLayer;
	int						m_GUIEventId;

	bool					m_bQuit;
};

//================================================//

#endif

//================================================//