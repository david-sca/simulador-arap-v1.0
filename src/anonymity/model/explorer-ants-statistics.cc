/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "explorer-ants-statistics.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("ExplorerAntsStatistics");

ExplorerAntsStatistics::ExplorerAntsStatistics(const uint32_t wMax, const double varsigma)
:m_mean(0), m_variance(0), m_WBest(DEFAULT_WBEST), m_varsigma(varsigma), m_WMax(wMax), m_WCount(0){

}

ExplorerAntsStatistics::~ExplorerAntsStatistics(){
}

void
ExplorerAntsStatistics::UpdateModel(const int64_t rtt){
	m_mean = m_mean + m_varsigma*(rtt-m_mean);
	m_variance = m_variance +m_varsigma*( ((rtt-m_mean)*(rtt-m_mean))-m_variance );

	/* Cuando se han recibido wMax muestras se reinicia el contador
	 * y el mejor de "w" (la ventana de observacion)
	 */
	if(m_WCount==m_WMax){
		m_WCount=0;
		m_WBest=DEFAULT_WBEST;
	}

	m_WCount++;
	if(rtt<m_WBest){ //Se actualiza el mejor, cuando el tiempo recibido es menor que el actual mejor
		m_WBest=rtt;
	}
}

const double
ExplorerAntsStatistics::GetMean() const{
	return m_mean;
}

const double
ExplorerAntsStatistics::GetVariance() const{
	return m_variance;
}


const int64_t
ExplorerAntsStatistics::GetWBest() const{
	return m_WBest;
}

const uint32_t
ExplorerAntsStatistics::GetWMax()const{
	return m_WMax;
}

const uint32_t
ExplorerAntsStatistics::GetWCount()const{
	return m_WCount;
}

} /* namespace ns3 */
