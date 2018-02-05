#pragma once
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <boost/predef.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <tinyxml2.h>

#if defined(BOOST_OS_WINDOWS)
	#include <Windows.h>
	#include <Shlobj.h>
	#include <Shlwapi.h>
	#ifndef CONFIGURATION_FILE_PATH
		#define CONFIGURATION_FILE_PATH L"My Games\\Timentris\\configuration.xml"
	#endif
#elif defined(BOOST_OS_LINUX)
//
#elif defined(BOOST_OS_MACOS)
//
#endif

#include "exceptions.h"

using namespace std;
//using namespace tinyxml2;

//define each default parameter
#define DEFAULT_screenWidth 1024
#define DEFAULT_screenHeight 768
#define DEFAULT_isFullscreen false
#define DEFAULT_volumeMaster 100
#define DEFAULT_volumeMusic 100
#define DEFAULT_volumeSFX 100
#define DEFAULT_gamePreset_name "Default"
#define DEFAULT_gamePreset_difficultyLevel 0
#define DEFAULT_gamePreset_cupDimX 10
#define DEFAULT_gamePreset_cupDimY 10
#define DEFAULT_gamePreset_cupDimW 10
#define DEFAULT_gamePreset_cupDimZ 20 //height
#define DEFAULT_gamePreset_level0Score 0
#define DEFAULT_gamePreset_level0Speed 240
#define DEFAULT_gamePreset_level1Score 100000
#define DEFAULT_gamePreset_level1Speed 160
#define DEFAULT_gamePreset_level2Score 1000000
#define DEFAULT_gamePreset_level2Speed 107
#define DEFAULT_gamePreset_level3Score 2000000
#define DEFAULT_gamePreset_level3Speed 71
#define DEFAULT_gamePreset_level4Score 4000000
#define DEFAULT_gamePreset_level4Speed 47
#define DEFAULT_gamePreset_level5Score 8000000
#define DEFAULT_gamePreset_level5Speed 32
#define DEFAULT_gamePreset_level6Score 12000000
#define DEFAULT_gamePreset_level6Speed 21
#define DEFAULT_gamePreset_level7Score 18000000
#define DEFAULT_gamePreset_level7Speed 14
#define DEFAULT_gamePreset_level8Score 30000000
#define DEFAULT_gamePreset_level8Speed 9
#define DEFAULT_gamePreset_level9Score 50000000
#define DEFAULT_gamePreset_level9Speed 6
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define DEFAULT_CONFIGURATION \
"<?xml version=\"1.1\" encoding=\"UTF-8\" ?>\n" \
"<configuration>\n" \
"	<parameter name=\"screenWidth\" value=\""##STR(DEFAULT_screenWidth)##"\" />\n" \
"	<parameter name=\"screenHeight\" value=\""##STR(DEFAULT_screenHeight)##"\" />\n" \
"	<parameter name=\"isFullscreen\" value=\""##STR(DEFAULT_isFullscreen)##"\" />\n" \
"	<parameter name=\"volumeMaster\" value=\""##STR(DEFAULT_volumeMaster)##"\" />\n" \
"	<parameter name=\"volumeMusic\" value=\""##STR(DEFAULT_volumeMusic)##"\" />\n" \
"	<parameter name=\"volumeSFX\" value=\""##STR(DEFAULT_volumeSFX)##"\" />\n" \
"	<gamePresets>\n" \
"		<default>\n" \
"			<parameter name=\"name\" value=\""##DEFAULT_gamePreset_name##"\" comment=\"Game preset player-friendly name\" />\n" \
"			<parameter name=\"difficultyLevel\" value=\""##STR(DEFAULT_gamePreset_difficultyLevel)##"\" comment=\"Should be used to load Pieces of different difficulty levels\" />\n" \
"			<parameter name=\"cupDimX\" value=\""##STR(DEFAULT_gamePreset_cupDimX)##"\" />\n" \
"			<parameter name=\"cupDimY\" value=\""##STR(DEFAULT_gamePreset_cupDimY)##"\" />\n" \
"			<parameter name=\"cupDimW\" value=\""##STR(DEFAULT_gamePreset_cupDimW)##"\" />\n" \
"			<parameter name=\"cupDimZ\" value=\""##STR(DEFAULT_gamePreset_cupDimZ)##"\" comment=\"Height\" />\n" \
"			<parameter name=\"level0Score\" value=\""##STR(DEFAULT_gamePreset_level0Score)##"\" comment=\"How much score player should accumulate to reach this level\" />\n" \
"			<parameter name=\"level0Speed\" value=\""##STR(DEFAULT_gamePreset_level0Speed)##"\" comment=\"Number of game frames it takes for a piece to descend 1 cell down\" />\n" \
"			<parameter name=\"level1Score\" value=\""##STR(DEFAULT_gamePreset_level1Score)##"\" />\n" \
"			<parameter name=\"level1Speed\" value=\""##STR(DEFAULT_gamePreset_level1Speed)##"\" />\n" \
"			<parameter name=\"level2Score\" value=\""##STR(DEFAULT_gamePreset_level2Score)##"\" />\n" \
"			<parameter name=\"level2Speed\" value=\""##STR(DEFAULT_gamePreset_level2Speed)##"\" />\n" \
"			<parameter name=\"level3Score\" value=\""##STR(DEFAULT_gamePreset_level3Score)##"\" />\n" \
"			<parameter name=\"level3Speed\" value=\""##STR(DEFAULT_gamePreset_level3Speed)##"\" />\n" \
"			<parameter name=\"level4Score\" value=\""##STR(DEFAULT_gamePreset_level4Score)##"\" />\n" \
"			<parameter name=\"level4Speed\" value=\""##STR(DEFAULT_gamePreset_level4Speed)##"\" />\n" \
"			<parameter name=\"level5Score\" value=\""##STR(DEFAULT_gamePreset_level5Score)##"\" />\n" \
"			<parameter name=\"level5Speed\" value=\""##STR(DEFAULT_gamePreset_level5Speed)##"\" />\n" \
"			<parameter name=\"level6Score\" value=\""##STR(DEFAULT_gamePreset_level6Score)##"\" />\n" \
"			<parameter name=\"level6Speed\" value=\""##STR(DEFAULT_gamePreset_level6Speed)##"\" />\n" \
"			<parameter name=\"level7Score\" value=\""##STR(DEFAULT_gamePreset_level7Score)##"\" />\n" \
"			<parameter name=\"level7Speed\" value=\""##STR(DEFAULT_gamePreset_level7Speed)##"\" />\n" \
"			<parameter name=\"level8Score\" value=\""##STR(DEFAULT_gamePreset_level8Score)##"\" />\n" \
"			<parameter name=\"level8Speed\" value=\""##STR(DEFAULT_gamePreset_level8Speed)##"\" />\n" \
"			<parameter name=\"level9Score\" value=\""##STR(DEFAULT_gamePreset_level9Score)##"\" />\n" \
"			<parameter name=\"level9Speed\" value=\""##STR(DEFAULT_gamePreset_level9Speed)##"\" />\n" \
"		</default>\n" \
"	</gamePresets>\n" \
"</configuration>"

