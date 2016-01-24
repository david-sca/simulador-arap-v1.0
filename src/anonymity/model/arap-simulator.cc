/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "arap-simulator.h"
#include "ns3/arap-node.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include <cmath>
#include <exception>

#define HUB_DATA_RATE 524288000 //500 Mbps
namespace ns3 {



NS_LOG_COMPONENT_DEFINE ("ArapSimulator");

/*Definicion de las variables static*/
std::map<std::string, int> ArapSimulator::s_mapStringValues;
PointToPointStarHelper* ArapSimulator::m_star =NULL;
uint32_t ArapSimulator::m_numNodes = 0;
uint32_t ArapSimulator::m_numHops = 0;
uint32_t ArapSimulator::m_antSize = 0;
uint16_t ArapSimulator::m_port = DEFAULT_PORT;
Time ArapSimulator::m_linkDelayInterval;
Time ArapSimulator::m_explorerAntsInterval;
Time ArapSimulator::m_printTablesInterval;
Ptr<RandomVariableStream> ArapSimulator::m_linkDelayChange=NULL;
std::list<OperationDelayFormat> ArapSimulator::m_computingDelayList;
std::list<DataRateFormat> ArapSimulator::m_dataRateList;
std::list<RandomStreamFormat> ArapSimulator::m_computingDistList;
std::list<RandomStreamFormat> ArapSimulator::m_loadAntsTimeList;
std::list<RandomStreamFormat> ArapSimulator::m_loadAntsQuantityList;
std::list<RandomStreamFormat> ArapSimulator::m_loadAntsTargetList;
std::list<AppTimeFormat> ArapSimulator::m_startTimeList;
uint64_t ArapSimulator::m_maximumSegmentSize=0;
uint64_t ArapSimulator::m_queueSize=0;
double ArapSimulator::m_stopTime=0;
bool ArapSimulator::m_enableTraces=DEFAULT_ENABLE_TRACING;
bool ArapSimulator::m_enableExplorerAnts=false;
PathManagerFactory ArapSimulator::m_pathManagerFactory;
ArapPathManager* ArapSimulator::m_arapPathManager = NULL;

void
ArapSimulator::InitializeValuesMap(){
	s_mapStringValues[PARAM_NODES] = PARAM_NODES_V;
	s_mapStringValues[PARAM_HOPS] = PARAM_HOPS_V;
	s_mapStringValues[PARAM_ANT_SIZE] = PARAM_ANT_SIZE_V;
	s_mapStringValues[PARAM_PORT] = PARAM_PORT_V;
	s_mapStringValues[PARAM_LINK_DELAY_INTERVAL] = PARAM_LINK_DELAY_INTERVAL_V;
	s_mapStringValues[PARAM_EXPLORER_INTERVAL] = PARAM_EXPLORER_INTERVAL_V;
	s_mapStringValues[PARAM_APP_START_TIME] = PARAM_APP_START_TIME_V;
	s_mapStringValues[PARAM_SIMULATOR_STOP_TIME] = PARAM_SIMULATOR_STOP_TIME_V;
	s_mapStringValues[PARAM_LINK_DELAY_DIST] = PARAM_LINK_DELAY_DIST_V;
	s_mapStringValues[PARAM_COMPUTING_DELAY_DIST] = PARAM_COMPUTING_DELAY_DIST_V;
	s_mapStringValues[PARAM_COMPUTING_DELAY_INCREMENT] = PARAM_COMPUTING_DELAY_INCREMENT_V;
	s_mapStringValues[PARAM_DATA_RATE] = PARAM_DATA_RATE_V;
	s_mapStringValues[PARAM_SEED_NUMBER] = PARAM_SEED_NUMBER_V;
	s_mapStringValues[PARAM_RUN_NUMBER] = PARAM_RUN_NUMBER_V;
	s_mapStringValues[PARAM_ENABLE_TRACING] = PARAM_ENABLE_TRACING_V;
	s_mapStringValues[PARAM_ENABLE_EXPLORER_ANTS] = PARAM_ENABLE_EXPLORER_ANTS_V;
	s_mapStringValues[PARAM_QUEUE_SIZE] = PARAM_QUEUE_SIZE_V;
	s_mapStringValues[PARAM_MAXIMUM_SEGMENT_SIZE] = PARAM_MAXIMUM_SEGMENT_SIZE_V;
	s_mapStringValues[PARAM_PATH_MANAGER] = PARAM_PATH_MANAGER_V;
	s_mapStringValues[PARAM_LOAD_ANT_TIME_DIST] = PARAM_LOAD_ANT_TIME_DIST_V;
	s_mapStringValues[PARAM_LOAD_ANT_QUANTITY_DIST] = PARAM_LOAD_ANT_QUANTITY_DIST_V;
	s_mapStringValues[PARAM_LOAD_ANT_TARGET_DIST] = PARAM_LOAD_ANT_TARGET_DIST_V;
	s_mapStringValues[PARAM_PRINT_PROB_TABLES_INTERVAL] = PARAM_PRINT_PROB_TABLES_INTERVAL_V;
}

Ptr<RandomVariableStream>
ArapSimulator::ReadRandomStream(const std::string& randomStream, std::istringstream& iss){
	if(randomStream=="ConstantRandomVariable"){
		double constant;
		iss>>constant;
		if(constant<0)
			NS_ABORT_MSG("ERROR: Rango de valores para la distribucion \"ConstantRandomVariable\" no validos");
		Ptr<ConstantRandomVariable> auxStream = CreateObject<ConstantRandomVariable>();
		auxStream->SetAttribute("Constant",DoubleValue(constant));
		return auxStream;
	}
	else if(randomStream=="UniformRandomVariable"){
		double min,max;
		iss>>min>>max;
		if(min<0 || max <0 || min>max)
			NS_ABORT_MSG("ERROR: Rango de valores para la distribucion \"UniformRandomVariable\" no validos");
		Ptr<UniformRandomVariable> auxStream = CreateObject<UniformRandomVariable>();
		auxStream->SetAttribute("Min",DoubleValue(min));
		auxStream->SetAttribute("Max",DoubleValue(max));
		return auxStream;
	}
	else if(randomStream=="TriangularRandomVariable"){
		double min,max,mean;
		iss>>min>>max>>mean;
		if(min<0 || max <0 || min>max || mean<min || mean > max)
			NS_ABORT_MSG("ERROR: Rango de valores para la distribucion \"TriangularRandomVariable\" no validos");
		Ptr<TriangularRandomVariable> auxStream = CreateObject<TriangularRandomVariable> ();
		auxStream->SetAttribute ("Mean", DoubleValue (mean));
		auxStream->SetAttribute ("Min", DoubleValue (min));
		auxStream->SetAttribute ("Max", DoubleValue (max));
		return auxStream;
	}
	else if(randomStream=="ExponentialRandomVariable"){
		double mean=0, bound=0;
		iss>>mean>>bound;
		if(mean<0 || bound <0 || bound<mean)
			NS_ABORT_MSG("ERROR: Rango de valores para la distribucion \"ExponentialRandomVariable\" no validos");
		Ptr<ExponentialRandomVariable> auxStream = CreateObject<ExponentialRandomVariable> ();
		auxStream->SetAttribute ("Mean", DoubleValue (mean));
		auxStream->SetAttribute ("Bound", DoubleValue (bound));
		return auxStream;
	}
	else if(randomStream=="NormalRandomVariable"){
		double mean, variance,bound;
		iss>>mean>>variance>>bound;
		if(mean<0 || variance<0 || bound<0 || bound>mean) //Para que no de valores negativos
			NS_ABORT_MSG("ERROR: Rango de valores para la distribucion \"NormalRandomVariable\" no validos");
		Ptr<NormalRandomVariable> auxStream = CreateObject<NormalRandomVariable> ();
		auxStream->SetAttribute ("Mean", DoubleValue (mean));
		auxStream->SetAttribute ("Variance", DoubleValue (variance));
		auxStream->SetAttribute ("Bound", DoubleValue (bound));
		return auxStream;
	}
	else{
		NS_ABORT_MSG("La distribucion de probabilidad leida no es valida");
		return NULL; //Deberia ser siempre alguno de los definidos anteriormente
	}
}

void
ArapSimulator::HandleParameter(const std::string& paramName, std::istringstream& iss){
	switch(s_mapStringValues[paramName]){
		case PARAM_NODES_V:{
			int auxNodes;
			iss>>auxNodes;
			m_numNodes = auxNodes;
			if(m_numNodes<MINIMUM_NODES)
				NS_ABORT_MSG("ERROR: Valor de numero de nodos no valido, nodos debe ser >="<<MINIMUM_NODES);
			break;
		}
		case PARAM_HOPS_V:{
			int auxHops;
			iss>>auxHops;
			m_numHops = auxHops;
			if(m_numHops<2 || m_numHops>=m_numNodes)
				NS_ABORT_MSG("ERROR: Valor de numero de saltos no valido, debe ser >=2 y <= nodos-1");
			break;
		}
		case PARAM_ANT_SIZE_V:{
			int auxAntSize;
			iss>>auxAntSize;
			m_antSize = auxAntSize;
			if(m_antSize<MINIMUM_ANT_SIZE){
				NS_ABORT_MSG("ERROR: Tamanio de hormiga leido muy pequenio, el valor minimo es: "<<MINIMUM_ANT_SIZE<<" Bytes");
			}
			break;
		}
		case PARAM_PORT_V:{
			int auxPort;
			iss>>auxPort;
			m_port = auxPort;
			if(m_port<1 || m_port>65535){ //El rango de puertos valido definido
				NS_ABORT_MSG("ERROR: Valor de puerto leido fuera de rango");
			}
			break;
		}
		case PARAM_LINK_DELAY_INTERVAL_V:{
			double linkDelta;
			iss>>linkDelta;
			if(linkDelta<0)
				NS_ABORT_MSG("ERROR: Valor de intervalo de delay no valido");
			m_linkDelayInterval = Seconds(linkDelta);
			break;
		}
		case PARAM_PRINT_PROB_TABLES_INTERVAL_V:{
			double printTablesDelta;
			iss>>printTablesDelta;
			if(printTablesDelta<0)
				NS_ABORT_MSG("ERROR: Valor de intervalo de impresion de tablas no valido");
			m_printTablesInterval = Seconds(printTablesDelta);
			break;
		}
		case PARAM_EXPLORER_INTERVAL_V:{
			double explorerInterval;
			iss>>explorerInterval;
			if(explorerInterval<0)
				NS_ABORT_MSG("ERROR: Valor de intervalo de envio de exploradoras no valido");
			m_explorerAntsInterval = Seconds(explorerInterval);
			break;
		}
		case PARAM_APP_START_TIME_V:{
			int timeMin;
			int timeMax;
			double startTime;
			iss>>timeMin>>timeMax>>startTime;
			if(startTime<1)
				NS_ABORT_MSG("ERROR: Valor de tiempo de inicio no valido");
			if(!IsRangeValid(timeMin,timeMax))
				NS_ABORT_MSG("ERROR: El rango leido en el parametro "<<PARAM_APP_START_TIME<<" no es valido");
			AppTimeFormat timeformat;
			timeformat.appTime = startTime;
			timeformat.min = timeMin;
			timeformat.max = timeMax;
			m_startTimeList.push_back(timeformat);
			break;
		}
		case PARAM_SIMULATOR_STOP_TIME_V:{
			double stopTime;
			iss>>stopTime;
			if(stopTime<1)
				NS_ABORT_MSG("ERROR: Valor de tiempo de finalizacion no valido. Debe ser >=1");
			m_stopTime=stopTime;
			break;
		}
		case PARAM_LINK_DELAY_DIST_V:{
			std::string randomStream;
			iss>>randomStream;
			m_linkDelayChange = ReadRandomStream(randomStream,iss);
			break;
		}
		case PARAM_COMPUTING_DELAY_DIST_V:{
			std::string randomStream;
			int computingMin,computingMax;
			iss>>computingMin>>computingMax>>randomStream;
			if(!IsRangeValid(computingMin,computingMax))
				NS_ABORT_MSG("ERROR: El rango leido en el parametro "<<PARAM_COMPUTING_DELAY_DIST<<" no es valido");
			m_computingDistList.push_back(RandomStreamFormat(ReadRandomStream(randomStream,iss),computingMin,computingMax));
			break;
		}
		case PARAM_COMPUTING_DELAY_INCREMENT_V:{
			float factor;
			int opDelayMin,opDelayMax;
			iss>>opDelayMin>>opDelayMax>>factor;
			if(factor<0 || factor>1)
				NS_ABORT_MSG("ERROR: El factor leido ("<<factor<<") en el parametro "<<PARAM_COMPUTING_DELAY_INCREMENT<<" no es valido");
			if(!IsRangeValid(opDelayMin,opDelayMax))
				NS_ABORT_MSG("ERROR: El rango leido en el parametro "<<PARAM_COMPUTING_DELAY_INCREMENT<<" no es valido");
			m_computingDelayList.push_back(OperationDelayFormat(factor,opDelayMin,opDelayMax));
			break;
		}
		case PARAM_DATA_RATE_V:{
			int dataRate;
			int dataRateMin,dataRateMax;
			iss>>dataRateMin>>dataRateMax>>dataRate;
			if(dataRate<0)
				NS_ABORT_MSG("ERROR: Valor de datarate no valido");
			if(!IsRangeValid(dataRateMin,dataRateMax))
				NS_ABORT_MSG("ERROR: El rango leido en el parametro "<<PARAM_DATA_RATE<<" no es valido");
			m_dataRateList.push_back(DataRateFormat(dataRate,dataRateMin,dataRateMax));
			break;
		}
		case PARAM_SEED_NUMBER_V:{
			int seedNumber;
			iss>>seedNumber;
			if(seedNumber<0)
				NS_ABORT_MSG("ERROR: Valor de semilla no valido");
			RngSeedManager::SetSeed(seedNumber);
			break;
		}
		case PARAM_RUN_NUMBER_V:{
			long int runNumber;
			iss>>runNumber;
			if(runNumber<0)
				NS_ABORT_MSG("ERROR: Valor de ejecucion no valido");
			RngSeedManager::SetRun(runNumber);
			break;
		}
		case PARAM_ENABLE_TRACING_V:{
			bool enableTraces;
			iss>>enableTraces;
			m_enableTraces=enableTraces;
			break;
		}
		case PARAM_ENABLE_EXPLORER_ANTS_V:{
			bool enableExplorerAnts;
			iss>>enableExplorerAnts;
			m_enableExplorerAnts=enableExplorerAnts;
			break;
		}
		case PARAM_QUEUE_SIZE_V:{
			uint64_t queueSize;
			iss>>queueSize;
			if(queueSize<0)
				NS_ABORT_MSG("ERROR: Valor de tamanio de cola no valido");
			m_queueSize = queueSize;
			break;
		}
		case PARAM_MAXIMUM_SEGMENT_SIZE_V:{
			long int mss;
			iss>>mss;
			if(mss<0)
				NS_ABORT_MSG("ERROR: Valor de MaximumSegmentSize no valido");
			m_maximumSegmentSize = (uint64_t)mss;
			break;
		}
		case PARAM_PATH_MANAGER_V:{
			std::string caseName;
			iss>>caseName;
			m_arapPathManager = m_pathManagerFactory.GetInstance(caseName,iss);
			if (m_arapPathManager==NULL)
				NS_ABORT_MSG("ERROR: Valor de ArapPathManager no valido");
			break;
		}
		case PARAM_LOAD_ANT_TIME_DIST_V:{
			std::string randomStream;
			int timeMin,timeMax;
			iss>>timeMin>>timeMax>>randomStream;
			if(!IsRangeValid(timeMin,timeMax))
				NS_ABORT_MSG("ERROR: El rango leido en el parametro "<<PARAM_LOAD_ANT_TIME_DIST<<" no es valido");
			m_loadAntsTimeList.push_back(RandomStreamFormat(ReadRandomStream(randomStream,iss),timeMin,timeMax));
			break;
		}
		case PARAM_LOAD_ANT_QUANTITY_DIST_V:{
			std::string randomStream;
			int quantityMin,quantityMax;
			iss>>quantityMin>>quantityMax>>randomStream;
			if(!IsRangeValid(quantityMin,quantityMax))
				NS_ABORT_MSG("ERROR: El rango leido en el parametro "<<PARAM_LOAD_ANT_QUANTITY_DIST<<" no es valido");
			m_loadAntsQuantityList.push_back(RandomStreamFormat(ReadRandomStream(randomStream,iss),quantityMin,quantityMax));
			break;
		}
		case PARAM_LOAD_ANT_TARGET_DIST_V:{
			std::string randomStream;
			int targetMin,targetMax;
			iss>>targetMin>>targetMax>>randomStream;
			if(!IsRangeValid(targetMin,targetMax))
				NS_ABORT_MSG("ERROR: El rango leido en el parametro "<<PARAM_LOAD_ANT_TARGET_DIST<<" no es valido");
			Ptr<RandomVariableStream> loadAntTargetStream = ReadRandomStream(randomStream,iss);
			ValidateLoadAntTargetStream(loadAntTargetStream,targetMin,targetMax);
			m_loadAntsTargetList.push_back(RandomStreamFormat(loadAntTargetStream,targetMin,targetMax));
			break;
		}
		default:{
			NS_ABORT_MSG("ERROR: Error en el formato del archivo, nombre de parametro desconocido");
		}
	}
}


void
ArapSimulator::ValidateLoadAntTargetStream(Ptr<RandomVariableStream>& loadAntTargetStream, int& targetMin,int& targetMax){
	if(loadAntTargetStream->GetInstanceTypeId().GetName()=="ns3::ConstantRandomVariable"){
		Ptr<ConstantRandomVariable> aux = DynamicCast<ConstantRandomVariable>(loadAntTargetStream);
		int constant = (int)aux->GetConstant();
		if(!IsRangeValid(constant,constant) || constant>=targetMin || constant<=targetMax)
			NS_ABORT_MSG("ERROR: El  valor de la distribucion ConstantRandomVariable para los destinos esta fuera de  rango");
	}
	else if(loadAntTargetStream->GetInstanceTypeId().GetName()=="ns3::UniformRandomVariable"){
		Ptr<UniformRandomVariable> aux = DynamicCast<UniformRandomVariable>(loadAntTargetStream);
		if(!IsRangeValid((int)aux->GetMin(),(int)aux->GetMax()))
			NS_ABORT_MSG("ERROR: El  valor de la distribucion UniformRandomVariable para los destinos esta fuera de  rango");
	}
	else if(loadAntTargetStream->GetInstanceTypeId().GetName()=="ns3::TriangularRandomVariable"){
		Ptr<TriangularRandomVariable> aux = DynamicCast<TriangularRandomVariable>(loadAntTargetStream);
		if(!IsRangeValid((int)aux->GetMin(),(int)aux->GetMax()))
			NS_ABORT_MSG("ERROR: El  valor de la distribucion TriangularRandomVariable para los destinos esta fuera de  rango");
	}
	else if(loadAntTargetStream->GetInstanceTypeId().GetName()=="ns3::ExponentialRandomVariable"){
		Ptr<ExponentialRandomVariable> aux = DynamicCast<ExponentialRandomVariable>(loadAntTargetStream);
		if((uint32_t)aux->GetBound()>=m_numNodes)
			NS_ABORT_MSG("ERROR: El  valor de la distribucion ExponentialRandomVariable para los destinos esta fuera de  rango");
	}
	else if(loadAntTargetStream->GetInstanceTypeId().GetName()=="ns3::NormalRandomVariable"){
		Ptr<NormalRandomVariable> aux = DynamicCast<NormalRandomVariable>(loadAntTargetStream);
		double mean = aux->GetMean();
		double bound = aux->GetBound();
		if(mean-bound<0 || mean+bound>=(double)m_numNodes)
			NS_ABORT_MSG("ERROR: El  valor de la distribucion NormalRandomVariable para los destinos esta fuera de  rango");
	}
}

void
ArapSimulator::ConfigNetwork(){
	PointToPointHelper pointToPoint;
	if(m_numNodes==0)
		NS_ABORT_MSG("ERROR: Debe indicar un valor para el número de nodos");

	m_star = new PointToPointStarHelper(m_numNodes, pointToPoint);
  InternetStackHelper internet;
  m_star->InstallStack(internet);
  m_star->AssignIpv4Addresses (Ipv4AddressHelper (NETWORK_BASE_IP, NETWORK_SUBNET_MASK));

  /* Asignar un DataRate bastante grande a los dispositivos de nodo HUB, para
   * que no genere cuellos de botella extra e innecesarios.
   */
  uint32_t hubDevCount = m_star->GetHub()->GetNDevices();
  for(uint32_t i=0;i<hubDevCount-1;i++){
  	Ptr<PointToPointNetDevice> p2pdev = DynamicCast<PointToPointNetDevice>(m_star->GetHub()->GetDevice(i));
  	DataRateValue dataRateSet(DataRate(HUB_DATA_RATE));
  	p2pdev->SetAttribute("DataRate",dataRateSet);
  }

  //Se activa el routeo global para poder usar el nodo central como router
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  /*Habilitar tracing */
  if(m_enableTraces){
  	AsciiTraceHelper ascii;
  	pointToPoint.EnableAsciiAll (ascii.CreateFileStream ("arap-trace.tr"));
  	pointToPoint.EnablePcapAll("arap-trace",false);
  }
}

void
ArapSimulator::ReadParametersFile(const std::string& filename){
	std::string line, paramName;
	std::ifstream file (filename.c_str());
  if (!file.is_open()) //Si no puede leer el archivo da error
  	NS_ABORT_MSG("No se pudo leer el archivo de configuracion");

  NS_LOG_UNCOND("Leyendo parámetros desde archivo: \""<<filename<<"\"");
  while (std::getline (file,line)) {
  	std::istringstream iss(line);
		if(line.empty()){ //Si la linea esta vacia, pasar a la siguiente
			continue;
		}
		iss>>paramName;
		if(paramName[0]=='#'){ //Si la linea empieza con "#" es un comentario, pasar a la siguiente linea
			continue;
		}
		HandleParameter(paramName, iss);
   }
  file.close();
}

void
ArapSimulator::ConfigSimulator(const std::string& filename){
	InitializeValuesMap(); //Para que se asigne un valor numerico  a cada nombre de parametro
	ReadParametersFile(filename);

  /* Hasta este punto ya se han leido todos los parametros, los cuales ahora se usarán
   * para configurar la red y los nodos. En caso de no haber realizado una configuración
   * completa en el archivo, se detectará el error al momento de tratar de asignar
   * un parámetro faltante
   */
  ConfigNetwork();
  ConfigNodes();
}

const PointToPointStarHelper&
ArapSimulator::GetStar(){
	return *m_star;
}

void
ArapSimulator::ConfigNodes (){
	/* Instalacion de las aplicaciones en las puntas de la estrella
	 * (el nodo central (HUB) se usa solo como router) y configuracion
	 * de los distintos parametros de cada nodo
	 */
	if(m_arapPathManager ==NULL){
		NS_ABORT_MSG("NO SE ASIGNO UNA ESPECIFICACION DE ArapPathManager");
	}

	ApplicationContainer applications;
	for (uint32_t i = 0; i < m_star->SpokeCount (); ++i){
		Ptr<ArapNode> app = CreateObject<ArapNode>();
		m_star->GetSpokeNode(i)->AddApplication(app);
		app->SetPathManager(*m_arapPathManager);
		app->ConfigureSockets();
		applications.Add (app);
	}

	/*Vector para verificar cuales parametros ya fueron asignados en los nodos y cuales no*/
	bool assignCheckVector[m_numNodes];

/***Asignar la variable aleatoria de computo a los nodos segun lo leido en el archivo de parametros*/
	memset(assignCheckVector,0,m_numNodes); //Para inicializarlos en falso
	if(m_computingDistList.empty()){ //Si no se leyo este parametro finaliza la simulacion
		NS_ABORT_MSG("ERROR: NO SE ASIGNARON VALORES PARA EL Delay de Computo");
	}
	for(std::list<RandomStreamFormat>::iterator it= m_computingDistList.begin();it!=m_computingDistList.end();it++){
		RandomStreamFormat item = *it;
		for (int i = item.min; i <= item.max; ++i){
			if(!assignCheckVector[i]){
				Ptr<ArapNode> app = DynamicCast<ArapNode>(m_star->GetSpokeNode(i)->GetApplication(0));
				app->SetComputingDelayStream(item.stream);
				assignCheckVector[i]=true;
			}
			else
				NS_ABORT_MSG("ERROR: El nodo "<<i<<" pertenece a dos rangos, verifique archivo de parametros");
		}
	}
	/* Si ya se han asignado todos los leidos, se revisan cuales nodos no
	 * fueron asignados, si existe alguno que no haya sido asignado finaliza la simulacion
	 */
	for(uint32_t i=0;i<m_numNodes;i++){
		if(!assignCheckVector[i]){
			NS_ABORT_MSG("ERROR: NO SE ASIGNARON VALORES PARA EL Delay de Computo");
		}
	}

/***Asignar el datarate a los nodos segun lo leido en el archivo de parametros*/
	memset(assignCheckVector,0,m_numNodes); //Para inicializarlos en falso
	if(m_dataRateList.empty()){ //Si no se leyo este parametro finaliza la simulacion
		NS_ABORT_MSG("ERROR: NO SE ASIGNARON VALORES PARA EL DataRate");
	}
	for(std::list<DataRateFormat>::iterator it= m_dataRateList.begin();it!=m_dataRateList.end();it++){
		DataRateFormat item = *it;
		for (int i = item.min; i <= item.max; ++i){
			if(!assignCheckVector[i]){
				m_star->GetSpokeNode(i)->GetDevice(0)->SetAttribute("DataRate",DataRateValue(DataRate(item.value)));
				assignCheckVector[i]=true;
			}
			else
				NS_ABORT_MSG("ERROR: El nodo "<<i<<" pertenece a dos rangos, verifique archivo de parametros");
		}
	}
	/* Si ya se han asignado todos los leidos, se revisan cuales nodos no
	 * fueron asignados, si existe alguno que no haya sido asignado finaliza la simulacion
	 */
	for(uint32_t i=0;i<m_numNodes;i++){
		if(!assignCheckVector[i]){
			NS_ABORT_MSG("ERROR: NO SE ASIGNARON VALORES PARA EL DataRate");
		}
	}

/****Asignar el tiempo de inicio de las aplicaciones*/
	memset(assignCheckVector,0,m_numNodes); //Para inicializarlos en falso
	if(m_startTimeList.empty()){ //Si no se leyo este parametro se asigna el valor por defecto
			m_startTimeList.push_back(AppTimeFormat(0,m_numNodes-1,DEFAULT_APP_START_TIME));
			NS_LOG_WARN("Se asigno el valor por defecto de \""PARAM_APP_START_TIME<<"\" a todos los nodos");
	}
	for(std::list<AppTimeFormat>::iterator it= m_startTimeList.begin();it!=m_startTimeList.end();it++){
		AppTimeFormat item = *it;
		for (int i = item.min; i <= item.max; ++i){
			if(!assignCheckVector[i]){
				m_star->GetSpokeNode(i)->GetApplication(0)->SetStartTime(Seconds(item.appTime));
				assignCheckVector[i]=true;
			}
			else
				NS_ABORT_MSG("ERROR: El nodo "<<i<<" pertenece a dos rangos, verifique archivo de parametros");
		}
	}

	/* Si ya se han asignado todos los leidos, se revisan cuales nodos no
	 * fueron asignados, si existe alguno que no haya sido asignado finaliza la simulacion.
	 * El valor por defecto es solo si no se asigna a ninguno de los nodos en este caso.
	 */
	for(uint32_t i=0;i<m_numNodes;i++){
		if(!assignCheckVector[i]){
			NS_ABORT_MSG("ERROR: NO SE ASIGNARON VALORES de AppStartTime A TODOS LOS NODOS");
		}
	}

/***Asignar el "factor de incremento de operacion" a cada nodo, segun los rangos leidos del archivo*/
	memset(assignCheckVector,0,m_numNodes); //Para inicializarlos en falso
	if(m_computingDelayList.empty()){ //Si no se leyo este parametro se asigna el valor por defecto
		m_computingDelayList.push_back(OperationDelayFormat(0,m_numNodes-1,0));
		NS_LOG_WARN("Se asigno el valor por defecto de \""PARAM_COMPUTING_DELAY_INCREMENT<<"\" a todos los nodos");
	}
	for(std::list<OperationDelayFormat>::iterator it= m_computingDelayList.begin();it!=m_computingDelayList.end();it++){
		OperationDelayFormat item = *it;
		for (int i = item.min; i <= item.max; ++i){
			if(!assignCheckVector[i]){
				Ptr<ArapNode> aux = DynamicCast<ArapNode>(m_star->GetSpokeNode(i)->GetApplication(0));
				aux->SetComputingDelayIncrement(item.factor);
				assignCheckVector[i]=true;
			}
			else
				NS_ABORT_MSG("ERROR: El nodo "<<i<<" pertenece a dos rangos, verifique archivo de parametros");
		}
	}
	/* Si ya se han asignado todos los leidos, se revisan cuales nodos no
	 * fueron asignados, si existe alguno que no haya sido asignado finaliza la simulacion.
	 * El valor por defecto es solo si no se asigna a ninguno de los nodos en este caso.
	 */
	for(uint32_t i=0;i<m_numNodes;i++){
		if(!assignCheckVector[i]){
			NS_ABORT_MSG("ERROR: NO SE ASIGNARON VALORES de Factor de Delay de computo A TODOS LOS NODOS");
		}
	}

/***Asignar la variable aleatoria de tiempo de envio de hormigas de carga a los nodos segun lo leido en el archivo de parametros*/
	memset(assignCheckVector,0,m_numNodes); //Para inicializarlos en falso
	if(m_loadAntsTimeList.empty()){ //Si no se leyo este parametro finaliza la simulacion
		NS_ABORT_MSG("ERROR: NO SE ASIGNARON VALORES PARA EL Tiempo de envio de hormigas de carga");
	}
	for(std::list<RandomStreamFormat>::iterator it= m_loadAntsTimeList.begin();it!=m_loadAntsTimeList.end();it++){
		RandomStreamFormat item = *it;
		for (int i = item.min; i <= item.max; ++i){
			if(!assignCheckVector[i]){
				Ptr<ArapNode> app = DynamicCast<ArapNode>(m_star->GetSpokeNode(i)->GetApplication(0));
				app->setLoadAntsTimeStream(item.stream);
				assignCheckVector[i]=true;
			}
			else
				NS_ABORT_MSG("ERROR: El nodo "<<i<<" pertenece a dos rangos, verifique archivo de parametros");
		}
	}
	/* Si ya se han asignado todos los leidos, se revisan cuales nodos no
	 * fueron asignados, si existe alguno que no haya sido asignado finaliza la simulacion
	 */
	for(uint32_t i=0;i<m_numNodes;i++){
		if(!assignCheckVector[i]){
			NS_ABORT_MSG("ERROR: NO SE ASIGNARON VALORES PARA EL Tiempo de envio de hormigas de carga");
		}
	}

/***Asignar la variable aleatoria de cantidad de hormigas de carga a enviar a los nodos segun lo leido en el archivo de parametros*/
	memset(assignCheckVector,0,m_numNodes); //Para inicializarlos en falso
	if(m_loadAntsQuantityList.empty()){ //Si no se leyo este parametro finaliza la simulacion
		NS_ABORT_MSG("ERROR: NO SE ASIGNARON VALORES PARA La cantidad de hormigas de carga por envio");
	}
	for(std::list<RandomStreamFormat>::iterator it= m_loadAntsQuantityList.begin();it!=m_loadAntsQuantityList.end();it++){
		RandomStreamFormat item = *it;
		for (int i = item.min; i <= item.max; ++i){
			if(!assignCheckVector[i]){
				Ptr<ArapNode> app = DynamicCast<ArapNode>(m_star->GetSpokeNode(i)->GetApplication(0));
				app->setLoadAntsQuantityStream(item.stream);
				assignCheckVector[i]=true;
			}
			else
				NS_ABORT_MSG("ERROR: El nodo "<<i<<" pertenece a dos rangos, verifique archivo de parametros");
		}
	}
	/* Si ya se han asignado todos los leidos, se revisan cuales nodos no
	 * fueron asignados, si existe alguno que no haya sido asignado finaliza la simulacion
	 */
	for(uint32_t i=0;i<m_numNodes;i++){
		if(!assignCheckVector[i]){
			NS_ABORT_MSG("ERROR: NO SE ASIGNARON VALORES PARA La cantidad de hormigas de carga por envio");
		}
	}

/***Asignar la variable aleatoria para los destinos de hormigas de carga a enviar a los nodos segun lo leido en el archivo de parametros*/
	memset(assignCheckVector,0,m_numNodes); //Para inicializarlos en falso
	if(m_loadAntsTargetList.empty()){ //Si no se leyo este parametro finaliza la simulacion
		NS_ABORT_MSG("ERROR: NO SE ASIGNARON VALORES PARA los destinos de hormigas de carga por envio");
	}
	for(std::list<RandomStreamFormat>::iterator it= m_loadAntsTargetList.begin();it!=m_loadAntsTargetList.end();it++){
		RandomStreamFormat item = *it;
		for (int i = item.min; i <= item.max; ++i){
			if(!assignCheckVector[i]){
				Ptr<ArapNode> app = DynamicCast<ArapNode>(m_star->GetSpokeNode(i)->GetApplication(0));
				app->setLoadAntsTargetStream(item.stream);
				assignCheckVector[i]=true;
			}
			else
				NS_ABORT_MSG("ERROR: El nodo "<<i<<" pertenece a dos rangos, verifique archivo de parametros");
		}
	}
	/* Si ya se han asignado todos los leidos, se revisan cuales nodos no
	 * fueron asignados, si existe alguno que no haya sido asignado finaliza la simulacion
	 */
	for(uint32_t i=0;i<m_numNodes;i++){
		if(!assignCheckVector[i]){
			NS_ABORT_MSG("ERROR: NO SE ASIGNARON VALORES PARA los destinos de hormigas de carga por envio");
		}
	}

/*Si no se leyo el parametro de cambio de delay de los enlaces finaliza la simulacion*/
	if(m_linkDelayChange==NULL){
		NS_ABORT_MSG("ERROR: NO SE ASIGNO UN VALOR PARA EL TIEMPO DE CAMBIO DE LOS ENLACES");
	}

	/*Asignar la cantidad máxima de paquetes que pueden haber en cola en cada uno de los nodos*/
	if(m_queueSize==0)
		NS_ABORT_MSG("ERROR: NO SE ASIGNO UN VALOR PARA EL Tamanio Maximo de Cola de Paquetes");

  uint32_t hubDevCount = m_star->GetHub()->GetNDevices();
  for(uint32_t i=0;i<hubDevCount-1;i++){
  	Ptr<PointToPointNetDevice> p2pdev = DynamicCast<PointToPointNetDevice>(m_star->GetHub()->GetDevice(i));
  	p2pdev->GetQueue()->SetAttribute("MaxPackets",UintegerValue(m_queueSize));
  }


  for(uint32_t i=0;i<m_star->SpokeCount();i++){
  	Ptr<PointToPointNetDevice> p2pdev = DynamicCast<PointToPointNetDevice>(m_star->GetSpokeNode(i)->GetDevice(0));
    p2pdev->GetQueue()->SetAttribute("MaxPackets",UintegerValue(m_queueSize));
  }


	/*Asignar el tiempo de finalizacion de la simulacion*/
	if(m_stopTime==0)
		NS_ABORT_MSG("ERROR: NO SE ASIGNO UN VALOR PARA EL Tiempo de simulacion");
	Simulator::Stop(Seconds(m_stopTime));
}

const Time&
ArapSimulator::GetExplorersDelta(){
	return m_explorerAntsInterval;
}

const uint32_t
ArapSimulator::GetNumHops(){
	return m_numHops;
}

const uint32_t
ArapSimulator::GetNumNodes(){
	return m_numNodes;
}

const uint32_t
ArapSimulator::GetAntsSize(){
	return m_antSize;
}

const uint16_t
ArapSimulator::GetPort(){
	return m_port;
}

bool
ArapSimulator::IsExplorerAntsEnabled(){
	return m_enableExplorerAnts;
}
const uint64_t
ArapSimulator::GetMaximumSegmentSize(){
	return m_maximumSegmentSize;
}

void
ArapSimulator::Run(){
	ChangeLinkDelay();
	PrintProbabilityTables();
	Simulator::Run();
}

bool
ArapSimulator::IsRangeValid(const int min,const int max){
	uint32_t  minAbs= abs(min);
	uint32_t  maxAbs = abs(max);
	if(minAbs>maxAbs || (minAbs<0 || minAbs>=m_numNodes) || (maxAbs<0 || maxAbs>=m_numNodes))
		return false;
	else
		return true;
}

void
ArapSimulator::ChangeLinkDelay(){
	for (uint32_t i = 0; i < m_star->SpokeCount (); ++i){
		double newDelay=m_linkDelayChange->GetValue();
		m_star->GetHub()->GetDevice(i)->GetChannel()->SetAttribute("Delay", TimeValue(MilliSeconds(newDelay)));
	}
	Simulator::Schedule(m_linkDelayInterval,&ArapSimulator::ChangeLinkDelay);
}

void ArapSimulator::PrintProbabilityTables(){
	for(uint32_t i=0;i< m_star->SpokeCount();i++){
		Ptr<ArapNode> nodeApp = DynamicCast<ArapNode>(m_star->GetSpokeNode(i)->GetApplication(0));
		nodeApp->PrintProbTable();
	}
	Simulator::Schedule(m_printTablesInterval,&ArapSimulator::PrintProbabilityTables);
}

void
ArapSimulator::PrintLoadAntsStatistics(){

	std::string name;
	double sumMeans =0;
	double sumSquareMeans =0;
	uint64_t totalSamples = 0;

	/* Creación del nombre de archivo con formato <IP_LOCAL>_<exploradoras habilitadas>_<Semilla>_<RunNumber> (segundos)>.csv*/
	std::ostringstream oss;
	oss<<"load-ants-model/"; //Carpeta donde se crearan los archivos de salida (dentro del Working Directory)
	if(IsExplorerAntsEnabled())
		oss<<"explorer-on_";
	else
		oss<<"explorer-off_";
	oss<<RngSeedManager::GetSeed()<<"_"<<RngSeedManager::GetRun()<<".csv";
	name = oss.str();
	std::ofstream file (name.c_str());
	if (!file.is_open()){
		NS_LOG_INFO("Fallo el crear el archivo de nombre: "+name);
		return;
	}
	//Imprimir la cabecera de la tabla
	file<<" "; //EL primer espacio para separar las columnas de las filas
	for(uint32_t i=0;i< m_star->SpokeCount();i++){
		Ptr<ArapNode> nodeApp = DynamicCast<ArapNode>(m_star->GetSpokeNode(i)->GetApplication(0));
		file<<","<<nodeApp->GetLocalIP();
	}
	file<<",\"Hormigas enviadas\"";
	file<<",Media";
	file<<",Varianza";
	file<<"\n";

	//Impresion al archivo linea por linea
	for(uint32_t i=0;i< m_star->SpokeCount();i++){
		Ptr<ArapNode> nodeApp = DynamicCast<ArapNode>(m_star->GetSpokeNode(i)->GetApplication(0));
		const LoadAntsStatistics& LoadAntsDelayModel = nodeApp->GetLoadAntsDelayModel();
		const std::map<Ipv4Address,uint64_t>& samplesCountMap = LoadAntsDelayModel.GetSampleCountsMap();
		file<<nodeApp->GetLocalIP();

		for (std::map<Ipv4Address,uint64_t>::const_iterator it = samplesCountMap.begin();it!=samplesCountMap.end(); it++) {
			file<<","<<it->second;
		}
		file<<","<<LoadAntsDelayModel.GetNumSamples();
		file<<","<<LoadAntsDelayModel.GetMean();
		file<<","<<LoadAntsDelayModel.GetVariance();
		file<<"\n";

		//Calculos de los valores totales para agregar al final del archivo
		totalSamples = totalSamples + LoadAntsDelayModel.GetNumSamples();
		sumMeans = sumMeans + LoadAntsDelayModel.GetMean();
		sumSquareMeans = sumSquareMeans + (LoadAntsDelayModel.GetMean()*LoadAntsDelayModel.GetMean());
	}
	//Imprimir al final los valores de interes
	file<<"\"Total de hormigas enviadas\","<<totalSamples<<"\n";
	file<<"\"Suma de las medias de los tiempos\","<<sumMeans<<"\n";
	double mean = sumMeans/m_numNodes;
	file<<"\"Media de las medias de los tiempos\","<<mean<<"\n";
	double squareXHope = sumSquareMeans/m_numNodes; //Esperanza de X^2 (E(X²))
	double xHopeSquare = mean*mean; //Esperanza al cuadrado de X([E(X)]²)
	file<<"\"Varianza de las medias de los tiempos\","<<(squareXHope - xHopeSquare);
	file.close();
}

void
ArapSimulator::Destroy(){
	delete m_arapPathManager;
	NS_LOG_UNCOND("Cantidad total de Hormigas creadas: "<<ArapAnts::GetAntID());
	PrintProbabilityTables();
	PrintLoadAntsStatistics();
	delete m_star;
	m_computingDelayList.clear();
	m_computingDistList.clear();
	m_dataRateList.clear();
	m_startTimeList.clear();
	Simulator::Destroy();
}

} // namespace ns3
