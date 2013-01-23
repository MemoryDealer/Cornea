//================================================//

#ifndef __APPSTATEMANAGER_HPP__
#define __APPSTATEMANAGER_HPP__

//================================================//

#include "AppState.hpp"

//================================================//

class AppStateManager : public AppStateListener
{
public:
	typedef struct{
		Ogre::String name;
		AppState* state;
	} state_info_t;

	AppStateManager(void);
	~AppStateManager(void);

	void manageAppState(Ogre::String stateName, AppState* state);

	AppState* findByName(Ogre::String stateName);

	void start(AppState* state);
	void changeAppState(AppState* state);
	bool pushAppState(AppState* state);
	void popAppState(void);
	void pauseAppState(void);
	void shutdown(void);

protected:
	void init(AppState* state);

	std::vector<AppState*>		m_ActiveStateStack;
	std::vector<state_info_t>	m_States;
	bool						m_bShutdown;
};

//================================================//

#endif

//================================================//