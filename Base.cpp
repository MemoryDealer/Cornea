//================================================//

#include "Base.hpp"

//================================================//

template<> Base* Ogre::Singleton<Base>::msSingleton = 0;

//================================================//

Base::Base(void)
	: m_pRoot(0),
	m_pRenderWindow(0),
	m_pSceneMgr(0),
	m_pViewport(0),
	m_resourcesCfg(Ogre::StringUtil::BLANK),
	m_pluginsCfg(Ogre::StringUtil::BLANK),
	m_pTimer(0),
	m_pInputMgr(0),
	m_pMouse(0),
	m_pKeyboard(0),
	m_useSkyX(true)
{
	m_clockSpeed = 1.0f; // 100% speed
}

//================================================//

Base::~Base(void)
{
	if(m_pInputMgr){
		m_pInputMgr->destroyInputObject(m_pMouse);
		m_pInputMgr->destroyInputObject(m_pKeyboard);
		OIS::InputManager::destroyInputSystem(m_pInputMgr);
	}
	
	if(m_GUI){
		//m_platform->shutdown(); // crash here
		//delete m_platform;
		//m_platform = 0;

		m_GUI->shutdown();
		delete m_GUI;
		m_GUI = 0;

		//m_platform->getRenderManagerPtr()->setSceneManager(m_pSceneMgr);
		//m_platform->getRenderManagerPtr()->shutdown();
	}

	if(m_soundSystem){
		m_soundSystem->release();
	}

	if(m_pRoot)				delete m_pRoot;
}

//================================================//

float Base::GetElapsedTimeInSeconds(void)
{
    // Returns the elapsed time (in seconds) since the last time this function
    // was called. This elaborate setup is to guard against large spikes in
    // the time returned by QueryPerformanceCounter().

    static const int MAX_SAMPLE_COUNT = 50;

    static float frameTimes[MAX_SAMPLE_COUNT];
    static float timeScale = 0.0f;
    static float actualElapsedTimeSec = 0.0f;
    static INT64 freq = 0;
    static INT64 lastTime = 0;
    static int sampleCount = 0;
    static bool initialized = false;

    INT64 time = 0;
    float elapsedTimeSec = 0.0f;

    if (!initialized)
    {
        initialized = true;
        QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&freq));
        QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&lastTime));
        timeScale = 1.0f / freq;
    }

    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&time));
    elapsedTimeSec = (time - lastTime) * timeScale;
    lastTime = time;

    if (fabsf(elapsedTimeSec - actualElapsedTimeSec) < 1.0f)
    {
        memmove(&frameTimes[1], frameTimes, sizeof(frameTimes) - sizeof(frameTimes[0]));
        frameTimes[0] = elapsedTimeSec;

        if (sampleCount < MAX_SAMPLE_COUNT)
            ++sampleCount;
    }

    actualElapsedTimeSec = 0.0f;

    for (int i = 0; i < sampleCount; ++i)
        actualElapsedTimeSec += frameTimes[i];

    if (sampleCount > 0)
        actualElapsedTimeSec /= sampleCount;

    return actualElapsedTimeSec;
}

//================================================//

void Base::initResources(void)
{
	Ogre::ConfigFile cf;
	cf.load(m_resourcesCfg);

	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
	Ogre::String secName, typeName, archName;

	for(;seci.hasMoreElements();){
		secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap* settings = seci.getNext();
		Ogre::ConfigFile::SettingsMultiMap::iterator i;
		for(i=settings->begin(); i!=settings->end(); ++i){
			typeName = i->first;
			archName = i->second;
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName);
		}
	}

	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	// Init other needed resource groups
	Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("SkyX");
}

//================================================//

