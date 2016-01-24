/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/** @file */
#ifndef LOAD_ANTS_STATISTICS_H
#define LOAD_ANTS_STATISTICS_H

#include <stdint.h>
#include <map>
#include "ns3/ipv4-address.h"

namespace ns3 {

/**
 * Modelo estocástico usado por los nodos para mantener
 * la media y la varianza de los delay generados por las
 * hormigas de carga.
 */
class LoadAntsStatistics {
public:
	LoadAntsStatistics();
	~LoadAntsStatistics();

	/**
	 * @return La media de la distribución.
	 */
	const double GetMean() const;

	/**
	 * @return La varianza de la distribución.
	 */
	const double GetVariance() const;

	/**
	 * @return La cantidad de muestras de la distribución.
	 */
	const uint64_t GetNumSamples() const;

	/**
	 * Actualizar modelo estocástico.
	 *
	 * Se recibe un nuevo valor (Una muestra) y con este se actualizan las variables del modelo
	 *
	 * @param[in] rtt Tiempo de ida y vuelta de la hormiga de carga (En milisegundos)
	 *
	 * @param[in] target IP del nodo final al que fue enviado la hormiga.
	 */
	void UpdateModel(const double rtt, const Ipv4Address& target);

	/**
	 * Inicializa la tabla de valores que contará la cantidad de hormigas a cada nodo. Este método
	 * debe llamarse una vez que se ha configurado la red y se ha asignado IP a cada nodo.
	 */
	void InitializeTable();

	/**
	 * @return Referencia a el mapa que contiene los valores de las hormigas de carga enviadas a cada nodo
	 */
	const std::map<Ipv4Address,uint64_t>& GetSampleCountsMap() const;

private:

	double m_sumDelay; //!< Suma de los delays obtenidos
	double m_sumDelaySquare; //!< Suma de los cuadrados de los delays obtenidos
	uint64_t m_numSamples; //!< Contador de la cantidad de muestras (delays) obtenidas
	std::map<Ipv4Address,uint64_t>  m_samplesCount; //!< Mapa de cantidad de hormigas enviadas a cada nodo

};

} /* namespace ns3 */

#endif /* LOAD_ANTS_STATISTICS_H */
