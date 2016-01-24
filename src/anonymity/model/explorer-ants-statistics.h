/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/** @file */
#ifndef EXPLORER_ANTS_STATISTICS_H
#define EXPLORER_ANTS_STATISTICS_H

#include"ns3/nstime.h"

#define DEFAULT_WBEST 10000000 //!< Valor ṕor defecto de WBest (Debe ser un valor muy grande)

namespace ns3 {

/**
 * Modelo estocástico usado por la clase SmartPathManager para
 * mantener y actualizar los distintos valores de la distribución
 * de probabilidad creada para cada uno de los nodos destino.
 */
class ExplorerAntsStatistics {
public:

	/**
	 * Constructor paramétrico.
	 *
	 * @param[in] wMax Tamaño máximo de la ventana de observación
	 *
	 * @param[in] varsigma Varsigma. Valor usado en el modelo estocástico
	 */
	ExplorerAntsStatistics(const uint32_t wMax, const double varsigma);
	~ExplorerAntsStatistics();

	/**
	 * @return La media de la distribución.
	 */
	const double GetMean() const;

	/**
	 * @return La varianza de la distribución.
	 */
	const double GetVariance() const;

	/**
	 * @return el mejor tiempo de la ventana de observación actual de la distribución.
	 */
	const int64_t GetWBest() const;

	/**
	 * @return El tamaño máximo de la ventana de observación.
	 */
	const uint32_t GetWMax()const;

	/**
	 * @return La cantidad de muestras en la ventana de observación actual.
	 */
	const uint32_t GetWCount()const;

	/**
	 * Actualizar modelo estocástico.
	 *
	 * Se recibe un nuevo valor (Una muestra) y con este se actualizan las variables del modelo
	 *
	 * @param[in] rtt Tiempo de ida y vuelta de la hormiga exploradora (En milisegundos)
	 */
	void UpdateModel(const int64_t rtt);

private:

	double m_mean; //!< Media de la distribución
	double m_variance; //!< Varianza de la distribución
	int64_t m_WBest; //!< Mejor tiempo de la ventana de observación actual de la distribución
	double m_varsigma; //!< Varsigma. Parámetro para el cálculo y actualización de la media y la varianza
	uint32_t m_WMax; //!< Tamaño máximo de la ventana de observación.
	uint32_t m_WCount; //!< Cantidad de muestras en la ventana de observación actual

};

} /* namespace ns3 */

#endif /* EXPLORER_ANTS_STATISTICS_H */
