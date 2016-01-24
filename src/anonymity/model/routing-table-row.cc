/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "routing-table-row.h"

namespace ns3 {

RoutingTableRow::RoutingTableRow()
:m_sourceIP(), m_targetIP(), m_antID() {
}

RoutingTableRow::RoutingTableRow(const Ipv4Address& source, const Ipv4Address& target, const uint64_t antID){
	m_sourceIP=source;
	m_targetIP=target;
	m_antID=antID;
}

RoutingTableRow::~RoutingTableRow() {
}

bool RoutingTableRow::operator==( const RoutingTableRow& row) const {
	if(	row.GetSourceIP() 	== GetSourceIP() 	&&
			row.GetTargetIP() 	== GetTargetIP() 	&&
			row.GetAntID() 	== GetAntID()
		)
			return true;
	return false;
}

const Ipv4Address& RoutingTableRow::GetTargetIP() const {
	return m_targetIP;
}

void RoutingTableRow::SetTargetIP(const Ipv4Address& targetIP) {
	m_targetIP = targetIP;
}

const Ipv4Address& RoutingTableRow::GetSourceIP() const {
	return m_sourceIP;
}

void RoutingTableRow::SetSourceIP(const Ipv4Address& sourceIP) {
	m_sourceIP = sourceIP;
}

const uint64_t RoutingTableRow::GetAntID() const {
	return m_antID;
}

void RoutingTableRow::SetPacketID(const uint64_t antID) {
	m_antID = antID;
}

} /* namespace ns3 */
