#include "Configuration.h"

Configuration::Configuration(void) {
#if defined(BOOST_OS_WINDOWS)
	WCHAR path[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL,
		SHGFP_TYPE_CURRENT, path))) {
			PathAppend(path, CONFIGURATION_FILE_PATH);
			_configFilePath += wstring(path);
	}
#elif defined(BOOST_OS_LINUX)
	//TODO
#elif defined(BOOST_OS_MACOS)
	//TODO
#endif

	//If no coniguration file exists
	if ( !boost::filesystem::exists(_configFilePath) ) {
		//Create default configuration file
		boost::filesystem::create_directories( //Create directories (tree) if don't exist
			boost::filesystem::path(_configFilePath).parent_path()
			);
		std::wofstream confFile(_configFilePath);
		if(!confFile.is_open()) {
			cerr << __FILE__ << ":" << __LINE__ << " Can't create configuration file" << endl;
			throw new ConfigurationExceptions::CantOpenConfigurationFile();
		}
		confFile << DEFAULT_CONFIGURATION << endl;
		confFile.close();
	}

	//Load configuration document
	std::ifstream confFile(_configFilePath);
	if (!confFile.is_open()) {
		cerr << __FILE__ << ":" << __LINE__ << " Can't load configuration file" << endl;
		throw new ConfigurationExceptions::CantOpenConfigurationFile();
	}
	std::string confText((std::istreambuf_iterator<char>(confFile)),
		std::istreambuf_iterator<char>());

	_confXmlDocument.Parse(confText.c_str());
	if (_confXmlDocument.Error()) {
		cerr << __FILE__ << ":" << __LINE__ << " Configuration file format error: " << _confXmlDocument.GetErrorStr1() << " - " << _confXmlDocument.GetErrorStr2() << endl;
		throw new ConfigurationExceptions::ConfigurationFileFormatError();
	}
}

Configuration::~Configuration(void) {
	//save on exit
	save();
}

void Configuration::save() {
	//save XML config to tmp file and read it to wstring variable
	//create tmp file which will not sieze to exist after closing it's handler
	boost::filesystem::path tmpFilePath = boost::filesystem::temp_directory_path();
	tmpFilePath += "%%%%%%%%%%%%%%%%%%%%";
	tmpFilePath = unique_path(tmpFilePath);
	//open tmp file for writing
	FILE* tmpF = fopen(tmpFilePath.generic_string().c_str(), "w");
	//write xml to this file
	tinyxml2::XMLPrinter* xmlPrinter = new tinyxml2::XMLPrinter(tmpF);
	_confXmlDocument.Print(xmlPrinter);
	//close tmp file
	fclose(tmpF);
	//delete XMLPrinter from memory
	delete xmlPrinter;
	//now open tmp file for reading
	tmpF = fopen(tmpFilePath.generic_string().c_str(), "r");
	std::wifstream tmpConfFile(tmpF);
	if (!tmpConfFile.is_open()) {
		cerr << __FILE__ << ":" << __LINE__ << " Can't save configuration file (failed to read/write in/from tmpfile)" << endl;
		throw new ConfigurationExceptions::CantSaveConfigurationFile();
		return;
	}
	//read text from tmp file to wstring ariable
	std::wstring tmpConfText((std::istreambuf_iterator<wchar_t>(tmpConfFile)),
		std::istreambuf_iterator<wchar_t>());
	fclose(tmpF);
	tmpConfFile.close();

	//save wstring variable to configuration file
	std::wofstream confFile(_configFilePath);
	if (!confFile.is_open()) {
		cerr << __FILE__ << ":" << __LINE__ << " Can't save configuration file" << endl;
		throw new ConfigurationExceptions::CantSaveConfigurationFile();
		return;
	}
	confFile << tmpConfText;
	confFile.close();

	//delete temp file
	remove(tmpFilePath.generic_string().c_str());
}


