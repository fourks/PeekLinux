#include "stdsoap2.h"
#include "emobiix_rpc_Stub.h"
#include "emobiix_rpc_emobiixObject.h" // get server object
#include "emobiix.nsmap" // get namespace bindings

#include <cstdio>
#include <string>  
#include <iostream>
#include <fstream>
#include <sys/stat.h>

#include "curl/curl.h"  
  
using namespace std;

int main()
{
   emobiixService c;
   return c.serve(); // calls soap_serve to serve as CGI application (using stdin/out)
}

int ns__AuthenticationRequest(struct soap* soap, std::string deviceId, std::string userName, std::string password, bool &isAuthenticated)
{
	cerr << "Authentication request for [" << deviceId << "], [" << userName << "], [" << password << "]" << endl;

	if (userName == "peek" && password == "peek123")
		isAuthenticated = true;
	else
		isAuthenticated = false;

	cerr << "Authenticated? " << isAuthenticated << endl;

	return SOAP_OK;
}

// Write any errors in here  
static char errorBuffer[CURL_ERROR_SIZE];  
 
#if 0 
// Write all expected data in here  
static string buffer;  
  
// This is the writer call back function used by curl  
static int writer(char *data, size_t size, size_t nmemb,  
                  std::string *buffer)  
{  
  // What we will return  
  int result = 0;  
  
  // Is there anything in the buffer?  
  if (buffer != NULL)  
  {  
    // Append the data to the buffer  
    buffer->append(data, size * nmemb);  
  
    // How much did we write?  
    result = size * nmemb;  
  }  
  
  return result;  
}  
 
#endif
 
bool curl_get_file(const std::string& filename, const char *szUrl)
{
	// Our curl objects  
	CURL *curl;  
	CURLcode result;  

	// Create our curl handle  
	curl = curl_easy_init();  
	if (!curl) 
		return false;

	FILE *file = fopen(filename.c_str(), "w");
	if (!file)
		return false;

	// Now set up all of the curl options  
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);  
	curl_easy_setopt(curl, CURLOPT_URL, szUrl);  
	curl_easy_setopt(curl, CURLOPT_HEADER, 0);  
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);  
//		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);  
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);  

	// Attempt to retrieve the remote page  
	result = curl_easy_perform(curl);  

	// Always cleanup  
	curl_easy_cleanup(curl);  

	fclose(file);

	// Did we succeed?  
	if (result != CURLE_OK)  
	{  
		cout << "Error: [" << result << "] - " << errorBuffer;  
		return false;
	}

	return true;
}  

int ns__BlockDataObjectRequest(struct soap* soap, std::string deviceId, std::string dataObjectURI, ns__Timestamp timeStamp, xsd__base64Binary &binaryData)
{
	if (dataObjectURI == "map.png")
	{
		if (!curl_get_file(dataObjectURI, "http://maps.google.com/maps/api/staticmap?center=Brooklyn+Bridge,New+York,NY&zoom=14&size=320x240&maptype=roadmap&markers=color:blue|label:S|40.702147,-74.015794&markers=color:green|label:G|40.711614,-74.012318&markers=color:red|color:red|label:C|40.718217,-73.998284&sensor=false"))
		{
			return 404;
		}
	}

	struct stat st;
	if (stat(dataObjectURI.c_str(), &st) != 0)
		return 404;

	ifstream file(dataObjectURI.c_str(), ios::in | ios::binary);
	if (!file)
		return 404;

	char *type = "unknown";
	if (char *dot = strrchr(dataObjectURI.c_str(), '.'))
		type = dot + 1;

	binaryData = xsd__base64Binary(soap, st.st_size, type);

	file.read((char *)binaryData.getPtr(), binaryData.getSize());

	return SOAP_OK;
}

int ns__RecordDataObjectRequest(struct soap* soap, std::string deviceId, std::string dataObjectURI, ns__Timestamp timestamp, recordArray &recordData)
{
	recordData = recordArray(soap, 3);

	recordData[0] = "Record #1";
	recordData[1] = "Record #2";
	recordData[2] = "Record #3";
	
	return SOAP_OK;
}

int ns__TextDataObjectRequest(struct soap* soap, std::string deviceId, std::string dataObjectURI, ns__Timestamp timestamp, std::string &textData)
{
	textData = "some textual data";
	return SOAP_OK;
}

int ns__TreeDataObjectRequest(struct soap* soap, std::string deviceId, std::string dataObjectURI, ns__Timestamp timeStamp, std::string &m__treeData)
{
	char path[2048] = "";
	sprintf(path, "%s.xml", dataObjectURI.c_str());

	struct stat st;
	if (stat(path, &st) != 0)
	{
		m__treeData = "<emobiix-gui></emobiix-gui>";
		return SOAP_OK;
	}

	ifstream file(path, ios::in);
	if (!file)
	{
		m__treeData = "<emobiix-gui></emobiix-gui>";
		return SOAP_OK;
	}

	std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	m__treeData = str;

	return SOAP_OK;
}

int ns__DataObjectPushRequest(struct soap* soap, std::string deviceId, std::string dataObjectURI, bool &isDelivered)
{
	return SOAP_OK;
}

