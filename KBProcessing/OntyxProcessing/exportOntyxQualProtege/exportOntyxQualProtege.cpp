 

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <ctime>

#include "OntyxKb.h"
#include "OntyxProperty.h"

using namespace std;

string getDayStamp (void);
string characterEscape (const string & inString);


int main (int argc, char * argv[])
{
	OntyxKb tdeKb, compareKb;		// input/output kb
	clock_t tstart, tend;

	vector<string> commLine;
	for ( int i = 0; i < argc; i++)
		commLine.push_back(*argv++);

	if( commLine.size() != 2 ) {
		cerr << "Error:  incorrect number of command line parameters." << endl
			<< "\nUsage:\n\texportOwlProtege tde.xml" << endl 
			<< endl << "Requires ancillary header.owl, and headerCode.owl files."  << endl;
		exit(0);
		}

	tstart = clock();

	tdeKb.readOntyxXMLFile( commLine[1] );	cerr << "Read \"" << commLine[1].c_str() << "\"" << endl;
	cerr << "TDE file read -----------------------------------------------------------------" << endl;


	// ************* CONVERT QUALIFIED PROPERTIES TO XML *************

	map<long, OntyxConcept>::iterator posCon;


	multimap<long, OntyxProperty> conPropMap;
	multimap<long, OntyxProperty>::iterator pConProp;
	string propValue, newValue;
	string propName;
	vector<OntyxProperty> propVecDel;
	vector<OntyxProperty> propVecAdd;
	vector<OntyxProperty>::iterator pPropVec;
	bool good;


	
	cerr << "Converting qualified annotations to xml" << endl;
	for( posCon = tdeKb.getFirstConceptIterator(); 
				posCon != tdeKb.getEndConceptIterator(); ++posCon) {
		good = false;
		propVecDel.clear();
		propVecAdd.clear();

		conPropMap = posCon->second.getProperties();
		for( pConProp = conPropMap.begin(); pConProp != conPropMap.end(); ++pConProp) {
			if( pConProp->second.hasQualifier() ) {
				good = true;
				propVecDel.push_back(pConProp->second);		// make a note of the property to later delete
				propName = pConProp->second.getName();
				propValue = pConProp->second.getValue();
				newValue = "<value>" + characterEscape(propValue) + "</value>";

				vector<OntyxQualifier> quals = pConProp->second.getQualifiers();
				vector<OntyxQualifier>::const_iterator pQuals = quals.begin();
				for( ; pQuals != quals.end(); ++pQuals ) {

					newValue = newValue + "<" + pQuals->getName() + ">" + characterEscape(pQuals->getValue()) + "</" + pQuals->getName() + ">";
					}
				OntyxProperty tmpProp(propName, newValue);
				propVecAdd.push_back(tmpProp);
				}
			}
		if( good ) {
			pPropVec = propVecDel.begin();
			for( ; pPropVec != propVecDel.end(); ++pPropVec ) {
				posCon->second.deleteProperty( *pPropVec );
				}
			pPropVec = propVecAdd.begin();
			for( ; pPropVec != propVecAdd.end(); ++pPropVec ) {
				posCon->second.addProperty( *pPropVec );
				}
			}
		}


	// ************* END CONVERT QUALIFIED PROPERTIES TO XML *************



	string filename;
	string daystamp = getDayStamp();




	// ************* WRITE BY NAME OWL FILE *************
	filename = commLine[1] + "-ByName-" + daystamp + ".owl";
	cerr << "Writing output OWL file '" << filename << "', properties with role names are not exported." << endl;
	tdeKb.writeOWLFileByName (filename, "header.owl", true); 
	// if noProps is true, properties with names derived from roles are not exported
	// ************* END WRITE OWL FILE *************

/*
	// ************* WRITE BY CODE OWL FILE *************
	filename = commLine[1] + "-ByCode-" + daystamp + ".owl";
	cerr << "Writing output OWL file '" << filename << "', properties with role names are not exported." << endl;
	tdeKb.writeOWLFile (filename, "headerCode.owl", true); 
	// if noProps is true, properties with names derived from roles are not exported
	// ************* END WRITE OWL FILE *************
*/



	tend = clock();
	cerr << "Execution took " << (tend-tstart)/1000.0 << " seconds" << endl;

	return 0;
}


string getDayStamp (void)
{
	char sumFile[10] = {0};
	struct tm when;
	time_t now;   
	char *p;
	char tempS[20] = {0}, monS[3] = {0}, dayS[3] = {0}, yearS[5] = {0};

	time( &now );
	when = *localtime( &now );
	sprintf (yearS, "%d", when.tm_year);	// years since 1900
	if (strlen(yearS) > 2) {		// use only last two digits of year
		p = &yearS[strlen(yearS)-2];
		strcpy(tempS, p);
		strcpy(yearS, tempS);
		}
	sprintf (monS, "%d", when.tm_mon + 1);
	sprintf (dayS, "%d", when.tm_mday);
	if (strlen(monS) == 1) {		// use month with two digits
		strcpy(tempS, "0");
		strcat(tempS, monS);
		strcpy(monS, tempS);
		}
	if (strlen(dayS) == 1) {		// use days with two digits
		strcpy(tempS, "0");
		strcat(tempS, dayS);
		strcpy(dayS, tempS);
		}
	strcat(sumFile, yearS);
	strcat(sumFile, monS);
	strcat(sumFile, dayS);
	string stamp = sumFile;

	return stamp;
}


string characterEscape (const string & inString)
{
	string searVec[] = { "&", "<" };
	string repVec[] = { "&amp;", "&lt;" };
	unsigned pos; // startPos, endPos, 
	unsigned i;
	string tmpString = inString;
	if( tmpString.find_first_of("<&",0) != string::npos ) {
		for( i = 0; i < sizeof(searVec)/sizeof(string); i++ ) {
			pos = 0;
			while ( (pos = tmpString.find(searVec[i], pos)) != string::npos ) {
				tmpString = tmpString.replace(pos++, searVec[i].size(), repVec[i]);
				++pos;
				}
			}
		}
	return tmpString;
}


