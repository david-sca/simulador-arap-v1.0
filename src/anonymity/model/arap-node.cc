/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "arap-node.h"
#include "ns3/log.h"
#include "ns3/abort.h"
#include "ns3/uinteger.h"
#include "ns3/simulator.h"
#include "ns3/drop-tail-queue.h"
#include "ns3/point-to-point-module.h"
#include "ns3/tcp-socket-factory.h"
#include "ns3/rng-seed-manager.h"
#include <string>

#define SEND_BUFFER_SIZE 104857600 //100 MB
namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("ArapNode");

TypeId
ArapNode::GetTypeId (void) {
	static TypeId tid = TypeId ("ArapNode")
		.SetParent<Application> ();
	return tid;
}

ArapNode::ArapNode()
: m_socketServer(0), m_localIP("0.0.0.0"), m_computingDelayIncrement(0), m_pathManager(0) {
}

ArapNode::~ArapNode (){
}

void
ArapNode::SetComputingDelayStream(Ptr<RandomVariableStream> computingDelayStream){
	m_computingDelayStream = computingDelayStream;
}

void
ArapNode::SetComputingDelayIncrement(const float computingDelayIncrement){
	m_computingDelayIncrement = computingDelayIncrement;
}

void
ArapNode::SetPathManager(const ArapPathManager& pathManager){
	m_pathManager = pathManager.GetCopy();
	m_pathManager->CreateProbTable(GetLocalIP());
}

void
ArapNode::DoDispose (void) {
  m_socketServer =0;
  m_socketClientMap.clear();
  delete m_pathManager;
  Application::DoDispose();
}

void
ArapNode::ConfigureSockets(){
	m_localIP =GetNode()->GetObject<Ipv4>()->GetAddress(1,0).GetLocal();
	TypeId tid = ns3::TcpSocketFactory::GetTypeId();
	if (m_socketServer == 0) {
		m_socketServer = Socket::CreateSocket (GetNode (), tid);
		InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny(), ArapSimulator::GetPort());
		m_socketServer->Bind(local);
		m_socketServer->Listen();
		m_socketServer->ShutdownSend(); //Este socket no envia (Es servidor)
		NS_LOG_LOGIC ("El socket servidor en el nodo "<<m_localIP<<" esta escuchando");
		/* Se asigna el callback para aceptar conexiones, el callback para recibir
		 * data se asigna en el callback cuando se acepta la conexion
		 */
		m_socketServer->SetAcceptCallback(MakeCallback(&ArapNode::ServerConnectRequestCallback,this),MakeCallback(&ArapNode::ServerConnectAcceptCallback,this));
	}

	if (m_socketClientMap.empty()){
		const PointToPointStarHelper& star=ArapSimulator::GetStar();
		for (uint32_t i = 0; i < star.SpokeCount (); ++i){
			Ipv4Address IP = star.GetSpokeIpv4Address(i);
			if(IP==m_localIP) //Para que no cree un Loop a mi mismo
				continue;
			Ptr<Socket> client = Socket::CreateSocket (GetNode(), tid);
			client->SetAttribute("SegmentSize",UintegerValue(ArapSimulator::GetMaximumSegmentSize()));

			/* Se asigna el valor de buffer de envio, un valor grande (Según la cantidad
			 * de nodos y el tráfico a generar) para evitar que se llene y no se puedan
			 * enviar hormigas.
			 */
			client->SetAttribute("SndBufSize",UintegerValue(SEND_BUFFER_SIZE));

			client->Bind(); //No se asigna a un puerto especifico ya que es cliente
			client->ShutdownRecv(); //Este socket no recibe (Es cliente)
			//client->SetSendCallback(MakeCallback(&ArapNode::ClientSendCallback,this));
			client->Connect(InetSocketAddress(IP,ArapSimulator::GetPort()));
			m_socketClientMap[IP]=client;
		}
	}
}

void
ArapNode::StartApplication (void) {
	m_loadAntsDelayModel.InitializeTable();

	/* Inicializar los envios de hormigas de carga, en esta version
	 * el "mensaje" no es relevante, por lo que se manda una cadena cualquiera.
	 */
	ScheduleLoadAntSend("mensaje");

	//Iniciar el envio de exploradoras, solo si están habilitadas
	if(ArapSimulator::IsExplorerAntsEnabled())
		ExplorePaths();
}

void
ArapNode::StopApplication (void) {
	/* Actualmente no se esta llamando este metodo
	 * ya que se esta finalizando la simulacion como tal,
	 * es posible que este metodo sea util si se llega a usar
	 * una topologia "dinamica", donde los nodos se activen
	 * y desactiven periodicamente.
	 */
  m_socketServer->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
  m_socketServer->Close();
  for(std::map<Ipv4Address,Ptr<Socket> >::iterator it = m_socketClientMap.begin();it!=m_socketClientMap.end();it++)
		it->second->Close();
}

