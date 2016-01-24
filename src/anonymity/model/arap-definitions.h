/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/** @file */
/**
 * Definiciones de macros usados por la clase ArapSimulator.
 *
 * Macros definidos para los nombres, valores por defecto de los parametros
 * del simulador y estructuras de datos usadas al momento de leer los mismos
 * del archivo de parámetros.
 */

#ifndef ARAP_DEFINITIONS_H
#define ARAP_DEFINITIONS_H

#include "ns3/ptr.h"
#include "ns3/random-variable-stream.h"

using namespace ns3;

/**
 * IP base para las direcciones a usar por los nodos pertenecientes a la red.
 */
#define NETWORK_BASE_IP "10.1.1.0"

/**
 * Máscara de red para las direcciones a usar por los nodos pertenecientes a la red.
 */
#define NETWORK_SUBNET_MASK "255.255.255.0"


/**
 * Número mínimo de nodos a crear en la red.
 */
#define MINIMUM_NODES 4

/**
 * Tamaño mínimo (En Bytes) de las hormigas a enviar por la red.
 */
#define MINIMUM_ANT_SIZE 128

/**
 * Número de puerto por defecto.
 */
#define DEFAULT_PORT 9

/**
 * Tiempo (En segundos) por defecto en que se inicia la aplicación (Se empiezan a enviar
 * hormigas exploradoras y de carga) en cada nodo.
 */
#define DEFAULT_APP_START_TIME 1

/**
 * Valor por defecto del parametro usado para habilitar la creación de archivos ".pcap" y ".tr".
 */
#define DEFAULT_ENABLE_TRACING false


/** \addtogroup ParametrosArchivo Parametros de Archivo de configuración
 * Nombres y formato de las variables a leer desde el archivo.
 * @{
 */

/**
 * Cantidad de nodos.
 * NOTA: Este valor debe estar definido antes que cualquier otro parámetro en el archivo.
 *
 * Formato en el archivo: <nombre> <valor>
 * Donde:
 * - nombre : VALOR DE PARAM_NODES
 * - valor : entero >= 4
 */
#define PARAM_NODES "nodos"

/**
 * Número de saltos que realizan las hormigas de carga.
 *
 * Formato en el archivo: <nombre> <valor>
 *
 * Donde:
 * - nombre : VALOR DE PARAM_HOPS
 * - valor : entero \f$ \in \f$ [2, cantidad de nodos-1]
 */
#define PARAM_HOPS "saltos"

/**
 * Tamaño en Bytes de las hormigas enviadas a través de la red.
 *
 * Formato en el archivo: <nombre> <valor>
 *
 * Donde:
 * - nombre : VALOR DE PARAM_ANT_SIZE
 * - valor : entero >= 128
 */
#define PARAM_ANT_SIZE "tamanio-hormiga"

/**
 * Número de puerto por el que se reciben las hormigas.
 *
 * Formato en el archivo: <nombre> <valor>
 *
 * Donde:
 * - nombre : VALOR DE PARAM_PORT
 * - valor : entero \f$ \in \f$ [1, 65535]
 */
#define PARAM_PORT "puerto"

/**
 * Intervalo de tiempo (segundos) en que se envian las hormigas exploradoras.
 *
 * Formato en el archivo: <nombre> <valor>
 *
 * Donde:
 * - nombre : VALOR DE PARAM_EXPLORER_INTERVAL
 * - valor : real > 0
 */
#define PARAM_EXPLORER_INTERVAL "intervalo-exploradoras"

/**
 * DataRate (bits por segundo) a asignar a los nodos especificados en el rango.
 *
 * Formato en el archivo: <nombre> <rangoMin> <rangoMax> <valor>
 *
 * Donde:
 * - nombre : VALOR DE PARAM_DATA_RATE
 * - rangoMin : entero \f$ \in \f$ [0, cantidad de nodos-1]
 * - rangoMax : entero \f$ \in \f$ [0, cantidad de nodos-1]
 * - rangoMin <= rangoMax
 * - valor : entero > 0
 */
#define PARAM_DATA_RATE "data-rate"

/**
 * Tiempo (segundos) en que se inicia el envio de hormigas exploradoras y de carga en
 * los nodos especificados en el rango.
 *
 * Formato en el archivo: <nombre> <rangoMin> <rangoMax> <valor>
 *
 * Donde:
 * - nombre : VALOR DE PARAM_APP_START_TIME
 * - rangoMin : entero \f$ \in \f$ [0, cantidad de nodos-1]
 * - rangoMax : entero \f$ \in \f$ [0, cantidad de nodos-1]
 * - rangoMin <= rangoMax
 * - valor : real >= 1
 */
