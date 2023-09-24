# Técnicas de organización de archivos

## Table of Contents  
### 1. Introducción
1. [Objetivo del proyecto](#id1)
2. [Dataset](#id2)
3. [Resultados esperados](#id3)
### Técnicas utilizadas
4. [Técnicas de indexación](#id4)
5. [Algoritmos implementados](#id5)
6. [Análisis comparativo - teórico](#id6)
7. [Optimización de memoria secundaria](#id7)
8. [Implementación de Parser](#id8)

### Resultados experimentales
9. [Gráfico comparativo de las técnicas de indexación](#id9)
10. [Análisis de resultados experimentales](#ida)
11. [Interfaz gráfica](#idb)
12. [Nuestro genial equipo](#idc)
## Objetivo del proyecto<a name="id1"></a>
El propósito de este proyecto es la visualización y comparación de diversas técnicas de indexación de archivos, mediante la implementación de las estructuras que aprendimos en el curso de Base de Datos II.
## Dataset<a name="id2"></a>
El proyecto hace uso de un dominio de datos conformado por:
1. una columna de tipo **int**
2. una columna de tipo **char[40]**
3. una columna de tipo **int**
4. una columna de tipo **char[25]**
5. una columna de tipo **float**

Además, el dataset proporcionado debe ser un archivo _.csv_ con registros de longitud fija.

*Para los casos de prueba, hemos generado dos datasets diferentes con ayuda de las librerías _faker_ y _random_ de Python

## Resultados esperados<a name="id3"></a>
Los resultados esperados al aplicar estas técnicas de indexación incluyen:
- **Disminuir los accesos a memoria secundaria:** esperamos que cada vez que se realice una query se acceda directamente a los archivos indexados y luego al dataset, disminuyendo considerablemente las lecturas en memoria secundaria.
- **Mejora en la velocidad de acceso:** esperamos que las búsquedas y consultas en los archivos indexados sean más rápidas y eficientes en comparación con métodos de búsqueda lineal o sin indexación (O(n)).

- **Reducción en el tiempo de procesamiento:** Anticipamos que el tiempo necesario para realizar operaciones como inserciones y eliminaciones de registros también se verá reducido significativamente.

## Técnicas de indexación <a name="id4"></a>
Las estructuras implementadas son: **AVL File**, **Sequential File** y **Extendible Hashing**. 
- **AVL File:** Esta estructura hace uso de un archivo _heapfile_ para guardar la información de los nodos
- **Sequential File:** Esta estructura hace uso de un archivo _data.bin_ para las inserciones de los registros, y  el registro _auxiliar.bin_ para las nuevas inserciones que se iran almacenando en un espacio auxiliar.
- **Extendible Hashing:** Esta estructura hace uso de un archivo _hashing_directory.dat_ para guardar la información de los nodos y _hashing_index.dat_ para guardar los addresses respectivos. Cuando ocurra desbordamiento, la información desbordada estará se guardará en _hashing_overflow.dat_.

## Algoritmos implementados <a name="id5"></a>
- 
## Análisis comparativo - teórico <a name="id6"></a>
Este análisis se realizará en base a los accesos a memoria secundaria.


### AVLFile
archivo avl
Implementación de árbol de búsqueda binaria autoequilibrado que funciona con memoria secundaria para almacenar registros. Admite inserción, búsqueda basada en claves, búsqueda y eliminación basada en rangos.

Funciones miembro

- **insertar (tecla KeyType, puntero largo)**

Inserta un nuevo nodo (clave, puntero) en el archivo de índice. Dicho nodo almacena una referencia a un registro en un archivo de montón y su clave correspondiente asociada.
Internamente, desciende el AVL hasta alcanzar un nodo DISK_NULL para colocar el nuevo nodo al final del archivo de índice y reasignar el puntero padre.
Si ya existe un nodo con la misma clave, hay dos casos: si el archivo de índice indexa una clave no repetible, en cuyo caso se lanza una excepción; De lo contrario, los nodos que comparten la misma clave se vinculan en una lista directa.

- **remove**
Elimina (lógicamente) todos los registros tales que index(record) = key marcando todos los record.removed como verdadero y eliminando dichos nodos de AVL.



- **Complejidad**
  
  | Insert | Search | Search Range | Remove | 
  | :---: | :---: | :---:  | :---:  |
  | :---: | :---: | :---:  | :---:  |

![Copy of Base de datos2 _ proyecto1](https://github.com/Mariana-Capunay/Proyecto_bd2_grupo4/assets/91238621/f0b3daac-644c-4922-b22a-727ca0227574)

### Sequential File
El objetivo es poder aplicar la busqueda binaria para conseguir una complejidad de acceso a memoria secundaria O(log n)
Para ello, el archivo debe mantener los registros ordenados fisicamente en base al valor del campo de busqueda (key).
Principalmente se ordena en base a la llave primaria de la tabla. 

- **El algoritmo de búsqueda binaria**
El algoritmo de búsqueda binaria es usado para localizar un registro en el archivo dado un valor de búsqueda k.
Se requiere O(log N) accesos a memoria secundaria. 
En la búsqueda se debe descartar los registros marcados como eliminados. 

- **Estrategia del espacio auxiliar**
Las nuevas inserciones se van almacenando en un espacio auxiliar
Mantener un limite máximo de K registros en el espacio auxiliar
La búsqueda se debe hacer en ambos espacios.
Cada cierto tiempo el archivo de datos debe reconstruirse con los registros del espacio auxiliar.
El espacio auxiliar tiene un tamaño máximo de k = log2(n) registros, haciendo que la búsqueda se mantenga en O(log n).

- **Inserciones enlazadas**
Localizar la posición en donde será insertado el nuevo registro.
Si el espacio está libre, insertar.
Sino, insertar el registro en un espacio auxiliar.
En este caso, los punteros deberían ser actualizados.
Se requiere reorganizar el archivo original cada cierto tiempo mezclando ordenadamente con el espacio auxiliar. 

- **Eliminación de un registro**
Se utiliza los punteros para saltar las tuplas eliminadas, e inmediatamente se recontruye el archivo ordenado con todos los registros.

- **Complejidad**
  
  | Insert | Search | Search Range | Remove | 
  | :---: | :---: | :---:  |:------:|
  | O(log N) | O(log N) | O(log N) + O(K)   | (O(N)) |

![image](https://github.com/Mariana-Capunay/Proyecto_bd2_grupo4/assets/91238621/76c00d4b-0b4a-4d71-9b32-8f46a9f430f7)

### Extendible Hashing

- **Funcion hashing**
    Se utilizó una función polinómica si _key_ = $a_n,a_{n-1},...a_2,a_1,a_0$ , entonces _hashing_ = $a_0 + a_1 \cdot p + a_2 \cdot p^2 + ...$ . Donde $p$ es un primo que sirve como base.

- **Árbol binario**
    Se implementó el arbol binario a partir del _hashing_ de las _key_. Se establece un límite para el tamaño de los bucket de los nodos hojas.  

- **Manejo de overflow**       
    Cuando la altura exceda el límite permitido, las llaves se guardan en un archivo separado en el cual se recorre linealmente en la búsqueda y eliminación.

- **Complejidad** 

    Asumiendo una distribución estable.
  
  | Insert | Search | Search Range | Remove | 
  | :---: | :---: | :---:  | :---:  |
  | O(log(n)) | O(log(n)) | O(n)  | O(log(n))  |

## Optimización de memoria secundaria <a name="id7"></a>

## Implementación de Parser <a name="id8"></a>

## Gráfico comparativo de las técnicas de indexación <a name="id9"></a>
En este apartado incluiremos los cuadros comparativos de desempeño de las técnicas de indexación de archivos sobre el dominio de datos. Tanto para inserción como para búsqueda.  Usaremos dos métricas: total de accesos a disco duro (read & write) y tiempo de ejecución en milisegundos. 

### Cuadro comparativo inserción
### Cuadro comparativo búsqueda

## Análisis de resultados experimentales <a name="ida"></a>

## Interfaz gráfica <a name="idb"></a>
## 👩‍💻 Nuestro Genial Equipo <a name="idc"></a>

¡Estos cracks están detrás de todo el caos (código) aquí!

| [<img src="https://avatars.githubusercontent.com/u/91635108?v=4" width=115><br><sub>Mariana Capuñay</sub>](https://github.com/Mariana-Capunay) |  [<img src="https://avatars.githubusercontent.com/u/91238621?v=4" width=115><br><sub>Manyory Cueva</sub>](https://github.com/manyorycuevamendoza) | [<img src="https://avatars.githubusercontent.com/u/83987561?v=4" width=115><br><sub>Jaime Ramos</sub>](https://github.com/JaimeRamosT) | [<img src="https://avatars.githubusercontent.com/u/44882884?v=4" width=115><br><sub>Willian Berrocal</sub>](https://github.com/Willian-Berrocal) | [<img src="https://avatars.githubusercontent.com/u/55224553?v=4" width=115><br><sub>Gustavo Orosco</sub>](https://github.com/Pulsatio) |
| :---: | :---: | :---:  | :---:  | :---: |