void Configuration::toDefaults(bool saveToFile) {
	getInstance()._confXmlDocument.Parse(DEFAULT_CONFIGURATION);
	if (saveToFile) getInstance().save();
}

void Configuration::saveChanges() {
	getInstance().save();
}


wstring Configuration::getConfigFilePath() {
	return getInstance()._configFilePath;
}

void Configuration::backup(wstring toFilename) {
	//save
	getInstance().save();

	//backup to filename
	boost::filesystem::copy_file(
		getInstance()._configFilePath,
		toFilename,
		boost::filesystem::copy_option::overwrite_if_exists);

	//check
	std::wifstream confFile(toFilename);
	if(!confFile.is_open()) {
		wcerr << __FILE__ << ":" << __LINE__ << " Failed to backup configuration file to " << toFilename << endl;
		throw new ConfigurationExceptions::CantSaveConfigurationFile();
	}
	confFile.close();
}

void Configuration::restore(wstring fromFilename) {
	//load backup to string
	std::ifstream confFile(fromFilename);
	if (!confFile.is_open()) {
		wcerr << __FILE__ << ":" << __LINE__ << " Can't load backup configuration file from " << fromFilename << endl;
		throw new ConfigurationExceptions::CantOpenConfigurationFile();
	}
	std::string confText((std::istreambuf_iterator<char>(confFile)),
		std::istreambuf_iterator<char>());

	//parse
	getInstance()._confXmlDocument.Parse(confText.c_str());
	if (getInstance()._confXmlDocument.Error()) {
		cerr << __FILE__ << ":" << __LINE__ << " Error restoring configuration file from backup - format error - " << getInstance()._confXmlDocument.GetErrorStr1() << " - " << getInstance()._confXmlDocument.GetErrorStr2() << endl;
		throw new ConfigurationExceptions::ConfigurationFileFormatError();
	}

	//save
	getInstance().save();
}


string Configuration::getStringParameter(const string& name, const string& pathToParent) {
	//parse path to parrent
	vector<string> path;
	boost::split(path, pathToParent, boost::is_any_of(" "));
	tinyxml2::XMLElement *levelElement = getInstance()._confXmlDocument.RootElement();
	BOOST_FOREACH(string& pathElem, path) {
		boost::algorithm::trim(pathElem);
		if (pathElem.length() > 0)
			levelElement = levelElement->FirstChildElement(pathElem.c_str());
		if (!levelElement) break;
	}
	if (!levelElement) {
		cerr << __FILE__ << ":" << __LINE__ << " Can't find path '" << pathToParent << "' in configuration file" << endl;
		return "";
	}

	for (tinyxml2::XMLElement* child = levelElement->FirstChildElement("parameter"); child != NULL; child = child->NextSiblingElement("parameter")) {
		if (string(child->Attribute("name")) == name) {
			return string(child->Attribute("value"));
			break;
		}
	}
	cerr << __FILE__ << ":" << __LINE__ << " Can't find attribute '(" << pathToParent << ") " << name << "' in configuration file while getting it's value" << endl;
	return "";
}

int Configuration::getIntParameter(const string& name, const string& pathToParent) {
	string value = getStringParameter(name, pathToParent);
	//cast bool to int
	if (value == "false")
		return 0;
	if (value == "true")
		return 1;
	return atoi(value.c_str());
}

double Configuration::getDoubleParameter(const string& name, const string& pathToParent) {
	string value = getStringParameter(name, pathToParent);
	return atof(value.c_str());
}

bool Configuration::getBoolParameter(const string& name, const string& pathToParent) {
	return (bool)getIntParameter(name, pathToParent);
}


