/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef ARAP_SIMULATOR_H
#define ARAP_SIMULATOR_H

#include "arap-definitions.h"
#include "path-manager-factory.h"
#include "arap-path-manager.h"
#include "ns3/random-variable-stream.h"
#include <map>

namespace ns3 {

/**
 * Clase de utilidades usada como nucleo del simulador, donde se crea y configura la red,
 *  se leen los parámetros y los envios de data de archivo, y se encarga de mantener los
 *  diferentes parametros de la simulación.
 */
class ArapSimulator {

public:

	/**
	 * Configura el simulador, este método se debe usar en el main, a partir de aquí se
	 * leen los parámetros, se crea y configura la red.
	 *
	 * @param[in] filename Nombre del archivo donde se encuentran los parámetros de la simulación a correr.
	 */
	static void ConfigSimulator(const std::string& filename="parametros.txt");

	/**
	 * Iniciar la ejecución del simulador luego que se haya creado y configurado la red.
	 */
	static void Run();

	/**
	 * Finalizar la ejecución del simulador y realizar limpieza en memoria.
	 */
	static void Destroy();

	/**
	 * @return Referencia constante a la estrella que mantiene la topología de red.
	 */
	static const PointToPointStarHelper& GetStar();

	/**
	 * @return Referencia constante al intervalo de envio de exploradoras
	 */
	static const Time& GetExplorersDelta();

	/**
	 * @return Número de saltos que dan las hormigas de carga
	 */
	static const uint32_t GetNumHops();

	/**
	 * @return Número de nodos en la red
	 */
	static const uint32_t GetNumNodes();

	/**
	 * @return Tamaño en Bytes de las hormigas a enviar a través de la red anónima
	 */
	static const uint32_t GetAntsSize();

	/**
	 * @return Número de puerto por el que se reciben los paquetes
	 */
	static const uint16_t GetPort();

	/**
	 * @return Booleano que indica si esta habilitado o no  el envio de hormigas exploradoras
	 */
	static bool IsExplorerAntsEnabled();

	/**
	 * @return Tamaño Máximo de Segmento en Bytes que enviarán los socket cliente
	 */
	static const uint64_t GetMaximumSegmentSize();

private:
	ArapSimulator (); //!<Constructor por defecto, se hace privado para no crear instancias de esta clase

	/**
	 * Método para inicializar los valores del mapa que "traduce" los nombres de los
	 * parámetros a un valor entero, este método no tiene ninguna funcionalidad en el
	 * simulador más que como un recurso de codificación.
	 *
	 * @see StringValue
	 */
	static void InitializeValuesMap();

	/**
	 * Valida que un rango dado se encuentre entre [0, cantidad de nodos).
	 *
	 * @param[in] min Valor inferior del rango a validar
	 *
	 * @param[in] max Valor superior del rango a validar
	 *
	 * @return true si el rango es valido, false en otro caso
	 */
	static bool IsRangeValid(const int min,const int max);

	/**
	 * Lee los parámetros de una distribución de probabilidad específica.
	 *
	 * @param[in] randomStream Nombre de la distribución a la cual se le leerán los parámetros.
	 *
	 * @param[in] iss Flujo de datos de donde se leerán los parámetros.
	 *
	 * @return Puntero a la distribución de probabilidad especificada luego de configurarla.
	 */
	static Ptr<RandomVariableStream> ReadRandomStream(const std::string& randomStream,std::istringstream& iss);

	/**
	 * Leer parámetros de la simulación desde un archivo.
	 *
	 * @param[in] filename Nombre del archivo de donde se leerán los parámetros
	 */
	static void ReadParametersFile(const std::string& filename);

	/**
	 * Leer parámetros de la simulación de archivo.
	 *
	 * @param[in] paramName Nombre del parámetro a leer y configurar
	 *
	 * @param[in] iss Flujo de datos de donde se leerán los parámetros.
	 */
	static void HandleParameter(const std::string& paramName, std::istringstream& iss);

	/**
	 * Crear y configurar la red a simular.
	 */
	static void ConfigNetwork();