class Configuration {
public:
	struct GamePreset {
		string name;
		unsigned int difficultyLevel;
		unsigned int cupDimX;
		unsigned int cupDimY;
		unsigned int cupDimW;
		unsigned int cupDimZ;
		map<unsigned int, unsigned int> speedAtLevels;
		map<unsigned int, unsigned int> scoreForLevels;
	};
private:
	//Private singleton
	static Configuration& getInstance() {
		static Configuration instance;
		return instance;
	}
	Configuration(void);
	~Configuration(void);
	Configuration(const Configuration&);  
	Configuration& operator= (Configuration&);

	wstring _configFilePath;
	tinyxml2::XMLDocument _confXmlDocument;

	void save(); //save to file
public:
	static void toDefaults(bool saveToFile = true); //set all values to it's defaults
	static void saveChanges();

	static wstring getConfigFilePath();
	//backup configuration to path given in filename variable
	static void backup(wstring toFilename);
	//restore configuration from path given in filename variable
	static void restore(wstring fromFilename);

	static string getStringParameter(const string& name, const string& pathToParent = "");
	static int getIntParameter(const string& name, const string& pathToParent = "");
	static double getDoubleParameter(const string& name, const string& pathToParent = "");
	static bool getBoolParameter(const string& name, const string& pathToParent = "");

	static void setStringParameter(const string& name, const string& paramenter, const string& pathToParent = "");
	static void setIntParameter(const string& name, const int paramenter, const string& pathToParent = "");
	static void setDoubleParameter(const string& name, const double paramenter, const string& pathToParent = "");
	static void setBoolParameter(const string& name, const bool paramenter, const string& pathToParent = "");

	static unsigned int getScreenWidth();
	static unsigned int getScreenHeight();
	static void setScreenWidth(const unsigned int width);
	static void setScreenHeight(const unsigned int height);
	static bool getIsFullscreen();
	static void setIsFullscreen(const bool fullscreen);

	static GamePreset getGamePreset(const string& presetName = "default");
	static void setGamePreset(const string& presetName, const GamePreset& gp);
};