void
ArapNode::ExplorePaths(){
	const PointToPointStarHelper& star=ArapSimulator::GetStar();
	for(uint32_t i=0; i<star.SpokeCount();i++){
		Ptr<ArapNode>app = DynamicCast<ArapNode>(star.GetSpokeNode(i)->GetApplication(0));
		if(app->GetLocalIP()==GetLocalIP())
			continue;
		SendExplorerAnts(app->GetLocalIP());
	}
	Simulator::Schedule(ArapSimulator::GetExplorersDelta(),&ArapNode::ExplorePaths,this);
}

const Ipv4Address
ArapNode::GetLocalIP() const{
	if(m_localIP=="0.0.0.0")
		return GetNode()->GetObject<Ipv4>()->GetAddress(1,0).GetLocal();
	else
		return m_localIP;
}

const LoadAntsStatistics&
ArapNode::GetLoadAntsDelayModel() const{
	return m_loadAntsDelayModel;
}

void
ArapNode::PrintProbTable(){
	m_pathManager->PrintProbTable(GetLocalIP());
}

void
ArapNode::SendExplorerAnts(const Ipv4Address& target){
	const PointToPointStarHelper& star=ArapSimulator::GetStar();
	for(uint32_t i=0; i< star.SpokeCount();i++){
		Ptr<ArapNode> app = DynamicCast<ArapNode>(star.GetSpokeNode(i)->GetApplication(0));
		if(app->GetLocalIP()==GetLocalIP() || app->GetLocalIP()==target)
			continue;
		uint64_t packetID = m_arapAnt.CreateExplorerAnt(target);
		Ptr<Packet> packet = Create<Packet> (m_arapAnt.GetBuffer(),ArapSimulator::GetAntsSize());
		Simulator::Schedule(MilliSeconds(GetComputingDelay()), &ArapNode::SendDelayedAnt, this,packet, GetLocalIP(),app->GetLocalIP(),packetID,true);
	}
}

Ptr<RoutingTableRow>
ArapNode::SearchRoutingTable(const uint64_t& antID){
	if(m_routingTable.count(antID)==1)
		return m_routingTable[antID];
	else
		return NULL;
}

void
ArapNode::ReceiveAnt(Ptr<Socket> socket) {
	Ptr<Packet> ant;
	Address socketAddress;
	Ipv4Address source;
	uint32_t antSize =ArapSimulator::GetAntsSize();
	while ((ant = socket->RecvFrom (antSize, 0, socketAddress))){
		ant->RemoveAllPacketTags ();
		ant->RemoveAllByteTags ();
		uint8_t* data = new uint8_t[antSize];
		ant->CopyData(data,antSize);
		uint64_t antID = ArapAnts::ReadAntID(data);
		source =  InetSocketAddress::ConvertFrom(socketAddress).GetIpv4();
		NS_LOG_LOGIC("RECEIVE_LOG -  En el tiempo "<<Simulator::Now().GetSeconds()<< "s el nodo "<<GetLocalIP()
				<<" recibio una hormiga con ID "<<antID <<" y tamanio "<<ant->GetSize ()<<" bytes desde "
				<<source<< " por el puerto: "<<InetSocketAddress::ConvertFrom(socketAddress).GetPort ());

		/*Manejar los diferentes casos de las hormigas recibidas*/
		Ptr<RoutingTableRow> routingRow = SearchRoutingTable(antID);
		if(routingRow!=NULL){ //Si es verdadero es porque si existe: Casos Response
			NS_ASSERT_MSG(routingRow->GetTargetIP()==source,"Hay un error en la tabla de rutas del nodo "<<GetLocalIP());
			if(routingRow->GetSourceIP()==GetLocalIP()){
				HandleRespFinal(data, source, antID);
			}
			else {
				HandleRespMed(data,routingRow->GetSourceIP(),antID);
			}
		}
		else{ //Cuando no existe la entrada: Casos Request
			std::string nodeType = ArapAnts::ReadNodeType(data);
			if(nodeType==ArapAnts::NODE_TYPE_MEDIUM){
				HandleReqMed(data,source,antID);
			}else if(nodeType==ArapAnts::NODE_TYPE_FINAL){
				HandleReqFinal(data,source,antID);
			}
		}
		delete[] data;
	} //END WHILE
}

