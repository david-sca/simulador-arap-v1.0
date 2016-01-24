/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef ROUTING_TABLE_ROW_H
#define ROUTING_TABLE_ROW_H

#include "ns3/ipv4-address.h"
namespace ns3 {

/**
 * Fila de la tabla de enrutamiento que posee cada nodo.
 *
 * Cada fila esta formada por una tupla que contiene los siguientes campos:
 * - IP Origen
 * - IP Destino
 * - ID de paquete
 */
class RoutingTableRow: public SimpleRefCount<RoutingTableRow>
{

public:
	RoutingTableRow();

	/**
	 * Constructor paramétrico.
	 *
	 * @param[in] source IP de nodo destino
	 *
	 * @param[in] target IP de nodo destino
	 *
	 * @param[in] antID ID de la hormiga
	 */
	RoutingTableRow(const Ipv4Address& source, const Ipv4Address& target, const uint64_t antID);
	~RoutingTableRow();

	/**
	 * Obtener la IP destino.
	 *
	 * @return Referencia constante al campo IP destino
	 */
	const Ipv4Address& GetTargetIP() const;

	/**
	 * Asignar el campo IP destino
	 *
	 * @param[in] targetIP IP destino
	 */
	void SetTargetIP(const Ipv4Address& targetIP);

	/**
	 * Obtener la IP origen.
	 *
	 * @return Referencia constante al campo IP origen
	 */
	const Ipv4Address& GetSourceIP() const;

	/**
	 * Asignar el campo IP origen
	 *
	 * @param[in] sourceIP IP origen
	 */
	void SetSourceIP(const Ipv4Address& sourceIP);


	/**
	 * Obtener el ID de hormiga.
	 *
	 * @return ID de la hormiga
	 */
	const uint64_t GetAntID() const;

	/**
	 * Asignar el campo ID de hormiga
	 *
	 * @param[in] antID ID de hormiga
	 */
	void SetPacketID(const uint64_t antID);

	/**
	 * Operador de comparación.
	 *
	 * @return true si las dos filas poseen los mismos campos, false en otro caso.
	 */
	bool operator==(const RoutingTableRow& row) const;

private:

	Ipv4Address m_sourceIP; //!< Campo IP Origen
	Ipv4Address m_targetIP; //!< Campo IP Destino
	uint64_t m_antID; //!< Campo ID de hormiga
};

} /* namespace ns3 */

#endif /*ROUTING_TABLE_ROW_H */
