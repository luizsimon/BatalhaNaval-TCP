# Jogo de Batalha Naval Peer-to-Peer

## Descrição

O **Jogo de Batalha Naval Peer-to-Peer** permite que dois jogadores (um atuando como **Server** e o outro como **Client**) joguem uma partida de Batalha Naval via conexão TCP. O **Server** começa jogando e a partida é realizada em turnos, com cada jogador enviando ataques na tentativa de afundar os navios do adversário. O jogo também oferece a possibilidade de **continuar uma partida anterior** que foi interrompida, mantendo o estado do jogo para que ambos os jogadores possam continuar de onde pararam.

## Funcionalidades
- **Modo Turnos**: A partida é jogada em turnos, onde cada jogador realiza um ataque por vez.
- **Server Começa Jogando**: O jogador que iniciar como **Server** será o primeiro a jogar.
- **Continuação de Partidas**: O jogo permite continuar uma partida anterior inacabada, mantendo o estado da partida anterior.
- **Exibição de Campos de Jogo**: O jogo exibe os campos de batalha de ambos os jogadores, atualizando após cada movimento.

## Requisitos
- **Sistema Operacional**: Linux
- **Compilador**: g++ com suporte a C++11
- **Porta**: a porta 8080 deve estar livre
- **Client**: deve saber o IP local do jogador server
- **Bibliotecas**:
  - `nlohmann/json` para manipulação de arquivos JSON.
  - Bibliotecas padrão do C++ para socket e manipulação de rede.

## Como Compilar e Executar

1. Para compilar, use o comando:

   ```bash
   g++ -I. main.cpp navio.cpp utils.cpp conexao.cpp -o batalhaNaval -std=c++11
   ```
2. Para executar o jogo:
   
   ```bash
   ./batalhaNaval
   ```
