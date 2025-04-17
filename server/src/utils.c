#include"utils.h"

t_log* logger;

int iniciar_servidor(void)
{
	int socket_servidor, err;
	struct addrinfo hints, *servinfo, *p; //servinfo es mi puntero a lista y P es mi puntero aux para recorrerla
	

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; 
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; //FLAG PASSIVE ACTIVADO PARA SOCKET DE ESCUCHA


	err = getaddrinfo(NULL, PUERTO, &hints, &servinfo); //NULL PARA SOCKET DE ESCUCHA

	if (err != 0)
	{
		log_error(logger, "Error en getaddrinfo: %s", gai_strerror(err));
		exit(EXIT_FAILURE);
	}

	p = servinfo;

	while (p != NULL) //Recorro la lista
	{
		// Creamos el socket de escucha del servidor
		socket_servidor = socket(p->ai_family, 
								 p->ai_socktype, 
								 p->ai_protocol);

		if (socket_servidor == -1)
		{
			log_warning(logger, "No se pudo crear el socket: %s", strerror(errno));
		}

		// Asociamos el socket a un puerto
		err = bind(socket_servidor, p->ai_addr, p->ai_addrlen);
		
		if (err == 0)
		{
			break; //Si se bindea exitosamente sale del bucle
		}

		log_error(logger, "Error al hacer bind: %s", strerror(errno));
		close(socket_servidor);

		p = p->ai_next; //ai_next es el puntero a siguiente en la lista
	}

	if (p == NULL)
	{
		log_error(logger, "No se pudo bindear a ninguna dirección");
		exit(EXIT_FAILURE);
	}


	// Escuchamos las conexiones entrantes
	err = listen(socket_servidor, SOMAXCONN);
	
	if (err == -1)
	{
		log_error(logger, "Error al poner en escucha: %s", strerror(errno));
		close(socket_servidor);
		exit(EXIT_FAILURE);
	}


	freeaddrinfo(servinfo);
	log_trace(logger, "Listo para escuchar a mi cliente");

	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{
  	struct sockaddr_in direccion_cliente;
    socklen_t tam_direccion = sizeof(struct sockaddr_in);


	// Aceptamos un nuevo cliente
	 int socket_cliente = accept(socket_servidor, (void*)&direccion_cliente, &tam_direccion);

	if (socket_cliente == -1)
	{
		log_error(logger, "Error al aceptar un cliente: %s", strerror(errno));
		return -1;
	}

	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;

	if (recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
	{
		return cod_op;
	}
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente) //BUFFER = espacio en memoria ocupado
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create(); //Ver commons/list.h
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);

	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);

		char* valor = malloc(tamanio);
		memcpy(valor, buffer + desplazamiento, tamanio);
		desplazamiento += tamanio;

		list_add(valores, valor); //Añade valor a mi lista valores
	}

	free(buffer);
	return valores;
}
