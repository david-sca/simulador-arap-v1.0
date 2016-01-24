/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/** @file */
#ifndef ARAP_NODE_H
#define ARAP_NODE_H

#include "ns3/application.h"
#include "ns3/socket.h"
#include "ns3/point-to-point-star.h"
#include "ns3/routing-table-row.h"
#include "ns3/arap-ants.h"
#include "ns3/arap-simulator.h"
#include "ns3/arap-path-manager.h"
#include "ns3/load-ants-statistics.h"
#include <stdio.h>
#include <map>

namespace ns3 {

/**
 * Nodo perteneciente a la red. Se encarga de enviar y recibir hormigas así como
 * de mantener la tabla de enrutamiento y las conexiones entre los demas nodos
 *
 * Esta clase es el pilar del simulador, simulandose en esta la capa aplicación y
 * al mismo tiempo la aplicación que dicta el funcionamiento del modelo a simular.
 */
class ArapNode : public Application {

public:

	ArapNode ();
	virtual ~ArapNode ();

	/**
	 * Usado para configurar los sockets y realizar las conexiones a los
	 * distintos nodos de la red a simular.
	 */
	void ConfigureSockets();

	/**
	 * Método heredado de la clase %ns3::Application, usado para asignar
	 * atributos que puedan asignarse mediante el sistema de configuración %ns3::Config,
	 * actualmente no hay ninguno definido (Ya que los parametros de simulación se leen
	 * desde un archivo).
	 */
	static TypeId GetTypeId (void);

	/**
	 * Asignar el valor de incremento de delay de computo.
	 *
	 * @param[in] increment Incremento a asignar en este nodo.
	 */
	void SetComputingDelayIncrement(const float increment);

	/**
	 * Asignar la distribución de probabilidad que indica los valores
	 * del delay de computo.
	 *
	 * @param[in] computingDelayStream Puntero a un objeto que define una distribución de probabilidad
	 */
	void SetComputingDelayStream(Ptr<RandomVariableStream> computingDelayStream);

	/**
	 * Asignar la especialización de ArapPathManager que usará el nodo.
	 *
	 * ArapPathManager se encarga de mantener la tabla de probabilidad para los
	 * distintos caminos y se encarga de generar los caminos a ser usados por las hormigas de carga
	 *
	 * @param[in] pathManager Referencia a un objeto especialización de la clase ArapPathManager
	 */
	void SetPathManager(const ArapPathManager& pathManager);

	/**
	 * Programar el envio de una hormiga de carga
	 *
	 * @param[in] dt Tiempo en el que se enviará la hormiga. Este tiempo se cuenta
	 * desde el momento en que se llama al método, es decir, si se llama en un tiempo X, la hormiga
	 * se enviará en el tiempo X+dt.
	 *
	 * @param[in] target IP del nodo destino al cual enviar la hormiga.
	 *
	 * @param[in] message Mensaje a enviar al nodo destino
	 */
	void ScheduleLoadAntSend(const std::string& message);

	/**
	 * @return La IP del nodo.
	 * */
	const Ipv4Address GetLocalIP() const;

	/**
	 * Imprime la tabla de probabilidad de este nodo.
	 */
	void PrintProbTable();

	/**
	 * @return Una referencia a el modelo donde se almacenan los diferentes valores
	 * de las distribucion de envio de hormigas de carga
	 */
	const LoadAntsStatistics& GetLoadAntsDelayModel() const;

	/**
	 * Asignar la distribución de probabilidad que indica la cantidad de hormigas
	 * de carga a enviar en cada tiempo de envio.
	 *
	 * @param[in] loadAntsQuantityStream Puntero a un objeto que define una distribución de probabilidad
	 */
	void setLoadAntsQuantityStream(Ptr<RandomVariableStream> loadAntsQuantityStream);

	/**
	 * Asignar la distribución de probabilidad que indica el destino final de las
	 * hormigas de carga a enviar en cada tiempo de envio.
	 *
	 * @param[in] loadAntsTargetStream Puntero a un objeto que define una distribución de probabilidad
	 */
	void setLoadAntsTargetStream(Ptr<RandomVariableStream> loadAntsTargetStream);

	/**
	 * Asignar la distribución de probabilidad que indica los tiempos de envia
	 * de las hormigas de carga.
	 *
	 * @param[in] loadAntsTimeStream Puntero a un objeto que define una distribución de probabilidad
	 */
	void setLoadAntsTimeStream(Ptr<RandomVariableStream> loadAntsTimeStream);

protected:

/**
 * Método heredado de la clase %ns3::Application, llamado al finalizar la simulación, usado como
 * una especie de Destructor por NS3.
 */
	virtual void DoDispose (void);

private:

