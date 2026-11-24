# Documentación
Nombre: Miguel Plaza
Rol: 202473058-4
Semestre: 2026-2
TAREA 2 
ALGORITMOS Y COMPLEJIDAD INF-221

## Entrega
La entrega se realiza vía **aula.usm.cl** en formato `.zip`.

## Grid pathfinding
Algoritmos: Fuerza Bruta, Programación Dinámica y Greedy.
- Las fuentes de dónde fueron extraídos los algoritmos, junto con sus decisiones de diseño, están al inicio de sus archivos cpp en la carpeta `algorithms/`.
- Se usa un archivo `algorithms/path_result.h` que contiene el struct `PathResult` y las firmas de los tres algoritmos, para importarlos a `grid_pathfinding.cpp`.
- Fuerza Bruta solo se ejecuta en las grillas con N + M ≤ 32. En el resto se ejecutan Programación Dinámica y Greedy.
- En caso de empate en puntaje, cualquier camino óptimo es válido: Fuerza Bruta conserva el primero encontrado y Programación Dinámica el que termina con menos vidas. Por eso pueden entregar caminos distintos con el mismo puntaje, y la verificación entre ambos compara solo el puntaje.
- Requisitos: Linux (la memoria se mide leyendo `/proc/self/status`), `g++` con C++17, `make` y Python 3 con `pandas` y `matplotlib`.

### Programa principal
- Todos los comandos se ejecutan desde la carpeta `code/grid_pathfinding/`, ya que el programa usa rutas relativas a ella.
- Compilación: escribir en terminal "make". Use "make clean" para eliminar el ejecutable.
- Ejecución: escribir el comando en terminal "./exe". Se procesan todas las grillas de `data/grid_input/`.
- Para procesar una sola grilla: "./exe data/grid_input/nombre.txt". Sus resultados se agregan al final de los CSV existentes.
- Al ejecutarse, los caminos encontrados por cada algoritmo se guardan en `data/grid_output/` y las mediciones en `data/measurements/`:
  - `mediciones.csv`: tiempo (ms), memoria (KB), puntaje y vidas de cada algoritmo en cada grilla.
  - `greedy_calidad.csv`: puntaje óptimo, puntaje de greedy y brecha en cada grilla.
- Al terminar se muestra un resumen con las grillas procesadas y las discrepancias entre Fuerza Bruta y Programación Dinámica, que debe ser 0.
- El tiempo se mide con `std::chrono`, repitiendo y promediando las ejecuciones de menos de 50 ms. La memoria se mide como el peak de memoria (`VmHWM`) menos el uso inicial (`VmRSS`); cada grilla se procesa en un proceso nuevo para que la memoria de grillas anteriores no afecte la medición.

### Scripts
- Para ejecutar los archivos de la carpeta scripts, ejecute el comando "python3 scripts/nombrearchivo.py" desde `code/grid_pathfinding/`.
- Tras ejecutar "python3 scripts/grid_generator.py", las grillas serán guardadas en el directorio `data/grid_input`.
- Tras ejecutar "python3 scripts/plot_generator.py" (después de "./exe"), los gráficos serán guardados en el directorio `data/plots` y se imprimirán en consola tablas resumen de tiempo, memoria, brecha y tasa de falla.

## Informe
- El informe se encuentra en la carpeta `report/`, y sus referencias en `report/references.bib`.
- Las figuras del informe se toman directamente de `code/grid_pathfinding/data/plots/`, por lo que corresponden a las generadas por `plot_generator.py` a partir de las mediciones.
- El enlace al manual de la GNU C Library citado en el informe (https://www.gnu.org/software/libc/manual/html_node/Malloc-Tunable-Parameters.html) redirige automáticamente a la nueva ubicación del manual en https://sourceware.org/glibc/manual/. La sección consultada es "Malloc Tunable Parameters".
