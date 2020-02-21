char* AR_SHM_SLOT_NAME_FORMAT = "AR_SHM_SLOT_%u";
//Permite crear un objeto en memoria compartida, y definirle un tamaño.
int crearSHM(uint32_t slotId, uint32_t size)
{
	int shm_fd;
	char name[100];

	// Convertimos el Id del slot de memoria
	sprintf(name, AR_SHM_SLOT_NAME_FORMAT, slotId);
	// Creamos el objeto de memoria compartida con acceso de lectura y escritura
	shm_fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, S_IRWXU);
	if( shm_fd == -1 ) {
		perror("shm_open ERROR: Error al crear o abrir objeto en memoria."); // Si no se puede crear, se indica
		exit(1);
	}
	// Definimos el tamaño de la memoria compartida
	if(ftruncate(shm_fd, size) == -1){
		perror("ftruncate ERROR: Error al asignar espacio en memoria."); // Si no se puede definir el tamaño, se indica
		exit(1);
	}
	return shm_fd;
}

//Mapea el objeto que ya está creado en memoria compartida dentro del ESPACIO DE DIRECCIONES DEL PROCESO
void* mapearSHM(uint32_t slotId, uint32_t size)
{
	int shm_fd;
	void *shared_memory = (void *)0;
	char name[100];
	// Convertimos el Id del slot de memoria
	sprintf(name, AR_SHM_SLOT_NAME_FORMAT, slotId);
	// Abrimos el objeto de memoria compartida con acceso de lectura y escritura
	shm_fd = shm_open(name, O_RDWR | O_CREAT, S_IRWXU);
	if( shm_fd == -1 ) {
		perror("shm_open ERROR: Error al abrir objeto en memoria."); // Si no se puede abrir, se indica
		exit(1);
	}
	if( shm_fd > 0 ) {
		// Mapeamos el objeto de memoria compartida
		shared_memory = mmap( 0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0 );
		if(shared_memory == MAP_FAILED){
			perror("mmap ERROR: Error al mapear el objeto en el espacio de direcciones del proceso."); // Si no se puede mapear, se indica
			exit(1);
		}
		return shared_memory;
	}
	return 0;
}

//Leer del espacio compartido
void leerSHM(void* slot,void* data,uint32_t size)
{
	//Realizamos una copia de los datos del slot a data
	memcpy(data, slot, size);
}

//Escribir en el espacio compartido
void escribirSHM(void* slot,void* data,uint32_t size)
{
	//Realizamos una copia de los data al slot de memoria
	memcpy(slot, data, size);
}

//Borra el slot de memoria compartida
void borrarSHM(uint32_t slotId, void* slot, uint32_t desObject, uint32_t size)
{
	char name[100];
	// Convertimos el Id del slot de memoria
	sprintf(name, AR_SHM_SLOT_NAME_FORMAT, slotId);
	//Cerramos el objeto de memoria compartida
	if (close(desObject) == -1){
		perror("close ERROR: Error al cerrar el objeto de memoria compartida."); // Si no se puede cerrar el slot, se indica
		exit(1);
	}
	//Desmapeamos el objeto de memoria compartida
	if (munmap(slot,size) == -1){
		perror("munmap ERROR: Error al eliminar el objeto del espacio de direcciones del proceso."); // Si no se puede "desmapear", se indica
		exit(1);
	}
	//Desvinculamos el objeto de memoria compartida
	if (shm_unlink(name) == -1){
		perror("shm_unlink ERROR: Error al desvincular el objeto de memoria."); // Si no se puede desvincular el objeto, se indica
		exit(1);
	}
}
