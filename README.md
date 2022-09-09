# go_km
Este projeto tem por objetivo realizar execuções de código no Kernel do Windows, visando a manipulação de memória em processos.
- Leitura de memória virtual;
- Escrita de memória virtual;
- Alocação de memória virtual;
- Alteração de proteções de páginas virtuais;
- Desalocação de página virtual.

Este repositório contém dois projetos:
- go_km (Driver no modo Kernel que irá realizar as operações);
- go_um (Aplicação no modo Usuário que irá realizar as solicitações ao driver no modo Kernel)
