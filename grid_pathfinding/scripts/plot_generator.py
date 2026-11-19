"""
Script para generar gráficos a partir de las mediciones experimentales.
Lee los resultados desde data/measurements/ y guarda los gráficos en data/plots/.

Métricas a graficar:
  - Tiempo de ejecución vs tamaño (N * M) para cada algoritmo y tipo de grilla.
  - Uso de memoria vs tamaño para cada algoritmo.
  - Brecha de puntaje Delta = s(OPT) - s(Greedy) para casos donde greedy encuentra solución.
  - Tasa de falla de Greedy (% de casos con 'SIN SOLUCION') según tipo de grilla y dominio.

Uso (desde code/grid_pathfinding/):
  python3 scripts/plot_generator.py

Requiere: pandas y matplotlib  (pip install pandas matplotlib)
"""
import os
import matplotlib.pyplot as plt
import pandas as pd

MEASUREMENTS_DIR = os.path.join(os.path.dirname(__file__), "..", "data", "measurements")
PLOTS_DIR = os.path.join(os.path.dirname(__file__), "..", "data", "plots")
V = 3
AZUL, NARANJA, VERDE_AGUA, AMARILLO = "#2a78d6", "#eb6834", "#1baf7a", "#eda100"

ESTILO_ALGORITMO = {
    "bruteforce": {"color": AZUL,       "marker": "o", "nombre": "Fuerza bruta"},
    "dp":         {"color": NARANJA,    "marker": "s", "nombre": "Programación dinámica"},
    "greedy":     {"color": VERDE_AGUA, "marker": "^", "nombre": "Greedy"},
}
ESTILO_TIPO = {
    "libre":    {"color": AZUL,       "marker": "o"},
    "murallas": {"color": NARANJA,    "marker": "s"},
    "minas":    {"color": VERDE_AGUA, "marker": "^"},
    "mixta":    {"color": AMARILLO,   "marker": "D"},
}
ESTILO_DOMINIO = {
    "D10":   {"color": AZUL},
    "D1000": {"color": NARANJA},
}

plt.rcParams.update({
    "figure.dpi": 150,
    "font.size": 10,
    "axes.spines.top": False,
    "axes.spines.right": False,
    "axes.grid": True,
    "grid.color": "#e4e3df",
    "grid.linewidth": 0.6,
    "axes.edgecolor": "#8a8984",
    "lines.linewidth": 2,
    "lines.markersize": 6,
    "legend.frameon": False,
})

def cargar_datos():
    mediciones = pd.read_csv(os.path.join(MEASUREMENTS_DIR, "mediciones.csv"))
    calidad = pd.read_csv(os.path.join(MEASUREMENTS_DIR, "greedy_calidad.csv"))
    mediciones["celdas"] = mediciones["n"] * mediciones["m"]
    calidad["celdas"] = calidad["n"] * calidad["m"]
    return mediciones, calidad

def ejes_de_tamanos(calidad):
    """Tamaños ordenados, sus etiquetas 'NxM' y la posición de cada uno en el eje X."""
    tamanos = calidad[["n", "m", "celdas"]].drop_duplicates().sort_values("celdas")
    etiquetas = [f"{n}×{m}" for n, m in zip(tamanos["n"], tamanos["m"])]
    posicion = {c: i for i, c in enumerate(tamanos["celdas"])}
    return etiquetas, posicion

def guardar(fig, nombre):
    fig.savefig(os.path.join(PLOTS_DIR, nombre))
    plt.close(fig)

