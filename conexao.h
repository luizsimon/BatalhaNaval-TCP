#include "utils.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <nlohmann/json.hpp>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <vector>

using namespace std;
using json = nlohmann::json;

void enviarNavios(int &rem_sockfd, json &navios);
void receberNavios(int &rem_sockfd);
void enviarMensagemAdversario(int &sockfd, vector<vector<int>> &posicoesJogadas, vector<vector<int>> &todasPosicoesAdv);
void serverTCP(json &navios, bool &continuarPartidaAnterior);
void clientTCP(int argc, char *argv[], json &navios, bool &continuarPartidaAnterior);