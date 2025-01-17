#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>

#include <boost/bind.hpp>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMDocumentTraversal.hpp>
#include <xercesc/dom/DOMNodeFilter.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include "connection.h"
#include "logger.h"
#include "tree_parser.h"
#include "record_parser.h"
#include "soap_request.h"
#include "shared_appdata.h"
#include "dataobject_factory.h"

#include "EmobiixField.h"

#define DEVICE_UDP_LISTEN_PORT  "7"

using namespace std;

namespace emobiix {

using namespace boost::asio;

connection::connection(io_service& io_service, const std::string app_path)
	: strand_(io_service),
	socket_(io_service),
	app_path_(app_path),
	m_currentSyncId(0),
	m_localSyncId(2)
{
}

connection::~connection()
{
#if 1
	if (connection_token_.length())
		shared_appdata::instance().remove(connection_token_);
	TRACELOG("Terminating connection: " << connection_token_);
#endif
}

ip::tcp::socket& connection::socket()
{
	return socket_;
}

void connection::start()
{
	// detect connection breaks without any data traffic
	socket_base::keep_alive option(true);
	socket_.set_option(option);

	const string ip = socket_.remote_endpoint().address().to_string();
	DEBUGLOG("Received connection from: " << ip);

	INFOLOG("Requesting authentication from client");
	request_auth(reply_);

	async_write(socket_, reply_.to_buffers(), strand_.wrap(boost::bind(&connection::handle_write, shared_from_this(), placeholders::error, placeholders::bytes_transferred)));

	INFOLOG("Waiting for authentication response from client");
	socket_.async_read_some(buffer(buffer_), strand_.wrap(boost::bind(&connection::handle_read, shared_from_this(), placeholders::error, placeholders::bytes_transferred)));
}

void connection::handle_read(const boost::system::error_code& e, std::size_t bytes_transferred)
{
	if (e)
	{
		ERRORLOG("A system error has occured: " << e << ": " << e.message());
		// If an error occurs then no new asynchronous operations are started. This
		// means that all shared_ptr references to the connection object will
		// disappear and the object will be destroyed automatically after this
		// handler returns. The connection class's destructor closes the socket.
		return;
	}

	TRACELOG("Handling a read of " << bytes_transferred);

	boost::tribool result = request_parser_.parse(request_, buffer_.data(), bytes_transferred);
	if (!result)
	{
		ERRORLOG("Received an invalid request");

		reply_ = reply::stock_error_reply();
		async_write(socket_, reply_.to_buffers(), strand_.wrap(boost::bind(&connection::handle_write, shared_from_this(), placeholders::error, placeholders::bytes_transferred)));
	}
	else 
	{
		bool readRemains = false;
		if (result)
			readRemains = true;

		if (request_.packets.size())
		{
			TRACELOG("Received valid requests: " << request_.packets.size());

			handle_request(request_, reply_);
			if (reply_.packets.size())
				async_write(socket_, reply_.to_buffers(), strand_.wrap(boost::bind(&connection::handle_write, shared_from_this(), placeholders::error, placeholders::bytes_transferred)));
		}

		TRACELOG("Resuming read...");
		socket_.async_read_some(buffer(buffer_), strand_.wrap(boost::bind(&connection::handle_read, shared_from_this(), placeholders::error, placeholders::bytes_transferred)));
	}
}

void connection::handle_write(const boost::system::error_code& e, std::size_t bytes_transferred)
{
	if (e)
	{
		ERRORLOG("A system error has occured: " << e << ": " << e.message());

		// Initiate graceful connection closure.
		boost::system::error_code ignored_ec;
		socket_.shutdown(ip::tcp::socket::shutdown_both, ignored_ec);
	}

	TRACELOG("Successfully wrote " << bytes_transferred);
	reply_.encodedPackets.clear();

	// No new asynchronous operations are started. This means that all shared_ptr
	// references to the connection object will disappear and the object will be
	// destroyed automatically after this handler returns. The connection class's
	// destructor closes the socket.
}

void connection::request_auth(reply& rep)
{
	FRIPacketP *packet = new FRIPacketP;
	packet->packetTypeP.present = packetTypeP_PR_authRequestP;

	AuthRequestP_t &authReq = packet->packetTypeP.choice.authRequestP;
	authReq.authSaltP.buf = NULL;
	OCTET_STRING_fromBuf(&authReq.authSaltP, "NaCl", -1);

	authReq.authTypesP.list.array = NULL;
	authReq.authTypesP.list.size = 0;
	authReq.authTypesP.list.count = 0;
	authReq.authTypesP.list.free = NULL;

	AuthTypeP_t *authType = new AuthTypeP_t;
	*authType = AuthTypeP_atUsernamePasswordP;
	asn_sequence_add(&authReq.authTypesP.list, authType);

	rep.packets.push_back(packet);
}

void connection::handle_request(request& req, reply& rep)
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

void connection::handle_packet(FRIPacketP *packet, reply& rep)
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

void connection::handle_protocolHandshake(FRIPacketP*, reply& rep)
{

}

void connection::handle_authRequest(FRIPacketP*, reply& rep)
{
}

void connection::handle_authUserPass(FRIPacketP* packet, reply& rep)
{
	DEBUGLOG("Received authentication request");

	AuthUserPassP_t &userPass = packet->packetTypeP.choice.authUserPassP;
	string user((char *)userPass.authUsernameP.buf, userPass.authUsernameP.size);
	string pass((char *)userPass.authPasswordP.buf, userPass.authPasswordP.size);

	INFOLOG("User: " << user << ", pass: " << pass);

	map<string, string> extraFields;
	for (size_t i = 0; i < userPass.authExtrasP.list.count; ++i)
	{
		AuthExtraP_t &extra = *(userPass.authExtrasP.list.array[i]);
		string field((char *)extra.authExtraNameP.buf, extra.authExtraNameP.size);
		string value((char *)extra.authExtraValueP.buf, extra.authExtraValueP.size);

		extraFields[field] = value;
		INFOLOG("Extra field " << field << " = " << value);
	}

	FRIPacketP *authResponse = new FRIPacketP;
	authResponse->packetTypeP.present = packetTypeP_PR_authResponseP;

	map<string, string>::const_iterator IMEI = extraFields.find("IMEI");
	if (IMEI == extraFields.end())
	{
		ERRORLOG("Required field IMEI missing from authentication");
		authResponse->packetTypeP.choice.authResponseP = RequestResponseP_responseErrorP;
	}
	else if (soap_request::GetAuthentication(app_path_, IMEI->second.c_str(), user.c_str(), pass.c_str(), extraFields))
	{
		INFOLOG("Authentication successful");

		connection_token_ = IMEI->second;

		authResponse->packetTypeP.choice.authResponseP = RequestResponseP_responseOKP;
		appdata data = { this };
		shared_appdata::instance().put(connection_token_, data);
	}
	else
	{
		ERRORLOG("Authentication failure");
		authResponse->packetTypeP.choice.authResponseP = RequestResponseP_responseFailP;
	}

	rep.packets.push_back(authResponse);
}

void connection::handle_authResponse(FRIPacketP*, reply& rep)
{

}

void connection::handle_subscriptionRequest(FRIPacketP*, reply& rep)
{

}

void connection::handle_subscriptionResponse(FRIPacketP*, reply& rep)
{

}

void connection::handle_dataObjectSyncStart(FRIPacketP* packet, reply& rep)
{
	DataObjectSyncStartP &s = packet->packetTypeP.choice.dataObjectSyncStartP;
	DEBUGLOG("Client Sync started for " << s.urlP.buf << ", revision: " << s.dataObjectStampMinorP << "." << s.dataObjectStampMajorP << ", seqId: " << s.syncSequenceIDP);

	m_currentSyncId = s.syncSequenceIDP;

	// TODO Remove hack
	if (char *quest = strrchr((const char *)s.urlP.buf, '?'))
	{
		connection_token_ += "|";
		connection_token_ += quest + 1;
		*quest = 0;
	}

	if (char *slash = strrchr((const char *)s.urlP.buf, '/'))
		url_request_= slash + 1;
	else
		url_request_ = "sample";


	// clean up the packet!
	free(s.urlP.buf);
	free(packet);
}

void connection::handle_dataObjectSync(FRIPacketP* packet, reply& rep)
{
	DEBUGLOG("Client data object sync");

	DataObjectSyncP &sync = packet->packetTypeP.choice.dataObjectSyncP;
	switch (sync.syncListP.present)
	{
		case SyncListP_PR_blockSyncListP:
		{
			TRACELOG("Got blockSyncList of " << sync.syncListP.choice.blockSyncListP.list.count << " elements");
			for (size_t i = 0; i < sync.syncListP.choice.blockSyncListP.list.count; ++i)
			{
				SyncOperandP_t *syncOp = sync.syncListP.choice.blockSyncListP.list.array[i];
				string operand;

				switch (syncOp->fieldNameP.present)
				{
					case FieldNameP_PR_fieldNameStringP:
					{
				 		operand = string((const char *)syncOp->fieldNameP.choice.fieldNameStringP.buf, syncOp->fieldNameP.choice.fieldNameStringP.size);
					}
					break;

					case FieldNameP_PR_fieldNameEnumP:
					{
						if (const char *fieldName = emo_field_to_string(syncOp->fieldNameP.choice.fieldNameEnumP))
						{
							operand = fieldName;
						}
						else
						{
							ERRORLOG("Uknown field name enum: " << syncOp->fieldNameP.choice.fieldNameEnumP);
							operand = "???";
						}
					}
					break;
				}

				TRACELOG("Got syncOperand: " << operand);

				switch (syncOp->syncP.present)
				{
					case syncP_PR_syncSetP:
					{
						string value((const char *)syncOp->syncP.choice.syncSetP.buf, syncOp->syncP.choice.syncSetP.size);
						TRACELOG("Got operand syncSet: " << value);

						DEBUGLOG("Sync param " << operand << " = " << value);
						m_currentSyncParams[operand] = value;
					}
					break;

					case syncP_PR_syncModifyP:
					{
						TRACELOG("Got operand syncModify");
					}
					break;

					case syncP_PR_syncRemoveP:
					{
						TRACELOG("Got operand syncRemove");
					}
					break;

					case syncP_PR_nodeOperationP:
					{
						TRACELOG("Got operand nodeOperation");
						
						switch (syncOp->syncP.choice.nodeOperationP.present)
						{
							case nodeOperationP_PR_nodeGotoTreeP:
							{
								TRACELOG("Got operation nodeGotoTree: " << syncOp->syncP.choice.nodeOperationP.choice.nodeGotoTreeP);
							}
							break;

							case nodeOperationP_PR_nodeAddP:
							{
								TRACELOG("Got operation nodeAdd");
							}
							break;

							case nodeOperationP_PR_nodeDeleteP:
							{
								TRACELOG("Got operation nodeDelete");
							}
							break;

							case nodeOperationP_PR_nodeCloseTreeP:
							{
								TRACELOG("Got operation nodeCloseTree");
							}
							break;

							default:
								ERRORLOG("Unknown nodeOperation: " << syncOp->syncP.choice.nodeOperationP.present);
						}
					}
					break;
		
					default:
						ERRORLOG("Unknown sync operation: " << syncOp->syncP.present);
				}
			}
		}
		break;
	
		case SyncListP_PR_recordSyncListP:
		{
			TRACELOG("Got recordSyncList");
		}
		break;

		default:
			ERRORLOG("Unknown sync type: " << sync.syncListP.present);
	}
}

void connection::handle_dataObjectSyncFinish(FRIPacketP* packet, reply& rep)
{
	DataObjectSyncFinishP &s = packet->packetTypeP.choice.dataObjectSyncFinishP;
	DEBUGLOG("Client Sync finished, seqId: " << s.syncSequenceIDP << ", response: " << s.responseP << " url: " << url_request_);

	if (!strncmp(url_request_.c_str(), "inbox", 5) || !strncmp(url_request_.c_str(), "messages", 8))
		start_arraySync(rep);
	else
		start_serverSync(rep);

	m_currentSyncId = 0;
	m_currentSyncParams.clear();
}

void connection::start_arraySync(reply& rep)
{
	DEBUGLOG("Starting array sync: " << m_currentSyncId);

	rep.packets.push_back(dataobject_factory::recordSyncListP(m_currentSyncId));

	string recordData;
	map<string, string> param;
	if (!soap_request::GetRecordDataObject(app_path_, connection_token_, url_request_, param, recordData))
	{
		ERRORLOG("NO record data...");
		return;
	}

	TRACELOG("Record data received: " << recordData);

	record_parser parser(recordData.c_str(), app_path_, connection_token_);
	parser.parse(rep.packets);

	DEBUGLOG("Finishing array sync: " << m_currentSyncId);
	rep.packets.push_back(dataobject_factory::dataObjectSyncFinishP(RequestResponseP_responseOKP, m_currentSyncId));
}

void connection::start_serverSync(reply& rep)
{
	DEBUGLOG("Starting server sync: " << m_currentSyncId);

	string treeData;
	if (!soap_request::GetTreeDataObject(app_path_, connection_token_, url_request_, m_currentSyncParams, treeData))
	{
		ERRORLOG("NO tree data...");
		return;
	}

	TRACELOG("Tree data received: " << treeData);
	
	xml_parser *parser;

	if (!strncmp(treeData.c_str(), "<record>", 8)) {
		rep.packets.push_back(dataobject_factory::recordSyncListP(m_currentSyncId));
		parser = new record_parser(treeData.c_str(), app_path_, connection_token_);
	} else {
		rep.packets.push_back(dataobject_factory::blockSyncListP(m_currentSyncId));
		parser = new tree_parser(treeData.c_str(), app_path_, connection_token_, m_currentSyncId);
	}
	parser->parse(rep.packets);

	DEBUGLOG("Finishing server sync: " << m_currentSyncId);
	rep.packets.push_back(dataobject_factory::dataObjectSyncFinishP(RequestResponseP_responseOKP, m_currentSyncId));

	delete parser;
}

void connection::handle_push(std::string URI)
{
#if 1
	reply rep, repA;
	FRIPacketP *packet;

	DEBUGLOG("push request to device " << URI);
	packet = dataobject_factory::dataObjectSyncStartP(URI.c_str(), m_localSyncId);
	handle_packet(packet, repA);
	rep.packets.push_back(dataobject_factory::dataObjectSyncStartP(URI.c_str(), m_localSyncId));
	// not async since rep is local and we are inside the strand 
	socket_.send(rep.to_buffers());
	m_localSyncId += 2;
#endif
}

void connection::push(const std::string &URI)
{
	DEBUGLOG("Generating push request to device " << socket_.remote_endpoint().address().to_string()
			<< " to URI " << URI);

	strand_.dispatch(boost::bind(&connection::handle_push, shared_from_this(), URI)); 

#if 0
	DEBUGLOG("Generating push request to device " << socket_.remote_endpoint().address().to_string() << " port " << DEVICE_UDP_LISTEN_PORT << " with data: " << data);

	using namespace boost::asio::ip;

	try
	{
		boost::asio::io_service io_service;

		udp::resolver resolver(io_service);
		udp::resolver::query query(udp::v4(), socket_.remote_endpoint().address().to_string(), DEVICE_UDP_LISTEN_PORT, ip::udp::resolver_query::numeric_service);
		udp::endpoint receiver_endpoint = *resolver.resolve(query);

		udp::socket device(io_service);
		device.open(udp::v4());
		device.send_to(boost::asio::buffer(data), receiver_endpoint);
	}
	catch (std::exception &e)
	{
		ERRORLOG("Failed to send UDP notification to device: " << e.what());
	}
#endif
}

}
