#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// Declaração das funções
void lerArquivoJSON(json &navios, string nome);
void salvarArquivoJSON(json &navios, string nome);
void exibirNavioNoCampo(json &campo, vector<int> &posTiro, const string &tipoNavio);
bool verificarMensagem(string &mensagem);

void exibirCampos();
void criarCampo(string nome, json navios = {});
void exibirCamposPosTiro(vector<vector<int>> &todasPosicoes, json &navios, vector<int> &posTiro, bool meuTiro);
void estruturarArquivoJSON(json &navios, string nome);
void posicoesNavios(vector<vector<int>> &todasPosicoes, vector<vector<int>> &todasPosicoesAdversario);
int verificarTiro(string tiro, bool meuTiro, vector<vector<int>> &posicoesJogadas, vector<vector<int>> &todasPosicoes);
void atualizarEstadoJogo(vector<vector<int>> &posicoesJogadas, vector<vector<int>> &posicoesJogadasAdv, vector<vector<int>> &todasPosicoes, vector<vector<int>> &todasPosicoesAdv);
bool verificarFinalizacaoPartidaAnterior(vector<vector<int>> &posicoesJogadas, vector<vector<int>> &posicoesJogadasAdv, vector<vector<int>> &todasPosicoes, vector<vector<int>> &todasPosicoesAdversario);

#endif