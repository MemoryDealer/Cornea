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
	m_pMusic = new Sound(Sound::TYPE_MUSIC_MAIN_MENU);
	//m_pMusic->play();

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
	delete m_pMusic;

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

	// Stop the music when entering the game, so it starts over when going back to main menu
	m_pMusic->stop();

	return true;
}

//================================================//

void MenuState::resume(void)
{
	Base::getSingletonPtr()->m_pLog->logMessage("[S] Resuming MenuState...");
	m_bQuit = false;

	// Create GUI items again
	createGUI();

	// Start music
	//m_pMusic->play();

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
	// Set the scene manager
	Base::getSingletonPtr()->m_GUI_Platform->getRenderManagerPtr()->setSceneManager(m_pSceneMgr);

	// Initialise all GUI layers for this state
	m_GUIRootLayer = new GUIMenuState::GUIRootLayer(&m_GUIEventId);
	m_GUIRootLayer->create();
	m_GUILayers.push_back(m_GUIRootLayer);

	m_GUINewGameLayer = new GUIMenuState::GUINewGameLayer(&m_GUIEventId);
	m_GUINewGameLayer->create();

	m_GUILoadGameLayer = new GUIMenuState::GUILoadGameLayer(&m_GUIEventId);
	m_GUILoadGameLayer->create();

	// Set the mouse pointer to visible
	if(!MyGUI::PointerManager::getInstancePtr()->isVisible())
		MyGUI::PointerManager::getInstancePtr()->setVisible(true);

	Base::getSingletonPtr()->m_injectGUI = true;
}

//================================================//

void MenuState::destroyGUI(void)
{
	m_GUIRootLayer->destroy();
	m_GUINewGameLayer->destroy();
	m_GUILoadGameLayer->destroy();

	Base::getSingletonPtr()->m_GUI_Platform->getRenderManagerPtr()->setSceneManager(nullptr);
}

//================================================//

bool MenuState::keyPressed(const OIS::KeyEvent& arg)
{
	switch(arg.key){
	case OIS::KC_ESCAPE:
		// Pop the back of the GUI layers vector until the root layer
		if(m_GUILayers.back() != m_GUIRootLayer){
			m_GUILayers.back()->setVisible(false);
			m_GUILayers.pop_back();

			m_GUILayers.back()->setVisible(true);
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
		// Resume Game
	case GUIMenuState::GUIRootLayer::EVENT_RESUMEGAME:

		break;

		// New Game
	case GUIMenuState::GUIRootLayer::EVENT_NEWGAME:
		{
			m_GUIRootLayer->setVisible(false);

			m_GUINewGameLayer->setVisible(true);
			m_GUILayers.push_back(m_GUINewGameLayer);
		}
		break;

		// Load Game
	case GUIMenuState::GUIRootLayer::EVENT_LOADGAME:
		{
			m_GUIRootLayer->setVisible(false);

			m_GUILoadGameLayer->setVisible(true);
			m_GUILayers.push_back(m_GUILoadGameLayer);
		}
		break;

		// Options
	case GUIMenuState::GUIRootLayer::EVENT_OPTIONS:

		break;

		// Credits
	case GUIMenuState::GUIRootLayer::EVENT_CREDITS:

		break;

		// Exit Game
	case GUIMenuState::GUIRootLayer::EVENT_EXITGAME:
		m_bQuit = true;
		break;

	// New Game Layer
		// Start Game
	case GUIMenuState::GUINewGameLayer::EVENT_START:
		{
			// Begins a new game
			Ogre::String name = m_GUINewGameLayer->getWidgetText(GUIMenuState::GUINewGameLayer::EDITBOX_NAME);
			if(name.length() > 0){
				Ogre::StringUtil strUtil;
				if(strUtil.match(name, "Enter Your Name...", false)){
					break;
				}

				// Clear main menu GUI layer stack
				m_GUILayers.clear();
				m_GUILayers.push_back(m_GUIRootLayer);

				SharedData::getSingletonPtr()->buffer = name;
				SharedData::getSingletonPtr()->action = 0;
				this->pushAppState(findByName("GameState"));
			}
		}
		break;

	// Load Game Layer
		// Load Game
	case GUIMenuState::GUILoadGameLayer::EVENT_LOAD:
		{
			Ogre::String name = m_GUILoadGameLayer->getListBoxSelectedText(GUIMenuState::GUILoadGameLayer::LISTBOX_SAVES);
			
			if(name != "None"){
				SharedData::getSingletonPtr()->buffer = name;
				SharedData::getSingletonPtr()->action = 1;
				this->pushAppState(findByName("GameState"));	
			}
		}
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