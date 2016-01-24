/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "arap-ants.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/abort.h"
#include "ns3/arap-simulator.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("ArapAnts");

uint64_t ArapAnts::m_antID= 0;

const uint32_t ArapAnts::NODE_TYPE_SIZE = 5;
const std::string ArapAnts::NODE_TYPE_MEDIUM = "MEDIO";
const std::string ArapAnts::NODE_TYPE_FINAL = "FINAL";

/*Offsets para manejar los desplazamientos de memoria al momento de leer y crear las hormigas*/
const uint32_t ArapAnts::m_offsetAntID = sizeof(uint64_t);
const uint32_t ArapAnts::m_offsetNodeType = m_offsetAntID + ArapAnts::NODE_TYPE_SIZE;
const uint32_t ArapAnts::m_offsetNextJumpIP= m_offsetNodeType+ IPV4_SIZE;
const uint32_t ArapAnts::m_offsetLayerSize = m_offsetNextJumpIP + sizeof(uint32_t);
const uint32_t ArapAnts::m_offsetAntType = m_offsetNodeType+ sizeof(ArapAnts::AntType);
const uint32_t ArapAnts::m_offsetSendTime = m_offsetAntType + sizeof(uint64_t);
const uint32_t ArapAnts::m_offsetTargetIP = m_offsetSendTime + IPV4_SIZE;
const uint32_t ArapAnts::m_offsetMessageSize = m_offsetTargetIP+ sizeof(uint32_t);

ArapAnts::ArapAnts() {
	m_buffer = new uint8_t[ArapSimulator::GetAntsSize()];
}

ArapAnts::~ArapAnts() {
	delete[] m_buffer;
}

const uint64_t
ArapAnts::GetAntID(){
	return m_antID;
}

const uint8_t*
ArapAnts::GetBuffer() const {
	return m_buffer;
}

const uint64_t
ArapAnts::ReadAntID(const uint8_t* data) {
	uint64_t antID;
	memcpy(&antID,data,sizeof(uint64_t));
	return antID;
}

const std::string
ArapAnts::ReadNodeType(const uint8_t* data) {
	std::string nodeType = std::string((char*)data+m_offsetAntID,ArapAnts::NODE_TYPE_SIZE);
	return nodeType;
}

const Ipv4Address
ArapAnts::ReadNextJumpIP(const uint8_t* data) {
	Ipv4Address nextJumpIP = Ipv4Address::Deserialize(data+m_offsetNodeType);
	return nextJumpIP;
}

const uint32_t
ArapAnts::ReadLayerSize(const uint8_t* data) {
	uint32_t layerSize;
	memcpy(&layerSize,data+m_offsetNextJumpIP,sizeof(uint32_t));
	return layerSize;
}

const ArapAnts::AntType
ArapAnts::ReadAntType(const uint8_t* data) {
	ArapAnts::AntType antType;
	memcpy(&antType,data+m_offsetNodeType,sizeof(ArapAnts::AntType));
	return antType;
}

const uint64_t
ArapAnts::ReadSendTime(const uint8_t* data) {
	uint64_t sendTime;
	memcpy(&sendTime,data+m_offsetAntType,sizeof(uint64_t));
	return sendTime;
}

const Ipv4Address
ArapAnts::ReadTargetIP(const uint8_t* data) {
	Ipv4Address targetIP = Ipv4Address::Deserialize(data+m_offsetSendTime);
	return targetIP;
}

const uint32_t
ArapAnts::ReadMessageSize(const uint8_t* data) {
	uint32_t messageSize;
	memcpy(&messageSize,data+m_offsetTargetIP,sizeof(uint32_t));
	return messageSize;
}

const std::string
ArapAnts::ReadMessage(const uint8_t* data, const uint32_t messageSize) {
	std::string message = std::string((char*)data+m_offsetMessageSize,messageSize);
	return message;
}