	/**
	 * Método heredado de la clase %ns3::Application, este método se llama en el tiempo definido
	 * según el archivo de parametros, se encarga de iniciar el envio de las hormigas,
	 * las cuales se enviarán periodicamente a partir de este momento.
	 *
	 * @see SendExplorerAnts
	 */
	virtual void StartApplication (void);

	/**
	 * Método heredado de la clase %ns3::Application, este método se llama cuando se
	 * detiene la aplicación, actualmente no tiene utilidad  ya que se usa una red
	 * "estática" y las aplicaciones se detienen solo cuando finaliza la simulación.
	 *
	 * Este método se podria definir para detener el envío de manera periodica de
	 * exploradoras manteniendo las conexiones o para cerrar la conexion de este nodo
	 * con el resto de la red, esta implementación queda abierta para futuras versiones.
	 */
	virtual void StopApplication (void);

	/**
	 * Crea una hormiga de carga y la envia a la IP indicada.
	 *
	 * @param[in] target IP del nodo destino al cual enviar la hormiga.
	 *
	 * @param[in] message Mensaje a enviar al nodo destino
	 */
	void SendLoadAnt (const Ipv4Address& target, const std::string& message);

	/**
	 * Agrega el camino de una hormiga de carga a un archivo de salida.
	 *
	 * @param[in] path La lista que posee el camino que recorrera la hormiga
	 */
	void PrintLoadAntPath(std::list<Ipv4Address>& path);

	/**
	 * Envia una hormiga luego de pasado el tiempo de computo al procesarlo, y actualiza
	 * la tabla de enrutamiento del nodo. La ejecución de este método siempre se hace
	 * mediante una llamada a ns3::Simulator::Schedule.
	 *
	 * @param[in] ant Hormiga a enviar
	 *
	 * @param[in] source IP del nodo desde donde se esta enviando la hormiga
	 *
	 * @param[in] target IP del nodo siguiente  al cual enviar la hormiga
	 *
	 * @param[in] antID ID de la hormiga con el cual se identifica cada una de manera inequivoca
	 *
	 * @param[in] isRequest Indica si la hormiga a enviar corresponde a una petición (Request)
	 * o a una respuesta (Response)
	 */
	void SendDelayedAnt(Ptr<Packet> ant, const Ipv4Address& source, const Ipv4Address& target, const uint64_t& antID,bool isRequest);

	/**
	 * Retorna un valor de delay de computo de la distribución de probabilidad asignada
	 * con su respectivo incremento.
	 *
	 * @see m_computingDelayStream
	 *
	 * @see m_computingDelayIncrement
	 */
	const uint64_t GetComputingDelay() const;

  /**
   * Callback para la solicitud de conexión a este nodo.
   *
   * @param[in] socket Socket que se desea conectar
   *
   * @param[in] from Dirección (InetSocketAddress) de donde se realiza la petición de conexión
   *
   * @return true si se acepto la conexión o false si se rechazo.
   */
  bool ServerConnectRequestCallback (Ptr<Socket> socket, const Address& from);

  /**
   * Callback para aceptar una conexión a este nodo. Aquí se asigna el callback
   * para manejar la recepción de paquetes  (hormigas) que vengan desde la IP que se conectó
   *
   * @param[in] socket Socket que se esta conectando
   *
   * @param[in] from Dirección (InetSocketAddress) de donde se realizó y se acepta la conexión
   *
   * @see ReceivePacket
   */
  void ServerConnectAcceptCallback(Ptr<Socket> socket, const Address & from);

  /**
   * Callback para el envio de data del socket.
   *
   * @param[in] socket Socket del cual se llama el callback
   *
   * @param[in] size Tamaño en bytes disponible en el buffer de envio.
   */
  void ClientSendCallback(Ptr<Socket> socket, uint32_t size);

  /**
   * Maneja la llegada de una hormiga y realiza las
   * acciones correspondientes según el tipo de hormiga, una hormiga recibida
   * se clasifica de cuatro formas:
   *
   * - Request en nodo Intermedio
   * - Request en nodo Final
   * - Response en nodo Intermedio
   * - Response en nodo Final
   *
   * @param[in] socket Socket donde se recibió el paquete y que contiene al mismo.
   *
   * @see HandleReqMed
   * @see HandleReqFinal
   * @see HandleRespMed
   * @see HandleRespFinal
   */
  void ReceiveAnt (Ptr<Socket> socket);

  /**
   * Maneja el caso en que la hormiga es del tipo: "Request en nodo Intermedio".
   *
   * En este caso se lee la capa correspondiente, la cual incluye la IP del próximo
   * salto y se programa el envio a dicha IP.
   *
   * @param[in] data arreglo de bytes que contiene la data de la hormiga recibida
   *
   * @param[in] source IP de donde fue enviada la hormiga recibido
   *
   * @param[in] antID ID de la hormiga recibida
   */
  void HandleReqMed(const uint8_t* data, const Ipv4Address& source, const uint64_t antID);

