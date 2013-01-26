//================================================//

#include "MenuState.hpp"

//================================================//

MenuState::MenuState(void)
{
	m_bQuit = false;
	m_FrameEvent = Ogre::FrameEvent();
	m_GUIEventId = 0;
}

//================================================//

void MenuState::enter(void)
{
	Base::getSingletonPtr()->m_pLog->logMessage("[S] Entering MenuState...");
	m_bQuit = false;

	// create the scene manager
	m_pSceneMgr = Base::getSingletonPtr()->m_pRoot->createSceneManager(Ogre::ST_GENERIC, "MenuSceneMgr");
	
	// create camera
	m_pCamera = m_pSceneMgr->createCamera("MenuCam");
	m_pCamera->setAutoAspectRatio(true);
	m_pCamera->setNearClipDistance(1);
	m_pCamera->setAspectRatio(Ogre::Real(Base::getSingletonPtr()->m_pViewport->getActualWidth()) / 
		Ogre::Real(Base::getSingletonPtr()->m_pViewport->getActualHeight()));

	Base::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);

	// play the menu music
	// ...

	// now create the scene
	createScene();

	// and the GUI
	createGUI();
}

//================================================//

void MenuState::exit(void)
{
	Base::getSingletonPtr()->m_pLog->logMessage("[S] Leaving MenuState...");

	// destroy the GUI
	destroyGUI();

	// release menu music
	// ...

	// destroy the camera and scene manager
	m_pSceneMgr->destroyCamera(m_pCamera);
	if(m_pSceneMgr){
		Base::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);
	}
}

//================================================//

bool MenuState::pause(void)
{
	Base::getSingletonPtr()->m_pLog->logMessage("[S] Pausing MenuState...");

	// Destroy GUI items for now
	destroyGUI();

	// Pause music
	m_musicChannel->setPaused(true);

	return true;
}

//================================================//

void MenuState::resume(void)
{
	Base::getSingletonPtr()->m_pLog->logMessage("[S] Resuming MenuState...");
	m_bQuit = false;

	// Create GUI items again
	createGUI();

	// Unpause music
	m_musicChannel->setPaused(false);

	// set the camera again
	Base::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);
}

//================================================//

void MenuState::createScene(void)
{
	// Environment
	m_pSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");

	// Camera animation
	m_pCamera->setAutoTracking(true, m_pSceneMgr->getRootSceneNode()->createChildSceneNode());

	Ogre::SceneNode* camNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
	camNode->attachObject(m_pCamera);

	Ogre::Animation* anim = m_pSceneMgr->createAnimation("MenuCamTrack", 20);
	anim->setInterpolationMode(Ogre::Animation::IM_SPLINE);
	
	Ogre::NodeAnimationTrack* track = anim->createNodeTrack(0, camNode);
	track->createNodeKeyFrame(0)->setTranslate(Ogre::Vector3(-200, 0, 0));
	track->createNodeKeyFrame(2.5)->setTranslate(Ogre::Vector3(-200, 0, -200));
	track->createNodeKeyFrame(5)->setTranslate(Ogre::Vector3( 0, 0, -200));
	track->createNodeKeyFrame(7.5)->setTranslate(Ogre::Vector3(200, 0, -200));
	track->createNodeKeyFrame(10)->setTranslate(Ogre::Vector3(200, 0, 0));
	track->createNodeKeyFrame(12.5)->setTranslate(Ogre::Vector3(200, 0, 200));
	track->createNodeKeyFrame(15)->setTranslate(Ogre::Vector3(0, 0, 200));
	track->createNodeKeyFrame(17.5)->setTranslate(Ogre::Vector3(-200, 0, 200));
	track->createNodeKeyFrame(20)->setTranslate(Ogre::Vector3(-200, 0, 0));

	m_animState = m_pSceneMgr->createAnimationState("MenuCamTrack");
	m_animState->setEnabled(true);
}

//================================================//

