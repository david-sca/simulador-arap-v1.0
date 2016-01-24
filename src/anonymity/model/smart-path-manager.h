/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/** @file */
#ifndef SMART_PATH_MANAGER_H
#define SMART_PATH_MANAGER_H

#include "ns3/arap-path-manager.h"
#include "ns3/explorer-ants-statistics.h"
#include <map>

#define DEFAULT_C1 0.7 //!<Valor por defecto de C1 para el cálculo de R
#define DEFAULT_C2 0.3 //!<Valor por defecto de C2 para el cálculo de R
#define DEFAULT_ZETA 1.7 //!<Valor por defecto de Z para el cálculo de R
#define DEFAULT_WMAX 50 //!<Valor por defecto del tamaño máximo de la ventana de observación
#define DEFAULT_VARSIGMA 0.5 //!<Valor por defecto de Varsigma para el modelo estocástico
#define MAX_PROB 0.8 //!<Máxima probabilidad posible a ser asignada para una posición en la tabla

namespace ns3 {

/**
 * Implementación de ArapPathManager que usa los tiempos de viaje de las hormigas
 * exploradoras para actualizar la tabla de probabilidades y darle mayor probabilidad
 * a aquellos caminos más rapidos, permitiendo seleccionar los caminos de manera "inteligente"
 * y no aleatoriamente.
 */
class SmartPathManager: public ArapPathManager {
public:

	SmartPathManager();
	/**
	 * Constructor paramétrico.
	 *
	 * @param[in] c1 Valor de C1
	 *
	 * @param[in] c2 Valor de C2
	 *
	 * @param[in] zeta Valor de Z
	 *
	 * @param[in] wMax Tamaño máximo de la ventana de observación
	 *
	 * @param[in] varsigma Varsigma. Valor usado en el modelo estocástico
	 */
	SmartPathManager(double c1, double c2, double zeta, uint32_t wMax, double varsigma);
	virtual ~SmartPathManager();

	virtual void HandleExplorer(const Ipv4Address& target, const Ipv4Address& medium, const Time& rtt);
	virtual ArapPathManager* GetCopy() const;
	virtual std::list<Ipv4Address> CreatePath(const Ipv4Address& target);

protected:

	/**
	 * Incrementar el valor de la probabilidad (feromona) para un camino (posición en la tabla) indicado.
	 *
	 * @param[in] target IP del nodo destino (Fila)
	 *
	 * @param[in] medium IP del nodo intermedio (Columna)
	 *
	 * @param[in] rtt Tiempo de ida y vuelta de la hormiga exploradora
	 *
	 * @param[in] reward Recompensa, valor calculado que indica que tan buena es la nueva probabilidad
	 *
	 * @return valor de la probabilidad nueva asignada
	 */
	virtual const double PheromoneIncrease(const Ipv4Address& target, const Ipv4Address& medium, const Time& rtt, const double& reward);

	/**
	 * Reducir el valor de la probabilidad (feromona) para todos los caminos excepto el indicado.
	 *
	 * @param[in] target IP del nodo destino (Fila)
	 *
	 * @param[in] medium IP del nodo intermedio (Columna)
	 *
	 * @param[in] rtt Tiempo de ida y vuelta de la hormiga exploradora
	 *
	 * @param[in] reward Recompensa, valor calculado que indica que tan buena es la nueva probabilidad
	 */
	virtual void PheromoneDecrease(const Ipv4Address& target, const Ipv4Address& medium, const Time& rtt, const double& reward);

	/**
	 * Calcula el valor de R (reward).
	 *
	 * @param[in] rtt Tiempo de ida y vuelta de la hormiga exploradora
	 *
	 * @param[in] mean Media de la distribución de probabilidad para los caminos hacia un destino
	 *
	 * @param[in] variance Varianza de la distribución de probabilidad para los caminos hacia un destino
	 *
	 * @param[in] wBest Mejor tiempo observado en la ventana de observación actual
	 *
	 * @param[in] wCount Muestra actual dentro de la ventana de observación
	 *
	 * @return Valor de R
	 */
	virtual const double GetR(const int64_t& rtt,const double& mean, const double& variance, const double& wBest, const uint32_t& wCount);

private:

	std::map<Ipv4Address,ExplorerAntsStatistics > m_stochasticModel; //!< Mapa de modelos estocásticos de los nodos destino
	double m_c1; //!< Valor C1 usado en el cálculo de R
	double m_c2; //!< Valor C2 usado en el cálculo de R
	double m_zeta; //!< Valor Z usado en el cálculo de R
	uint32_t m_WMax; //!< Valor de la ventana máxima de observación
	double m_varsigma; //!< Valor de varsigma

	/**
	 * Inicializa los modelos estocásticos para los nodos de la simulación
	 */
	void InitModels();
};

} /* namespace ns3 */

#endif /* SMART_PATH_MANAGER_H */
