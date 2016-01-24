/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "smart-path-manager.h"
#include "ns3/arap-simulator.h"
#include "ns3/log.h"
#include "ns3/abort.h"
#include <algorithm>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SmartPathManager");

SmartPathManager::SmartPathManager()
:m_c1(DEFAULT_C1), m_c2(DEFAULT_C2), m_zeta(DEFAULT_ZETA),m_WMax(DEFAULT_WMAX),m_varsigma(DEFAULT_VARSIGMA){
	NS_LOG_FUNCTION("CONSTRUCTOR POR DEFECTO DE SmartPathManager");
}

SmartPathManager::SmartPathManager(double c1, double c2, double zeta, uint32_t wMax, double varsigma)
:m_c1(c1), m_c2(c2), m_zeta(zeta),m_WMax(wMax),m_varsigma(varsigma){
	NS_LOG_FUNCTION("CONSTRUCTOR PARAMETRICO DE SmartPathManager: "<<c1<<c2<<zeta<<wMax<<varsigma);
}

SmartPathManager::~SmartPathManager() {
	m_stochasticModel.clear();
}

const double
SmartPathManager::GetR(const int64_t& rtt,const double& mean, const double& variance, const double& wBest, const uint32_t& wCount){
	double r=0;
	double term1 =0;
	double term2 =0;

	/*Se hace cada calculo linea por linea para prevenir perdidas de precision*/

	double term1Div = wBest/rtt;
	term1 = m_c1*term1Div;
	double standardDeviation = sqrt(variance);
	double wCountRoot = sqrt(wCount);
	double iSupDen = standardDeviation/wCountRoot;
	double iSup = mean + m_zeta*(iSupDen);
	double iInf = wBest;
	double term2Den = iSup-iInf+rtt-iInf;
	double term2Num = iSup-iInf;
	double term2Div = (term2Num)/(term2Den);
	term2 = m_c2*(term2Div);
	r = term1 + term2;
	return r;
}

void
SmartPathManager::HandleExplorer(const Ipv4Address& target, const Ipv4Address& medium, const Time& rtt){
	std::map<Ipv4Address, ExplorerAntsStatistics>::iterator it;
	it = m_stochasticModel.find(target);
	if(it==m_stochasticModel.end()) //Esta condicion siempre deberia ser false (Si debe existir el modelo)
		NS_ABORT_MSG("No se ha encontrado un modelo estocastico para el nodo con IP "<<target);

	ExplorerAntsStatistics& stats = it->second;
	stats.UpdateModel(rtt.GetMilliSeconds());
	double r = GetR(rtt.GetMilliSeconds(), stats.GetMean(), stats.GetVariance(),stats.GetWBest(), stats.GetWCount());
	const double prob = PheromoneIncrease(target,medium,rtt,r);
	PheromoneDecrease(target,medium,rtt,r);


	/* Aplicar la correccion a la probabilidad si esta excede el valor maximo.
	 * En este caso se esta asignando el valor maximo al nodo que corresponde (El que excedio
	 * la probabilidad), y el excedente se divide entre los demas nodos de manera uniforme.
	 */

	if(prob> MAX_PROB){
		double diff = prob - MAX_PROB;
		m_probTable[target][medium] = MAX_PROB;
		std::map<Ipv4Address,double>& targetMap = m_probTable[target];
		for(std::map<Ipv4Address,double>::iterator it = targetMap.begin(); it!= targetMap.end();it++){
			if(it->first==medium || it->first==target)
				continue;
			double& p_IT =it->second;
			double probGain = diff/(ArapSimulator::GetNumNodes()-2-1);
			p_IT = p_IT+probGain;
		}
	}
}

ArapPathManager* SmartPathManager::GetCopy() const {
	SmartPathManager* aux = new SmartPathManager(*this);
	aux->InitModels();
	return aux;
}

void
SmartPathManager::InitModels(){
	const PointToPointStarHelper& star = ArapSimulator::GetStar();
	for (uint32_t i = 0; i < star.SpokeCount (); ++i){
		m_stochasticModel.insert(std::pair<Ipv4Address,ExplorerAntsStatistics>(star.GetSpokeIpv4Address(i),ExplorerAntsStatistics(m_WMax, m_varsigma)));
	}
}

std::list<Ipv4Address>
SmartPathManager::CreatePath(const Ipv4Address& target) {
	std::map<Ipv4Address,double> targetProb = m_probTable[target];
	std::list<Ipv4Address> path;
	Ptr<UniformRandomVariable>  rng = CreateObject<UniformRandomVariable>();
	while(path.size()<ArapSimulator::GetNumHops()-1){
		double x = rng->GetValue();
		double acum = 0;
		for(std::map<Ipv4Address,double>::iterator it =  targetProb.begin();it!=targetProb.end();it++){
			if(it->first == target)
				continue;
			if(x>=acum && x<acum+it->second){
				if(std::find(path.begin(), path.end(), it->first) == path.end()) //Si el nodo NO esta en el camino
					path.push_back(it->first);
				break;
			}else{
				acum+=it->second;
			}
		}
	}
	/* Se agrega el nodo destino como elemento final del camino */
	path.push_back(target);
	return path;
}

const double
SmartPathManager::PheromoneIncrease(const Ipv4Address& target, const Ipv4Address& medium, const Time& rtt, const double& reward){
	double& p_TM =m_probTable[target][medium];
	p_TM = p_TM+reward*(1-p_TM);
	return p_TM;
}

void
SmartPathManager::PheromoneDecrease(const Ipv4Address& target, const Ipv4Address& medium, const Time& rtt, const double& reward){
	std::map<Ipv4Address,double>& targetMap = m_probTable[target];
	for(std::map<Ipv4Address,double>::iterator it = targetMap.begin(); it!= targetMap.end();it++){
		if(it->first==medium || it->first==target)
			continue;
		double& p_TI =it->second;
		p_TI = p_TI-reward*p_TI;
	}
}

} /* namespace ns3 */
