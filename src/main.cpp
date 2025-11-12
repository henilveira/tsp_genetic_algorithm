#include <unordered_map>
#include <tuple>
#include <cmath>
#include <string>
#include <cstdlib> 
#include <fstream>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>

#define POPULATION_SIZE 100
#define TARGET_FITNESS 0.0
#define MAX_GENERATIONS 10

std::ofstream csv("results/fitness_data.csv");

const std::string CITIES = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const int MAX_DISTANCE = 100;

// std::unordered_map<char, std::tuple<int,int>> MAP;
std::unordered_map<char, std::tuple<int, int>> MAP = {
    {'A', {0, 0}},
    {'B', {2, 3}},
    {'C', {5, 4}},
    {'D', {1, 6}},
    {'E', {6, 1}},
    {'F', {3, 7}},
    {'G', {0, 9}},
    {'H', {54, 0}},
    {'I', {4, 6}},
};

double brute_force_tsp(const std::unordered_map<char, std::tuple<int, int>> &mapa)
{
    std::vector<char> cities;
    for (auto &p : mapa)
        cities.push_back(p.first);

    double min_distance = 1e9;

    std::sort(cities.begin(), cities.end());
    do
    {
        double total = 0.0;
        for (int i = 0; i < (int)cities.size(); i++)
        {
            char from = cities[i];
            char to = cities[(i + 1) % cities.size()];
            auto [x1, y1] = mapa.at(from);
            auto [x2, y2] = mapa.at(to);
            total += std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
        }

        min_distance = std::min(min_distance, total);
    } while (std::next_permutation(cities.begin(), cities.end()));

    return min_distance;
}


int random_num(int start, int end)
{
    int range = (end-start)+1;
    int random_int = start+(rand()%range);
    return random_int;
}

char generate_gene(const int &index)
{
    int r = random_num(0, index-1);
    return CITIES[r];
}


void generate_map()
{
    int MAX_CITIES = CITIES.size();
    int NUM_CITIES = random_num(4,MAX_CITIES);
    std::cout << "Número de cidades: " << NUM_CITIES << std::endl;
    for (int i = 0; i < NUM_CITIES; i++)
    {
        char city = CITIES[i];
        MAP[city] = std::make_tuple(random_num(0, MAX_DISTANCE), random_num(0, MAX_DISTANCE));
    }
}

std::string generate_chromossome()
{
    int len = MAP.size();
    std::string chromossome = "";
    char gene;
    std::set<char> used;

    while (chromossome.length() < len)
    {
        gene = generate_gene(len);
        if (!used.count(gene))
        {
            chromossome += gene;
            used.insert(gene);
        }
    }
    return chromossome;
}

void print_map(const std::unordered_map<char, std::tuple<int, int>>& mapa) {
    std::cout << "Conteúdo do Mapa:" << std::endl;
    
    for (const auto& par : mapa) {
        int x = std::get<0>(par.second);
        int y = std::get<1>(par.second);

            std::cout << "  Chave: " << par.first
                  << " -> Valor: (" << x << ", " << y << ")" << std::endl;
    }
}

int dist(int x1, int x2, int y1, int y2)
{
    return std::sqrt(std::pow((x2-x1),2)+std::pow((y2-y1),2));
}


struct Route {
    std::string chromosome;
    double total_dist;
    double fitness;
    
    Route(std::string chromosome);
    Route cross_over(const Route& parent2) const;
    double cal_total_dist() const;
    double cal_fitness() const;
};


Route::Route(std::string chromosome)
{

    this->chromosome = chromosome;
    // compute total distance first, then fitness (fitness depends on total_dist)
    total_dist = cal_total_dist();
    fitness = cal_fitness();
}

double Route::cal_total_dist() const
{
    double total_dist = 0.0;
    double fitness = 0.0;
    int len = chromosome.size();

    for (int i = 0; i < len; i++)
    {
        char from = chromosome[i];
        char to = chromosome[(i + 1) % len];

        auto [x1, y1] = MAP.at(from);
        auto [x2, y2] = MAP.at(to);

        total_dist += std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
    }
    return total_dist;
}

