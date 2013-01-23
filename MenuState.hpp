//================================================//

#ifndef __MENUSTATE_HPP__
#define __MENUSTATE_HPP__

//================================================//

#include "AppState.hpp"

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

	// GUI callbacks
	void GUI_startButton(MyGUI::WidgetPtr _sender);
	void GUI_optionsButton(MyGUI::WidgetPtr _sender);
	void GUI_exitButton(MyGUI::WidgetPtr _sender);

	// update
	void update(double timeSinceLastFrame);

private:
	// Ogre
	Ogre::AnimationState*		m_animState;

	// GUI
	MyGUI::ButtonPtr			m_GUI_ButtonPtr_Start;

	// FMOD
	FMOD::Sound*				m_music;
	FMOD::Channel*				m_musicChannel;

	// misc.
	bool						m_bQuit;
};

//================================================//

#endif

//================================================//