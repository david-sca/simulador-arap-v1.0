/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/log.h"
#include "arap-simulator.h"
#include "arap-node.h"
#include "load-ants-statistics.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LoadAntsStatistics");

LoadAntsStatistics::LoadAntsStatistics()
:m_sumDelay(0), m_sumDelaySquare(0), m_numSamples(0){
}

LoadAntsStatistics::~LoadAntsStatistics(){
}

void
LoadAntsStatistics::UpdateModel(const double rtt, const Ipv4Address& target){
	m_numSamples++;
	m_sumDelay = m_sumDelay + rtt;
	m_sumDelaySquare = m_sumDelaySquare + (rtt*rtt);
	uint64_t sampleCount = m_samplesCount[target];
	m_samplesCount[target] =++sampleCount;
}

void
LoadAntsStatistics::InitializeTable(){
	const PointToPointStarHelper& star = ArapSimulator::GetStar();
	for(uint32_t i=0; i<star.SpokeCount();i++){
		Ptr<ArapNode> app = DynamicCast<ArapNode>(star.GetSpokeNode(i)->GetApplication(0));
		m_samplesCount.insert(std::pair<Ipv4Address,uint64_t>(app->GetLocalIP(),0)); //Inicializar todos los valores en 0
	}
}

const std::map<Ipv4Address,uint64_t>&
LoadAntsStatistics::GetSampleCountsMap() const{
	return m_samplesCount;
}

const double
LoadAntsStatistics::GetMean() const{
	if(m_numSamples==0)
		return 0;
	else
		return m_sumDelay/m_numSamples;
}

const double
LoadAntsStatistics::GetVariance() const{
	if(m_numSamples==0)
		return 0;
	else{
		double squareXHope = m_sumDelaySquare/m_numSamples; //Esperanza de X^2 (E(X²))
		double mean = GetMean();
		double xHopeSquare = mean*mean; //Esperanza al cuadrado de X([E(X)]²)

		return squareXHope - xHopeSquare;
	}
}

const uint64_t
LoadAntsStatistics::GetNumSamples() const{
	return m_numSamples;
}

} /* namespace ns3 */
