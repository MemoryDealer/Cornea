//================================================//

#ifndef __GAMESTATE_HPP__
#define __GAMESTATE_HPP__

//================================================//

#include "AppState.hpp"
#include "Camera.hpp"
#include "Sparks.hpp"
#include "DotSceneLoader.hpp"
#include "BtOgre.hpp"

#include "EventManager.hpp"
#include "Boots.hpp"
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

	void preloadMeshData(void);

	// OIS::KeyListener
	bool keyPressed(const OIS::KeyEvent& arg);
	bool keyReleased(const OIS::KeyEvent& arg);

	// OIS::MouseListener
	bool mouseMoved(const OIS::MouseEvent& arg);
	bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id);

	// GUI callbacks

	// Updaters
	void update(double timeSinceLastFrame);
	void updateUI(void);
	void updateBullet(double timeSinceLastFrame);

private:
	enum{
		SUBSTATE_DEFAULT = 0,
		SUBSTATE_LOADING
	};

	// Environment
	EventManager*			m_pEventManager;
	Ogre::Light*			m_pTorch;
	Ogre::Light*			m_pFlashlight;
	
	// Player 
	Player*					m_player;

	// NPCs
	NPC*					m_npc; // for testing

	// Physics
	BtOgre::DebugDrawer*	m_debugDrawer;

	// HUD elements
	MyGUI::ButtonPtr		m_button;
	MyGUI::ImageBox			m_bootsImage;
	
	// Overlay
	Ogre::Overlay*			m_overlay;
	Ogre::OverlayContainer* m_overlayReticule;

	bool					m_bQuit;
};

//================================================//

#endif

//================================================//