bool Base::init(OIS::KeyListener* pKeyListener, OIS::MouseListener* pMouseListener)
{
#ifdef _DEBUG
	m_resourcesCfg = "resources_d.cfg";
	m_pluginsCfg = "plugins_d.cfg";
#else
	m_resourcesCfg = "resources.cfg";
	m_pluginsCfg = "plugins.cfg";
#endif

	Ogre::LogManager* logMgr = new Ogre::LogManager();

	m_pLog = Ogre::LogManager::getSingleton().createLog("SparksOgreLogFile.log", true, true, false);
	m_pLog->setDebugOutputEnabled(true);

	// root
	m_pRoot = new Ogre::Root(m_pluginsCfg);
	//m_pRoot->setFrameSmoothingPeriod(4.0);

	m_pRoot->showConfigDialog();
	m_pRenderWindow = m_pRoot->initialise(true, "Engine2A");

	m_pViewport = m_pRenderWindow->addViewport(0);
	m_pViewport->setBackgroundColour(Ogre::ColourValue(1.0f, 1.0f, 1.0f));

	m_pViewport->setCamera(0);

	// window
	unsigned long hWnd = 0;
	OIS::ParamList paramList;
	m_pRenderWindow->getCustomAttribute("WINDOW", &hWnd);

	paramList.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));

	// input
	m_pInputMgr = OIS::InputManager::createInputSystem(paramList);

	m_pKeyboard = static_cast<OIS::Keyboard*>(m_pInputMgr->createInputObject(OIS::OISKeyboard, true));
	m_pMouse = static_cast<OIS::Mouse*>(m_pInputMgr->createInputObject(OIS::OISMouse, true));

	m_pMouse->getMouseState().height = m_pRenderWindow->getHeight();
	m_pMouse->getMouseState().width = m_pRenderWindow->getWidth();

	if(pKeyListener == 0){
		m_pKeyboard->setEventCallback(this);
	}
	else
		m_pKeyboard->setEventCallback(pKeyListener);

	if(pMouseListener == 0){
		m_pMouse->setEventCallback(this);
	}
	else
		m_pMouse->setEventCallback(pMouseListener);

	initResources();

	// GUI
	m_GUI_Platform = new MyGUI::OgrePlatform();
	m_GUI_Platform->initialise(m_pRenderWindow, m_pSceneMgr);
	m_GUI = new MyGUI::Gui();
	m_GUI->initialise();

	// FMOD
	FMOD_RESULT result = FMOD::System_Create(&m_soundSystem);
	FMOD_ERRCHECK(result);
	result = m_soundSystem->init(100, FMOD_INIT_NORMAL, 0);
	FMOD_ERRCHECK(result);

	// Timer
	m_pTimer = new Ogre::Timer();
	m_pTimer->reset();

	m_pRenderWindow->setActive(true);

	return true;
}

//================================================//

bool Base::keyPressed(const OIS::KeyEvent& arg)
{
	switch(arg.key){
	case OIS::KC_SYSRQ:
		m_pRenderWindow->writeContentsToTimestampedFile("SparkysFramework-Screenshot", ".jpg");
		break;

	case OIS::KC_G:
		m_injectGUI = !m_injectGUI; // DEBUG ONLY
		break;

	default:
		break;
	}

	if(m_injectGUI){
		MyGUI::InputManager::getInstance().injectKeyPress(MyGUI::KeyCode::Enum(arg.key), arg.text);
	}

	return true;
}

//================================================//

bool Base::keyReleased(const OIS::KeyEvent& arg)
{
	switch(arg.key){
	default:
		break;
	}

	if(m_injectGUI){
		MyGUI::InputManager::getInstance().injectKeyRelease(MyGUI::KeyCode::Enum(arg.key));
	}

	return true;
}

//================================================//

bool Base::mouseMoved(const OIS::MouseEvent& arg)
{
	if(m_injectGUI){
		MyGUI::InputManager::getInstance().injectMouseMove(arg.state.X.abs, arg.state.Y.abs, arg.state.Z.abs);
	}

	return true;
}

//================================================//

bool Base::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
	if(m_injectGUI){
		MyGUI::InputManager::getInstance().injectMousePress(arg.state.X.abs, arg.state.Y.abs, MyGUI::MouseButton::Enum(id));
	}

	return true;
}

//================================================//

bool Base::mouseReleased(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
	if(m_injectGUI){
		MyGUI::InputManager::getInstance().injectMouseRelease(arg.state.X.abs, arg.state.Y.abs, MyGUI::MouseButton::Enum(id));
	}

	return true;
}

//================================================//

void Base::windowResized(Ogre::RenderWindow* rw)
{
	unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState& ms = m_pMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

//================================================//

void Base::updateOgre(double timeSinceLastFrame)
{
	// update FMOD
	m_soundSystem->update();

	return;
}

//================================================//