#define PARAM_APP_START_TIME "app-tiempo-iniciar"

/**
 * Tiempo (segundos) de duración de la simulación
 *
 * Formato en el archivo: <nombre> <valor>
 *
 * Donde:
 * - nombre : VALOR DE PARAM_SIMULATOR_STOP_TIME
 * - valor : real > 0
 */
#define PARAM_SIMULATOR_STOP_TIME "tiempo-detener-simulador"

/**
 * Intervalo de tiempo (segundos) con que se cambia el delay de los enlaces.
 *
 * Formato en el archivo: <nombre> <valor>
 *
 * Donde:
 * - nombre : VALOR DE PARAM_LINK_DELAY_INTERVAL
 * - valor : real > 0
 */
#define PARAM_LINK_DELAY_INTERVAL "delay-enlaces-intervalo"

/**
 * Intervalo de tiempo (segundos) con que se imprimen las tablas de probabilidad de los nodos.
 *
 * Formato en el archivo: <nombre> <valor>
 *
 * Donde:
 * - nombre : VALOR DE PARAM_PRINT_PROB_TABLES_INTERVAL
 * - valor : real > 0
 */
#define PARAM_PRINT_PROB_TABLES_INTERVAL "imprimir-tablas-intervalo"

/**
 * Distribución de probabilidad que provee los valores de
 * delay (milisegundos) en los enlaces de la red.
 *
 * Formato en el archivo: <nombre> <nombre-distribución> <parametros-distribución>
 *
 * Donde:
 * - nombre : VALOR DE PARAM_LINK_DELAY_DIST
 * - nombre-distribución : Nombre de la distribución de probabilidad a usar
 * - parametros-distribución : Parámetros requeridos por la distribución de probabilidad a usar
 * \see Distribuciones
 */
#define PARAM_LINK_DELAY_DIST "delay-enlaces-dist"

/**
 * Distribución de probabilidad que provee los valores de delay generado por
 * el tiempo de computo (milisegundos) en los nodos especificados
 *
 * Formato en el archivo: <nombre> <rangoMin> <rangoMax> <nombre-distribución> <parametros-distribución>
 *
 * Donde:
 * - nombre : VALOR DE PARAM_COMPUTING_DELAY_DIST
 * - rangoMin : entero \f$ \in \f$ [0, cantidad de nodos-1]
 * - rangoMax : entero \f$ \in \f$ [0, cantidad de nodos-1]
 * - rangoMin <= rangoMax
 * - nombre-distribución : Nombre de la distribución de probabilidad a usar
 * - parametros-distribución : Parámetros requeridos por la distribución de probabilidad a usar
 * \see Distribuciones
 */
#define PARAM_COMPUTING_DELAY_DIST "delay-computo-dist"

/**
 * Factor de incremento de tiempo de computo, respecto a la referencia fijada
 * según sea la distribución de probabilidad seleccionada para cada nodo.
 *
 * Formato en el archivo: <nombre> <rangoMin> <rangoMax> <factor>
 *
 * Donde:
 * - nombre : VALOR DE PARAM_COMPUTING_DELAY_INCREMENT

 * - rangoMin : entero \f$ \in \f$ [0, cantidad de nodos-1]
 * - rangoMax : entero \f$ \in \f$ [0, cantidad de nodos-1]
 * - rangoMin <= rangoMax
 * - factor : real \f$ \in \f$ [0, 1]
 */
#define PARAM_COMPUTING_DELAY_INCREMENT "delay-computo-incremento"

/**
 * Valor de la semilla a usar para la generación de números aleatorios en la simulación.
 *
 * Formato en el archivo: <nombre> <valor>
 *
 * Donde:
 * - nombre : VALOR DE PARAM_SEED_NUMBER
 * - valor : entero >= 0
 */
#define PARAM_SEED_NUMBER "semilla"

/**
 * Número de ejecución a usar en conjunto con la semilla para
 * la generación de números aleatorios en la simulación. Este valor
 * se usa principalmente para cambiar los valores de un experimento
 * (También es posible cambiar la semilla, pero para diferentes ejecuciones con
 * una misma configuración se recomienda cambiar el número de ejecución).
 *
 * Formato en el archivo: <nombre> <valor>
 *
 * Donde:
 * - nombre : VALOR DE PARAM_RUN_NUMBER
 * - valor : entero >= 0
 */
#define PARAM_RUN_NUMBER "ejecucion"

/**
 * Parametro (booleano) que indica si se deben o no crear archivos ".pcap" y ".tr"
 * de cada uno de los nodos que forman la red simulada.
 *
 * Formato en el archivo: <nombre> <valor>
 *
 * Donde:
 * - nombre : VALOR DE PARAM_ENABLE_TRACING
 * - valor : booleano (0 = falso, 1 = verdadero)
 */