const uint64_t
ArapAnts::CreateLoadAnt(std::list<Ipv4Address>& path, const std::string& message) {
	uint32_t dataSize;
	uint8_t* data=NULL;
	uint64_t currentTime = Simulator::Now().GetTimeStep();
	dataSize = CreateLoadAntData(data, currentTime,path.back(),message);
	uint32_t headerSizeMedium= m_offsetLayerSize;
	uint32_t headerSizeFinal= m_offsetNodeType+dataSize;
	uint32_t usefullBytes = headerSizeMedium*(path.size()-1)+headerSizeFinal;

	NS_ABORT_MSG_IF(usefullBytes > ArapSimulator::GetAntsSize(),
									"El tamanio del paquete a crear excede el tamanio maximo del paquete en ArapPacket::CreateLoadAnt");

	uint32_t layerIndex=1;
	uint8_t* bufferPtr = NULL;
	uint64_t packetID= ++m_antID;
	/*Escribir la data de las capas 1 a N-1 ( Todas menos la que llega el nodo final) */
	for(std::list<Ipv4Address>::iterator it = ++path.begin(); it!=path.end();it++){
		bufferPtr = m_buffer+((layerIndex-1)*headerSizeMedium);
		memcpy(bufferPtr,&packetID,sizeof(uint64_t));
		bufferPtr = m_buffer + m_offsetAntID+((layerIndex-1)*headerSizeMedium);
		memcpy(bufferPtr,ArapAnts::NODE_TYPE_MEDIUM.c_str(),ArapAnts::NODE_TYPE_SIZE);
		bufferPtr = m_buffer + m_offsetNodeType+((layerIndex-1)*headerSizeMedium);
		(*it).Serialize(bufferPtr);
		bufferPtr = m_buffer + m_offsetNextJumpIP+((layerIndex-1)*headerSizeMedium);
		uint32_t tamCapa = headerSizeMedium*(path.size()-1-layerIndex)+headerSizeFinal;
		memcpy(bufferPtr,&tamCapa,sizeof(uint32_t));
		bufferPtr = m_buffer + m_offsetLayerSize+((layerIndex-1)*headerSizeMedium);
		layerIndex++;
	}

	/*Escribir la capa que llega al nodo final*/
	bufferPtr = m_buffer+((layerIndex-1)*headerSizeMedium);
	memcpy(bufferPtr,&packetID,sizeof(uint64_t));
	bufferPtr = m_buffer+m_offsetAntID+((layerIndex-1)*headerSizeMedium);
	memcpy(bufferPtr,ArapAnts::NODE_TYPE_FINAL.c_str(),ArapAnts::NODE_TYPE_SIZE);
	bufferPtr = m_buffer+m_offsetNodeType+((layerIndex-1)*headerSizeMedium);
	memcpy(bufferPtr,data,dataSize);

	/*Hacer el fill*/
	memset(bufferPtr+dataSize,0,ArapSimulator::GetAntsSize()-usefullBytes);
	delete[] data;
	return m_antID;
}

const uint64_t
ArapAnts::CreateExplorerAnt(const Ipv4Address& destino){
	uint32_t explorerHops= 2;
	uint32_t dataSize;
	uint8_t* data=NULL;
	dataSize = CreateExplorerAntData(data,destino);

	uint32_t headerSizeMedio= m_offsetLayerSize;
	uint32_t headerSizeFinal= m_offsetNodeType+dataSize;
	uint32_t usefullBytes = headerSizeMedio+headerSizeFinal;
	NS_ABORT_MSG_IF(usefullBytes > ArapSimulator::GetAntsSize(),
									"El tamanio del paquete a crear excede el tamanio maximo del paquete en ArapPacket::CreateExplorerAnt");
	uint32_t layerIndex=1;
	uint8_t* bufferPtr = NULL;
	uint64_t packetID= ++m_antID;

	/*Crear capa para el nodo intermedio*/
	bufferPtr = m_buffer+((layerIndex-1)*headerSizeMedio);
	memcpy(bufferPtr,&packetID,sizeof(uint64_t));
	bufferPtr = m_buffer + m_offsetAntID+((layerIndex-1)*headerSizeMedio);
	memcpy(bufferPtr,ArapAnts::NODE_TYPE_MEDIUM.c_str(),ArapAnts::NODE_TYPE_SIZE);
	bufferPtr = m_buffer + m_offsetNodeType+((layerIndex-1)*headerSizeMedio);
	destino.Serialize(bufferPtr);
	bufferPtr = m_buffer + m_offsetNextJumpIP+((layerIndex-1)*headerSizeMedio);
	uint32_t tamCapa = headerSizeMedio*(explorerHops-1-layerIndex)+headerSizeFinal;
	memcpy(bufferPtr,&tamCapa,sizeof(uint32_t));
	bufferPtr = m_buffer + m_offsetLayerSize+((layerIndex-1)*headerSizeMedio);
	layerIndex++;

	/*Escribir la capa que llega al nodo final*/
	memcpy(bufferPtr,&packetID,sizeof(uint64_t));
	bufferPtr = m_buffer+m_offsetAntID+((layerIndex-1)*headerSizeMedio);
	memcpy(bufferPtr,ArapAnts::NODE_TYPE_FINAL.c_str(),ArapAnts::NODE_TYPE_SIZE);
	bufferPtr = m_buffer+m_offsetNodeType+((layerIndex-1)*headerSizeMedio);
	memcpy(bufferPtr,data,dataSize);
	memset(bufferPtr+dataSize,0,ArapSimulator::GetAntsSize()-usefullBytes);

	delete[] data;
	return m_antID;
}

