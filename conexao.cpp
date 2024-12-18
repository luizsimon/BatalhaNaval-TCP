#include <arpa/inet.h>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include "utils.h"
#include "conexao.h"

using namespace std;
using json = nlohmann::json;

#define ECHOMAX 1024

void enviarNavios(int &rem_sockfd, json &navios) {
	cout << "> Enviando os navios para o adversário..." << "\n";
	
	string naviosJSON = json(navios).dump();

	send(rem_sockfd, naviosJSON.c_str(), naviosJSON.size(), 0);
}

void receberNavios(int &rem_sockfd) {
	vector<char> buffer(ECHOMAX);
	string recebido;

	while(true) {
		int bytes = recv(rem_sockfd, buffer.data(), buffer.size(), 0);

		recebido.append(buffer.data(), bytes);
		break;
	}

	cout << "> Recebendo os Navios do Adversário..." << "\n";

	json naviosAdv = json::parse(recebido);

	estruturarArquivoJSON(naviosAdv, "naviosAdv.json");
}

void enviarMensagemAdversario(int &sockfd, vector<vector<int>> &posicoesJogadas, vector<vector<int>> &todasPosicoesAdv) {
	bool verificacao = false;

	do {
		string mensagem, buffer;
		buffer.resize(ECHOMAX);

		cout << "> ";

		getline(cin, mensagem);

		if(verificarMensagem(mensagem)) {
			int retorno = verificarTiro(mensagem, true, posicoesJogadas, todasPosicoesAdv);
			if(retorno == 1) {
				cout << "> Tiro nessa posição já realizado! Informe outra posição" << "\n";
				continue;
			}
			buffer.resize(mensagem.size());
			buffer = mensagem;
			send(sockfd, &buffer[0], buffer.size(), 0);
			verificacao = true;
		} else {
			cout << "> Formato da mensagem deve ser -> [0-9][0-9] (ex: 12 - linha 1 / coluna 2)" << "\n";
		}
	} while(verificacao == false);
}

void serverTCP(json &navios, bool &continuarPartidaAnterior) {

	int loc_sockfd, loc_newsockfd;
	socklen_t tamanho;
	struct sockaddr_in loc_addr;
	const char *porta = "8080";

	loc_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(loc_sockfd < 0) {
		perror("Criando stream socket");
		exit(1);
	}

	/* Construcao da estrutura do endereco local */
	/* Preenchendo a estrutura socket loc_addr (familia, IP, porta) */
	loc_addr.sin_family = AF_INET;			 /* familia do protocolo*/
	loc_addr.sin_addr.s_addr = INADDR_ANY;	 /* endereco IP local */
	loc_addr.sin_port = htons(atoi(porta)); /* porta local  */
	bzero(&(loc_addr.sin_zero), 8);

	if(bind(loc_sockfd, (struct sockaddr *)&loc_addr, sizeof(struct sockaddr)) < 0) {
		perror("Ligando stream socket");
		exit(1);
	}

	listen(loc_sockfd, 1);

	printf("> aguardando conexao\n");

	tamanho = sizeof(struct sockaddr_in);

	loc_newsockfd = accept(loc_sockfd, (struct sockaddr *)&loc_addr, &tamanho);

	string mensagem, buffer;
	vector<vector<int>> todasPosicoes, todasPosicoesAdversario, posicoesJogadas, posicoesJogadasAdv;

	enviarNavios(loc_newsockfd, navios);
	receberNavios(loc_newsockfd);

	posicoesNavios(todasPosicoes, todasPosicoesAdversario);

	bool finalizacaoPartida = false;
	bool myTurn = true;

	if(continuarPartidaAnterior) {
		atualizarEstadoJogo(posicoesJogadas, posicoesJogadasAdv, todasPosicoes, todasPosicoesAdversario);
		finalizacaoPartida = verificarFinalizacaoPartida(posicoesJogadas, posicoesJogadasAdv, todasPosicoes, todasPosicoesAdversario);
		myTurn = verificarTurno();
	}

	if(finalizacaoPartida) {
		exibirCampos();
		cout << "Partida já finalizada anteriormente!" << "\n";
	} else {
		exibirCampos();
	}

	while(finalizacaoPartida == false) {
		buffer.resize(ECHOMAX);

		if(myTurn) {
			cout << "> Seu turno!" << "\n";
			cout << "> Digite [0-9][0-9] (ex: 12 - linha 1 / coluna 2) para tentar acertar um navio!" << "\n";

			enviarMensagemAdversario(loc_newsockfd, posicoesJogadas, todasPosicoesAdversario);
			finalizacaoPartida = verificarFinalizacaoPartida(posicoesJogadas, posicoesJogadasAdv, todasPosicoes, todasPosicoesAdversario);

			myTurn = false;

			json campo;
			lerArquivoJSON(campo, "campo.json");
			campo["myTurn"] = myTurn;
			salvarArquivoJSON(campo, "campo.json");

			if(finalizacaoPartida) {
				cout << "> Você ganhou!!! 😃" << "\n";
				break;
			}
		} else {
			cout << "> Aguardando adversário atirar..." << "\n";

			recv(loc_newsockfd, &buffer[0], buffer.size(), 0);
			verificarTiro(buffer, false, posicoesJogadasAdv, todasPosicoes);
			finalizacaoPartida = verificarFinalizacaoPartida(posicoesJogadas, posicoesJogadasAdv, todasPosicoes, todasPosicoesAdversario);

			myTurn = true;

			json campo;
			lerArquivoJSON(campo, "campo.json");
			campo["myTurn"] = myTurn;
			salvarArquivoJSON(campo, "campo.json");

			if(finalizacaoPartida) {
				cout << "> Você perdeu!!! 😢" << "\n";
				break;
			}
		}
	};

	close(loc_sockfd);
	close(loc_newsockfd);
}