	/**
	 * Cambiar el delay de los enlaces y programar el próximo cambio.
	 */
	static void ChangeLinkDelay();

	/**
	 * Imprime a un archivo los valores de las distribuciones del delay de las
	 * hormigas de carga de cada nodo, así como la suma de dichos valores.
	 */
	static void PrintLoadAntsStatistics();

	/**
	 * Imprime a diferentes archivos la tabla de probabilidad de cada uno de los nodos
	 * (Una tabla por archivo).
	 */
	static void PrintProbabilityTables();

	/**
	 * Crear y configurar las aplicaciones en los nodos que enviarán y recibirán las hormigas
	 *
	 */
	static void ConfigNodes();

	/**
	 * Validar que el rango de la distribucion de destinos para hormigas
	 * de carga sea valido ( Que este entre [0, cantidad de nodos-1] ). Si el
	 * método detecta que un rango no es valido, inmediatamente termina la simulación.
	 *
	 * @param[in] loadAntTargetStream Referencia a la distribución que se desea  validar
	 *
	 */
	static void ValidateLoadAntTargetStream(Ptr<RandomVariableStream>& loadAntTargetStream, int& targetMin,int& targetMax);

	static std::map<std::string, int> s_mapStringValues;//!< Mapa para asociar los nombres de parametros a enteros
	static PointToPointStarHelper* m_star; //!< Puntero a la estrella que mantiene la topologia
	static uint32_t m_numHops; //!< Numero de saltos que hace un paquete de carga desde el origen hasta el destino
	static uint16_t m_port; //!< Puerto en el que se reciben las hormigas
	static uint32_t m_numNodes; //!< Número de nodos en la red
	static Time m_explorerAntsInterval; //!< Invertalo de tiempo para enviar las hormigas exploradoras
	static Time m_linkDelayInterval; //!< Intervalo de tiempo para cambiar el delay de los enlaces
	static Time m_printTablesInterval; //!< Intervalo de tiempo para imprimir las tablas de probabilidad de los nodos
	static uint32_t m_antSize; //!< Tamaño en Bytes de las hormigas a enviar a través de la red anónima
	static Ptr<RandomVariableStream> m_linkDelayChange; //!< Distribución de probabilidad que provee los valores para el delay de los enlaces
	static std::list<OperationDelayFormat> m_computingDelayList; //!< Distribución de probabilidad que provee los valores para el delay de computo
	static std::list<DataRateFormat> m_dataRateList; //!< Lista que guarda los distintos rangos de valores de DataRate
	static std::list<AppTimeFormat> m_startTimeList; //!< Lista que guarda los distintos rangos de tiempo de inicio de envio de las hormigas
	static double m_stopTime; //!<Tiempo de finalización de la simulación
	static bool m_enableTraces; //!< Habilitar creación de archivos ".pcap" y ".tr"
	static std::list<RandomStreamFormat> m_computingDistList; //!< Lista que guarda los distintos rangos de valores de las distribuciones de probabilidad para el delay de computo
	static bool m_enableExplorerAnts; //!< Habilitar el envio de hormigas exploradoras
	static uint64_t m_maximumSegmentSize; //!< Tamaño máximo de segmento
	static uint64_t m_queueSize; //!< Tamaño máximo (En paquetes) de las colas en los nodos
	static PathManagerFactory m_pathManagerFactory; //!< Creador de instancias de especializaciones de ArapPathManager
	static ArapPathManager * m_arapPathManager; //!< Puntero a objetos de las especializaciones de ArapPathManager
	static std::list<RandomStreamFormat> m_loadAntsTimeList; //!< Lista que guarda las distribuciones para los tiempos de envio de las hormigas de carga en cada nodo
	static std::list<RandomStreamFormat> m_loadAntsQuantityList; //!< Lista que guarda las distribuciones para la cantidad de hormigas de carga a enviar en cada tiempo en cada nodo
	static std::list<RandomStreamFormat> m_loadAntsTargetList; //!< Lista que guarda las distribuciones para seleccionar los destinos de las hormigas de carga en cada nodo

};

} // namespace ns3

#endif /* ARAP_SIMULATOR_H */