void
ArapNode::HandleReqMed(const uint8_t* data, const Ipv4Address& source, const uint64_t antID){
	Ipv4Address target = ArapAnts::ReadNextJumpIP(data);
	uint8_t* dataAux = new uint8_t[ArapSimulator::GetAntsSize()];
	m_arapAnt.CreateMediumAnt(data,dataAux);
	Ptr<Packet> ant = Create<Packet> (dataAux,ArapSimulator::GetAntsSize());
	delete [] dataAux;
	Simulator::Schedule(MilliSeconds(GetComputingDelay()), &ArapNode::SendDelayedAnt, this,ant, source,target,antID,true);
}

void
ArapNode::HandleReqFinal(const uint8_t* data, const Ipv4Address& source, const uint64_t antID){
	const ArapAnts::AntType antType = ArapAnts::ReadAntType(data);
	Ptr<Packet> ant;
	if(antType==ArapAnts::ANT_EXPLORER){
		ant = Create<Packet> (data,ArapSimulator::GetAntsSize());
	}
	else if(antType==ArapAnts::ANT_LOAD){
		uint64_t sendTime = ArapAnts::ReadSendTime(data);
		const Ipv4Address target = ArapAnts::ReadTargetIP(data);
		uint32_t msgSize = ArapAnts::ReadMessageSize(data);
		std::string message = ArapAnts::ReadMessage(data,msgSize);
		std::ostringstream oss;
		oss<<"Final del recorrido para la hormiga con ID "<<antID<<" enviado al NODO con IP "<<target;
		std::string answer = oss.str();
		uint8_t* answerData = new uint8_t[ArapSimulator::GetAntsSize()];
		m_arapAnt.CreateAnswerAnt(answerData,sendTime,target,antID,answer);
		ant = Create<Packet> (answerData,ArapSimulator::GetAntsSize());
		delete[] answerData;
	}
	/* En este caso ni se agregan ni eliminan filas a la tabla de enrutamiento,
	 * pero como se debe pasar el parametro, se pasa como false, aunque en realidad
	 * no se hace nigun cambio a la tabla.
	 */
	Simulator::Schedule(MilliSeconds(GetComputingDelay()), &ArapNode::SendDelayedAnt, this,ant, source,source,antID,false);
}

void
ArapNode::HandleRespMed(const uint8_t* data, const Ipv4Address& target, const uint64_t antID){
	Ptr<Packet> ant = Create<Packet> (data,ArapSimulator::GetAntsSize());
	Simulator::Schedule(MilliSeconds(GetComputingDelay()), &ArapNode::SendDelayedAnt, this,ant, target,target,antID,false);
}

void
ArapNode::HandleRespFinal(const uint8_t* data, const Ipv4Address& medium, const uint64_t antID){
	const ArapAnts::AntType antType = ArapAnts::ReadAntType(data);
	if(antType==ArapAnts::ANT_EXPLORER){
		NS_LOG_LOGIC("Se recibio una hormiga exploradora con ID "<<antID<<" de vuelta al nido "<<GetLocalIP());
		const uint64_t time = ArapAnts::ReadSendTime(data);
		const Ipv4Address target = ArapAnts::ReadTargetIP(data);
		Time sendTime(time);
		Time rtt = Simulator::Now()-sendTime; //Calculo del delta
		m_pathManager->HandleExplorer(target,medium,rtt);
	}
	else if(antType==ArapAnts::ANT_LOAD){
		NS_LOG_LOGIC("Se recibio una hormiga de carga con ID "<<antID<<" de vuelta al nido "<<GetLocalIP());
		const uint64_t time = ArapAnts::ReadSendTime(data);
		const Ipv4Address target = ArapAnts::ReadTargetIP(data);
		Time sendTime(time);
		Time rtt = Simulator::Now()-sendTime; //Calculo del delta
		uint32_t msgSize = ArapAnts::ReadMessageSize(data);
		std::string message = ArapAnts::ReadMessage(data,msgSize);
		m_loadAntsDelayModel.UpdateModel(rtt.GetSeconds(),target);
	}
	//Eliminar la fila de la tabla de enrutamiento
	m_routingTable.erase(antID);
}

void
ArapNode::ScheduleLoadAntSend(const std::string& message){
	Time dt(Seconds(m_LoadAntsTimeStream->GetValue()));
	Ipv4Address target;
	do{
		uint32_t nodeIndex = m_LoadAntsTargetStream->GetInteger();
		Ptr<ArapNode> nodeApp = DynamicCast<ArapNode>(ArapSimulator::GetStar().GetSpokeNode(nodeIndex)->GetApplication(0));
		target = nodeApp->GetLocalIP();
	}while(target==GetLocalIP()); //Para evitar que se envie hormigas a si mismo

	uint32_t quantity = m_LoadAntsQuantityStream->GetInteger();
	for(uint32_t i=0;i<quantity;i++){
		Simulator::Schedule (dt, &ArapNode::SendLoadAnt, this, target,message);
	}
	Simulator::Schedule (dt, &ArapNode::ScheduleLoadAntSend, this,"mensaje");
}

