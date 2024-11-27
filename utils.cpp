#include <vector>
#include <fstream>
#include <regex>
#include <iostream>
#include "utils.h"

#define CAMPO 10

using json = nlohmann::json;
using namespace std;

void lerArquivoJSON(json &navios, string nome) {
	ifstream arquivo(nome);
	if(arquivo.is_open()) {
		arquivo >> navios;
		arquivo.close();
	}
}

void salvarArquivoJSON(json &navios, string nome) {
	ofstream arquivo(nome, ios::out | ios::trunc);

	if(arquivo.is_open()) {
		arquivo << navios;
		arquivo.close();
	}
}

void exibirNavioNoCampo(json &campo, vector<int> &posTiro, const string &tipoNavio) {
	static const unordered_map<string, string> simbolos = {
		 {"porta-avioes", "🛳️ "},
		 {"encouracado", "🚢"},
		 {"cruzador", "🛥️ "},
		 {"destroier", "🚤"}};
	campo[posTiro[0]][posTiro[1]] = simbolos.at(tipoNavio);
}

bool verificarMensagem(string &mensagem) {
	regex padrao("^[0-9][0-9]$");

	return regex_match(mensagem, padrao);
}

void exibirCampos() {
	json campo, campoAdv;
	lerArquivoJSON(campo, "campo.json");
	lerArquivoJSON(campoAdv, "campoAdv.json");

	string espacoLateral(12, ' ');
	string espacoEntreTabuleiros(26, ' ');

	cout << "\n";
	cout << espacoLateral << "MEU CAMPO" << espacoEntreTabuleiros << "CAMPO ADVERSÁRIO" << "\n";
	cout << "   ";
	for(int col = 0; col < 10; ++col) {
		cout << col << "  ";
	}
	cout << "       ";
	for(int col = 0; col < 10; ++col) {
		cout << col << "  ";
	}
	cout << "\n";

	for(int i = 0; i < 10; ++i) {
		cout << " " << i << " ";
		for(int j = 0; j < 10; ++j) {
			cout << campo[i][j].get<string>() << " ";
		}

		cout << "    ";

		cout << " " << i << " ";
		for(int j = 0; j < 10; ++j) {
			cout << campoAdv[i][j].get<string>() << " ";
		}
		cout << "\n";
	}
	cout << "\n";
}

void criarCampo(string nome, json navios) {
	vector<vector<string>> campoBatalha(CAMPO, vector<string>(CAMPO, "🟦"));
	json campo = campoBatalha;

	for(const auto &navio : navios) {
		for(const auto &pos : navio["posicoes"]) {
			string tipoNavio = navio["tipo"].get<string>();
			vector<int> posicao = pos.get<vector<int>>();
			exibirNavioNoCampo(campo, posicao, tipoNavio);
		}
	}

	salvarArquivoJSON(campo, nome);
}

void exibirCamposPosTiro(vector<vector<int>> &todasPosicoes, json &navios, vector<int> &posTiro, bool meuTiro) {
	json campo, campoAdv;

	lerArquivoJSON(campo, "campo.json");
	lerArquivoJSON(campoAdv, "campoAdv.json");

	bool acertou = false;

	if(meuTiro) {
		for(const auto &navio : navios) {
			for(const auto &pos : navio["posicoes"]) {
				if(pos == posTiro) {
					string tipoNavio = navio["tipo"].get<string>();
					vector<int> posicao = pos.get<vector<int>>();
					exibirNavioNoCampo(campoAdv, posicao, tipoNavio);
					acertou = true;
				}
			}
		}

		if(!acertou) {
			campoAdv[posTiro[0]][posTiro[1]] = "❌";
		}

	} else {
		for(const auto &navio : navios) {
			for(const auto &pos : navio["posicoes"]) {
				if(pos == posTiro) {
					campo[posTiro[0]][posTiro[1]] = "💥";
					acertou = true;
				}
			}
		}

		if(!acertou) {
			campo[posTiro[0]][posTiro[1]] = "❌";
		}
	}

	for(int i = 0; i < todasPosicoes.size(); ++i) {
		if(todasPosicoes[i] == posTiro) {
			todasPosicoes.erase(todasPosicoes.begin() + i);
			break;
		}
	}

	salvarArquivoJSON(campo, "campo.json");
	salvarArquivoJSON(campoAdv, "campoAdv.json");

	exibirCampos();
}

void estruturarArquivoJSON(json &navios, string nome) {
	ofstream arquivo(nome);

	arquivo << "[\n";
	for(int i = 0; i < navios.size(); ++i) {
		arquivo << "    {\n";
		arquivo << "        \"tipo\": \"" << navios[i]["tipo"].get<string>() << "\",\n";
		arquivo << "        \"posicoes\": ";
		arquivo << navios[i]["posicoes"];
		arquivo << "\n    }";
		if(i != navios.size() - 1) {
			arquivo << ",";
		}
		arquivo << "\n";
	}
	arquivo << "]";
}

void posicoesNavios(vector<vector<int>> &todasPosicoes, vector<vector<int>> &todasPosicoesAdversario) {
	json navios, naviosAdversarios;

	lerArquivoJSON(navios, "navios.json");
	lerArquivoJSON(naviosAdversarios, "naviosAdv.json");

	for(const auto &navio : naviosAdversarios) {
		for(const auto &pos : navio["posicoes"]) {
			todasPosicoesAdversario.push_back(pos);
		}
	}

	for(const auto &navio : navios) {
		for(const auto &pos : navio["posicoes"]) {
			todasPosicoes.push_back(pos);
		}
	}
}