void clientTCP(int argc, char *argv[], json &navios, bool &continuarPartidaAnterior) {
	const char *rem_hostname;
	int rem_port;
	struct sockaddr_in rem_addr;
	int rem_sockfd;
	const string porta = "8080";
	string ip;

	cout << "Digite o endereço IP do server: ";
	getline(cin, ip);

	rem_hostname = ip.c_str();
	rem_port = atoi(porta.c_str());
	rem_addr.sin_family = AF_INET;							 /* familia do protocolo*/
	rem_addr.sin_addr.s_addr = inet_addr(rem_hostname); /* endereco IP local */
	rem_addr.sin_port = htons(rem_port);					 /* porta local  */

	rem_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(rem_sockfd < 0) {
		perror("Criando stream socket");
		exit(1);
	}

	cout << "> Conectando no servidor " << rem_hostname << ":" << rem_port << "\n";

	if(connect(rem_sockfd, (struct sockaddr *)&rem_addr, sizeof(rem_addr)) < 0) {
		perror("Conectando stream socket");
		exit(1);
	}

	string buffer, mensagem;
	vector<vector<int>> posicoesJogadas, posicoesJogadasAdv, todasPosicoes, todasPosicoesAdversario;

	receberNavios(rem_sockfd);
	enviarNavios(rem_sockfd, navios);

	posicoesNavios(todasPosicoes, todasPosicoesAdversario);

	bool finalizacaoPartida = false;
	bool myTurn = false;

	if(continuarPartidaAnterior) {
		atualizarEstadoJogo(posicoesJogadas, posicoesJogadasAdv, todasPosicoes, todasPosicoesAdversario);
		finalizacaoPartida = verificarFinalizacaoPartida(posicoesJogadas, posicoesJogadasAdv, todasPosicoes, todasPosicoesAdversario);
		myTurn = verificarTurno();
	}

	if(finalizacaoPartida) {
		exibirCampos();
		cout << "Partida já finalizada anteriormente!" << "\n";
	} else {
		exibirCampos();
	}
		
	while(finalizacaoPartida == false) {
		buffer.resize(ECHOMAX);

		if (myTurn) {
			cout << "> Seu turno!" << "\n";
			cout << "> Digite [0-9][0-9] (ex: 12 - linha 1 / coluna 2) para tentar acertar um navio!" << "\n";

			enviarMensagemAdversario(rem_sockfd, posicoesJogadas, todasPosicoesAdversario);
			finalizacaoPartida = verificarFinalizacaoPartida(posicoesJogadas, posicoesJogadasAdv, todasPosicoes, todasPosicoesAdversario);

			myTurn = false;
			
			json campo;
			lerArquivoJSON(campo, "campo.json");
			campo["myTurn"] = myTurn;
			salvarArquivoJSON(campo, "campo.json");

			if(finalizacaoPartida) {
				cout << "> Você ganhou!!! 😃" << "\n";
				break;
			}
		} else {
			cout << "> Aguardando adversário atirar..." << "\n";

			recv(rem_sockfd, &buffer[0], buffer.size(), 0);
			verificarTiro(buffer, false, posicoesJogadasAdv, todasPosicoes);
			finalizacaoPartida = verificarFinalizacaoPartida(posicoesJogadas, posicoesJogadasAdv, todasPosicoes, todasPosicoesAdversario);

			myTurn = true;

			json campo;
			lerArquivoJSON(campo, "campo.json");
			campo["myTurn"] = myTurn;
			salvarArquivoJSON(campo, "campo.json");

			if(finalizacaoPartida) {
				cout << "> Você perdeu!!! 😢" << "\n";
				break;
			}
		}
	};

	close(rem_sockfd);
}