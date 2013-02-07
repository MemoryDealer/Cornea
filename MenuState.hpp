//================================================//

#ifndef __MENUSTATE_HPP__
#define __MENUSTATE_HPP__

//================================================//

#include "AppState.hpp"
#include "SharedData.hpp"
#include "GUILayerMenuState.hpp"

//================================================//

class MenuState : public AppState
{
public:
	MenuState(void);

	DECLARE_APPSTATE_CLASS(MenuState);

	void enter(void);
	void exit(void);
	bool pause(void);
	void resume(void);
	void createScene(void);
	void createGUI(void);
	void destroyGUI(void);

	// OIS::KeyListener
	bool keyPressed(const OIS::KeyEvent& arg);
	bool keyReleased(const OIS::KeyEvent& arg);

	// OIS::MouseListener
	bool mouseMoved(const OIS::MouseEvent& arg);
	bool mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id);

	// GUI
	void handleGUIEvent(void);

	// update
	void update(double timeSinceLastFrame);

private:
	// Ogre
	Ogre::AnimationState*		m_animState;
	Ogre::Camera*				m_pCamera;

	// GUI
	GUILayer*					m_GUIRootLayer;
	GUILayer*					m_GUINewGameLayer;
	GUILayer*					m_GUILoadGameLayer;
	GUILayer*					m_GUIOptionsLayer;
	GUILayer*					m_GUICreditsLayer;
	int							m_GUIEventId;
	std::vector<GUILayer*>		m_GUILayers;


	// FMOD
	FMOD::Sound*				m_music;
	FMOD::Channel*				m_musicChannel;

	// misc.
	bool						m_bQuit;
};

//================================================//

#endif

//================================================//