int verificarTiro(string tiro, bool meuTiro, vector<vector<int>> &posicoesJogadas, vector<vector<int>> &todasPosicoes) {
	json navios, navioAtingido;
	vector<int> posTiro = {tiro[0] - '0', tiro[1] - '0'};

	lerArquivoJSON(navios, meuTiro ? "naviosAdv.json" : "navios.json");

	bool acertou = false;

	for(const auto &pos : posicoesJogadas) {
		if(pos == posTiro) {
			return 1;
		}
	}

	for(const auto &navio : navios) {
		for(const auto &pos : navio["posicoes"]) {
			if(pos == posTiro) {
				acertou = true;
				navioAtingido = navio;
				break;
			}
		}
	}

	posicoesJogadas.push_back(posTiro);

	exibirCamposPosTiro(todasPosicoes, navios, posTiro, meuTiro);

	int celulas = 0;

	if(acertou) {
		if(navioAtingido["tipo"].get<string>() == "porta-avioes")
			celulas = 5;
		else if(navioAtingido["tipo"].get<string>() == "encouracado")
			celulas = 4;
		else if(navioAtingido["tipo"].get<string>() == "cruzador")
			celulas = 3;
		else if(navioAtingido["tipo"].get<string>() == "destroier")
			celulas = 2;
	}

	if(todasPosicoes.size() == 0) {
		if(meuTiro) {
			if(acertou) {
				cout << "> Você atirou na posição " << posTiro[0] << ":" << posTiro[1] << " e ACERTOU! Navio: " << navioAtingido["tipo"].get<string>() << " [" << celulas << " células]" << "\n";
			} else {
				cout << "> Você atirou na posição " << posTiro[0] << ":" << posTiro[1] << " e ERROU!" << "\n";
			}
		} else {
			if(acertou) {
				cout << "> Adversário atirou na posição " << posTiro[0] << ":" << posTiro[1] << " e ACERTOU! Navio: " << navioAtingido["tipo"].get<string>() << " [" << celulas << " células]" << "\n";
			} else {
				cout << "> Adversário atirou na posição " << posTiro[0] << ":" << posTiro[1] << " e ERROU!" << "\n";
			}
		}
		return 0;
	}

	string mensagemAtirador = meuTiro ? "Você atirou" : "Adversário atirou";
	string mensagemTurno = meuTiro ? "> Aguardando adversário atirar..."
											 : "> É a sua vez de atirar!\n> Digite [0-9][0-9] (ex: 12 - linha 1 / coluna 2) para tentar acertar um navio";

	if(acertou) {
		cout << "> " << mensagemAtirador << " na posição " << posTiro[0] << ":" << posTiro[1]
			  << " e ACERTOU! Navio: " << navioAtingido["tipo"].get<string>()
			  << " [" << celulas << " células]" << "\n";
	} else {
		cout << "> " << mensagemAtirador << " na posição " << posTiro[0] << ":" << posTiro[1]
			  << " e ERROU!" << "\n";
	}

	cout << mensagemTurno << "\n";

	return 0;
}

void atualizarEstadoJogo(vector<vector<int>> &posicoesJogadas, vector<vector<int>> &posicoesJogadasAdv, vector<vector<int>> &todasPosicoes, vector<vector<int>> &todasPosicoesAdv) {
	json campo, campoAdv;

	lerArquivoJSON(campo, "campo.json");
	lerArquivoJSON(campoAdv, "campoAdv.json");

	for(int i = 0; i < campo.size(); ++i) {
		for(int j = 0; j < campo[i].size(); ++j) {
			if(campo[i][j].get<string>() == "💥" || campo[i][j].get<string>() == "❌") {
				vector<int> pos = {i, j};
				posicoesJogadasAdv.push_back(pos);
			}
		}
	}

	for(int i = 0; i < campoAdv.size(); ++i) {
		for(int j = 0; j < campoAdv[i].size(); ++j) {
			if(campoAdv[i][j].get<string>() != "🟦") {
				vector<int> pos = {i, j};
				posicoesJogadas.push_back(pos);
			}
		}
	}

	for(const auto &posicaoJogada : posicoesJogadas) {
		for(int i = 0; i < todasPosicoesAdv.size(); ++i) {
			if(posicaoJogada == todasPosicoesAdv[i]) {
				todasPosicoesAdv.erase(todasPosicoesAdv.begin() + i);
			}
		}
	}

	for(const auto &posicaoJogada : posicoesJogadasAdv) {
		for(int i = 0; i < todasPosicoes.size(); ++i) {
			if(posicaoJogada == todasPosicoes[i]) {
				todasPosicoes.erase(todasPosicoes.begin() + i);
			}
		}
	}
}

bool verificarFinalizacaoPartidaAnterior(vector<vector<int>> &posicoesJogadas, vector<vector<int>> &posicoesJogadasAdv, vector<vector<int>> &todasPosicoes, vector<vector<int>> &todasPosicoesAdversario) {
	vector<vector<int>> temp, tempAdv;

	for(const auto &pos : todasPosicoes) {
		for(const auto &posJogada : posicoesJogadasAdv) {
			if(posJogada == pos) {
				temp.push_back(pos);
			}
		}
	}

	for(const auto &pos : todasPosicoesAdversario) {
		for(const auto &posJogada : posicoesJogadas) {
			if(posJogada == pos) {
				tempAdv.push_back(pos);
			}
		}
	}

	if(temp.size() == todasPosicoes.size() || tempAdv.size() == todasPosicoesAdversario.size()) {
		return true;
	}

	return false;
}