def graficar_tiempo(mediciones):
    fig, ejes = plt.subplots(2, 2, figsize=(10, 7.5), sharex=True, sharey=True)
    for ax, tipo in zip(ejes.flat, ESTILO_TIPO):
        datos_tipo = mediciones[mediciones["tipo"] == tipo]
        for algoritmo, estilo in ESTILO_ALGORITMO.items():
            datos = datos_tipo[datos_tipo["algoritmo"] == algoritmo]
            promedio = datos.groupby("celdas")["tiempo_ms"].mean()
            ax.plot(promedio.index, promedio.values, color=estilo["color"],
                    marker=estilo["marker"], label=estilo["nombre"])
        ax.set_xscale("log")
        ax.set_yscale("log")
        ax.set_title(f"Grilla {tipo}")
    for ax in ejes[1]:
        ax.set_xlabel("Tamaño de la grilla N·M (celdas)")
    for ax in ejes[:, 0]:
        ax.set_ylabel("Tiempo promedio (ms)")

    handles, labels = ejes[0, 0].get_legend_handles_labels()
    fig.legend(handles, labels, loc="lower center", ncol=3)
    fig.suptitle("Tiempo de ejecución según tamaño y tipo de grilla")
    fig.tight_layout(rect=(0, 0.05, 1, 1))
    guardar(fig, "tiempo_vs_tamano.png")

def graficar_memoria(mediciones):
    fig, ax = plt.subplots(figsize=(7, 4.5))
    for algoritmo, estilo in ESTILO_ALGORITMO.items():
        datos = mediciones[mediciones["algoritmo"] == algoritmo]
        promedio = datos.groupby("celdas")["memoria_kb"].mean()
        ax.plot(promedio.index, promedio.values, color=estilo["color"],
                marker=estilo["marker"], label=estilo["nombre"])
        if algoritmo == "dp":
            teorica = promedio.index * (V + 1) * 9 / 1024
            ax.plot(promedio.index, teorica, color="#8a8984", linestyle="--",
                    label="DP teórica: N·M·(V+1)·9 bytes")
    ax.set_xscale("log")
    ax.set_yscale("symlog", linthresh=10)
    ax.set_ylim(bottom=0)
    ax.set_xlabel("Tamaño de la grilla N·M (celdas)")
    ax.set_ylabel("Memoria extra (KB)")
    ax.set_title("Uso de memoria según tamaño de la grilla")
    ax.legend(loc="upper left")
    fig.text(0.01, 0.01, "Fuerza bruta y greedy usan O(N+M) memoria: menos de una "
             "página de 4 KB, por eso se miden como 0 KB.", fontsize=8, color="#52514e")
    fig.tight_layout(rect=(0, 0.04, 1, 1))
    guardar(fig, "memoria_vs_tamano.png")

def graficar_brecha(calidad):
    etiquetas, posicion = ejes_de_tamanos(calidad)
    con_camino = calidad[calidad["greedy_encontrado"] == 1]
    dominios = sorted(calidad["dominio"].unique())

    fig, ejes = plt.subplots(1, len(dominios), figsize=(11, 4.5), squeeze=False)
    for ax, dominio in zip(ejes[0], dominios):
        for tipo, estilo in ESTILO_TIPO.items():
            datos = con_camino[(con_camino["dominio"] == dominio) & (con_camino["tipo"] == tipo)]
            if len(datos) == 0:
                continue
            promedio = datos.groupby("celdas")["brecha"].mean()
            x = [posicion[c] for c in promedio.index]
            ax.plot(x, promedio.values, color=estilo["color"], marker=estilo["marker"], label=tipo)
        ax.set_xticks(range(len(etiquetas)))
        ax.set_xticklabels(etiquetas, rotation=45, ha="right")
        ax.set_yscale("symlog", linthresh=10)
        ax.set_ylim(bottom=0)
        ax.set_xlabel("Tamaño de la grilla (N×M)")
        ax.set_ylabel("Brecha promedio Δ = s(OPT) − s(Greedy)")
        ax.set_title(f"Dominio {dominio}")
    ejes[0][0].legend(title="Tipo de grilla", loc="upper left")
    fig.suptitle("Brecha de greedy respecto al óptimo")
    fig.tight_layout()
    guardar(fig, "brecha_greedy.png")

