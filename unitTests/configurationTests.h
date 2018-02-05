#define __MSXML_LIBRARY_DEFINED__ //solve XMLDocument conflict error
#include "unitTests.h"
#include "Configuration.h"

FILE* tmpCerr = NULL;

BOOST_AUTO_TEST_SUITE(configuration_test)

BOOST_AUTO_TEST_CASE(start) {
	cout << endl;
}

BOOST_AUTO_TEST_CASE(basic) {
	srand(time(NULL)); //random seed

	wstring backupFilename = TEXT("_back.xml");

	Configuration::backup(backupFilename);

	Configuration::toDefaults();
	
	Configuration::getStringParameter(to_string(rand())); //get non-existing parameter without path
	Configuration::getStringParameter(to_string(rand()), to_string(rand())); //get non-existing parameter with short path
	Configuration::getStringParameter(to_string(rand()), to_string(rand()) + " " + to_string(rand())); //get non-existing parameter with long path

	BOOST_CHECK_NO_THROW(
		BOOST_CHECK_EQUAL(Configuration::getScreenWidth() > 0, true);
		BOOST_CHECK_EQUAL(Configuration::getScreenHeight() > 0, true);
		BOOST_CHECK_EQUAL(Configuration::getIsFullscreen(), false);

		int rand1 = rand();
		Configuration::setScreenWidth(rand1);
		BOOST_CHECK_EQUAL(Configuration::getScreenWidth() , rand1);
		
		bool rand2 = rand()%2 == 0;
		Configuration::setIsFullscreen(rand2);
		BOOST_CHECK_EQUAL(Configuration::getIsFullscreen(), rand2);
		Configuration::setIsFullscreen(!rand2);
		BOOST_CHECK_EQUAL(Configuration::getIsFullscreen(), !rand2);


		//test non-existing string parameter default value
		BOOST_CHECK_EQUAL(Configuration::getStringParameter(to_string(rand()) + to_string(rand())), "");

		//test string parameters without nodes
		string rand3 = to_string(rand()) + to_string(rand());
		string rand4 = to_string(rand()) + to_string(rand());
		Configuration::setStringParameter(rand3, rand4);
		BOOST_CHECK_EQUAL(Configuration::getStringParameter(rand3), rand4);

		//test string parameters with nodes
		string rand5 = to_string(rand()) + to_string(rand());
		string rand6 = to_string(rand()) + to_string(rand());
		string rand7 = to_string(rand()) + " " + to_string(rand());
		Configuration::setStringParameter(rand5, rand6, rand7);
		BOOST_CHECK_EQUAL(Configuration::getStringParameter(rand5, rand7), rand6);


		//test non-existing int parameter default value
		BOOST_CHECK_EQUAL(Configuration::getIntParameter(to_string(rand()) + to_string(rand())), 0);

		//test int parameters without nodes
		string rand8 = to_string(rand()) + to_string(rand());
		int rand9 = rand();
		Configuration::setIntParameter(rand8, rand9);
		BOOST_CHECK_EQUAL(Configuration::getIntParameter(rand8), rand9);
		Configuration::setIntParameter(rand8, -rand9);
		BOOST_CHECK_EQUAL(Configuration::getIntParameter(rand8), -rand9);

		//test int parameters with nodes
		string rand10 = to_string(rand()) + to_string(rand());
		int rand11 = rand();
		string rand12 = to_string(rand()) + " " + to_string(rand());
		Configuration::setIntParameter(rand10, rand11, rand12);
		BOOST_CHECK_EQUAL(Configuration::getIntParameter(rand10, rand12), rand11);
		Configuration::setIntParameter(rand10, -rand11, rand12);
		BOOST_CHECK_EQUAL(Configuration::getIntParameter(rand10, rand12), -rand11);
	)

	BOOST_CHECK_NO_THROW (
		//test non-existing double parameter default value
		BOOST_CHECK_EQUAL(Configuration::getDoubleParameter(to_string(rand()) + to_string(rand())), 0.0);

		//test double parameters without nodes
		string rand13 = to_string(rand()) + to_string(rand());
		double rand14 = rand() / 1000;
		Configuration::setDoubleParameter(rand13, rand14);
		BOOST_CHECK_EQUAL(Configuration::getDoubleParameter(rand13), rand14);
		Configuration::setDoubleParameter(rand13, -rand14);
		BOOST_CHECK_EQUAL(Configuration::getDoubleParameter(rand13), -rand14);

		//test int double with nodes
		string rand15 = to_string(rand()) + to_string(rand());
		double rand16 = rand() / 1000;
		string rand17 = to_string(rand()) + " " + to_string(rand());
		Configuration::setDoubleParameter(rand15, rand16, rand17);
		BOOST_CHECK_EQUAL(Configuration::getDoubleParameter(rand15, rand17), rand16);
		Configuration::setDoubleParameter(rand15, -rand16, rand17);
		BOOST_CHECK_EQUAL(Configuration::getDoubleParameter(rand15, rand17), -rand16);

		
		//test non-existing bool parameter default value
		BOOST_CHECK_EQUAL(Configuration::getDoubleParameter(to_string(rand()) + to_string(rand())), false);

		//test bool parameters without nodes
		string rand18 = to_string(rand()) + to_string(rand());
		bool rand19 = rand() % 2 == 0;
		Configuration::setBoolParameter(rand18, rand19);
		BOOST_CHECK_EQUAL(Configuration::getBoolParameter(rand18), rand19);
		Configuration::setBoolParameter(rand18, !rand19);
		BOOST_CHECK_EQUAL(Configuration::getBoolParameter(rand18), !rand19);
		Configuration::setBoolParameter(rand18, 0);
		BOOST_CHECK_EQUAL(Configuration::getBoolParameter(rand18), false);
		Configuration::setBoolParameter(rand18, 1);
		BOOST_CHECK_EQUAL(Configuration::getBoolParameter(rand18), true);
		Configuration::setBoolParameter(rand18, -1);
		BOOST_CHECK_EQUAL(Configuration::getBoolParameter(rand18), true);
		Configuration::setBoolParameter(rand18, rand()+1);
		BOOST_CHECK_EQUAL(Configuration::getBoolParameter(rand18), true);

		//test int bool with nodes
		string rand20 = to_string(rand()) + to_string(rand());
		bool rand21 = rand() % 2 == 0;
		string rand22 = to_string(rand()) + " " + to_string(rand());
		Configuration::setBoolParameter(rand20, rand21, rand22);
		BOOST_CHECK_EQUAL(Configuration::getBoolParameter(rand20, rand22), rand21);
		Configuration::setBoolParameter(rand20, !rand21, rand22);
		BOOST_CHECK_EQUAL(Configuration::getBoolParameter(rand20, rand22), !rand21);
	)

	Configuration::restore(backupFilename);
	boost::filesystem::remove(backupFilename); //delete temporary backup file
}

