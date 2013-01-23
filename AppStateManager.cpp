//================================================//

#include "AppStateManager.hpp"

//================================================//

AppStateManager::AppStateManager(void)
{
	m_bShutdown = false;
}

//================================================//

AppStateManager::~AppStateManager(void)
{
	state_info_t si;

	for(;!m_ActiveStateStack.empty();){
		m_ActiveStateStack.back()->exit();
		m_ActiveStateStack.pop_back();
	}

	for(;m_States.empty();){
		si = m_States.back();
		si.state->destroy();
		m_States.pop_back();
	}
}

//================================================//

void AppStateManager::manageAppState(Ogre::String stateName, AppState* state)
{
	try{
		state_info_t new_state_info;
		new_state_info.name = stateName;
		new_state_info.state = state;
		m_States.push_back(new_state_info);
	} catch(std::exception& e){
		delete state;
		throw Ogre::Exception(Ogre::Exception::ERR_INTERNAL_ERROR, "Error while trying to manage a new app state\n" + Ogre::String(e.what()), "AppStateManager.cpp (41)");
	}
}

//================================================//

AppState* AppStateManager::findByName(Ogre::String stateName)
{
	std::vector<state_info_t>::iterator itr;

	for(itr=m_States.begin(); itr!=m_States.end(); ++itr){
		if(itr->name == stateName){
			return itr->state;
		}
	}

	return 0;
}

//================================================//

/* The main application loop */
void AppStateManager::start(AppState* state)
{
	changeAppState(state);

	int timeSinceLastFrame = 1;
	int startTime = 0;

	for(;!m_bShutdown;){
		if(Base::getSingletonPtr()->m_pRenderWindow->isClosed())
			m_bShutdown = true;

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		Ogre::WindowEventUtilities::messagePump();
#endif

		if(Base::getSingletonPtr()->m_pRenderWindow->isActive()){
			startTime = Base::getSingletonPtr()->m_pTimer->getMillisecondsCPU();

			Base::getSingletonPtr()->m_pKeyboard->capture();
			Base::getSingletonPtr()->m_pMouse->capture();

			// Update the active state
			m_ActiveStateStack.back()->update(timeSinceLastFrame); 

			// General Ogre updates
			Base::getSingletonPtr()->updateOgre(timeSinceLastFrame);

			// Render the frame
			Base::getSingletonPtr()->m_pRoot->renderOneFrame();

			timeSinceLastFrame = Base::getSingletonPtr()->m_pTimer->getMillisecondsCPU() - startTime;
		}
		else{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			Sleep(1000);
#else
			Sleep(1);
#endif
		}
	}

	Base::getSingletonPtr()->m_pLog->logMessage("Shutting down...");
}

//================================================//

void AppStateManager::changeAppState(AppState* state)
{
	if(!m_ActiveStateStack.empty()){
		m_ActiveStateStack.back()->exit();
		m_ActiveStateStack.pop_back();
	}

	m_ActiveStateStack.push_back(state);
	init(state);
	m_ActiveStateStack.back()->enter();
}

//================================================//

bool AppStateManager::pushAppState(AppState* state)
{
	if(!m_ActiveStateStack.empty()){
		if(!m_ActiveStateStack.back()->pause())
			return false;
	}

	m_ActiveStateStack.push_back(state);
	init(state);
	m_ActiveStateStack.back()->enter();

	return true;
}

//================================================//

void AppStateManager::popAppState(void)
{
	if(!m_ActiveStateStack.empty()){
		m_ActiveStateStack.back()->exit();
		m_ActiveStateStack.pop_back();
	}

	if(!m_ActiveStateStack.empty()){
		init(m_ActiveStateStack.back());
		m_ActiveStateStack.back()->resume();
	}
	else{
		shutdown();
	}
}

//================================================//

void AppStateManager::pauseAppState(void)
{
	if(!m_ActiveStateStack.empty()){
		m_ActiveStateStack.back()->pause();
	}

	if(m_ActiveStateStack.size() > 2){
		init(m_ActiveStateStack.at(m_ActiveStateStack.size() - 2));
		m_ActiveStateStack.at(m_ActiveStateStack.size() - 2)->resume();
	}
}

//================================================//

void AppStateManager::shutdown(void)
{
	m_bShutdown = true;
}

//================================================//

void AppStateManager::init(AppState* state)
{
	Base::getSingletonPtr()->m_pKeyboard->setEventCallback(state);
	Base::getSingletonPtr()->m_pMouse->setEventCallback(state);

	Base::getSingletonPtr()->m_pRenderWindow->resetStatistics();
}

//================================================//