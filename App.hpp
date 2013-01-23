//================================================//

#ifndef __APP_HPP__
#define __APP_HPP__

//================================================//

#include "AppStateManager.hpp"

#include "MenuState.hpp"
#include "GameState.hpp"

//================================================//

class App
{
public:
	App(void);
	~App(void);

	void init(void);

private:
	AppStateManager*	m_pAppStateManager; // the big dog
};

//================================================//

#endif

//================================================//