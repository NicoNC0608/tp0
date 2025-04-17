#include "client.h"

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	int conexion;
	char* ip;
	char* puerto;
	char* valor;

	
	t_log* logger;
	t_config* config;
	

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();

	// Usando el logger creado previamente
	// Escribi: "Hola! Soy un log"
	log_info(logger, "Hola!! Soy un log");
	//Recordar destruir el log cuando no lo use más con log_destroy()


	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();

	if (config == NULL)
	{
		log_info(logger, "¡No se pudo crear el config!");
		abort();
	}

	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'
	valor = config_get_string_value(config, "CLAVE");
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");

	// Loggeamos el valor de config
	log_info(logger, "KEY: %s", valor);
	log_info(logger, "IP: %s", ip);
	log_info(logger, "PORT: %s", puerto); //Por pantalla me muestra PORT: pero en el archivo .config la variable es PUERTO


	/* ---------------- LEER DE CONSOLA ---------------- */

	leer_consola(logger);


	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip, puerto);

	// Enviamos al servidor el valor de CLAVE como mensaje
	enviar_mensaje(valor, conexion);


	// Armamos y enviamos el paquete
	paquete(conexion);

	terminar_programa(conexion, logger, config);

	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/
	// Proximamente
}


t_log* iniciar_logger(void) 
{
	t_log* nuevo_logger;
	nuevo_logger = log_create("tp0.log", "TP0", true, LOG_LEVEL_INFO);

	return nuevo_logger;
}


t_config* iniciar_config(void)
{
	t_config* nuevo_config;
	nuevo_config = config_create("/home/utnso/Documents/tp0-ssh/client/cliente.config"); //Vinculo con el archivo .config

	return nuevo_config;
} 


void leer_consola(t_log* logger)
{
	char* leido;

	// Revisar manual readline
	while (1)
	{
		leido = readline("> "); //Lee lo que le ingreso por consola desde el > hasta que apriete enter

		if (strlen(leido) == 0) //Si solo ingreso enter por consola, sale
		{
			free(leido); //Libero la memoria reservada
			break;
		}

		log_info(logger, "%s", leido); //Si ingreso algo por consola, lo loggeo en el momento
		free(leido);
	}
}


void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido;
	t_paquete* paquete = crear_paquete();

	// Leemos y esta vez agregamos las lineas al paquete
	while (1)
	{
		leido = readline("> ");

		if (strlen(leido) == 0)
		{
			free(leido);
			break;
		}

		agregar_a_paquete(paquete, leido, strlen(leido) + 1); // +1 para el '\0' por ser streams y no strings
		free(leido);
	}

	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	enviar_paquete(paquete, conexion); //Sendeo el paquete al server
	eliminar_paquete(paquete);
}


void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	liberar_conexion(conexion);
	log_destroy(logger);
	config_destroy(config);
}