double Route::cal_fitness() const
{
    double fitness = 0.0;
    double total_dist = cal_total_dist();
    fitness = 1.0 / (total_dist + 1);
    return fitness;
}

Route Route::cross_over(const Route &par2) const
{
    int len = MAP.size();
    std::string child_chromossome(len, ' ');

    // --- CROSSOVER OX ---
    int start = random_num(0, len - 2);
    int end = random_num(start + 1, len - 1);

    for (int i = start; i <= end; i++)
        child_chromossome[i] = chromosome[i];

    int current = (end + 1) % len;
    for (int i = 0; i < len; i++)
    {
        char gene = par2.chromosome[(end + 1 + i) % len];
        if (child_chromossome.find(gene) == std::string::npos)
        {
            child_chromossome[current] = gene;
            current = (current + 1) % len;
        }
    }

    // --- MUTAÇÃO ---
    double mutation_rate = 0.05; // 5% de chance
    if (((double)rand() / RAND_MAX) < mutation_rate)
    {
        int i = random_num(0, len - 1);
        int j = random_num(0, len - 1);
        std::swap(child_chromossome[i], child_chromossome[j]);
    }

    return Route(child_chromossome);
}

struct FitnessData {
    int generation;
    double fitness;
    double distance;
};

void save_to_csv(const std::string &filename, const std::vector<FitnessData> &data, bool write_header = false) {
    std::ofstream file;
    file.open(filename, std::ios::app); // abre no modo append

    if (!file.is_open()) {
        std::cerr << "Erro ao abrir arquivo CSV: " << filename << std::endl;
        return;
    }

    if (write_header) {
        file << "generation,best_fitness,best_distance\n";
    }

    for (const auto &entry : data) {
        file << entry.generation << "," << entry.fitness << "," << entry.distance << "\n";
    }

    file.close();
}


bool operator<(const Route &route1, const Route &route2)
{
    return route1.fitness > route2.fitness;
}

int main()
{
    srand((unsigned)(time(0)));
    double best_brute = brute_force_tsp(MAP);

    // generate_map();
    print_map(MAP);

    int generation = 0;

    std::vector<Route> routes;

    std::string filename = "/Users/henilveira/Documents/UDESC/AGT/PROJETO_FINAL/results/fitness_data.csv";


    save_to_csv(filename, {}, true);

    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        std::string chromossome = generate_chromossome();
        routes.push_back(chromossome);
    }


    while (generation < MAX_GENERATIONS)
    {
        std::sort(routes.begin(), routes.end());

        const Route &best = routes.front();

        save_to_csv(filename, {{generation, best.fitness, best.total_dist}});

        std::vector<Route> new_generation;

        int s = 0.1 * POPULATION_SIZE;
        for (int i = 0; i < s; i++)
        {
            new_generation.push_back(routes[i]);
        }

        s = 0.9 * POPULATION_SIZE;
        int half_fitness = 0.5 * POPULATION_SIZE;
        for (int i = 0; i < s; i++)
        {
            int random = random_num(0, half_fitness);
            Route parent1 = routes[random];
            random = random_num(0, half_fitness);
            Route parent2 = routes[random];

            Route children = parent1.cross_over(parent2);

            new_generation.push_back(children);
        }

        routes = new_generation;

        std::cout << "Generation: " << generation << "\t";
        std::cout << "Chromossome: " << routes[0].chromosome << "\t";
        std::cout << "Fitness: " << routes[0].fitness << "\n";
        std::cout << "Total distance: " << routes[0].total_dist << "\n";

        generation++;
    }

    std::cout << "Generation: " << generation << "\t";
    std::cout << "Chromossome: " << routes[0].chromosome << "\t";
    std::cout << "Fitness: " << routes[0].fitness << "\n";
    std::cout << "Total distance: " << routes[0].total_dist << "\n";
    std::cout << "Distância ótima determinística: " << best_brute << std::endl;
    csv.close();

}