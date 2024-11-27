#include <iostream>
#include <string>
#include <vector>
#include "conexao.h"
#include "utils.h"
#include "navio.h"

using namespace std;
using json = nlohmann::json;

int main(int argc, char *argv[]) {
	srand(time(NULL));

	string conexao, partidaAnterior;
	bool continuarPartidaAnterior;

	vector<vector<int>> todasPosicoes;
	vector<string> tipos = {"porta-avioes", "encouracado", "cruzador", "cruzador", "destroier", "destroier"};
	vector<int> tamanhos = {5, 4, 3, 3, 2, 2};
	json navios;

	cout << "Continuar a partida anterior? (sim/nao): ";

	do {
		getline(cin, partidaAnterior);
		if(partidaAnterior == "sim")
			continuarPartidaAnterior = true;
		else if(partidaAnterior == "nao")
			continuarPartidaAnterior = false;
		else
			cout << "Informe sim ou nao: ";
	} while(partidaAnterior != "sim" && partidaAnterior != "nao");

	if(continuarPartidaAnterior) {
		lerArquivoJSON(navios, "navios.json");
	} else {
		for(int i = 0; i < tipos.size(); ++i) {
			gerarPosicoes(tamanhos[i], todasPosicoes, tipos[i], navios);
		}

		estruturarArquivoJSON(navios, "navios.json");
		criarCampo("campo.json", navios);
		criarCampo("campoAdv.json");
	}

	cout << "Será client ou server? ";
	do {
		getline(cin, conexao);

		if(conexao == "server") {
			serverTCP(navios, continuarPartidaAnterior);
		} else if(conexao == "client") {
			clientTCP(argc, argv, navios, continuarPartidaAnterior);
		} else {
			cout << "Deve ser informado client ou server: ";
		}
	} while(conexao != "server" && conexao != "client");

	return 0;
}