#define PARAM_ENABLE_TRACING "usar-trazas"

/**
 * Parametro (booleano) que indica si se deben o no enviar hormigas exploradoras
 * a través de la red.
 *
 * Formato en el archivo: <nombre> <valor>
 *
 * Donde:
 * - nombre : VALOR DE PARAM_ENABLE_EXPLORER_ANTS
 * - valor : booleano (0 = falso, 1 = verdadero)
 */
#define PARAM_ENABLE_EXPLORER_ANTS "habilitar-exploradoras"

/**
 * Parametro que indica la capacidad máxima de paquetes que pueden haber
 * en cada una de las colas de los nodos pertenecientes a la red.
 *
 * Formato en el archivo: <nombre> <valor>
 *
 * Donde:
 * - nombre : VALOR DE PARAM_QUEUE_SIZE
 * - valor : entero => 0
 */
#define PARAM_QUEUE_SIZE "tamanio-cola"

/**
 * Parametro que indica el valor del MSS (Maximum Segment Size) a ser usado
 * por los sockets cliente de los nodos pertenecientes a la red
 *
 * Formato en el archivo: <nombre> <valor>
 *
 * Donde:
 * - nombre : VALOR DE PARAM_MAXIMUM_SEGMENT_SIZE
 * - valor : entero => 0
 */
#define PARAM_MAXIMUM_SEGMENT_SIZE "tamanio-segmento"

/**
 * Parametro que indica la especialización de ArapPathManager a usar
 * para el manejo de las tablas de probabilidad y la creación de caminos.
 * Los distintos valores de <nombre> dependen de los definidos en
 * PathManagerFactory::GetInstance().
 *
 * Formato en el archivo: <nombre> <identificador de caso> [<parámetros de caso>]
 *
 * Donde:
 * - nombre : VALOR DE PARAM_PATH_MANAGER
 * - identificador de caso : cadena identificadora del caso a usar para crear la especialización
 * - parámetros de caso: Lista de parámetros definida según el caso en el método
 * PathManagerFactory::GetInstance()
 *
 * @see PathManagerFactory::GetInstance()
 */
#define PARAM_PATH_MANAGER "path-manager"

/**
 * Parametro que indica la distribución de probabilidad de la cual se obtendrán los distintos
 * tiempos de envio (Segundos) de hormigas de carga en los nodos indicados según el rango dado
 *
 * Formato en el archivo: <nombre> <rangoMin> <rangoMax> <nombre-distribución> <parametros-distribución>
 *
 * Donde:
 * - nombre : VALOR DE PARAM_LOAD_ANT_TIME_DIST
 * - rangoMin : entero \f$ \in \f$ [0, cantidad de nodos-1]
 * - rangoMax : entero \f$ \in \f$ [0, cantidad de nodos-1]
 * - rangoMin <= rangoMax
 * - nombre-distribución : Nombre de la distribución de probabilidad a usar
 * - parametros-distribución : Parámetros requeridos por la distribución de probabilidad a usar
 * \see Distribuciones
 */
#define PARAM_LOAD_ANT_TIME_DIST "hormigacarga-tiempo-dist"

/**
 * Parametro que indica la distribución de probabilidad de la cual se obtendrá la cantidad de
 * hormigas de carga a enviar en cada tiempo indicado por la distribución de PARAM_LOAD_ANT_TIME_DIST.
 * Este valor sirve para simular en cuantas partes es dividido un mensaje creado por el usuario. Se
 * recomienda que el rango de valores que usen las distribuciones sean valores pequeños, adaptados a
 * la realidad de un mensaje p.e. una petición HTTP)
 *
 * Formato en el archivo: <nombre> <rangoMin> <rangoMax> <nombre-distribución> <parametros-distribución>
 *
 * Donde:
 * - nombre : VALOR DE PARAM_LOAD_ANT_QUANTITY_DIST
 * - rangoMin : entero \f$ \in \f$ [0, cantidad de nodos-1]
 * - rangoMax : entero \f$ \in \f$ [0, cantidad de nodos-1]
 * - rangoMin <= rangoMax
 * - nombre-distribución : Nombre de la distribución de probabilidad a usar
 * - parametros-distribución : Parámetros requeridos por la distribución de probabilidad a usar
 * \see Distribuciones
 */
#define PARAM_LOAD_ANT_QUANTITY_DIST "hormigacarga-cantidad-dist"