BOOST_AUTO_TEST_CASE(backup_restore) {
	wstring backupFilename = TEXT("_back2.xml");

	std::wifstream* confFileStream = new std::wifstream(Configuration::getConfigFilePath());
	std::wstring confFileText((std::istreambuf_iterator<wchar_t>(*confFileStream)),
		std::istreambuf_iterator<wchar_t>());
	confFileStream->close();
	delete confFileStream;

	BOOST_CHECK_NO_THROW(
		Configuration::backup(backupFilename); //make backup

		std::wifstream* backupConfFileStream = new std::wifstream(backupFilename);
		std::wstring backupConfFileText((std::istreambuf_iterator<wchar_t>(*backupConfFileStream)),
			std::istreambuf_iterator<wchar_t>());
		backupConfFileStream->close();
		delete backupConfFileStream;

		BOOST_CHECK_EQUAL(confFileText == backupConfFileText, true); //compare contents of two files
		
		//make changes
		Configuration::setScreenWidth(Configuration::getScreenWidth()+1);
		Configuration::saveChanges();

		std::wifstream* confFileStream2 = new std::wifstream(Configuration::getConfigFilePath());
		std::wstring confFileText2((std::istreambuf_iterator<wchar_t>(*confFileStream2)),
			std::istreambuf_iterator<wchar_t>());
		confFileStream2->close();
		delete confFileStream2;

		BOOST_CHECK_EQUAL(confFileText2 == backupConfFileText, false);
		BOOST_CHECK_EQUAL(confFileText2 == confFileText, false);

		Configuration::restore(backupFilename); //restore from backup

		std::wifstream* confFileStream3 = new std::wifstream(Configuration::getConfigFilePath());
		std::wstring confFileText3((std::istreambuf_iterator<wchar_t>(*confFileStream3)),
			std::istreambuf_iterator<wchar_t>());
		confFileStream3->close();
		delete confFileStream3;

		BOOST_CHECK_EQUAL(confFileText3 == backupConfFileText, true);
		BOOST_CHECK_EQUAL(confFileText3 == confFileText, true);
		BOOST_CHECK_EQUAL(confFileText3 == confFileText2, false);
	)

	boost::filesystem::remove(backupFilename); //delete temporary backup file
}

BOOST_AUTO_TEST_CASE(end) {
	cout << endl;
}
	
BOOST_AUTO_TEST_SUITE_END( )