void
ArapAnts::CreateMediumAnt(const uint8_t* dataIn ,uint8_t * const dataOut){
	//headerSizeMedium == offsetLayerSize en este caso

	/* Eliminar los Bytes de la capa que fue leida y hacer el
	 * fill de "0" con esa misma cantidad de Bytes
	 */
	memcpy(dataOut,dataIn+m_offsetLayerSize,ArapSimulator::GetAntsSize()-m_offsetLayerSize);
	memset(dataOut+ArapSimulator::GetAntsSize()-m_offsetLayerSize,0,m_offsetLayerSize);
}

void
ArapAnts::CreateAnswerAnt(uint8_t * const dataOut, const uint64_t& sendTime, const Ipv4Address& target, const uint64_t& antID, const std::string& answer){
	uint8_t* data=NULL;
	uint32_t dataSize = CreateLoadAntData(data,sendTime,target,answer);
	uint32_t headerSizeFinal= m_offsetNodeType+dataSize;
	uint32_t usefullBytes = headerSizeFinal;
	NS_ABORT_MSG_IF(usefullBytes > ArapSimulator::GetAntsSize(),
			"El tamanio del paquete a crear excede el tamanio maximo del paquete en ArapPacket::CreateAnswerPacket");
	uint8_t* bufferPtr = dataOut;
	memcpy(dataOut,&antID,sizeof(uint64_t));
	bufferPtr = dataOut+m_offsetAntID;
	memcpy(bufferPtr,ArapAnts::NODE_TYPE_FINAL.c_str(),ArapAnts::NODE_TYPE_SIZE);
	bufferPtr = dataOut+m_offsetNodeType;
	memcpy(bufferPtr,data,dataSize);
	memset(bufferPtr+dataSize,0,ArapSimulator::GetAntsSize()-usefullBytes);

	delete[] data;
}

uint32_t const
ArapAnts::CreateExplorerAntData(uint8_t*& data, const Ipv4Address& target) {
	uint32_t size= sizeof(AntType)+sizeof(uint64_t)+IPV4_SIZE;/*TIPO DE HORMIGA + TIEMPO DE ENVIO + IP DESTINO*/
	data = new uint8_t[size];
	uint64_t currentTime = Simulator::Now().GetTimeStep();
	const AntType antType = ANT_EXPLORER;
	memcpy(data,&antType,sizeof(AntType)); // Tipo EXPLORADORA
	memcpy(data+sizeof(AntType),&currentTime,sizeof(uint64_t)); // Tiempo de envio
	target.Serialize(data+sizeof(AntType)+sizeof(uint64_t)); // IP Destino

	return size;
}

uint32_t const
ArapAnts::CreateLoadAntData(uint8_t*& data, const uint64_t& sendTime, const Ipv4Address& target, const std::string& message) {
	uint32_t size= sizeof(AntType)+sizeof(uint64_t)+IPV4_SIZE+sizeof(uint32_t)+message.size();/*TIPO DE HORMIGA +TIEMPO DE ENVIO +IP DESTINO+ TAM DE MENSAJE + MENSAJE*/
	data = new uint8_t[size];
	uint32_t messageSize = message.size();
	const AntType antType = ANT_LOAD;
	memcpy(data,&antType,sizeof(AntType)); // Tipo CARGA
	memcpy(data+sizeof(AntType),&sendTime,sizeof(uint64_t)); // Tiempo de envio

	target.Serialize(data+sizeof(AntType)+sizeof(uint64_t)); // IP Destino

	memcpy(data+sizeof(AntType)+sizeof(uint64_t)+IPV4_SIZE,&messageSize,sizeof(uint32_t)); // Tamanio del mensaje
	memcpy(data+sizeof(AntType)+sizeof(uint64_t)+IPV4_SIZE+sizeof(uint32_t),message.c_str(),messageSize); // Mensaje

	return size;
}

} /* namespace ns3 */