/**
 * Parametro que indica la distribución de probabilidad de la cual se obtendrá el nodo destino
 * al cual se enviarán las hormigas de carga en cada tiempo indicado por la distribución de
 * PARAM_LOAD_ANT_TIME_DIST.
 * El rango de valores que usen las distribuciones DEBEN ser valores \f$ \in \f$ [0, cantidad de nodos-1].
 *
 * Formato en el archivo: <nombre> <rangoMin> <rangoMax> <nombre-distribución> <parametros-distribución>
 *
 * Donde:
 * - nombre : VALOR DE PARAM_LOAD_ANT_TARGET_DIST
 * - rangoMin : entero \f$ \in \f$ [0, cantidad de nodos-1]
 * - rangoMax : entero \f$ \in \f$ [0, cantidad de nodos-1]
 * - rangoMin <= rangoMax
 * - nombre-distribución : Nombre de la distribución de probabilidad a usar
 * - parametros-distribución : Parámetros requeridos por la distribución de probabilidad a usar
 * \see Distribuciones
 */
#define PARAM_LOAD_ANT_TARGET_DIST "hormigacarga-destino-dist"


/** \addtogroup Distribuciones
 * Distribuciones de probabilidad validas para ser usada en los
 * diferentes parámetros del simulador que las requieran, junto con sus respectivos
 * parámetros y el orden de los mismos en el archivo.
 * @{
 * - UniformRandomVariable <rangoMin> <rangoMax>
 * - ConstantRandomVariable <valor>
 * - TriangularRandomVariable <rangoMin> <rangoMax> <media>
 * - ExponentialRandomVariable <media> <limite>
 * - NormalRandomVariable <media> <varianza> <limite>
 *
 * Para más información sobre los valores posibles revisar la
 * documentación de NS3 sobre las clases que heredan de %RandomVariableStream.
 * @} */


/** @} */

/**
 * Enum usado de manera auxiliar para poder hacer una conversión
 * entre los nombres de parametros y un valor entero, este enum
 * no tiene ninguna funcionalidad en el simulador más que como un
 * recurso de codificación.
 */
enum StringValue {
	PARAM_NODES_V,
	PARAM_HOPS_V,
	PARAM_ANT_SIZE_V,
	PARAM_PORT_V,
	PARAM_LINK_DELAY_INTERVAL_V,
	PARAM_EXPLORER_INTERVAL_V,
	PARAM_APP_START_TIME_V,
	PARAM_SIMULATOR_STOP_TIME_V,
	PARAM_LINK_DELAY_DIST_V,
	PARAM_COMPUTING_DELAY_DIST_V,
	PARAM_COMPUTING_DELAY_INCREMENT_V,
	PARAM_DATA_RATE_V,
	PARAM_SEED_NUMBER_V,
	PARAM_RUN_NUMBER_V,
	PARAM_ENABLE_TRACING_V,
	PARAM_ENABLE_EXPLORER_ANTS_V,
	PARAM_QUEUE_SIZE_V,
	PARAM_MAXIMUM_SEGMENT_SIZE_V,
	PARAM_PATH_MANAGER_V,
	PARAM_LOAD_ANT_TIME_DIST_V,
	PARAM_LOAD_ANT_QUANTITY_DIST_V,
	PARAM_LOAD_ANT_TARGET_DIST_V,
	PARAM_PRINT_PROB_TABLES_INTERVAL_V
};

/**
 * Struct usado para almacenar los diferentes rangos del incremento de tiempo de computo.
 */
struct OperationDelayFormat {
  float factor;
  int min;
  int max;

  OperationDelayFormat():factor(0),min(0),max(0){};
  OperationDelayFormat(float f, int mi, int ma):factor(f),min(mi),max(ma){};
};

/**
 * Struct usado para almacenar los diferentes rangos del DataRate.
 */
struct DataRateFormat {
  int value;
  int min;
  int max;

  DataRateFormat():value(0),min(0),max(0){};
  DataRateFormat(int v, int mi, int ma):value(v),min(mi),max(ma){};
};

/**
 * Struct usado para almacenar los diferentes rangos de tiempo de
 * inicio del envio de las exploradoras.
 */
struct AppTimeFormat{
  double appTime;
  int min;
  int max;

  AppTimeFormat():appTime(0),min(0),max(0){};
  AppTimeFormat(double v, int mi, int ma):appTime(v),min(mi),max(ma){};
};

/**
 * Struct usado para almacenar los diferentes rangos de las distribuciones
 * de probabilidad usadas en diferentes parámetros
 */
struct RandomStreamFormat {
	Ptr<RandomVariableStream> stream;
  int min;
  int max;

  RandomStreamFormat():stream(0),min(0),max(0){};
  RandomStreamFormat(Ptr<RandomVariableStream> s, int mi, int ma):stream(s),min(mi),max(ma){};
};

#endif /* ARAP_DEFINITIONS_H */