def graficar_falla(calidad):
    etiquetas, posicion = ejes_de_tamanos(calidad)
    tipos = list(ESTILO_TIPO)
    dominios = sorted(calidad["dominio"].unique())

    fig, (ax_tipo, ax_tamano) = plt.subplots(1, 2, figsize=(12, 4.5),
                                             gridspec_kw={"width_ratios": [1, 1.6]})

    ancho = 0.8 / len(dominios)
    for k, dominio in enumerate(dominios):
        tasas = []
        for tipo in tipos:
            datos = calidad[(calidad["tipo"] == tipo) & (calidad["dominio"] == dominio)]
            tasas.append(100 * (1 - datos["greedy_encontrado"].mean()))
        x = [i - 0.4 + ancho * (k + 0.5) for i in range(len(tipos))]
        barras = ax_tipo.bar(x, tasas, width=ancho * 0.92, color=ESTILO_DOMINIO[dominio]["color"],
                             label=dominio)
        ax_tipo.bar_label(barras, fmt="%.0f%%", fontsize=8, color="#52514e")
    ax_tipo.set_xticks(range(len(tipos)))
    ax_tipo.set_xticklabels(tipos)
    ax_tipo.set_ylim(0, 105)
    ax_tipo.grid(axis="x", visible=False)
    ax_tipo.set_xlabel("Tipo de grilla")
    ax_tipo.set_ylabel("Tasa de falla (%)")
    ax_tipo.set_title("Por tipo de grilla y dominio")
    ax_tipo.legend(title="Dominio", loc="upper left")

    for tipo, estilo in ESTILO_TIPO.items():
        datos = calidad[calidad["tipo"] == tipo]
        tasa = 100 * (1 - datos.groupby("celdas")["greedy_encontrado"].mean())
        x = [posicion[c] for c in tasa.index]
        ax_tamano.plot(x, tasa.values, color=estilo["color"], marker=estilo["marker"], label=tipo)
    ax_tamano.set_xticks(range(len(etiquetas)))
    ax_tamano.set_xticklabels(etiquetas, rotation=45, ha="right")
    ax_tamano.set_ylim(-5, 105)
    ax_tamano.set_xlabel("Tamaño de la grilla (N×M)")
    ax_tamano.set_title("Por tamaño y tipo de grilla")
    ax_tamano.legend(title="Tipo de grilla", loc="upper left", bbox_to_anchor=(1, 1))
    fig.suptitle("Tasa de falla de greedy (% de casos SIN SOLUCION)")
    fig.tight_layout()
    guardar(fig, "falla_greedy.png")

def imprimir_resumen(mediciones, calidad):
    pd.set_option("display.width", 200)

    print("\n=== Tiempo promedio (ms) por tamaño y algoritmo ===")
    tabla_tiempo = mediciones.pivot_table(index=["n", "m"], columns="algoritmo",
                                          values="tiempo_ms", aggfunc="mean")
    print(tabla_tiempo.to_string(float_format=lambda v: f"{v:.4g}"))

    print("\n=== Memoria promedio (KB) por tamaño y algoritmo ===")
    tabla_memoria = mediciones.pivot_table(index=["n", "m"], columns="algoritmo",
                                           values="memoria_kb", aggfunc="mean")
    print(tabla_memoria.to_string(float_format=lambda v: f"{v:.0f}"))

    print("\n=== Greedy por tipo y dominio ===")
    resumen = calidad.groupby(["tipo", "dominio"]).agg(
        instancias=("greedy_encontrado", "size"),
        tasa_falla_pct=("greedy_encontrado", lambda s: 100 * (1 - s.mean())),
        brecha_promedio=("brecha", "mean"),
        brecha_maxima=("brecha", "max"),
    )
    print(resumen.to_string(float_format=lambda v: f"{v:.2f}"))
def main():
    os.makedirs(PLOTS_DIR, exist_ok=True)
    mediciones, calidad = cargar_datos()
    graficar_tiempo(mediciones)
    graficar_memoria(mediciones)
    graficar_brecha(calidad)
    graficar_falla(calidad)
    imprimir_resumen(mediciones, calidad)
    print("\nGráficos guardados en:", os.path.abspath(PLOTS_DIR))
if __name__ == "__main__":
    main()
