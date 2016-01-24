/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/** @file */
#ifndef ARAP_PATH_MANAGER_H
#define ARAP_PATH_MANAGER_H

#include "ns3/ipv4-address.h"
#include "ns3/nstime.h"
#include "ns3/application-container.h"
#include "ns3/point-to-point-star.h"
#include <map>
#include <list>

namespace ns3 {

/**
 * Clase encargada de mantener la tabla de probabilidades para los distintos
 * caminos hacia los distintos destinos, esta es una clase totalmente abstracta,
 * los usuarios deben implementar la especialización de esta clase con sus propias
 * definiciones para los métodos virtuales que posee.
 *
 * La tabla de probabilidades esta definida la siguiente manera:
 * - Las filas indican el nodo destino
 * - Las columnas indican los vecinos inmediatos a un destino (En
 * este caso todos los nodos excepto al que pertenece la tabla).
 *
 * Si por ejemplo se pide una probabilidad P[i,j], es la probabilidad de ir al nodo
 * "i" a través del nodo "j".
 */
class ArapPathManager {

public:
	ArapPathManager();
	virtual ~ArapPathManager() =0;

	/**
	 * Obtener una probabilidad de la tabla
	 *
	 * @param[in] target IP del nodo destino (Fila)
	 *
	 * @param[in] medium IP del nodo intermedio (Columna)
	 *
	 * @return La probabilidad correspondiente en la tabla
	 */
	const double GetProbability(const Ipv4Address& target, const Ipv4Address& medium);

	/**
	 * Asigna una probabilidad en la tabla
	 *
	 * @param[in] prob Valor de probabilidad a asignar en la posición indicada
	 *
	 * @param[in] target IP del nodo destino (Fila)
	 *
	 * @param[in] medium IP del nodo intermedio (Columna)
	 */
	void SetProbability(const double& prob, const Ipv4Address& target, const Ipv4Address& medium);

	/**
	 * Inicializa la tabla de probabilidades para el nodo cuya IP es localIP.
	 *
	 * Todas las probabilidades se inicializan con una distribución uniforme
	 * ( 1 / (cantidad de nodo -2), excepto los valores pertenecientes a la
	 * diagonal principal que se inicializan a cero).
	 *
	 * @param[in] localIP IP del nodo al que pertenece la tabla de probabilidad a crear.
	 */
	void CreateProbTable(const Ipv4Address& localIP);

	/**
	 * Imprime los valores de la tabla de probabilidades a un archivo.
	 *
	 * El archivo se guarda en la carpeta "salidas" y tiene como nombre:\n
	 * "<IP_LOCAL>_<tiempo de generacion (segundos)>_<Semilla>_<Run>.csv"
	 *
	 * @param[in] localIP IP del nodo al que pertenece la tabla a imprimir en archivo.
	 */
	void PrintProbTable(const Ipv4Address& localIP);

	/**
	 * Método usado para procesar una hormiga exploradora una vez que esta vuelve al nido.
	 *
	 * @param[in] target IP del nodo destino (Fila)
	 *
	 * @param[in] medium IP del nodo intermedio (Columna)
	 *
	 * @param[in] rtt Tiempo de ida (al nodo destino) y vuelta (al origen) de la exploradora (Round Trip Time)
	 */
	virtual void HandleExplorer(const Ipv4Address& target, const Ipv4Address& medium, const Time& rtt);

	/**
	 * Método para crear una copia en memoria de "this", para esto las clases que hereden
	 * de ArapPathManager deben poseer también un constructor por copia (El compilador genera uno
	 * automáticamente que la mayoria de los casos es suficiente).
	 *
	 * @return Puntero a una nueva copia de este objeto.
	 */
	virtual ArapPathManager* GetCopy() const;

	/**
	 * Crea un recorrido según la cantidad de saltos definida en ArapSimulator::m_numHops.
	 *
	 * @param[in] target IP del destino final hacia el cual se debe crear el camino.
	 *
	 * @return Lista del recorrido en orden (Primer elemento = primer salto ...)
	 *
	 * @see ArapSimulator::m_numHops
	 */
	virtual std::list<Ipv4Address> CreatePath(const Ipv4Address& target);

protected:
	std::map<Ipv4Address,std::map<Ipv4Address,double> > m_probTable; //!< Tabla de probabilidades
};

} /* namespace ns3 */

#endif /* ARAP_PATH_MANAGER_H */
