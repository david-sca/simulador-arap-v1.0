/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "path-manager-factory.h"
#include "smart-path-manager.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PathManagerFactory");

PathManagerFactory::PathManagerFactory(){
}

PathManagerFactory::~PathManagerFactory(){
}

ArapPathManager*
PathManagerFactory::GetInstance(const std::string& caseName, std::istringstream& iss){
	if(caseName=="SmartPathManager"){
		/* En este caso se está definiendo el orden de lectura de los parámetros
		 * desde el archivo en el mismo orden que se usan en el constructor
		 */
		double c1, c2, zeta,varsigma;
		uint32_t wMax;
		iss>>c1>>c2>>zeta>>wMax>>varsigma;
		return new SmartPathManager(c1,c2,zeta,wMax,varsigma);
	}
	else if(caseName=="SmartPathManagerDefault"){
		/* Este es un segundo caso para una misma clase, para este caso se están
		 * usando los valores por defecto que proveé la clase, lo importante es
		 * usar un identificador diferente para cada caso
		 */
			return new SmartPathManager();
		}
	/*Aqui se agrega otro caso cuando se desee extender esta clase especializando una nueva de ArapPathManager
	else if(className==<Identificador de caso nuevo>){
		... <Definicion del caso>
	}*/
	else
		return NULL;
}
} /* namespace ns3 */
