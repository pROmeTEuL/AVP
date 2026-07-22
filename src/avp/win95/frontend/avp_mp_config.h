#ifndef _included_AvP_MP_Config_h_
#define _included_AvP_MP_Config_h_

#include <string>

bool BuildLoadMPConfigMenu();
void LoadMultiplayerConfigurationByIndex(int index);
void LoadMultiplayerConfiguration(const std::string &name);
void SaveMultiplayerConfiguration(const std::string &name);
std::string GetMultiplayerConfigDescription(int index);
void DeleteMultiplayerConfigurationByIndex(int index);

bool BuildLoadIPAddressMenu();
void SaveIPAddress(const std::string &name, const std::string &address);
void LoadIPAddress(const std::string &name);

#define LOAD_NEW_MPCONFIG_ENTRIES (1)
#define SAVE_NEW_MPCONFIG_ENTRIES (1)

//list of all multiplayer level names as they appear in the menus

extern int NumCustomLevels;
extern int NumMultiplayerLevels;
extern int NumCoopLevels;

//list of all multiplayer level names as they appear in the menus
extern char **MultiplayerLevelNames;
extern char **CoopLevelNames;

extern void BuildMultiplayerLevelNameArray();

//returns local index of a custom level (if it is a custom level)
int GetCustomMultiplayerLevelIndex(char *name, int gameType);
//returns name of custom level (without stuff tacked on the end)
char *GetCustomMultiplayerLevelName(int index, int gameType);

int GetLocalMultiplayerLevelIndex(int index, char *customLevelName, int gameType);

#endif // _included_AvP_MP_Config_h_
