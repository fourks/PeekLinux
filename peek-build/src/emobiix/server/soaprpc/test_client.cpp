#include "soap_request.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <map>

using namespace std;

int main(int argc, char **argv)
{
//	cerr << "got: " << emobiix::soap_request::Test_Push(argv[1]);
//	return 0;

	const char *url = "http://linux.emobiix.com:8082/cgi-bin/test.cgi";
	if (argc != 2)
		cerr << "Usage: " << argv[0] << " <request_url>" << endl;
	else
		url = argv[1];
	
	cerr << "Will use URL: " << url << " for soap requests" << endl;

	map<string, string> param;
	param["key"] = "value";

	cerr << "Using \"peek\" pass \"peek123\" for authentication..." << endl;
	if (emobiix::soap_request::GetAuthentication(url, "1234567", "peek", "peek123", param))
		cerr << "Authentication successful!" << endl;

	param["lac"] = "35621";
	param["ci"] = "10055";

	string tree;
	if (emobiix::soap_request::GetTreeDataObject(url, "1234567", "weather", param, tree))
		cerr << "Tree data request successful: " << endl << tree << endl;

	return 0;
	string mime;
	vector<pair<size_t, unsigned char *> > blocks;
	if (emobiix::soap_request::GetBlockDataObject(url, "1234567", "image.jpg", param, mime, blocks))
		cerr << "Block data request successful, got data of type " << mime << " of size " << blocks.size() << " blocks of data" << endl;

	for (size_t i = 0; i < blocks.size(); ++i)
	{
		cerr << "Block " << i << ", size: " << blocks[i].first << endl;
		for (size_t j = 0; j < blocks[i].first; ++j)
		{
			cerr << hex << "0x" << setfill('0') << setw(2) << (int)blocks[i].second[j] << " ";
		}
		cerr << endl;
	}
}
