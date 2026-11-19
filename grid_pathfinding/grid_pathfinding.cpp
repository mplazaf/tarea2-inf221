#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <cstdlib> 
#include "algorithms/path_result.h"
using namespace std;
namespace fs = std::filesystem;
const string dirInput = "data/grid_input";
const string dirOutput = "data/grid_output";
const string dirMeasurements = "data/measurements";
const int limiteFuerzaBruta = 32;
const double minTiempo = 50.0; 
const int maxRepeticiones = 1000;

long long leerMemoria(const string& campo){
    ifstream status("/proc/self/status");
    string linea;
    while (getline(status, linea)){
        if (linea.compare(0, campo.size(), campo) == 0) {
            return stoll(linea.substr(campo.size()));
        }
    }
    return 0;
}
void reiniciarPeakMemoria() {
    ofstream clearRefs("/proc/self/clear_refs");
    clearRefs << "5";
}

struct Grilla {
    int n, m, v, p;
    vector<vector<int>> types;
    vector<vector<long long>> values;
};

struct Instancia {
    string n, m, tipo, dominio, muestra;
    string nombreBase;  
};

struct Medicion {
    PathResult resultado;
    double tiempo;
    long long memoria;
    int repeticiones;
};
typedef PathResult (*Algoritmo)(int, int, int, int, const vector<vector<int>>&,const vector<vector<long long>>&);

bool sacarDatos(const fs::path& archivo, Instancia& inst) {
    inst.nombreBase = archivo.stem().string();

    vector<string> partes;
    stringstream ss(inst.nombreBase);
    string parte;
    while (getline(ss, parte, '_')) {
        partes.push_back(parte);
    }
    if (partes.size() != 5) {
        return false;
    }
    inst.n = partes[0];
    inst.m = partes[1];
    inst.tipo = partes[2];
    inst.dominio = partes[3];
    inst.muestra = partes[4];
    return true;
}
bool leerGrilla(const fs::path& archivo, Grilla& g) {
    ifstream in(archivo);
    if (!in) {
        return false;
    }
    if (!(in >> g.n >> g.m >> g.v >> g.p)) {
        return false;
    }
    g.types.clear();
    for (int i = 0; i < g.n; i++){
        vector<int> fila;
        for (int j = 0; j < g.m; j++){
            int x;
            if (!(in >> x)) {
                return false;
            }
            fila.push_back(x);
        }
        g.types.push_back(fila);
    }
    g.values.clear();
    for (int i = 0; i < g.n; i++) {
        vector<long long> fila;
        for (int j = 0; j < g.m; j++) {
            long long x;
            if (!(in >> x)) {
                return false;
            }
            fila.push_back(x);
        }
        g.values.push_back(fila);
    }
    return true;
}

Medicion medir(Algoritmo alg, const Grilla& g) {
    Medicion med;
    reiniciarPeakMemoria();
    long long memoriaBase = leerMemoria("VmRSS:");
    auto t0 = chrono::steady_clock::now();
    med.resultado = alg(g.n, g.m, g.v, g.p, g.types, g.values);
    auto t1 = chrono::steady_clock::now();
    med.memoria = leerMemoria("VmHWM:") - memoriaBase;
    double total = chrono::duration<double, milli>(t1 - t0).count();
    med.repeticiones = 1;

    while (total < minTiempo && med.repeticiones < maxRepeticiones) {
        t0 = chrono::steady_clock::now();
        PathResult r = alg(g.n, g.m, g.v, g.p, g.types, g.values);
        t1 = chrono::steady_clock::now();
        total += chrono::duration<double, milli>(t1 - t0).count();
        med.repeticiones++;
    }

    med.tiempo = total/med.repeticiones;
    return med;
}

void escribirSalida(const Instancia& inst, const string& nombreAlg, const PathResult& r){
    string ruta = dirOutput + "/" + inst.nombreBase + "_" + nombreAlg + "_out.txt";
    ofstream out(ruta);
    if (r.found) {
        out << r.score << "\n";
        out << r.path << "\n";
        out << r.remaining_lives << "\n";
    } else {
        out << "SIN SOLUCION\n";
    }
}

void escribirMedicion(ofstream& csv, const Instancia& inst, const string& nombreAlg, const Medicion& med){
    csv << inst.n << "," << inst.m << "," << inst.tipo << "," << inst.dominio << "," << inst.muestra << "," << nombreAlg << ","
        << med.tiempo << "," << med.memoria << "," << med.repeticiones << ",";
    if (med.resultado.found) {
        csv << 1 << "," << med.resultado.score << "," << med.resultado.remaining_lives;
    } else {
        csv << 0 << ",,";
    }
    csv << "\n";
}

