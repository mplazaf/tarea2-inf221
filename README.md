# Documentación

## Entrega

La entrega se realiza vía **aula.usm.cl** en formato `.zip`.

## Grid pathfinding

Algoritmos: Fuerza Bruta, Programación Dinámica, Greedy.

Se busca el camino de mayor puntaje desde la celda (1,1) hasta la celda (N,M) de una grilla con murallas y minas, moviéndose solo hacia la derecha (R) o hacia abajo (D) y sin quedarse sin vidas.

### Requisitos

- Linux (la medición de memoria lee `/proc/self/status`).
- `g++` con soporte para C++17 y `make`.
- Python 3 con `pandas` y `matplotlib` 

### Estructura

```
grid_pathfinding/
├── grid_pathfinding.cpp          Programa principal
├── makefile
├── algorithms/
│   ├── path_result.h             Struct PathResult y firmas de los algoritmos
│   ├── brute_force.cpp           Fuerza bruta 
│   ├── dynamic_programming.cpp   DP 
│   └── greedy.cpp                Greedy
├── scripts/
│   ├── grid_generator.py         Generador de grillas de entrada
│   └── plot_generator.py         Generador de gráficos
└── data/
    ├── grid_input/               Grillas de entrada {n}_{m}_{t}_{d}_{s}.txt
    ├── grid_output/              Caminos encontrados por cada algoritmo
    ├── measurements/             Mediciones
    └── plots/                    Gráficos
```

### Programa principal

Todos los comandos se ejecutan desde la carpeta `code/grid_pathfinding/`, porque el programa usa rutas relativas a ella.

**Compilación**

```bash
cd code/grid_pathfinding
make
```

Esto genera el ejecutable `exe`. Para borrarlo, usar `make clean`.

**Ejecución**

```bash
./exe
```

El programa recorre todas las grillas de `data/grid_input/` y, para cada una:

1. Ejecuta Fuerza Bruta (sólo si N + M ≤ 32), Programación Dinámica y Greedy.
2. Mide el tiempo y la memoria de cada algoritmo.
3. Verifica que Fuerza Bruta y Programación Dinámica obtengan el mismo puntaje.
4. Guarda los resultados en `data/grid_output/` y `data/measurements/`.

Al final muestra un resumen con la cantidad de grillas procesadas y la cantidad de grillas con error o con discrepancia entre Fuerza Bruta y Programación Dinámica (debe ser 0). Con las 288 grillas la ejecución tarda unos pocos minutos; lo más lento es Fuerza Bruta en las grillas de 16×16.

También se puede procesar una sola grilla, por ejemplo para probarla:

```bash
./exe data/grid_input/4_4_libre_D10_a.txt
```

En ese caso las filas se agregan al final de los CSV existentes. Para regenerar los CSV desde cero se ejecuta `./exe` sin argumentos.

**Salidas en `data/grid_output/`**

Un archivo `{n}_{m}_{t}_{d}_{s}_{alg}_out.txt` por grilla y algoritmo, con `alg` ∈ {`bruteforce`, `dp`, `greedy`}. Contiene el puntaje, la secuencia de movimientos y las vidas restantes:

```
29
RRRRDDD
1
```

o bien la única línea `SIN SOLUCION` si el algoritmo no encontró un camino válido.

**Mediciones en `data/measurements/`**

- `mediciones.csv`: una fila por grilla y algoritmo.

  | Columna | Descripción |
  |---|---|
  | `n`, `m`, `tipo`, `dominio`, `muestra` | Datos de la grilla, sacados del nombre del archivo |
  | `algoritmo` | `bruteforce`, `dp` o `greedy` |
  | `tiempo_ms` | Tiempo promedio por ejecución, en milisegundos |
  | `memoria_kb` | Peak de memoria extra durante la ejecución, en KB |
  | `repeticiones` | Veces que se ejecutó el algoritmo para promediar el tiempo |
  | `encontrado` | 1 si encontró un camino válido, 0 si no |
  | `puntaje`, `vidas` | Puntaje y vidas restantes (vacíos si no encontró camino) |

- `greedy_calidad.csv`: una fila por grilla, con `puntaje_optimo` (el de Programación Dinámica), `greedy_encontrado`, `puntaje_greedy` y `brecha` = s(C\*) − s(C_greedy). La brecha queda vacía cuando greedy no encuentra camino.

**Cómo se mide**

- *Tiempo:* con `std::chrono::steady_clock`. Si una ejecución tarda menos de 50 ms, se repite (hasta 1000 veces) y se promedia.
- *Memoria:* con los datos que Linux entrega en `/proc/self/status`. Antes de cada algoritmo se reinicia el peak de memoria (`VmHWM`) y se anota el uso actual (`VmRSS`); la memoria del algoritmo es la diferencia. Cada grilla se procesa en un proceso nuevo (el programa se llama a sí mismo con `system`) para que la memoria liberada por grillas anteriores no afecte la medición. La resolución es de páginas de 4 KB, por lo que Fuerza Bruta y Greedy, que usan memoria O(N + M), se miden como 0 KB.

### Scripts

**Generación de grillas**

```bash
python3 scripts/grid_generator.py
```

Genera las grillas de entrada en `data/grid_input/`. Se deben generar las 288 instancias (6 valores de N × 2 de M × 4 tipos × 2 dominios × 3 muestras) antes de ejecutar `./exe`.

**Generación de gráficos**

```bash
python3 scripts/plot_generator.py
```

Lee los CSV de `data/measurements/` (por lo tanto, se ejecuta después de `./exe`) y guarda en `data/plots/`:

| Archivo | Contenido |
|---|---|
| `tiempo_vs_tamano.png` | Tiempo promedio vs N·M de los tres algoritmos, un panel por tipo de grilla |
| `memoria_vs_tamano.png` | Memoria vs N·M de los tres algoritmos, junto a la memoria teórica de Programación Dinámica |
| `brecha_greedy.png` | Brecha promedio de greedy por tamaño y tipo de grilla, un panel por dominio |
| `falla_greedy.png` | Tasa de falla de greedy por tipo de grilla y dominio, y por tamaño |

Además imprime en consola tablas resumen de tiempo, memoria, tasa de falla y brecha.

### Ejecución completa

```bash
cd code/grid_pathfinding
python3 scripts/grid_generator.py   # solo si data/grid_input/ está vacía
make
./exe
python3 scripts/plot_generator.py
```

# tarea2-inf221
