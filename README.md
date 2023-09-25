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
11. [Nuestro equipo](#idc)
## Objetivo del proyecto<a name="id1"></a>
El propósito de este proyecto es la visualización y comparación de diversas técnicas de indexación de archivos, mediante la implementación de las estructuras que aprendimos en el curso de Base de Datos II.
<p align="center">
  <img src="images/objective.jpg" alt="Objetivo" width="300" height="200">
</p>

## Dataset<a name="id2"></a>
El proyecto hace uso de un dominio de datos conformado por:
<p align="center">
  <img src="images/dataset_csv.png" alt="Dataset .csv" width="340" height="235">
</p>

1. una columna de tipo **int**
2. una columna de tipo **char[40]**
3. una columna de tipo **int**
4. una columna de tipo **char[25]**
5. una columna de tipo **float**

Además, el dataset proporcionado debe ser un archivo _.csv_ con registros de longitud fija. 
<p align="center">
  <img src="images/dataset.png" alt="Dataset" width="150" height="150">
</p>

Luego de ingresar un archivo_.csv_ válido (con las columnas indicadas previamente), se procede a convertir toda la data a binario para guardarla en un nuevo archivo (binario) con la siguiente estructura:
<p align="center">
  <img src="images/dataset_bin.png" alt="Dataset Binario" width="400" height="220">
</p>
<p align="center">
  <img src="images/record_struct.png" alt="Struct Record" width="500" height="130">
</p>
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
- Para el **AVL File** y **Sequential File** hemos implementado  **_insert_**, **_search_**, **_search_by_range_** y **_remove_**.
- Para el **Extendible Hashing** implementamos **_insert_**, **_search_** y **_search_by_range_**, debido a que no soporta la búsqueda por rango.
## Análisis comparativo - teórico <a name="id6"></a>
Este análisis se realizará en base a los accesos a memoria secundaria.


### AVLFile

<p align="center">
  <img src="images/avl_estructura.png" alt="Class AVL" width="500" height="330">
</p>




- **Complejidad**
  
  |  Insert   |  Search   | Search Range  |  Remove   | 
  |:---------:|:---------:|:-------------:|:---------:|       
  | O(log(n)) | O(log(n)) | O(log(n) + k) | O(log(n)) |       
    - n: Número de elementos almacenados.
    - k: Número de nodos dentro del rango objetivo.


### Sequential File
El objetivo es poder aplicar la busqueda binaria para conseguir una complejidad de acceso a memoria secundaria O(log n)
Para ello, el archivo debe mantener los registros ordenados fisicamente en base al valor del campo de busqueda (key).
Principalmente se ordena en base a la llave primaria de la tabla. 
<p align="center">
  <img src="images/sequential_estructura.png" alt="Class AVL" width="500" height="330">
</p>

- **El algoritmo de búsqueda binaria:**
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



### Extendible Hashing

<p align="center">
  <img src="images/Extendible_Hashing.jpg" alt="Class AVL" width="600" height="400">
</p>

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

   En el peor de los casos.
  
  | Insert | Search | Search Range | Remove | 
  | :---: | :---: | :---:  | :---:  |
  | O(n) | O(n) | O(n)  | O(n)  |

## Optimización de memoria secundaria <a name="id7"></a>
### Sequential file
Almacenar un archivo como una secuencia contigua de bytes tiene el problema obvio de que si el archivo crece, probablemente tendrá que moverse en el disco. Por ello, se puede optar por dividir los archivos en bloques de tamaño fijo que no necesitan ser adyacentes, sino apuntados uno tras otro por punteros.

### AVL File
La capacidad de autobalanceo de un árbol AVL hace eficiente uso de la memoria secundaria debido a que practicamente todas sus operaciones son logarítmicas con respecto al tamaño del archivo.

### Extendible Hashing
El indexado por Hashing es útil para acelerar la búsqueda y recuperación de datos mediante el uso de una función Hash. Debido a que un bucket puede contener varios registros, la inserción de datos resultó teniendo complejidad logarítmica.

## Implementación de Parser <a name="id8"></a>
Para implementar el parser(básico) de SQL, hicimos uso de la librería **regex**, la cual se encarga de procesar expresiones regulares y generamos instrucciones de acuerdo a las consultas disponibles (CREATE, INSERT, DELETE, SELECT)
- **CREATE TABLE <NOMBRE_TABLA> FROM FILE "ruta_achivo"**: esta instrucción espera recibir el nombre de la tabla y la ruta de un archivo .csv. Lo que hace es generar el dataset en binario (añadiendo el atributo removed a cada record) y, además, se encarga de insertar los elementos a las estructuras implementadas.
<p align="center">
  <img src="images/query_create.png" alt="Query CREATE" width="400" height="120">
</p>

- **SELECT * FROM <NOMBRE_TABLA>**: esta instrucción espera recibir el nombre de la tabla (creada previamente). Solo imprime 20 registros del dataset para no sobrecargar la terminal.
<p align="center">
  <img src="images/query_select_all.png" alt="Query SELECT*" width="400" height="200">
</p>
En caso de no recibir el mismo nombre que se asignó, imprime un mensaje de error en el que incluye el nombre de la tabla creada, ya que es la única en la que se pueden hacer las consultas. 

<p align="center">
  <img src="images/select_error_nombre_tabla.png" alt="Query SELECT* error" width="400" height="50">
</p>

- **SELECT * FROM <NOMBRE_TABLA> WHERE <_CONDICION_>** : esta instrucción espera recibir el nombre de la tabla y una condición (que incluya "=" o "between") evaluada sobre un atributo.
<p align="center">
  <img src="images/select_where.png" alt="Query SELECT* + WHERE" width="400" height="120">
</p>
Si recibe como atributo uno no válido (que no pertenece al dataset), bota un mensaje error.
<p align="center">
  <img src="images/select_atributo_no_valido.png" alt="Query SELECT* con error en nombre de atributo" width="400" height="90">
</p>

- **INSERT INTO <NOMBRE_TABLA> VALUES (<VALUE_1>, <VALUE_2>,<VALUE_3>, <VALUE_4>, <VALUE_5>)**:
Esta consulta inserta los valores ingresados a las estructuras correspondientes (por atributo) y al dataset en binario,considerando al record.removed = false

Además, verifica que los tipos de cada *value* sean los correctos.




## Gráfico comparativo de las técnicas de indexación <a name="id9"></a>
En este apartado incluiremos los cuadros comparativos de desempeño de las técnicas de indexación de archivos sobre el dominio de datos. Tanto para inserción como para búsqueda.  Usaremos dos métricas: total de accesos a disco duro (read & write) y tiempo de ejecución en milisegundos. 

### Cuadro comparativo inserción y búsqueda
<p align="center">
  <img src="images/hashing.jpeg" alt="Extendible Hashing" width="400" height="330">
</p>

  |  Nro de elementos   | 1000 | 5000  | 10000 | 50000 |100000|
  |:-------------------:| :---: | :---:  | :---:  | :---:  | :---:  |
  | Tiempo de búsqueda  |0.000213 | 0.000285  |0.000233  | 0.000359  | 0.000302 |
  | Tiempo de inserción | 0.000138 | 0.000124 | 0.000135  | 0.000206 | 0.000159 |
  
## Análisis de resultados experimentales <a name="ida"></a>


## Cómo ejecutar el programa
g++ parser.cpp dataset_bin/binary_conversor.cpp structs/AVL_File/avl_file.cpp .\structs\Extendible_Hashing_File\extendible_hashing_file.cpp
## 👩‍💻 Nuestro Genial Equipo <a name="idc"></a>

¡Estos cracks están detrás de todo el caos (código) aquí!

| [<img src="https://avatars.githubusercontent.com/u/91635108?v=4" width=115><br><sub>Mariana Capuñay</sub>](https://github.com/Mariana-Capunay) |  [<img src="https://avatars.githubusercontent.com/u/91238621?v=4" width=115><br><sub>Manyory Cueva</sub>](https://github.com/manyorycuevamendoza) | [<img src="https://avatars.githubusercontent.com/u/83987561?v=4" width=115><br><sub>Jaime Ramos</sub>](https://github.com/JaimeRamosT) | [<img src="https://avatars.githubusercontent.com/u/44882884?v=4" width=115><br><sub>Willian Berrocal</sub>](https://github.com/Willian-Berrocal) | [<img src="https://avatars.githubusercontent.com/u/55224553?v=4" width=115><br><sub>Gustavo Orosco</sub>](https://github.com/Pulsatio) |
| :---: | :---: | :---:  | :---:  | :---: |


