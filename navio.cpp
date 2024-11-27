#include <vector>
#include <nlohmann/json.hpp>
#include "navio.h"

using namespace std;
using json = nlohmann::json;

#define CAMPO 10

void gerarNavio(string tipo, vector<vector<int>> &posicoesNavio, json &navios) {
	json navio;
	navio["tipo"] = tipo;
	navio["posicoes"] = posicoesNavio;
	navios.push_back(navio);
}

bool verificarSobreposicao(const vector<vector<int>> &todasPosicoes, const vector<int> &novaPosicao) {
	for(const auto &pos : todasPosicoes) {
		if(pos == novaPosicao) {
			return true;
		}
	}
	return false;
}

void gerarPosicoes(int tamanho, vector<vector<int>> &todasPosicoes, string tipo, json &navios) {
	bool sobrepor;
	bool isHorizontal = rand() % 2;
	vector<vector<int>> posicoesNavio;

	if(isHorizontal) {
		while(true) {
			vector<int> posicaoInicial = {rand() % CAMPO, rand() % CAMPO};

			if(posicaoInicial[1] + tamanho > CAMPO) {
				continue;
			}

			posicoesNavio.clear();

			sobrepor = false;
			for(int i = 0; i < tamanho; ++i) {
				vector<int> posicao = {posicaoInicial[0], posicaoInicial[1] + i};
				if(verificarSobreposicao(todasPosicoes, posicao)) {
					sobrepor = true;
					break;
				}
				posicoesNavio.push_back(posicao);
			}

			if(!sobrepor) {
				gerarNavio(tipo, posicoesNavio, navios);
				for(const auto &pos : posicoesNavio) {
					todasPosicoes.push_back(pos);
				}
				break;
			}
		}
	} else {
		while(true) {
			vector<int> posicaoInicial = {rand() % CAMPO, rand() % CAMPO};

			if(posicaoInicial[0] + tamanho > CAMPO) {
				continue;
			}

			vector<vector<int>> posicoesNavio;
			sobrepor = false;
			for(int i = 0; i < tamanho; ++i) {
				vector<int> posicao = {posicaoInicial[0] + i, posicaoInicial[1]};
				if(verificarSobreposicao(todasPosicoes, posicao)) {
					sobrepor = true;
					break;
				}
				posicoesNavio.push_back(posicao);
			}

			if(!sobrepor) {
				gerarNavio(tipo, posicoesNavio, navios);
				for(const auto &pos : posicoesNavio) {
					todasPosicoes.push_back(pos);
				}
				break;
			}
		}
	}
}