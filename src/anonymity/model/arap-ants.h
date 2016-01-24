/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/** @file */
#ifndef ARAP_ANTS_H
#define ARAP_ANTS_H

#include <stdio.h>
#include <string>
#include <stdint.h>
#include "ns3/ipv4.h"

/**
 * Tamaño en Bytes de una dirección IPV4
 */
#define IPV4_SIZE 4

namespace ns3 {

/**
 * Clase encargada de manejar la estructura interna de las hormigas, así como
 * de leer y escribir los diferentes campos de las mismas.
 */
class ArapAnts {

public:
	/**
	 * Enum para identificar los tipos de hormiga:
	 * - Exploradora
	 * - De carga
	 */
	enum AntType{ANT_EXPLORER, ANT_LOAD};

	/**
	 * Constante para identificar un nodo como intermedio.
	 */
	static const std::string NODE_TYPE_MEDIUM;

	/**
	 * Constante para identificar un nodo como final.
	 */
	static const std::string NODE_TYPE_FINAL;

	/**
	 * Tamaño en Bytes del identificador de tipo de nodo.
	 */
	static const uint32_t NODE_TYPE_SIZE;

	ArapAnts();
	~ArapAnts();

	/** \addtogroup CamposHormigas Campos de las Hormigas
	 * Métodos para leer los distintos campos de una hormiga enviada a través de la red.
	 * Reciben como parametro el array de bytes que contiene la data de la hormiga.
	 * @{
	 */
	static const uint64_t ReadAntID(const uint8_t* data); //!< Leer ID de hormiga
	static const std::string ReadNodeType(const uint8_t* data); //!< Leer tipo de nodo
	static const Ipv4Address ReadNextJumpIP(const uint8_t* data); //!< Leer IP del siguiente salto
	static const uint32_t ReadLayerSize(const uint8_t* data); //!< Leer tamaño en Bytes de la capa
	static const AntType ReadAntType(const uint8_t* data); //!< Leer tipo  de hormiga
	static const uint64_t ReadSendTime(const uint8_t* data); //!< Leer tiempo de envio
	static const Ipv4Address ReadTargetIP(const uint8_t* data); //!< Leer IP destino
	static const uint32_t ReadMessageSize(const uint8_t* data); //!< Leer tamaño en bytes del mensaje
	static const std::string ReadMessage(const uint8_t* data, const uint32_t messageSize); //!< Leer mensaje
	/** @} */


	/**
	 * Crear paquete para hormiga de carga.
	 *
	 * @param[in] path Camino a recorrer por la hormiga desde el origen hasta el destino final.
	 *
	 * @param[in] message Mensaje a enviar al nodo final.
	 *
	 * @return ID de la hormiga creada
	 */
	const uint64_t CreateLoadAnt(std::list<Ipv4Address>& path, const std::string& message);

	/**
	 * Crear hormiga exploradora.
	 *
	 * @param[in] target IP del nodo destino hacia el cual se esta explorando el camino.
	 *
	 * @return ID de la hormiga creada
	 */
	const uint64_t CreateExplorerAnt(const Ipv4Address& target);

	/**
	 * Crea una hormiga de respuesta desde un nodo intermedio al siguiente.
	 *
	 * @param[in] dataIn Data de la hormiga recibida
	 *
	 * @param[out] dataOut Data de la nueva hormiga a enviar
	 */
	void CreateMediumAnt(const uint8_t* dataIn ,uint8_t * const dataOut);

	/**
	 * Crea una hormiga de respuesta desde el nodo final (A donde se dirige finalmente un request)
	 *
	 * @param[in] dataIn Data de la hormiga recibida
	 *
	 * @param[out] dataOut Data de la nueva hormiga a enviar
	 *
	 * @param[in] sendTime Tiempo en el que se envio la hormiga original
	 *
	 * @param[in] antID ID de la hormiga recibida y al cual se le creará la respuesta
	 *
	 * @param[in] aswer Mensaje de respuesta a enviar
	 */
	void CreateAnswerAnt(uint8_t * const dataOut, const uint64_t& sendTime, const Ipv4Address& target, const uint64_t& antID, const std::string& answer);

	/**
	 * Crea la "Data" de una hormiga exploradora,
	 *
	 * @param[out] data Puntero a un vector de bytes donde se escribira la data creada
	 *
	 * @param[in] target IP del nodo destino hacia el cual se esta explorando el camino.
	 *
	 * @return Tamaño en Bytes de la data creada (Tamaño de data)
	 *
	 * @see CreateExplorerAnt()
	 */
	uint32_t const CreateExplorerAntData(uint8_t*& data, const Ipv4Address& target);

	/**
	 * Crea la "Data" de una hormiga de carga.
	 *
	 * @param[out] data Puntero a un vector de bytes donde se escribira la data creada
	 *
	 * @param[in] sendTime Tiempo en el que se envio el paquete original
	 *
	 * @param[in] target IP del nodo destino hacia el cual se enviara la hormiga.
	 *
	 * @param[in] message Mensaje a enviar al nodo final.
	 *
	 * @return Tamaño en Bytes de la data creada (Tamaño de data)
	 *
	 * @see CreateLoadAnt()
	 */
	uint32_t const CreateLoadAntData(uint8_t*& data, const uint64_t& sendTime, const Ipv4Address& target, const std::string& message);

	/**
	 * @return El buffer interno donde se escribe el contenido de la hormiga creada.
	 *
	 * @see CreateLoadAnt()
	 * @see CreateExplorerAnt()
	 */
	const uint8_t* GetBuffer() const;

	/**
	 * @return El contador de ID de las hormigas creadas.
	 */
	static const uint64_t GetAntID();

private:

	uint8_t* m_buffer; //!< Buffer interno para mantener la data del paquete creado

	/**
	 * ID para idenfiticar inequivocamente cada hormiga.
	 *
	 * Se incrementa cada vez que se crea una hormiga exploradora o de carga
	 *
	 * @see CreateLoadAnt()
	 * @see CreateExplorerAnt()
	 */
	static uint64_t m_antID;
	static const uint32_t m_offsetAntID; //!< Offset de Bytes para leer el ID de hormiga
	static const uint32_t m_offsetNodeType; //!< Offset de Bytes para leer el Tipo de nodo
	static const uint32_t m_offsetNextJumpIP; //!< Offset de Bytes para leer la IP del siguiente salto
	static const uint32_t m_offsetLayerSize; //!< Offset de Bytes para leer el tamaño de la capa
	static const uint32_t m_offsetAntType; //!< Offset de Bytes para leer el tipo de hormiga
	static const uint32_t m_offsetSendTime; //!< Offset de Bytes para leer el tiempo de envio
	static const uint32_t m_offsetTargetIP; //!< Offset para la IP del nodo destino
	static const uint32_t m_offsetMessageSize; //!< Offset de Bytes para leer el tamaño del mensaje
};

} /* namespace ns3 */

#endif /* ARAP_ANTS_H */