  /**
   * Maneja el caso en que la hormiga es del tipo: "Request en nodo final".
   *
   * En este caso se lee la capa correspondiente, la cual incluye el tipo de hormiga y según
   * sea el tipo de esta realiza las acciones correspondientes. y luego envia la respuesta al
   * mismo nodo desde donde se envió la hormiga original.
   *
   * @param[in] data arreglo de bytes que contiene la data de la hormiga recibida
   *
   * @param[in] source IP de donde fue enviado el paquete recibido
   *
   * @param[in] antID ID de la hormiga recibida
   */
  void HandleReqFinal(const uint8_t* data, const Ipv4Address& source, const uint64_t antID);

  /**
   * Maneja el caso en que la hormiga es del tipo: "Response en nodo intermedio".
   *
   * En este caso se lee la capa correspondiente y se verifica en la tabla de enrutamiendo
   * desde que IP se genero el Request para esta respuesta, se envia la hormiga a dicho IP,
   * y se actualiza la tabla de enrutamiento
   *
   * @param[in] data arreglo de bytes que contiene la data de la hormiga recibida
   *
   * @param[in] source IP de donde fue enviado la hormiga recibida
   *
   * @param[in] antID ID de la hormiga  recibida
   */
  void HandleRespMed(const uint8_t* data, const Ipv4Address& target, const uint64_t antID);

  /**
   * Maneja el caso en que la hormiga es del tipo: "Response en nodo final".
   *
   * En este caso se lee y procesa la data enviada como respuesta y se actualiza
   * la tabla de enrutamiento.
   *
   * @param[in] data arreglo de bytes que contiene la data de la hormiga recibida
   *
   * @param[in] medium IP del nodo desde donde fue enviada la hormiga recibida
   *
   * @param[in] antID ID de la hormiga recibida
   */
  void HandleRespFinal(const uint8_t* data, const Ipv4Address& medium, const uint64_t antID);

  /**
   * Método usado en conjunto con SendExplorerAnts() para realizar el envio de un conjunto
   * hormigas exploradoras a los demás nodos de la red. y configurar el envio periódico de las mismas
   *
   * @see SendExplorerAnts
   */
  void ExplorePaths();

  /**
   * Envia hormigas a explorar todos los posibles caminos que llevan al destino indicado.
   *
   * @param[in] target IP del nodo destino para el cual se están explorando los caminos.
   *
   * @see ExplorePaths
   */
  void SendExplorerAnts(const Ipv4Address& target);

  /**
   * Busca una fila en la tabla de enrutamiento.
   *
   * Las filas de la tabla de enrutamiento son tuplas de la forma (IP Origen, IP Destino, ID de Hormiga).
   *
   * @param[in] antID ID de hormiga que identifica inequivocamente cada fila (Al igual que cada hormiga)
   *
   * @return La fila que contiene el antID o NULL si no hay ninguna fila con ese ID.
   */
  Ptr<RoutingTableRow> SearchRoutingTable(const uint64_t& antID);

  Ptr<RandomVariableStream> m_computingDelayStream; //!< Distribución de probabilidad para el delay de computo
  Ptr<Socket> m_socketServer;//!< Socket por el que se reciben los paquetes (hormigas)
  std::map<Ipv4Address, Ptr<Socket> > m_socketClientMap;//!< Mapa de Sockets por los cuales se envian paquetes (hormigas) a los demás nodos
  Ipv4Address m_localIP; //!< IP del nodo.
  ArapAnts m_arapAnt; //!< Objeto encargado de manejar la estructura y creación de los distintos tipos de hormigas
  std::map<uint64_t,Ptr<RoutingTableRow> > m_routingTable; //!< Tabla de enrutamiento
  float m_computingDelayIncrement;//!< Incremento del delay de computo
  ArapPathManager* m_pathManager; //!< Maneja las tablas de probabilidad y la creación de caminos según la especialización
  LoadAntsStatistics m_loadAntsDelayModel; //!< Modelo para mantener los valores de la distribución generada por los delay de las hormigas de carga

  Ptr<RandomVariableStream> m_LoadAntsTimeStream; //!< Distribución de probabilidad para el tiempo de envio de hormigas de carga
  Ptr<RandomVariableStream> m_LoadAntsQuantityStream; //!< Distribución de probabilidad para la cantidad de hormigas de carga por cada tiempo de envio
  Ptr<RandomVariableStream> m_LoadAntsTargetStream; //!< Distribución de probabilidad para el destino de las hormigas de carga en cada tiempo de envio
};

} /* namespace ns3 */
#endif /* ARAP_NODE_H */
