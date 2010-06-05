#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMDocumentTraversal.hpp>
#include <xercesc/dom/DOMNodeFilter.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include "request_handler.h"
#include "logger.h"
#include "reply.h"
#include "request.h"
#include "soap_request.h"
#include "shared_appdata.h"
#include "dataobject_factory.h"

using namespace std;

namespace emobiix {

request_handler::request_handler(const std::string& app_path)
	: app_path_(app_path)
{
}

request_handler::~request_handler()
{
	shared_appdata::instance().remove("IP ADDRESS");
}

void request_handler::handle_request(request& req, reply& rep)
{
	if (!req.packets.size())
	{
		ERRORLOG("Unknown packet received, will not handle");
		rep = reply::stock_error_reply();
		return;
	}

	for (size_t i = 0; i < req.packets.size(); ++i)
		handle_packet(req.packets[i], rep);

	req.packets.clear();
}

void request_handler::handle_packet(FRIPacketP *packet, reply& rep)
{
	DEBUGLOG("Received packet of type: " << packet->packetTypeP.present);
	switch (packet->packetTypeP.present)
	{
		case packetTypeP_PR_protocolHandshakeP:
			handle_protocolHandshake(packet, rep);
			break;

		case packetTypeP_PR_authRequestP:
			handle_authRequest(packet, rep);
			break;

		case packetTypeP_PR_authUserPassP:
			handle_authUserPass(packet, rep);
			break;

		case packetTypeP_PR_authResponseP:
			handle_authResponse(packet, rep);
			break;

		case packetTypeP_PR_subscriptionRequestP:
			handle_subscriptionRequest(packet, rep);
			break;

		case packetTypeP_PR_subscriptionResponseP:
			handle_subscriptionResponse(packet, rep);
			break;

		case packetTypeP_PR_dataObjectSyncStartP:
			handle_dataObjectSyncStart(packet, rep);
			break;

		case packetTypeP_PR_dataObjectSyncP:
			handle_dataObjectSync(packet, rep);
			break;

		case packetTypeP_PR_dataObjectSyncFinishP:
			handle_dataObjectSyncFinish(packet, rep);
			break;

		case packetTypeP_PR_NOTHING:
			break;
	}
}

void request_handler::handle_protocolHandshake(FRIPacketP*, reply& rep)
{

}

void request_handler::handle_authRequest(FRIPacketP*, reply& rep)
{
	DEBUGLOG("Received authentication request");

	//	rep.packet->packetTypeP.present = packetTypeP_PR_authResponseP;
	//	rep.packet.packetType.choice.authResponse = RequestResponse_responseOK;
	if (soap_request::GetAuthentication("http://linux.emobiix.com:8082/cgi-bin/test.cgi", "123456", "bob", "torulethemall"))
	{
		appdata data = { "123456" };
		shared_appdata::instance().put("IP ADDRESS", data);
		return;
	}
}

void request_handler::handle_authUserPass(FRIPacketP*, reply& rep)
{

}

void request_handler::handle_authResponse(FRIPacketP*, reply& rep)
{

}

void request_handler::handle_subscriptionRequest(FRIPacketP*, reply& rep)
{

}

void request_handler::handle_subscriptionResponse(FRIPacketP*, reply& rep)
{

}

void request_handler::handle_dataObjectSyncStart(FRIPacketP* packet, reply& rep)
{
	DataObjectSyncStartP &s = packet->packetTypeP.choice.dataObjectSyncStartP;
	DEBUGLOG("Client Sync started for " << s.urlP.buf << ", revision: " << s.dataObjectStampMinorP << "." << s.dataObjectStampMajorP << ", seqId: " << s.syncSequenceIDP);

	if (char *slash = strrchr((const char *)s.urlP.buf, '/'))
		url_request_ = slash + 1;
	else
		url_request_ = "sample";

	// clean up the packet!
	free(s.urlP.buf);
	free(packet);
}

void request_handler::handle_dataObjectSync(FRIPacketP*, reply& rep)
{

}

void request_handler::handle_dataObjectSyncFinish(FRIPacketP* packet, reply& rep)
{
	DataObjectSyncFinishP &s = packet->packetTypeP.choice.dataObjectSyncFinishP;
	DEBUGLOG("Client Sync finished, seqId: " << s.syncSequenceIDP << ", response: " << s.responseP);

	start_serverSync(rep);
}

void request_handler::start_serverSync(reply& rep)
{
	DEBUGLOG("Starting server sync");

	FRIPacketP *start = new FRIPacketP;
	start->packetTypeP.present = packetTypeP_PR_dataObjectSyncP;
	DataObjectSyncP &s = start->packetTypeP.choice.dataObjectSyncP;

	s.syncListP.present = SyncListP_PR_blockSyncListP;
	s.syncListP.choice.blockSyncListP.list.array = NULL;
	s.syncListP.choice.blockSyncListP.list.size = 0;
	s.syncListP.choice.blockSyncListP.list.count = 0;
	s.syncSequenceIDP = 1;

	rep.packets.push_back(start);

	string treeData;
	if (!soap_request::GetTreeDataObject("http://linux.emobiix.com:8082/cgi-bin/test.cgi", url_request_, treeData))
	{
		ERRORLOG("NO tree data...");
		return;
	}

	TRACELOG("Tree data received: " << treeData);
	parse(treeData.c_str(), rep.packets);

	DEBUGLOG("Finishing server sync");
	FRIPacketP *finish = new FRIPacketP;
	finish->packetTypeP.present = packetTypeP_PR_dataObjectSyncFinishP;
	finish->packetTypeP.choice.dataObjectSyncFinishP.responseP = RequestResponseP_responseExpiredP;
	finish->packetTypeP.choice.dataObjectSyncFinishP.syncSequenceIDP = 1;

	rep.packets.push_back(finish);
}

bool request_handler::parseTree(DOMNode *node, vector<FRIPacketP *>& packets, int& nodeCount)
{
	if (!node)
		return false;

	int self = nodeCount;
	if (FRIPacketP *dataObject = dataobject_factory::create(node))
	{
		packets.push_back(dataObject);
		nodeCount++;
	}

	node = node->getFirstChild();
	while (node) 
	{
		if (node->getNodeType() == DOMNode::ELEMENT_NODE) 
		{
			if (self != nodeCount)
				dataobject_factory::addChild(packets.back());

			parseTree(node, packets, nodeCount);
		}

		node = node->getNextSibling();
		if (node && node->getNodeType() == DOMNode::ELEMENT_NODE) 
			dataobject_factory::goToTree(packets.back(), self);
	}
}

bool request_handler::parse(const char *doc, vector<FRIPacketP *>& packets)
{
	XMLPlatformUtils::Initialize();
	{
		XercesDOMParser *parser = new XercesDOMParser;
		parser->setValidationScheme(XercesDOMParser::Val_Always);
		parser->setDoNamespaces(false);
		parser->setDoSchema(false);
		parser->setLoadExternalDTD(false); // perhaps later?

		MemBufInputSource buffer((const XMLByte *)doc, strlen(doc), "MemoryBuffer");
		parser->parse(buffer);

		DOMDocument *document = parser->getDocument();

		int nodeCount = 0;
		parseTree(document->getFirstChild(), packets, nodeCount);

		delete parser;
	}
	XMLPlatformUtils::Terminate();
}

}
