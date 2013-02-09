//================================================//

#include "App.hpp"

//================================================//

App::App(void) : 
	m_pAppStateManager(0)
{

}

//================================================//

App::~App(void)
{
	delete m_pAppStateManager;
	delete Base::getSingletonPtr();
}

//================================================//

void App::init(void)
{
	// Declare the Base singleton
	new Base(); 
	if(!Base::getSingletonPtr()->init(0, 0)) 
		return;

	// Declare the Settings singleton
	new Settings();

	// Declare shared data singleton
	new SharedData();
	

#ifdef _DEBUG
	if(AllocConsole()){
		freopen("CONOUT$", "w", stdout);
		SetConsoleTitle("Sparks Ogre Framework Debug Console");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
	}
#endif

	// Seed rand 
	srand(time(0));

	Base::getSingletonPtr()->m_pLog->logMessage("App initialized!");

	m_pAppStateManager = new AppStateManager();

	// create all states
	MenuState::create(m_pAppStateManager, "MenuState");
	GameState::create(m_pAppStateManager, "GameState");

	// start the first state
	m_pAppStateManager->start(m_pAppStateManager->findByName("MenuState"));
}

//================================================//