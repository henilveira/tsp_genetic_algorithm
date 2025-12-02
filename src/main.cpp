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

//define um tamanho fixo de individuos por populacao
#define POPULATION_SIZE 100

// qual o melhor fitness possível
#define TARGET_FITNESS 0.0

//numero maximo de geracoes para gerar offspring
#define MAX_GENERATIONS 10

std::ofstream csv("results/fitness_data.csv");

// define uma string com todas as cidades disponiveis
const std::string CITIES = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// define um range de distancia maxima que o caxeiro vai viajar
const int MAX_DISTANCE = 100;

// inicializar o mapa para gerar um aleatório
// std::unordered_map<char, std::tuple<int,int>> MAP;

// inicializar o mapa com variáveis fixas para monitorar a evolução
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

// para comparar resultados
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

//gerar um numero aleatório
int random_num(int start, int end)
{
    int range = (end-start)+1;
    int random_int = start+(rand()%range);
    return random_int;
}

//gera um gene aleatório que é um char e busca o indice na lista de cidades disponíveis (alfabeto)
char generate_gene(const int &index)
{
    int r = random_num(0, index-1);
    return CITIES[r];
}

// não está sendo utilizado porque não estou gerando mapa
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

// gera um chromossomo que é uma string com o tamanho da quantidade de cidades que o caxeiro vai viajar
std::string generate_chromossome()
{
    int len = MAP.size();
    std::string chromossome = "";
    char gene;
    std::set<char> used;

    // nao pode repetir cidade
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
// printa o mapa pra poder verificar se ta td certo quando eu quiser gerar o mapa
void print_map(const std::unordered_map<char, std::tuple<int, int>>& mapa) {
    std::cout << "Conteúdo do Mapa:" << std::endl;
    
    for (const auto& par : mapa) {
        int x = std::get<0>(par.second);
        int y = std::get<1>(par.second);

            std::cout << "  Chave: " << par.first
                  << " -> Valor: (" << x << ", " << y << ")" << std::endl;
    }
}
// calcula distancia de dois pontos num plano euclidiano
int dist(int x1, int x2, int y1, int y2)
{
    return std::sqrt(std::pow((x2-x1),2)+std::pow((y2-y1),2));
}

// define uma estrutura para o individuo
struct Route {

    // atributos, contem um chromossomo, uma distancia total e um fitness que é o quao bom é essa rota
    std::string chromosome;
    double total_dist;
    double fitness;
    
    // O individuo vai receber um chromossomo quando inicializa um individuo novo (rota)
    Route(std::string chromosome);

    // faz um corss_over com outro parent e gera offspring
    Route cross_over(const Route& parent2) const;

    // calcula a distancia total da rota pra comparar
    double cal_total_dist() const;

    // calcula o quao bom a rota é, fitness
    double cal_fitness() const;
};


Route::Route(std::string chromosome)
{
    // inicializa o individuo sempre recebendo o resultado do fitness e a total distance nos atributos
    this->chromosome = chromosome;
    total_dist = cal_total_dist();
    fitness = cal_fitness();
}


// itera sobre todas as cidades e tuplas e calcula a distancia total
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

//calcula o fitness que é 1/total_dist
double Route::cal_fitness() const
{
    double fitness = 0.0;
    double total_dist = cal_total_dist();
    fitness = 1.0 / (total_dist + 1);
    return fitness;
}


// utiliza PMX para fazer cross over, porque nao repete as cidades e 
// garante que os offsprings receberao todas as cidades
Route Route::cross_over(const Route &par2) const
{
    int len = MAP.size();
    std::string child_chromossome(len, ' ');

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

    return Route(child_chromossome);
}

// indiferente
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

// reescreve o operator de < para poder aplicar um sort nos individuos e colocar os melhores no comeco do vetor
bool operator<(const Route &route1, const Route &route2)
{
    return route1.fitness > route2.fitness;
}

int main()
{
    // biblioteca de random, inicializando a seed
    srand((unsigned)(time(0)));

    // calculando a melhor rota por brute force O(n!)
    double best_brute = brute_force_tsp(MAP);

    // generate_map();
    print_map(MAP);

    // inicializa a contagem de geracoes
    int generation = 0;

    // vetor de populacao de individuos
    std::vector<Route> routes;

    std::string filename = "/Users/henilveira/Documents/UDESC/AGT/PROJETO_FINAL/results/fitness_data.csv";


    save_to_csv(filename, {}, true);

    // itera sobre o vetor de populacao e insere chromossomos aleatorios
    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        std::string chromossome = generate_chromossome();
        routes.push_back(chromossome);
    }


    // condicao de parada para parar os cruzamentos
    while (generation < MAX_GENERATIONS)
    {

        // da um sort para os melhores virem primeiro
        std::sort(routes.begin(), routes.end());

        // isso é para o artigo
        const Route &best = routes.front();
        save_to_csv(filename, {{generation, best.fitness, best.total_dist}});

        // cria uma nova geracao de novos individuos
        std::vector<Route> new_generation;

        // aplica o que chamamos de elitismo, pegamos os 10% dos que tiveram o melhor fitness da ultima geracao
        int s = 0.1 * POPULATION_SIZE;
        // adiciono esses 10% na nova geracao
        for (int i = 0; i < s; i++)
        {
            new_generation.push_back(routes[i]);
        }

        // pego o conjunto complementar para completar o tamanho da populacao
        s = 0.9 * POPULATION_SIZE;
        // pego os 50% dos que tiveram melhor fitness da ultima geracao
        int half_fitness = 0.5 * POPULATION_SIZE;

        // itero e agora cruzo os 50% melhores até fechar o tamanho da populacao
        for (int i = 0; i < s; i++)
        {
            int random = random_num(0, half_fitness);
            Route parent1 = routes[random];
            random = random_num(0, half_fitness);
            Route parent2 = routes[random];

            Route children = parent1.cross_over(parent2);

            // sempre adicionando as offsprings
            new_generation.push_back(children);
        }

        // agora a nossa nova geracao, virou a geracao atual e o while se repete até a condicao de parada
        routes = new_generation;

        //prints para poder monitorar os atributos
        std::cout << "Generation: " << generation << "\t";
        std::cout << "Chromossome: " << routes[0].chromosome << "\t";
        std::cout << "Fitness: " << routes[0].fitness << "\n";
        std::cout << "Total distance: " << routes[0].total_dist << "\n";

        // incremento as geracoes a cada iteracao do while para contribuir na condicao de parada
        generation++;
    }

    std::cout << "Generation: " << generation << "\t";
    std::cout << "Chromossome: " << routes[0].chromosome << "\t";
    std::cout << "Fitness: " << routes[0].fitness << "\n";
    std::cout << "Total distance: " << routes[0].total_dist << "\n";
    std::cout << "Distância ótima determinística: " << best_brute << std::endl;
    csv.close();

}