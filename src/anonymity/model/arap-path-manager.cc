/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "arap-path-manager.h"
#include "ns3/log.h"
#include "ns3/rng-seed-manager.h"
#include "arap-simulator.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("ArapPathManager");

ArapPathManager::ArapPathManager() {
	NS_LOG_INFO("Constructor de ArapPathManager");
}

ArapPathManager::~ArapPathManager() {
	m_probTable.clear();
}

const double
ArapPathManager::GetProbability(const Ipv4Address& target, const Ipv4Address& medium){
	std::map<Ipv4Address,double>& targetMap = m_probTable[target];
	return targetMap[medium];
}

void
ArapPathManager::SetProbability(const double& prob, const Ipv4Address& target, const Ipv4Address& medium){
	std::map<Ipv4Address,double>& targetMap = m_probTable[target];
	targetMap[medium] = prob;
}

void
ArapPathManager::CreateProbTable(const Ipv4Address& localIP){
	const PointToPointStarHelper& star = ArapSimulator::GetStar();
	const uint32_t nodes = star.SpokeCount();
	for (uint32_t i = 0; i < star.SpokeCount (); ++i){
		if(star.GetSpokeIpv4Address(i)==localIP)
			continue;
		m_probTable[star.GetSpokeIpv4Address(i)]= std::map<Ipv4Address,double>();
		std::map<Ipv4Address,double>& targetMap = m_probTable[star.GetSpokeIpv4Address(i)];
		for (uint32_t j = 0; j < star.SpokeCount (); ++j){
			if(star.GetSpokeIpv4Address(j)==localIP)
				continue;
			if(i==j)
				targetMap[star.GetSpokeIpv4Address(j)]= 0;
			else
				targetMap[star.GetSpokeIpv4Address(j)]= double(1)/(nodes-2);
		}
	}
}

void
ArapPathManager::PrintProbTable(const Ipv4Address& localIP){
	std::string name;
	std::ostringstream oss;
	/*Los archivos tienen un formato de nombre <IP_LOCAL>_<tiempo de generacion (segundos)>_<Semilla>_<Run>.csv*/
	oss<<"tablas/"<<localIP<<"_"<<Simulator::Now().GetSeconds()<<"_"<<RngSeedManager::GetSeed()<<"_"<<RngSeedManager::GetRun()<<".csv";
	name = oss.str();
	std::ofstream file (name.c_str());
  if (!file.is_open()){
  	NS_LOG_INFO("Fallo el crear el archivo de nombre: "+name);
  	return;
  }
  //Imprimir en archivo la cabecera de la tabla
  file<<" ";
  for (std::map<Ipv4Address,std::map<Ipv4Address,double> >::iterator it = m_probTable.begin();it!=m_probTable.end(); it++) {
  	 file<<","<<it->first;
	}
  file<<",Suma";
  file<<std::endl;

  //Imrpimir fila por fila al archivo
  for (std::map<Ipv4Address,std::map<Ipv4Address,double> >::iterator it = m_probTable.begin();it!=m_probTable.end(); it++) {
  	file<<it->first;
  	double sumProb = 0;
  	for (std::map<Ipv4Address,double>::iterator it2 = it->second.begin();it2!=it->second.end(); it2++) {
  		file<<","<<std::fixed<<it2->second;
  		sumProb = sumProb +it2->second;
  	}
  	file<<","<<sumProb;
  	file<<std::endl;
  }
  file.close();
}

ArapPathManager* ArapPathManager::GetCopy() const{
	return NULL; //Esta definicion del metodo nunca se llama ya que la clase es totalmente abstracta
}

void
ArapPathManager::HandleExplorer(const Ipv4Address& target, const Ipv4Address& medium, const Time& rtt){
	//La definicion de este método se deja a las clases que hereden de esta.
}

std::list<Ipv4Address>
ArapPathManager::CreatePath(const Ipv4Address& target){
	//Esta definicion del metodo nunca se llama ya que la clase es totalmente abstracta
	std::list<Ipv4Address> returnValue;
	return returnValue;
}

} /* namespace ns3 */
