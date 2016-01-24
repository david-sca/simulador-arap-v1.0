/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/anonymity-module.h"
#include "ns3/log.h"
#include "ns3/rng-seed-manager.h"
#include "ns3/config.h"
#include "ns3/uinteger.h"


// Ejemplo de la topologia a simular (Con 8 Nodos)
//
//        n2 n3 n4              .
//         \ | /                .
//          \|/                 .
//     n1--- n0---n5            .
//          /|\                 .
//         / | \                .
//        n8 n7 n6              .
//

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Simulador");

int
main (int argc, char *argv[]){

	/*Habilitar los distintos LOG (Mayormente para pruebas)*/
	/*LogComponentEnable ("ArapSimulator",LogLevel(LOG_LEVEL_ALL | LOG_PREFIX_FUNC | LOG_PREFIX_TIME));
	*/

	NS_LOG_UNCOND ("Simulador ARAP - INICIO");

	std::string parameterFileName="parametros.txt"; //Nombre por defecto del archivo de parametros
	uint64_t runNumber=0; //RunNumber
	if (argc>=2){
		runNumber = atoll(argv[1]);
		RngSeedManager::SetRun(runNumber);
	}
	if (argc>=3){
		parameterFileName=argv[2];
	}

	/* En ArapSimulator::ConfigSimulator se realiza la creación y
	 * configuración de la red y las aplicaciones, en los nodos a simular.
	 */
	ArapSimulator::ConfigSimulator(parameterFileName);

  NS_LOG_UNCOND("Iniciando Simulacion");
  ArapSimulator::Run ();
  NS_LOG_UNCOND("Finalizando Simulacion");
  ArapSimulator::Destroy ();
  NS_LOG_UNCOND ("Simulador Arap - FIN RUN");
  return 0;
}