void MenuState::createGUI(void)
{
	// set the scene manager
	Base::getSingletonPtr()->m_GUI_Platform->getRenderManagerPtr()->setSceneManager(m_pSceneMgr);

	// Initialise all GUI layers for this state
	m_GUIRootLayer = new GUIMenuState::GUIRootLayer(&m_GUIEventId);
	m_GUIRootLayer->create();

	m_GUINewGameLayer = new GUIMenuState::GUINewGameLayer(&m_GUIEventId);
	m_GUINewGameLayer->create();

	//int buttonWidth = 300, buttonHeight = 26;
	//int screenWidth = Base::getSingletonPtr()->m_pViewport->getActualWidth();
	//int screenHeight = Base::getSingletonPtr()->m_pViewport->getActualHeight();

	//m_GUI_ButtonPtr_Start = Base::getSingletonPtr()->m_GUI->createWidget<MyGUI::Button>("Button", 
	//	(screenWidth / 2) - (buttonWidth / 2), (screenHeight / 2) - (buttonHeight / 2), buttonWidth, buttonHeight, MyGUI::Align::Default, "Main");
	//static_cast<MyGUI::ButtonPtr>(m_GUI_ButtonPtr_Start)->setCaption("Start Game");
	//m_GUI_ButtonPtr_Start->eventMouseButtonClick += MyGUI::newDelegate(this, &MenuState::GUI_startButton);

	//m_GUI_EditPtr_Name = Base::getSingletonPtr()->m_GUI->createWidget<MyGUI::EditBox>("EditBox", (screenWidth / 2) - (buttonWidth / 2), 
	//	(screenHeight / 2) - (buttonHeight / 2) + 100, buttonWidth, buttonHeight, MyGUI::Align::Default, "Main");
	//m_GUI_EditPtr_Name->setCaption("Enter Name Here");

	//// set the pointer to visible
	//MyGUI::PointerManager::getInstancePtr()->setVisible(true);
}

//================================================//

void MenuState::destroyGUI(void)
{
	m_GUIRootLayer->destroy();
	m_GUINewGameLayer->destroy();

	Base::getSingletonPtr()->m_GUI_Platform->getRenderManagerPtr()->setSceneManager(nullptr);
}

//================================================//

bool MenuState::keyPressed(const OIS::KeyEvent& arg)
{
	switch(arg.key){
	case OIS::KC_ESCAPE:
		// Store the layers in a vecotr and iterate backwards instead
		if(m_GUICurrentLayer != m_GUIRootLayer){
			m_GUICurrentLayer->setVisible(false);

			m_GUIRootLayer->setVisible(true);
			m_GUICurrentLayer = m_GUIRootLayer;
		}
		else{
			m_bQuit = true;
		}
		break;

	default:
		break;
	}

	Base::getSingletonPtr()->keyPressed(arg);
	return true;
}

//================================================//

bool MenuState::keyReleased(const OIS::KeyEvent& arg)
{
	Base::getSingletonPtr()->keyReleased(arg);

	return true;
}

//================================================//

bool MenuState::mouseMoved(const OIS::MouseEvent& arg)
{
	Base::getSingletonPtr()->mouseMoved(arg);

	return true;
}

//================================================//

bool MenuState::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
	Base::getSingletonPtr()->mousePressed(arg, id);

	return true;
}

//================================================//

bool MenuState::mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
	Base::getSingletonPtr()->mouseReleased(arg, id);

	return true;
}

//================================================//

void MenuState::handleGUIEvent(void)
{
	switch(m_GUIEventId){
	default:
		break;

	// Root Layer
	case GUIMenuState::GUIRootLayer::EVENT_NEWGAME:
		//printf("%s\n", m_GUIRootLayer->getWidgetText("Button_NewGame").c_str());
		{
			m_GUIRootLayer->setVisible(false);

			m_GUINewGameLayer->setVisible(true);
			m_GUICurrentLayer = m_GUINewGameLayer;
			
			/*Profile::getSingletonPtr()->create("TestProfile");
			this->pushAppState(findByName("GameState"));*/
		}
		break;

	// New Game Layer
	case GUIMenuState::GUINewGameLayer::EVENT_START:
		// Start a new game
		Profile::getSingletonPtr()->create(m_GUINewGameLayer->getWidgetText("EditBox_Name"));
		this->pushAppState(findByName("GameState"));
		break;
	}

	// Reset GUI event
	m_GUIEventId = 0;
}

//================================================//

void MenuState::update(double timeSinceLastFrame)
{
	m_FrameEvent.timeSinceLastFrame = timeSinceLastFrame;

	// update camera animation
	m_animState->addTime(timeSinceLastFrame / 500.0);

	// GUI
	if(m_GUIEventId){
		handleGUIEvent();
	}

	if(m_bQuit == true){
		this->popAppState();
		return;
	}
}

//================================================//