void Configuration::setStringParameter(const string& name, const string& paramenter, const string& pathToParent) {
	vector<string> path;
	boost::split(path, pathToParent, boost::is_any_of(" "));
	tinyxml2::XMLElement *levelElement = getInstance()._confXmlDocument.RootElement();
	BOOST_FOREACH(string& pathElem, path) {
		boost::algorithm::trim(pathElem);
		if (pathElem.length() > 0) {
			tinyxml2::XMLElement *tmpElement = levelElement->FirstChildElement(pathElem.c_str());
			if (!tmpElement) { //add new child node
				cerr << __FILE__ << ":" << __LINE__ << " Can't find node " << pathElem << " at path '" << pathToParent << "' in configuration file; creating one" << endl;
				tinyxml2::XMLElement *node = getInstance()._confXmlDocument.NewElement(pathElem.c_str());
				levelElement->LinkEndChild(node);
			}
			levelElement = levelElement->FirstChildElement(pathElem.c_str());
		}
	}
	if (!levelElement) {
		cerr << __FILE__ << ":" << __LINE__ << " Can't find path '" << pathToParent << "' in configuration file" << endl;
		return;
	}

	tinyxml2::XMLElement* child = NULL;
	for (child = levelElement->FirstChildElement("parameter"); child != NULL; child = child->NextSiblingElement("parameter")) {
		if (string(child->Attribute("name")) == name) {
			break;
		}
	}
	if (!child) { //nothing found => add new parameter
		cerr << __FILE__ << ":" << __LINE__ << " Can't find attribute '(" << pathToParent << ") " << name << "' in configuration file while setting it's value to '" << paramenter << "'; creating one" << endl;
		child = getInstance()._confXmlDocument.NewElement("parameter");
		child->SetAttribute("name", name.c_str());
		levelElement->LinkEndChild(child);
	}
	//set value
	child->SetAttribute("value", paramenter.c_str());

	getInstance().save();
}

void Configuration::setIntParameter(const string& name, const int paramenter, const string& pathToParent) {
	setStringParameter(name, std::to_string(paramenter), pathToParent);
}

void Configuration::setDoubleParameter(const string& name, const double paramenter, const string& pathToParent) {
	setStringParameter(name, std::to_string(paramenter), pathToParent);
}

void Configuration::setBoolParameter(const string& name, const bool paramenter, const string& pathToParent) {
	setStringParameter(name, !paramenter ? "false" : "true", pathToParent);
}



unsigned int Configuration::getScreenWidth() {
	int v = getIntParameter("screenWidth");
	if (v > 0) {
		return v;
	} else {
		setScreenWidth(DEFAULT_screenWidth);
		return DEFAULT_screenWidth;
	}
}

unsigned int Configuration::getScreenHeight() {
	int h = getIntParameter("screenHeight");
	if (h > 0) {
		return h;
	} else {
		setScreenWidth(DEFAULT_screenHeight);
		return DEFAULT_screenHeight;
	}
}

void Configuration::setScreenWidth(const unsigned int width) {
	setIntParameter("screenWidth", width);
}

void Configuration::setScreenHeight(const unsigned int height) {
	setIntParameter("screenHeight", height);
}

bool Configuration::getIsFullscreen() {
	return getBoolParameter("isFullscreen");
}

void Configuration::setIsFullscreen(const bool fullscreen) {
	setBoolParameter("isFullscreen", fullscreen);
}

