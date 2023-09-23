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
- **Sequential File:** Esta estructura ...
- **Extendible Hashing:** Esta estructura ...

## Algoritmos implementados <a name="id5"></a>
- 
## Análisis comparativo - teórico <a name="id6"></a>
Este análisis se realizará en base a los accesos a memoria secundaria.

### AVLFile
### Sequential File
### B+ Tree


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


