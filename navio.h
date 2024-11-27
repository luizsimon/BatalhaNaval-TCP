#include <nlohmann/json.hpp>
#include <vector>

using namespace std;
using json = nlohmann::json;

bool verificarSobreposicao(const vector<vector<int>> &todasPosicoes, const vector<int> &novaPosicao);
void gerarPosicoes(int tamanho, vector<vector<int>> &todasPosicoes, string tipo, json &navios);
void gerarNavio(string tipo, vector<vector<int>> &posicoesNavio, json &navios);