Configuration::GamePreset Configuration::getGamePreset(const string& presetName) {
	string path = "gamePresets " + presetName;
	GamePreset gp;
	gp.name = getStringParameter("name", path);
	gp.difficultyLevel = getIntParameter("difficultyLevel", path);
	gp.cupDimX = getIntParameter("cupDimX", path);
	gp.cupDimY = getIntParameter("cupDimY", path);
	gp.cupDimW = getIntParameter("cupDimW", path);
	gp.cupDimZ = getIntParameter("cupDimZ", path);

	for(unsigned int i = 0; ; i++) {
		unsigned int level = i;
		int speed = getIntParameter("level"+to_string(level)+"Score", path);
		int score = getIntParameter("level"+to_string(level)+"Score", path);
		if (speed == 0 || score == 0 && level > 0) {
			break;
		}
		gp.speedAtLevels[level] = speed;
		gp.scoreForLevels[level] = score;
	}
	
	//Check for wrong data
	if (
		gp.name == ""
		||
		gp.difficultyLevel < 0
		||
		gp.cupDimX <= 0
		||
		gp.cupDimY <= 0
		||
		gp.cupDimW <= 0
		||
		gp.cupDimZ <= 0
		||
		gp.scoreForLevels.size() == 0
		||
		gp.scoreForLevels[0] > 0
		||
		gp.speedAtLevels.size() == 0
		||
		gp.speedAtLevels[0] == 0
		) {
			cerr << __FILE__ << ":" << __LINE__ << " Wrong game presets for preset '" << presetName << "' in configuration file; resetting to default preset" << endl;

			//set default parameters
			gp.name = DEFAULT_gamePreset_name;
			gp.difficultyLevel = DEFAULT_gamePreset_difficultyLevel;
			gp.cupDimX = DEFAULT_gamePreset_cupDimX;
			gp.cupDimY = DEFAULT_gamePreset_cupDimY;
			gp.cupDimW = DEFAULT_gamePreset_cupDimW;
			gp.cupDimZ = DEFAULT_gamePreset_cupDimZ;
			gp.speedAtLevels[0] = DEFAULT_gamePreset_level0Speed;
			gp.scoreForLevels[0] = DEFAULT_gamePreset_level0Score;
			gp.speedAtLevels[1] = DEFAULT_gamePreset_level1Speed;
			gp.scoreForLevels[1] = DEFAULT_gamePreset_level1Score;
			gp.speedAtLevels[2] = DEFAULT_gamePreset_level2Speed;
			gp.scoreForLevels[2] = DEFAULT_gamePreset_level2Score;
			gp.speedAtLevels[3] = DEFAULT_gamePreset_level3Speed;
			gp.scoreForLevels[3] = DEFAULT_gamePreset_level3Score;
			gp.speedAtLevels[4] = DEFAULT_gamePreset_level4Speed;
			gp.scoreForLevels[4] = DEFAULT_gamePreset_level4Score;
			gp.speedAtLevels[5] = DEFAULT_gamePreset_level5Speed;
			gp.scoreForLevels[5] = DEFAULT_gamePreset_level5Score;
			gp.speedAtLevels[6] = DEFAULT_gamePreset_level6Speed;
			gp.scoreForLevels[6] = DEFAULT_gamePreset_level6Score;
			gp.speedAtLevels[7] = DEFAULT_gamePreset_level7Speed;
			gp.scoreForLevels[7] = DEFAULT_gamePreset_level7Score;
			gp.speedAtLevels[8] = DEFAULT_gamePreset_level8Speed;
			gp.scoreForLevels[8] = DEFAULT_gamePreset_level8Score;
			gp.speedAtLevels[9] = DEFAULT_gamePreset_level9Speed;
			gp.scoreForLevels[9] = DEFAULT_gamePreset_level9Score;

			//save
			setGamePreset(presetName, gp);
	}

	return gp;
}

void Configuration::setGamePreset(const string& presetName, const Configuration::GamePreset& gp) {
	string path = "gamePresets " + presetName;
	Configuration::GamePreset _gp(gp);

	setStringParameter("name", _gp.name, path);
	setIntParameter("difficultyLevel", _gp.difficultyLevel, path);
	setIntParameter("cupDimX", _gp.cupDimX, path);
	setIntParameter("cupDimY", _gp.cupDimY, path);
	setIntParameter("cupDimW", _gp.cupDimW, path);
	setIntParameter("cupDimZ", _gp.cupDimZ, path);
	for (auto &p : _gp.speedAtLevels) {
		setIntParameter("level"+to_string(p.first)+"Speed", p.second, path);
	}
	for (auto &p : _gp.speedAtLevels) {
		setIntParameter("level"+to_string(p.first)+"Score", p.second, path);
	}
}