/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/** @file */
#ifndef PATH_MANAGER_FACTORY_H
#define PATH_MANAGER_FACTORY_H

#include "arap-path-manager.h"
namespace ns3 {

/**
 * Fábrica de objetos que heredan de ArapPathManager, se usa esta clase para proveer
 * a los usuarios que deseen expandir el simulador con sus propias especializaciones, de
 * una interfaz sencilla e intuitiva y permitir leer los parámetros de dicha especialización
 * desde el archivo de parámetros.
 *
 * Para agregar una nueva especializacion solo se debe crear una nueva clase que herede
 * de ArapPathManager, implementar los métodos necesarios y luego definir los casos que se deseen
 * en el método PathManagerFactory::GetInstance(), donde se define luego del identificador, los
 * parámetros de la nueva especialización y el orden de los mismos en el archivo (El orden de
 * lectura del flujo de datos).
 */
class PathManagerFactory {

public:
	PathManagerFactory();
	~PathManagerFactory();

	/**
	 * Crea las instancias de las especializaciones de ArapPathManager
	 * @param[in] caseName Identificador del caso para el cual se creará la especialización
	 * @param[in] iss Flujo de datos de donde se leeran los parámetros del caso (De tenerlos)
	 *
	 * @return Puntero a un nuevo objeto creado según el caso indicado. La memoria creada por este
	 * método debe ser liberada por cada nodo (Donde se mantiene un puntero a dicha memoria)
	 */
	ArapPathManager* GetInstance(const std::string& caseName, std::istringstream& iss);
};
} /* namespace ns3 */

#endif /* PATH_MANAGER_FACTORY_H */