int procesarGrilla(const fs::path& archivo){
    Instancia inst;
    Grilla g;
    if (!sacarDatos(archivo, inst)){
        cerr << "Nombre con formato inesperado: " << archivo << "\n";
        return 1;
    }
    if (!leerGrilla(archivo, g)){
        cerr << "No se pudo leer la grilla: " << archivo << "\n";
        return 1;
    }

    ofstream csvMed(dirMeasurements + "/mediciones.csv", ios::app);
    ofstream csvGreedy(dirMeasurements + "/greedy_calidad.csv", ios::app);
    reiniciarPeakMemoria();
    leerMemoria("VmRSS:");
    leerMemoria("VmHWM:");
    chrono::steady_clock::now();
    bool corrioBF = false;
    Medicion medBF;
    if (g.n + g.m <= limiteFuerzaBruta){
        corrioBF = true;
        medBF = medir(solveBruteForce, g);
        escribirSalida(inst, "bruteforce", medBF.resultado);
        escribirMedicion(csvMed, inst, "bruteforce", medBF);
    }

    Medicion medDP = medir(solveDP, g);
    escribirSalida(inst, "dp", medDP.resultado);
    escribirMedicion(csvMed, inst, "dp", medDP);

    Medicion medG = medir(solveGreedy, g);
    escribirSalida(inst, "greedy", medG.resultado);
    escribirMedicion(csvMed, inst, "greedy", medG);
    csvGreedy << inst.n << "," << inst.m << "," << inst.tipo << "," << inst.dominio << ","
              << inst.muestra << ",";
    if (medDP.resultado.found) {
        csvGreedy << medDP.resultado.score;
    }
    csvGreedy << ",";
    if (medG.resultado.found) {
        csvGreedy << 1 << "," << medG.resultado.score << ",";
        if (medDP.resultado.found) {
            csvGreedy << medDP.resultado.score - medG.resultado.score;
        }
    } else {
        csvGreedy << 0 << ",,";
    }
    csvGreedy << "\n";
    if (corrioBF) {
        PathResult a = medBF.resultado;
        PathResult b = medDP.resultado;
        bool coinciden = true;
        if (a.found != b.found) {
            coinciden = false;
        }
        if (a.found && b.found && a.score != b.score) {
            coinciden = false;
        }
        if (!coinciden) {
            cout << "ERROR: Fuerza bruta y DP no coinciden" << endl;
            return 1;
        }
    }
    cout << "ok" << endl;
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 2) {
        return procesarGrilla(argv[1]);
    }
    if (!fs::exists(dirInput)){
        cerr << "No existe " << dirInput << ". Ejecute desde code/grid_pathfinding/\n";
        return 1;
    }
    vector<fs::path> archivos;
    for (const auto& entrada : fs::directory_iterator(dirInput)) {
        if (entrada.path().extension() == ".txt") {
            archivos.push_back(entrada.path());
        }
    }
    sort(archivos.begin(), archivos.end());  
    fs::create_directories(dirOutput);
    fs::create_directories(dirMeasurements);
    ofstream csvMed(dirMeasurements + "/mediciones.csv");
    csvMed << "n,m,tipo,dominio,muestra,algoritmo,tiempo_ms,memoria_kb,repeticiones,"
              "encontrado,puntaje,vidas\n";
    csvMed.close();
    ofstream csvGreedy(dirMeasurements + "/greedy_calidad.csv");
    csvGreedy << "n,m,tipo,dominio,muestra,puntaje_optimo,greedy_encontrado,"
                 "puntaje_greedy,brecha\n";
    csvGreedy.close();
    int errores = 0;
    for (int i = 0; i < (int)archivos.size(); i++) {
        cout << "[" << i + 1 << "/" << archivos.size() << "] " << archivos[i].stem().string() << " ... " << flush;
        string comando = string(argv[0]) + " \"" + archivos[i].string() + "\"";
        int estado = system(comando.c_str());
        if (estado != 0) {
            errores++;
        }
    }
    cout << "\nGrillas procesadas: " << archivos.size() << "\n";
    cout << "Grillas con error o discrepancia Fuerza Bruta vs DP: " << errores << "\n";
    cout << "Mediciones guardadas en " << dirMeasurements << "/\n";
    return 0;
}