bool
ArapNode::ServerConnectRequestCallback (Ptr<Socket> socket, const Address& from){
	NS_LOG_INFO("Solicitud de conexion a "<<GetLocalIP()<<" desde: "<< InetSocketAddress::ConvertFrom (from).GetIpv4 ());
	return true;
}

void
ArapNode::ServerConnectAcceptCallback(Ptr<Socket> socket, const Address& from){
	NS_LOG_INFO("Se acepto una conexion a "<<GetLocalIP()<<" desde: "<<InetSocketAddress::ConvertFrom (from).GetIpv4 ());
	socket->SetRecvCallback (MakeCallback (&ArapNode::ReceiveAnt,this));
}

void
ArapNode::ClientSendCallback(Ptr<Socket> socket, uint32_t size){
  //TODO Definir
	/* Es funcion se llama cuando se ha liberado espacio del buffer de envio
	 * actualmente no se usa ya que no es posible indicarle que reintente el envio de
	 * una misma hormiga, por lo cual se esta asignando un valor tamaño de buffer "grande"
	 * para evitar que se llene.
 	 */
	NS_LOG_FUNCTION(this<<socket<<size<<" IP "<<GetLocalIP());
}

void
ArapNode::SendLoadAnt(const Ipv4Address& target, const std::string& message) {
	std::list<Ipv4Address> path = m_pathManager->CreatePath(target);
	uint64_t antID = m_arapAnt.CreateLoadAnt(path,message);
	Ptr<Packet> ant = Create<Packet> (m_arapAnt.GetBuffer(),ArapSimulator::GetAntsSize());
	Simulator::Schedule(MilliSeconds(GetComputingDelay()), &ArapNode::SendDelayedAnt, this,ant, GetLocalIP(),path.front(),antID,true);
	PrintLoadAntPath(path);
}

void
ArapNode::PrintLoadAntPath(std::list<Ipv4Address>& path){
	std::fstream pathsFile;
	std::ostringstream oss;
	oss<<"load-ants-paths/"; //Carpeta donde se crearan los archivos de salida (dentro del Working Directory)
	oss<<"paths_";
	if(ArapSimulator::IsExplorerAntsEnabled())
		oss<<"explorer-on_";
	else
		oss<<"explorer-off_";
	oss<<RngSeedManager::GetSeed()<<"_"<<RngSeedManager::GetRun()<<".csv";
	pathsFile.open(oss.str().c_str(),std::ios_base::out | std::ios_base::app);
	if(pathsFile.is_open()){
		pathsFile<<Simulator::Now().GetSeconds();
		pathsFile<<","<<GetLocalIP();
		for(std::list<Ipv4Address>::iterator it = path.begin();it!=path.end();it++)
			pathsFile<<","<<*it;
		pathsFile<<"\n";
		pathsFile.close();
	}
}

void
ArapNode::setLoadAntsQuantityStream(Ptr<RandomVariableStream> loadAntsQuantityStream) {
	m_LoadAntsQuantityStream = loadAntsQuantityStream;
}

void
ArapNode::setLoadAntsTargetStream(Ptr<RandomVariableStream> loadAntsTargetStream) {
	m_LoadAntsTargetStream = loadAntsTargetStream;
}

void
ArapNode::setLoadAntsTimeStream(Ptr<RandomVariableStream> loadAntsTimeStream) {
	m_LoadAntsTimeStream = loadAntsTimeStream;
}

const uint64_t
ArapNode::GetComputingDelay() const {
	return (m_computingDelayStream->GetValue()*(1+m_computingDelayIncrement));
}

void
ArapNode::SendDelayedAnt(Ptr<Packet> ant, const Ipv4Address& source, const Ipv4Address& target, const uint64_t& antID,bool isRequest){
	int value = m_socketClientMap[target]->Send(ant);
	if(value==-1){
		NS_ABORT_MSG("FALLO EL ENVIO DEL PAQUETE. VALOR DE ERRNO: "<<m_socketClientMap[target]->GetErrno());
		return; //No deberia llegar nunca a este punto
	}

	if(isRequest){ //Si es Request agrega la fila a la tabla de rutas
		m_routingTable[antID]= Create<RoutingTableRow>(source,target,antID);
	}
	else{ //Si NO es Request (es response) elimina la fila de la tabla de rutas
		m_routingTable.erase(antID);
	}
	NS_LOG_LOGIC("SEND_LOG - En el tiempo "<<Simulator::Now().GetSeconds()<<"s, el nodo "<<GetLocalIP()<<" envio una hormiga con ID "<<antID<<" y tamanio "<<ant->GetSize ()<<" bytes a "<<target);
}

} /*namespace ns3*/
